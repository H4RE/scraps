#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <clahe.hpp>

int main(int argc, char **argv)
{
    const std::string path = argv[1];
    const cv::Mat src = cv::imread(path, cv::IMREAD_GRAYSCALE);

    const cv::Size tiles(7, 13);
    const float clip_limit = 4.0;

    const cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clip_limit, tiles);
    cv::Mat dest_cv;
    clahe->apply(src, dest_cv);

    cv::Mat dest = cvu::clahe(src, tiles, clip_limit);

    cv::imshow("src", src);
    cv::imshow("dest", dest);
    cv::imshow("dest_cv", dest_cv);

    cv::Mat diff;
    cv::absdiff(dest, dest_cv, diff);
    cv::imshow("diff", diff * 10);
    cv::imwrite("dest.png", dest);
    cv::imwrite("dest_cv.png", dest_cv);
    cv::waitKey();
    return 0;
}