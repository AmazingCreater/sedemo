/***********************************************************************
 * Copyright 2018 cambricon Inc.
 ***********************************************************************/
#include <iostream>
#include <string>
#include "cnstreamworker.h"
#include "detection_post_proc.h"
#include "displayer.h"
#include "ssd_pre_proc.h"

void CnstreamWorker::Start() {
  std::string video_path = "/home/dell/Desktop/cnstream-sedemo/resources/cars.mp4";
  std::string model_path = "/home/dell/Desktop/resnet34ssd/resnet34ssd_fp16_sparse_optimized_70to80.cambricon";
  std::string label_path = "/home/dell/Desktop/cnstream-sedemo/resources/label_voc.txt";
  double threshold = 0.6;
  int nm_chn = 32;
  mrtcxx::MluMode mode = mrtcxx::MLU_MODE_UNION2;
  int batch_size = 0;
  if (mrtcxx::MLU_MODE_UNION2 == mode) {
    batch_size = 8;
  } else if (mrtcxx::MLU_MODE_BLOCK == mode) {
    batch_size = 1;
  }
  try {
    // create decoder
    cnstream::Decoder::Attr d_attr;
    d_attr.src_frame_rate = 26;
    d_attr.dst_frame_rate = 26;
    d_attr.target_w = 1088;
    d_attr.target_h = 720;
    auto pdecoder = cnstream::Decoder::Create(d_attr, 2, batch_size);
    // create pre proc
    int mean_value[3] = {104, 117, 123};
    auto ppre_proc = SsdPreProc::Create({pdecoder->outputs()[0]},
        mean_value,
        300, 300,
        mrtcxx::DataType::FLOAT32);
    // create inferencer
    auto pinferencer = cnstream::Inferencer::Create(model_path,
        "subnet0", ppre_proc->outputs()[0],
        {mrtcxx::DataType::FLOAT32},
        {mrtcxx::NCHW}, {mrtcxx::NCHW}, mode);
    // create displayer
    auto pdisplayer = Displayer::Create(26, &detection::SsdPostProc, this,
        {pdecoder->outputs()[1], pinferencer->outputs()[0]});
    // add module
    pipeline_.AddModule(pdecoder);
    pipeline_.AddModule(ppre_proc);
    pipeline_.AddModule(pinferencer);
    pipeline_.AddModule(pdisplayer);
    // add video
    for (int i = 0; i < nm_chn; ++i) {
      pipeline_.AddVideo(video_path);
    }
    // init post proc
    detection::init(label_path, threshold);
    // change channel signal
    QObject::connect(this, SIGNAL(sig_change_chn(int)), pdisplayer.get(), SLOT(ChangeChn(int)));
    // start pipeline
    pipeline_.Start();
  } catch (cnstream::CnstreamError& e) {
    std::cerr << e.what() << std::endl;
  }
}

void CnstreamWorker::Stop() {
  pipeline_.Stop();
}

void CnstreamWorker::ChangeChn(int chn) {
  emit sig_change_chn(chn);
}

