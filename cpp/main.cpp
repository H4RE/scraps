#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <opencv2/opencv.hpp>

namespace cvu
{
    cv::Mat contrast_limited_adaptive_histgram_equalization(const cv::Mat_<uchar> &src, cv::Size tile = cv::Size(8, 8), float clip_limit = 2.f)
    {
        cv::Mat_<uchar> dest = cv::Mat_<uchar>::zeros(src.size());
        cv::Mat_<uchar> src_ex;
        {
            const cv::Size remain = cv::Size(src.cols % tile.width, src.rows % tile.height);
            int bottom_ex = remain.height > 0 ? tile.height - remain.height : 0;
            int right_ex = remain.width > 0 ? tile.width - remain.width : 0;
            cv::copyMakeBorder(src, src_ex, 0, bottom_ex, 0, right_ex, cv::BORDER_REFLECT101);
        }
        cv::Size tile_size = cv::Size(src_ex.cols / tile.width, src_ex.rows / tile.height);
        int clip_limit_i = static_cast<int>((clip_limit * tile_size.area()) / 256);
        clip_limit_i = std::max(clip_limit_i, 1);
        std::cout << clip_limit_i << "\n";

        std::vector<std::array<int, 256>> hist_vec;

        // calculate histgram on each tiles.
        for (int y = 0; y < src_ex.rows; y += tile_size.height)
        {
            for (int x = 0; x < src_ex.cols; x += tile_size.width)
            {
                std::array<int, 256> hist{};
                for (int ty = 0; ty < tile_size.height; ty++)
                {
                    for (int tx = 0; tx < tile_size.width; tx++)
                    {
                        hist.at(src_ex(y + ty, x + tx))++;
                    }
                }
                hist_vec.emplace_back(hist);
            }
        }
        // redestribute clipped pixels.
        if(clip_limit>0)
        for (auto &hist : hist_vec)
        {
            int total = 0;
            for (int i = 0; i < hist.size(); i++)
            {
                if (hist.at(i) > clip_limit_i)
                {
                    total += hist.at(i) - clip_limit_i;
                    hist.at(i) = clip_limit_i;
                }
            }
            const int n = total / hist.size();
            for (int i = 0; i < hist.size(); i++)
            {
                hist.at(i) += n;
            }
            int m = total - n * hist.size();
            if (m != 0)
            {
                const int step = std::max((int)hist.size() / m, 1);
                for (int i = 0; i < hist.size() && m > 0; i += step, m--)
                {
                    hist.at(i)++;
                }
            }
        }

        // calculate look up table
        std::vector<std::array<int, 256>> lut_vec;
        for (auto &hist : hist_vec)
        {
            int acc = 0;
            std::array<int, 256> lut{};
            for (int i = 0; i < hist.size(); i++)
            {
                acc += hist.at(i);
                lut.at(i) = cv::saturate_cast<uchar>(((hist.size() - 1.f) * acc) / tile_size.area());
            }
            lut_vec.emplace_back(lut);
        }

        // bilinear interpolation
        for (int y = 0; y < src_ex.rows; y++)
        {
            for (int x = 0; x < src_ex.cols; x++)
            {
                const cv::Point p(x, y);
                if (p.x < src.cols && p.y < src.rows)
                {
                    float vx = ((float)p.x) / tile_size.width - 0.5f; //
                    int left = std::floor(vx);
                    int right = left + 1;
                    float ax = vx - left;
                    float bx = 1.f - ax;
                    right = std::min(right, tile.width - 1);
                    left = std::max(left, 0);

                    float vy = ((float)p.y) / tile_size.height - 0.5f; //
                    int top = std::floor(vy);
                    int bottom = top + 1;
                    float ay = vy - top;
                    float by = 1.f - ay;
                    bottom = std::min(bottom, tile.height - 1);
                    top = std::max(top, 0);

                    const auto top_left = lut_vec[top * tile.width + left];
                    const auto top_right = lut_vec[top * tile.width + right];
                    const auto bottom_left = lut_vec[bottom * tile.width + left];
                    const auto bottom_right = lut_vec[bottom * tile.width + right];

                    const auto i = src_ex(p);
                    const float pixf = (top_right.at(i) * ax + top_left.at(i) * bx) * by + (bottom_right.at(i) * ax + bottom_left.at(i) * bx) * ay;
                    dest(p) = cv::saturate_cast<uchar>(pixf);
                }
            }
        }
        return dest;
    } // contrast_limited_adaptive_histgram_equalization
    const auto clahe = contrast_limited_adaptive_histgram_equalization;
} // namespace

int main(int argc, char **argv)
{
    const std::string path = argv[1];
    cv::Mat_<uchar> src = cv::imread(path, cv::IMREAD_GRAYSCALE);

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(4, cv::Size(8, 8));
    cv::Mat dest_cv;
    clahe->apply(src, dest_cv);

    cv::Mat dest = cvu::clahe(src, cv::Size(8,8), 4);

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