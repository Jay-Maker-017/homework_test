#pragma once
#include <opencv2/core.hpp>

// depth 非正时 valid 为 false，此时 uv 无意义。
struct Reprojection {
    cv::Mat_<double> uv;  // 像素坐标 (2x1)
    double depth = 0.0;   // 相机坐标系下的深度 Z
    bool valid = false;
};

// Pc = R * Pw + t，R 为 3x3，t 为 3x1。
// K 为 3x3 内参矩阵
// 深度非正时不产生投影，valid 置 false
Reprojection Reproject(const cv::Mat_<double>& Pw,
                       const cv::Mat_<double>& K,
                       const cv::Mat_<double>& R,
                       const cv::Mat_<double>& t);

// 两个像素坐标之间的欧氏距离
double PixelDistance(const cv::Mat_<double>& uv, const cv::Mat_<double>& uv_obs);
