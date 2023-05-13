#include <QObject>
#include <QAudioOutput>
#include <QIODevice>
#include <QMutex>
#include <QWaitCondition>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

class AudioDecode : public QObject
{
    Q_OBJECT
public:
    explicit AudioDecode(QObject *parent = nullptr);
    ~AudioDecode();

    void open(const QString& url = QString());
    void close();

signals:
    void audioReady(const QByteArray &data);

private:
    AVCodecContext *m_codecCtx = nullptr;
    AVCodec *m_codec = nullptr;
    SwrContext *m_swrCtx = nullptr;
    AVFrame *m_frame = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QIODevice *m_audioDevice = nullptr;
    QMutex m_mutex;
    QWaitCondition m_waitCond;
    bool m_quit = false;

    void decodePacket(AVPacket *pkt);
    void convertFrame();
};
