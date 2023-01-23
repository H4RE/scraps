#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <opencv2/opencv.hpp>

cv::Mat contrast_limited_adaptive_histgram_equalization(const cv::Mat_<uchar> &src, cv::Size tile_size = cv::Size(8, 8), int clip_limit = 2)
{
    cv::Mat_<uchar> dest = cv::Mat_<uchar>::zeros(src.size());
    cv::Mat_<uchar> src_ex;
    {
        const cv::Size remain = cv::Size(src.cols % tile_size.width, src.rows % tile_size.height);
        int bottom_ex = remain.height > 0 ? tile_size.height - remain.height : 0;
        int right_ex = remain.width > 0 ? tile_size.width - remain.width : 0;
        cv::copyMakeBorder(src, src_ex, 0, bottom_ex, 0, right_ex, cv::BORDER_REFLECT101);
    }
    const cv::Size tile_num = cv::Size(src_ex.cols / tile_size.width, src_ex.rows / tile_size.height);

    std::vector<std::array<uchar, 256>> hist_vec;

    // calc histgram on each tiles.
    for (int y = 0; y < src_ex.rows; y+=tile_size.height)
    {
        for (int x = 0; x < src_ex.cols; x += tile_size.width)
        {
            std::array<uchar, 256> hist{};
            for (int ty = 0; ty < tile_size.height; ty++)
            {
                for (int tx = 0; tx < tile_size.width; tx++)
                {
                    const cv::Point p(x + tx, y + ty);
                    hist.at((int)src_ex(p))++;
                }
            }
            hist_vec.emplace_back(hist);
        }
    }
    // redestribute
    for (auto &hist : hist_vec)
    {
        int total = 0;
        for (int i = 0; i < hist.size(); i++)
        {
            if (hist[i] > clip_limit)
            {
                total += hist[i] - clip_limit;
                hist[i] = clip_limit;
            }
        }
        const int n = total / hist.size();
        for (int i = 0; i < hist.size(); i++)
        {
            hist[i] += n;
        }
        int m = total - n * hist.size();
        if (m != 0)
        {
            const int step = std::max((int)hist.size() / m, 1);
            for (int i = 0; i < hist.size() && m > 0; i += step, m--)
            {
                hist[i]++;
            }
        }
    }

    // calc look up table
    std::vector<std::array<uchar, 256>> lut_vec{};
    for (auto &hist : hist_vec)
    {
        float acc = 0.f;
        std::array<uchar, 256> lut{};
        for (int i = 0; i < hist.size(); i++)
        {
            acc += hist[i];
            lut[i] = cv::saturate_cast<uchar>((hist.size() - 1) * acc / (float)tile_size.area());
        }
        lut_vec.emplace_back(lut);
    }

    // bilinear interpolation
    for (int y = 0; y < src_ex.rows; y+=tile_size.height)
    {
        for (int x = 0; x < src_ex.cols; x+=tile_size.width)
        {
            for (int ty = 0; ty < tile_size.height; ty++)
            {
                for (int tx = 0; tx < tile_size.width; tx++)
                {
                    const cv::Point p(x + tx, y + ty);
                    if (p.x < src.cols && p.y < src.rows)
                    {
                        float v = ((float)p.x) / tile_size.width - 0.5f;//
                        int left = std::floor(v);
                        int right = left + 1;
                        const float ax = v - left;
                        const float bx = 1.f - ax;
                        right = std::min(right, tile_num.width - 1);
                        left = std::max(left, 0);

                        v = ((float)p.y) / tile_size.height - 0.5f;//
                        int top = std::floor(v);
                        int bottom = top + 1;
                        const float ay = v - top;
                        const float by = 1.f - ay;
                        bottom = std::min(bottom, tile_num.height - 1);
                        top = std::max(top, 0);

                        const int idx = (int)src_ex(p);
                        const int ofs = tile_num.width;
                        const float pf = (lut_vec[top * ofs + right][idx] * ax + lut_vec[top * ofs + left][idx] * bx) * by
                        + (lut_vec[bottom * ofs + right][idx] * ax + lut_vec[bottom * ofs + left][idx] * bx) * ay;
                        dest(p) = cv::saturate_cast<uchar>(pf);
                    }
                }
            }
        }
    }
    return dest;
}

const auto clahe_ = contrast_limited_adaptive_histgram_equalization;

int main(int argc, char **argv)
{
    const std::string path = argv[1];
    const cv::Mat_<uchar> src = cv::imread(path, cv::IMREAD_GRAYSCALE);

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setTilesGridSize(cv::Size(8, 8));
    clahe->setClipLimit(0.f);
    cv::Mat dest_cv;
    clahe->apply(src, dest_cv);

    cv::Mat dest = clahe_(src, cv::Size(8, 8), 1);


    cv::imshow("src", src);
    cv::imshow("dest", dest);
    cv::imshow("dest_cv", dest_cv);

    cv::Mat diff;
    cv::absdiff(dest, dest_cv, diff);
    cv::imshow("diff", diff * 10);
    cv::waitKey();
    return 0;
}