#include <QApplication>
#include <QThread>
#include <QTimer>
#include <QFileInfo>
#include "video_thumbnail_generator.h"

//! Sample application
class CSampleApplication : public QApplication
{
public:
    CVideoThumbnailGenerator    m_generator;    //!< Thumbnail generator
    QTimer                      m_timer;        //!< Update timer

    CSampleApplication(int &argc, char **argv)
        : QApplication(argc, argv)
    {
        // Setup update timer
        connect(&m_timer, &QTimer::timeout, this, &CSampleApplication::Update);
        m_timer.setInterval(100);

        // Setup generator
        m_generator.SetThumnailSize({64,64});
        m_generator.connect(&m_generator, &CVideoThumbnailGenerator::GenerationFinished, this, &CSampleApplication::HandleGenerationFinished);

        constexpr const char* VIDEO_PATH = "../media/test_short.mp4";
        int requestId = m_generator.RequestThumbnail(QFileInfo(VIDEO_PATH).absoluteFilePath()); // absolute file path needed for GStreamer based backend
        qDebug() << "Request created, id:" << requestId;

        m_timer.start();
    }

private slots:
    //! Handle generation finished
    void HandleGenerationFinished(int requestId, const QString& filePath, bool succeeded)
    {
        qDebug() << "Thumbnail generation for file "<< filePath << " finished, result: " << (succeeded ? 1 : 0);
        if(succeeded)
        {
            QPixmap pm = m_generator.GetPixmap(requestId);
            pm.save("thumbnail.png");
        }
        m_generator.DeleteFinishedRequest(requestId);
        quit();
    }

    //! Handle generation finished
    void Update()
    {
        m_generator.Update();
    }
};

int main(int argc, char** argv)
{
    CSampleApplication app(argc, argv);
    return app.exec();
}
