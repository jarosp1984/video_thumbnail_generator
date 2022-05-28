#include <QVideoSurfaceFormat>
#include <QDebug>
#include "offscreen_video_surface.h"

COffscreenVideoSurface::COffscreenVideoSurface()
    : m_frameRendered(false)
{
}

QList<QVideoFrame::PixelFormat> COffscreenVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
{
    return { QVideoFrame::Format_ARGB32 };
}

bool COffscreenVideoSurface::present(const QVideoFrame &frame)
{
    qDebug() << "COfflineVideoSurface::present" << frame.width() << "x" << frame.height();
    if(!m_frameRendered && frame.width() != -1)
    {
        m_image = frame.image();
        m_frameRendered = true;
        emit FrameRendered();
    }
    return true;
}
