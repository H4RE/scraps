#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include <clahe.hpp>
int argc_test;
char **argv_test;

TEST(CLAHETest, AccuracyCheck)
{
    const cv::Mat src = cv::imread(argv_test[1], cv::IMREAD_GRAYSCALE);

    const cv::Ptr<cv::CLAHE> cv_clahe = cv::createCLAHE();
    for (int ty = 1; ty < 30; ty+=3)
    {
        for (int tx = 1; tx < 30; tx+=3)
        {
            const cv::Size tiles(tx, ty);

            cv_clahe->setTilesGridSize(tiles);

            for (float cl = 1.f; cl < 100.f; cl += 5.f)
            {
                cv::Mat dest_cv;
                cv_clahe->setClipLimit(cl);
                cv_clahe->apply(src, dest_cv);

                cv::Mat dest = cvu::clahe(src, tiles, cl);

                const double psnr = cv::PSNR(dest_cv, dest);
                EXPECT_GT(psnr, 50.0) << "Low accuracy: (tile, clip_limit) = (" << tiles << ", " << cl<<")";
            }
        }
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    argc_test = argc;
    argv_test = argv;

    return RUN_ALL_TESTS();
}