#include <fstream>
#include <string>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <glog/logging.h>
#include "detection_post_proc.h"

namespace detection {
std::vector<std::string> g_labels;
static std::vector<cv::Scalar> g_colors;
static float g_threshold;
std::vector<std::string> LoadLabels(const std::string &filename) {
  std::vector<std::string> labels;
  std::ifstream file(filename);
  LOG_IF(FATAL, !file.is_open()) << "file:" <<
    filename << " open failed.";
  std::string line;
  while (std::getline(file, line)) {
    labels.push_back(std::string(line));
  }
  file.close();
  return labels;
}

// http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically
cv::Scalar HSV2RGB(const float h, const float s, const float v) {
  const int h_i = static_cast<int>(h * 6);
  const float f = h * 6 - h_i;
  const float p = v * (1 - s);
  const float q = v * (1 - f*s);
  const float t = v * (1 - (1 - f) * s);
  float r, g, b;
  switch (h_i) {
    case 0:
      r = v; g = t; b = p;
      break;
    case 1:
      r = q; g = v; b = p;
      break;
    case 2:
      r = p; g = v; b = t;
      break;
    case 3:
      r = p; g = q; b = v;
      break;
    case 4:
      r = t; g = p; b = v;
      break;
    case 5:
      r = v; g = p; b = q;
      break;
    default:
      r = 1; g = 1; b = 1;
      break;
  }
  return cv::Scalar(r * 255, g * 255, b * 255);
}


std::vector<cv::Scalar> GetColors(const int n) {
  std::vector<cv::Scalar> colors;
  cv::RNG rng(12345);
  const float golden_ratio_conjugate = 0.618033988749895f;
  const float s = 0.3f;
  const float v = 0.99f;
  for (int i = 0; i < n; ++i) {
    const float h = std::fmod(rng.uniform(0.0f, 1.0f) + golden_ratio_conjugate, 1.0f);
    colors.push_back(HSV2RGB(h, s, v));
  }
  return colors;
}

void init(const std::string& label_filename, float threshold) {
  g_labels = LoadLabels(label_filename);
  g_colors = GetColors(g_labels.size());
  g_threshold = threshold;
}

void SsdPostProc(cv::Mat& img, DS_DetectObjects &detect_objects, const std::vector<float*> &nn_output_ptrs,
    const std::vector<mrtcxx::Shape> &nn_output_shapes) {
  auto shape = nn_output_shapes[0];
  auto data_count = shape.nhwc();
  float* result = nn_output_ptrs[0];
  std::vector<std::vector<float>> detections;
  for (uint32_t k = 0; k < data_count / 6; ++k) {
    if (result[0] == 0 && result[1] == 0 &&
        result[2] == 0 && result[3] == 0 &&
        result[4] == 0 && result[5] == 1) {
      // Skip invalid detection.
      result += 6;
      continue;
    }
    std::vector<float> detection(7, 0);
    detection[0] = 0;
    detection[1] = result[5];
    detection[2] = result[4];
    detection[3] = result[0];
    detection[4] = result[1];
    detection[5] = result[2];
    detection[6] = result[3];
    detections.push_back(detection);
    result += 6;
  }

  DS_Rect rect;
  DS_DetectObject detect_object;

  char buffer[50];
  for (auto it : detections) {
    const int label = it[1] - 1;
    const float score = it[2];
    if (score < g_threshold) {
      continue;
    }
    // find label string
    std::string label_name = "Unknown";
    if (label < (int)g_labels.size() && label >= 0) {
      label_name = g_labels[label];
    }
    float xmin, xmax, ymin, ymax;
    xmin = it[3] * img.cols;
    ymin = it[4] * img.rows;
    xmax = it[5] * img.cols;
    ymax = it[6] * img.rows;

    rect.x = (int)xmin;
    rect.y = (int)ymin;
    rect.width = (int)(xmax - xmin);
    rect.height = (int)(ymax - ymin);

    // for mot track
    detect_object.confidence = score;
    detect_object.class_id = label;
    detect_object.rect = rect;
    detect_objects.push_back(detect_object);

    // paint
    cv::Point top_left(xmin, ymin);
    cv::Point bottom_right(xmax, ymax);
    CHECK_LT(label, g_colors.size());
    auto color = g_colors[label];
    // rectangle
    cv::rectangle(img, top_left, bottom_right, color, 3);
    cv::Point bottom_left(xmin, ymax);
    snprintf(buffer, sizeof(buffer), "%s: %.2f", label_name.c_str(),
                 score);
    std::string text = buffer;
    double scale = 0.8;
    // double scale = 0.9 - (0.6 * displayer.container()->channel_count() / 32);
    auto text_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, scale, 1, nullptr);
    cv::rectangle(
      img, bottom_left + cv::Point(0, 0),
      bottom_left + cv::Point(text_size.width, -text_size.height),
      color, CV_FILLED);
    cv::putText(img, text, bottom_left - cv::Point(0, 0),
        cv::FONT_HERSHEY_SIMPLEX, scale,
        CV_RGB(0, 0, 0), 1, 8, false);
  }
}
}  // namespace detection
