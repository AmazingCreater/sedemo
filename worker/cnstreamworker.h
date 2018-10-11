/***********************************************************************
 * Copyright 2018 cambricon Inc.
 ***********************************************************************/
#ifndef CNSTREAMWORKER_H
#define CNSTREAMWORKER_H
#include <QVector>
#include <QObject>
#include <opencv2/core/core.hpp>
#include <cnstream/cnstream.hpp>
class CnstreamWorker : public QObject {
  Q_OBJECT
 public:
  void Start();

 public Q_SLOTS:
  void Stop();
  void ChangeChn(int chn);

 Q_SIGNALS:
  void sig_refresh(cv::Mat img);
  void sig_change_chn(int chn);
  void sig_refresh_detail(cv::Mat img);
  void sig_refresh_obj(cv::Mat img, int id);

 private:
  cnstream::Pipeline pipeline_;
};  // class CnstreamWorker
#endif  // CNSTREAMWORKER_H

