#pragma once
#include <QMediaPlayer>
#include <QPixmap>
#include "offscreen_video_surface.h"

//! Class for generating thumbnails for videos
class CVideoThumbnailGenerator : public QObject
{
    Q_OBJECT

    //! Request state
    enum class ERequestState
    {
        Idle,                   //!< Request is idle
        InProgress,             //!< Request in progress
        Failed,                 //!< Request failed
        Suceeded                //!< Request suceeded
    };

    //! Thumbnail generation request
    struct SRequest
    {
        int             m_requestId = -1;                       //!< Request id
        ERequestState   m_requestState = ERequestState::Idle;   //!< Request state
        QString         m_filePath;                             //!< Movie file path
        QPixmap         m_pixmap;                               //!< Resulting pixmap
    };

private:    
    QMediaPlayer                m_mediaPlayer;          //!< The media player
    COffscreenVideoSurface      m_surface;              //!< Offscreen surface
    QVector<SRequest*>          m_requests;             //!< Requests to process
    QVector<SRequest*>          m_finishedRequests;     //!< Finished requests
    int                         m_nextRequestId = 1;    //!< Next request id
    QPixmap                     m_emptyPixap;           //!< Empty pixmap for returning in GetPixmap()
    QSize                       m_thumbnailSize;        //!< Target size for thumbnail

public:
    //! Constructor
    CVideoThumbnailGenerator();

    //! Destructor
    ~CVideoThumbnailGenerator();

    //! Request thumbnail. Returns request Id
    int RequestThumbnail(const QString& filePath);

    //! Update generator (this should be called from maim thread)
    void Update();

    //! Get pixmap from finsihed requests list
    const QPixmap& GetPixmap(int requestId) const;

    //! Delete finished request. Returns true if found.
    bool DeleteFinishedRequest(int requestId);

    //! Cancel request. Returns true if found.
    bool CancelRequest(int requestId);

    //! Set thumnail size (scaling using aspect ratio)
    void SetThumnailSize(const QSize& thumbnailSize);

private:
    //! Update request
    void UpdateRequest(SRequest* request);

    //! Start processing request
    void StartProcessingRequest(SRequest* request);

    //! Do processing of started request. Returned true means that request finished
    bool CarryOnProcessingRequest(SRequest* request);

    //! Stop media player if needed. Can wait if wait is true.
    void StopMediaPlayerIfNeeded(bool wait);

signals:
    //! Thumbnail generation finished
    void GenerationFinished(int requestId, const QString& filePath, bool succeeded);
};
