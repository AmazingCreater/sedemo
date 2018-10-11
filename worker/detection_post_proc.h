#ifndef _DETECTION_POST_PROC_HPP_
#define _DETECTION_POST_PROC_HPP_

#include <string>
#include <opencv2/core/core.hpp>
#include <cnstream/mrtcxx_types.h>
#include <deepsort/deepsort.h>

namespace detection {
void init(const std::string& label_filename, float threshold);
void SsdPostProc(cv::Mat& img,
    DS_DetectObjects &detect_objects,
    const std::vector<float*> &nn_output_ptrs,
    const std::vector<mrtcxx::Shape> &nn_output_shapes);
}  // namespace detection

#endif // _DETECTION_POST_PROC_HPP_
