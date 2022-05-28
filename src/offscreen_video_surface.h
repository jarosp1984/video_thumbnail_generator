#pragma once
#include <QAbstractVideoSurface>
#include <QImage>

//! Video surface for generating thumbnails
class COffscreenVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT

    QImage              m_image;            //!< Renderd image
    std::atomic_bool    m_frameRendered;    //!< Frame rendered flag

public:
    //! Constructor: clears m_frameRendered
    COffscreenVideoSurface();

    //! Get image
    const QImage& GetImage() const { return m_image; }

    //! Reset surface
    void Reset() { m_frameRendered.store(false); }

    //! Get suuported video formats
    virtual QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType type = QAbstractVideoBuffer::NoHandle) const override;    

    //! Present frame
    virtual bool present(const QVideoFrame &frame) override;

    //! Returns frame rendered status
    bool WasFrameRendered() const { return m_frameRendered.load(); }

signals:
    //! Frame was rendered
    void FrameRendered();
};
