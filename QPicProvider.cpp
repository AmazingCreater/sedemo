#include "QPicProvider.h"

QPicProvider::QPicProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{
  this->moveToThread(&thread_);
  thread_.start();
}
QPicProvider::~QPicProvider(){
  thread_.exit();
  thread_.wait();
}
QImage QPicProvider::requestImage(const QString &id, QSize *size, const QSize& requestedSize)
{
    if (id == "detail") {
        detail_lock_.lock();
        auto t = detail_;
        detail_lock_.unlock();
      return t;
    } else {
      auto str = id.toStdString();
      QImage obj;
      if (str.find("obj") == 0) {
        int obj_id = str[3] - '0';
        obj_lock_.lock();
        if (objs_.size() > obj_id) {
          obj = objs_[obj_id].copy();
        }
        obj_lock_.unlock();
        return obj;
      }
    }
    return QImage();
}
void QPicProvider::RefreshDetail(cv::Mat img) {
    QImage t(reinterpret_cast<uchar*>(img.data), img.cols, img.rows, img.step, QImage::Format_RGB888);
    detail_lock_.lock();
    detail_ = t.copy();
    detail_lock_.unlock();
}
void QPicProvider::RefreshObj(QVector<cv::Mat> imgs) {
    obj_lock_.lock();
    for (auto &it : imgs) {
        QImage t(reinterpret_cast<uchar*>(it.data), it.cols, it.rows, it.step, QImage::Format_RGB888);
        if (objs_.size() > 10) {
            objs_.pop_front();
        }
        objs_.push_back(t.copy());
    }
    obj_lock_.unlock();
}

