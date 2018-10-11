#pragma once

#include <QQuickImageProvider>
#include <QVector>
#include <QThread>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class QPicProvider : public QObject, public QQuickImageProvider
{
Q_OBJECT
public:
    QPicProvider();
    ~QPicProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize);
public Q_SLOTS:
    void RefreshDetail(cv::Mat img);
    void RefreshObj(cv::Mat img, int id);
private:
    QImage detail_;
    QVector<QImage> objs_;
    QThread thread_;
};
