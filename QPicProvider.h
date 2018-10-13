#pragma once

#include <QQuickImageProvider>
#include <QVector>
#include <QList>
#include <QThread>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <mutex>

class QPicProvider : public QObject, public QQuickImageProvider
{
Q_OBJECT
public:
    QPicProvider();
    ~QPicProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize);
public Q_SLOTS:
    void RefreshDetail(cv::Mat img);
    void RefreshObj(QVector<cv::Mat> imgs);
private:
    QImage detail_;
    QList<QImage> objs_;
    QThread thread_;
    std::mutex detail_lock_, obj_lock_;
};
