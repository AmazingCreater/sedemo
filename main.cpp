#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QGuiApplication>
#include <QtQml/QtQml>
#include <QVector>
#include <QObject>
#include <opencv2/core/core.hpp>
#include "QPicProvider.h"
#include "qvideocapture.h"
#include "worker/cnstreamworker.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    qmlRegisterType<QVideoCapture>("CambriconControl", 1, 0, "VideoCapture");
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<QVector<cv::Mat>>("QVector<cv::Mat>");

    QQmlApplicationEngine engine;
    auto *pic_provider = new QPicProvider;
    engine.addImageProvider("imageProvider", pic_provider);
    engine.load(QUrl(QStringLiteral("qrc:/resources/main.qml")));
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    CnstreamWorker worker;
    engine.rootContext()->setContextProperty("fps_updater", &worker);

   QList<QObject*> aryRoots = engine.rootObjects();
   QVideoCapture* objVideoView = nullptr;

   if (aryRoots.size() > 0)
   {
       QObject* objRoot = aryRoots[0];
    for(int i=0;i<1;i++)
    {
        QObject* objItem = objRoot->findChild<QObject*>("videoView");
        objVideoView = qobject_cast<QVideoCapture*>(objItem);

        if (objVideoView == nullptr)  {

            return -1;
        }
        QObject::connect(&worker,SIGNAL(sig_refresh(cv::Mat)),objVideoView,SLOT(receiveFrames(cv::Mat)));
        QObject::connect(objVideoView, SIGNAL(sig_change_chn(int)), &worker, SLOT(ChangeChn(int)));
    }
   }
   QObject::connect(&engine, SIGNAL(quit()), &worker, SLOT(Stop()));
   QObject::connect(&worker, SIGNAL(sig_refresh_detail(cv::Mat)), pic_provider, SLOT(RefreshDetail(cv::Mat)));
   QObject::connect(&worker, SIGNAL(sig_refresh_obj(QVector<cv::Mat>)), pic_provider, SLOT(RefreshObj(QVector<cv::Mat>)));
   worker.Start();
    return app.exec();
}
