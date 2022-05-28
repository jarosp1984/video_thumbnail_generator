#include <QtTest>
#include "video_thumbnail_generator.h"

class TestVideoThumnailGenerator : public QObject
{
    Q_OBJECT

private slots:
    void test_RequestThumbnail();
    void test_RequestThumbnail_CancelImmediately();
    void test_RequestThumbnail_CancelInProgress();
    void test_RequestThumbnail_CancelFinished();
    void test_RequestThumbnail_MissingVideo();
};

constexpr const char* VIDEO_PATH = "../media/test_short.mp4";

void TestVideoThumnailGenerator::test_RequestThumbnail()
{
    CVideoThumbnailGenerator generator;
    generator.SetThumnailSize({64,64});
    bool finished = false;
    bool generationSucceeded = false;

    auto funGenerationFinishedHandler = [&finished, &generationSucceeded](int requestId, const QString& filePath, bool succeeded)
    {
        finished = true;
        generationSucceeded = succeeded;
    };

    connect(&generator, &CVideoThumbnailGenerator::GenerationFinished, funGenerationFinishedHandler);
    int requestId = generator.RequestThumbnail(VIDEO_PATH);
    while(!finished)
    {
        generator.thread()->msleep(100);
        QApplication::instance()->processEvents();
        generator.Update();
    }

    QPixmap pixmap = generator.GetPixmap(requestId);
    generator.DeleteFinishedRequest(requestId);

    QVERIFY(generationSucceeded);
    QCOMPARE(pixmap.width(), 64);
    QCOMPARE(pixmap.height(), 36);
}

void TestVideoThumnailGenerator::test_RequestThumbnail_MissingVideo()
{
    QTest::ignoreMessage(QtMsgType::QtWarningMsg, "Media error:  QMediaPlayer::ResourceError , file:  \"some_invalid_path.mp4\"");

    CVideoThumbnailGenerator generator;
    bool finished = false;
    bool generationSucceeded = false;

    auto funGenerationFinishedHandler = [&finished, &generationSucceeded](int requestId, const QString& filePath, bool succeeded)
    {
        finished = true;
        generationSucceeded = succeeded;
    };

    connect(&generator, &CVideoThumbnailGenerator::GenerationFinished, funGenerationFinishedHandler);
    int requestId = generator.RequestThumbnail("some_invalid_path.mp4");
    while(!finished)
    {
        generator.thread()->msleep(100);
        QApplication::instance()->processEvents();
        generator.Update();
    }

    QPixmap pixmap = generator.GetPixmap(requestId);
    generator.DeleteFinishedRequest(requestId);

    QVERIFY(!generationSucceeded);
    QVERIFY(pixmap.isNull());
}

void TestVideoThumnailGenerator::test_RequestThumbnail_CancelFinished()
{
    CVideoThumbnailGenerator generator;
    bool finished = false;
    bool generationSucceeded = false;

    auto funGenerationFinishedHandler = [&finished, &generationSucceeded](int requestId, const QString& filePath, bool succeeded)
    {
        finished = true;
        generationSucceeded = succeeded;
    };

    connect(&generator, &CVideoThumbnailGenerator::GenerationFinished, funGenerationFinishedHandler);

    int requestId = generator.RequestThumbnail(VIDEO_PATH);
    while(!finished)
    {
        generator.thread()->msleep(100);
        QApplication::instance()->processEvents();
        generator.Update();
    }

    QPixmap pixmap = generator.GetPixmap(requestId);
    bool cancelResult = generator.CancelRequest(requestId);
    generator.DeleteFinishedRequest(requestId);

    QVERIFY(generationSucceeded);
    QVERIFY(cancelResult);
}

void TestVideoThumnailGenerator::test_RequestThumbnail_CancelInProgress()
{
    CVideoThumbnailGenerator generator;

    int requestId = generator.RequestThumbnail(VIDEO_PATH);
    generator.Update();
    QApplication::instance()->processEvents();
    bool cancelResult = generator.CancelRequest(requestId);

    QVERIFY(cancelResult);
}

void TestVideoThumnailGenerator::test_RequestThumbnail_CancelImmediately()
{
    CVideoThumbnailGenerator generator;

    int requestId = generator.RequestThumbnail(VIDEO_PATH);
    bool cancelResult = generator.CancelRequest(requestId);
    QPixmap pixmap = generator.GetPixmap(requestId);
    generator.DeleteFinishedRequest(requestId);

    QVERIFY(cancelResult);
    QVERIFY(pixmap.isNull());
}


QTEST_MAIN(TestVideoThumnailGenerator)

#include "test_video_thumbnail_generator.moc"
