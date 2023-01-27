#pragma once
#include <opencv2/opencv.hpp>

namespace cvu
{
    cv::Mat contrast_limited_adaptive_histogram_equalization(const cv::Mat &src, cv::Size tile = cv::Size(8, 8), float clip_limit = 2.f);
    const auto clahe = contrast_limited_adaptive_histogram_equalization;
}