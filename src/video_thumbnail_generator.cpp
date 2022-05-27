#include <QApplication>
#include <QThread>
#include "video_thumbnail_generator.h"

CVideoThumbnailGenerator::CVideoThumbnailGenerator()
{
    connect(&m_surface, &COffscreenVideoSurface::FrameRendered, [this](){ Q_ASSERT(m_mediaPlayer.state() == QMediaPlayer::PlayingState); m_mediaPlayer.stop(); });
}

CVideoThumbnailGenerator::~CVideoThumbnailGenerator()
{
    m_mediaPlayer.setVideoOutput(static_cast<QAbstractVideoSurface *>(nullptr));
    StopMediaPlayerIfNeeded(true);
    Q_ASSERT(m_requests.size() == 0);
    Q_ASSERT(m_finishedRequests.size() == 0);
}

void CVideoThumbnailGenerator::StopMediaPlayerIfNeeded(bool wait)
{
    if(m_mediaPlayer.state() != QMediaPlayer::State::StoppedState)
    {
        m_mediaPlayer.stop();
        while(wait && m_mediaPlayer.state() != QMediaPlayer::State::StoppedState)
        {
            this->thread()->msleep(100);
            QApplication::processEvents();
        }
    }
}

int CVideoThumbnailGenerator::RequestThumbnail(const QString& filePath)
{
    SRequest* request = new SRequest;
    request->m_filePath = filePath;

    int requestId = -1;
    requestId = m_nextRequestId++;
    request->m_requestId = requestId;
    m_requests.push_back(request);

    return requestId;
}

void CVideoThumbnailGenerator::Update()
{
    if(m_requests.size())
    {
        UpdateRequest(m_requests[0]);
    }
}

void CVideoThumbnailGenerator::UpdateRequest(SRequest* request)
{
    switch(request->m_requestState)
    {
        case ERequestState::Idle:
            StartProcessingRequest(request);
            break;

        case ERequestState::InProgress:
            if( CarryOnProcessingRequest(request) )
            {
                emit GenerationFinished(request->m_requestId, request->m_filePath, request->m_requestState == ERequestState::Suceeded);
                m_requests.removeOne(request);
                m_finishedRequests.push_back(request);
            }
            break;

        case ERequestState::Failed:
        case ERequestState::Suceeded:
            Q_ASSERT(0 && "Finished requests should never be here");
            break;
    }
}

void CVideoThumbnailGenerator::StartProcessingRequest(SRequest* request)
{
    if(m_mediaPlayer.state() == QMediaPlayer::StoppedState)
    {
        m_surface.Reset();
        m_mediaPlayer.setVideoOutput(&m_surface);
        m_mediaPlayer.setMedia(QUrl::fromLocalFile(request->m_filePath));
        m_mediaPlayer.setMuted(true);
        m_mediaPlayer.play();

        request->m_requestState = ERequestState::InProgress;
    }
    else
    {
        qDebug() << "Waiting for media player stop";
    }
}

bool CVideoThumbnailGenerator::CarryOnProcessingRequest(SRequest* request)
{
    if(m_mediaPlayer.error() != QMediaPlayer::NoError)
    {
        qWarning() << "Media error: " << m_mediaPlayer.error() << ", file: " << request->m_filePath;
        StopMediaPlayerIfNeeded(false);
        request->m_requestState = ERequestState::Failed;
        return true;
    }
    else if( m_surface.WasFrameRendered() )
    {
        m_mediaPlayer.stop();
        request->m_pixmap = QPixmap::fromImage( m_surface.GetImage() );
        if(m_thumbnailSize.width() != -1)
        {
            request->m_pixmap = request->m_pixmap.scaled(m_thumbnailSize, Qt::KeepAspectRatio, Qt::FastTransformation);
        }
        request->m_requestState = ERequestState::Suceeded;
        return true;
    }
    else
    {
        return false;
    }
}

const QPixmap& CVideoThumbnailGenerator::GetPixmap(int requestId) const
{
    for(int i = 0; i < m_finishedRequests.size(); ++i)
    {
        if(m_finishedRequests[i]->m_requestId == requestId)
        {
            return m_finishedRequests[i]->m_pixmap;
        }
    }

    return m_emptyPixap;
}

bool CVideoThumbnailGenerator::DeleteFinishedRequest(int requestId)
{
    for(int i = 0; i < m_finishedRequests.size(); ++i)
    {
        if(m_finishedRequests[i]->m_requestId == requestId)
        {
            delete m_finishedRequests[i];
            m_finishedRequests.erase(m_finishedRequests.begin() + i);
            return true;
        }
    }

    return false;
}

bool CVideoThumbnailGenerator::CancelRequest(int requestId)
{
    if( !DeleteFinishedRequest(requestId) )
    {
        for(SRequest* request : m_requests)
        {
            if(request->m_requestId == requestId)
            {
                if(request->m_requestState == ERequestState::InProgress)
                {
                    StopMediaPlayerIfNeeded(false);
                }
                m_requests.removeOne(request);
                delete request;
                return true;
            }
        }

        // Not found
        return false;
    }
    else
    {
        // Found in finished list
        return true;
    }
}

void CVideoThumbnailGenerator::SetThumnailSize(const QSize& thumbnailSize)
{
   m_thumbnailSize = thumbnailSize;
}
