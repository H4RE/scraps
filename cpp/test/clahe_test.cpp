#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include <clahe.hpp>
int argc_test;
char ** argv_test;

TEST(DiffFromCVClahe, clipLimit0)
{
    const cv::Size tiles(8, 8);
    const float clip_limit = 0.f;
    // cv::Mat src = cv::Mat::zeros(cv::Size(512, 512), CV_8UC1);
    cv::Mat src = cv::imread(argv_test[1], cv::IMREAD_GRAYSCALE);

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clip_limit, tiles);
    cv::Mat dest_cv;
    clahe->apply(src, dest_cv);

    cv::Mat dest = cvu::clahe(src, tiles, clip_limit);

    const auto psnr = cv::PSNR(src, src);
    EXPECT_GT(psnr, 100.0);
}
int main (int argc, char * argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    argc_test = argc;
    argv_test = argv;

    return RUN_ALL_TESTS();
}