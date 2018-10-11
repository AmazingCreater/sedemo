#include "QPicProvider.h"

QPicProvider::QPicProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{
  objs_.resize(5);
  this->moveToThread(&thread_);
  thread_.start();
}
QPicProvider::~QPicProvider(){
  thread_.quit();
}
QImage QPicProvider::requestImage(const QString &id, QSize *size, const QSize& requestedSize)
{
    if (id == "detail") {
      return detail_;
    } else {
      auto str = id.toStdString();
      if (str.find("obj") == 0) {
        int obj_id = str[3] - '0';
        return objs_[obj_id];
      }
    }
    return QImage();
}
#include <QDebug>
void QPicProvider::RefreshDetail(cv::Mat img) {
    QImage t(reinterpret_cast<uchar*>(img.data), img.cols, img.rows, QImage::Format_RGB888);
    detail_ = t.copy();
}
void QPicProvider::RefreshObj(cv::Mat img, int id) {
    QImage t(reinterpret_cast<uchar*>(img.data), img.cols, img.rows, QImage::Format_RGB888);
    if (id < 0) id = 0;
    if (id > 4) id = 4;
    objs_[id] = t.copy();
}
