#pragma once
#include <opencv2/core.hpp>

// 重投影结果。
// depth 非正时 valid 为 false，此时 uv 无意义。
struct Reprojection {
    cv::Mat_<double> uv;  // 像素坐标 (2x1)
    double depth = 0.0;   // 相机坐标系下的深度 Z
    bool valid = false;
};

// 理想针孔模型重投影（忽略畸变）。
// 外参约定为世界到相机的变换: Pc = R * Pw + t，R 为 3x3，t 为 3x1。
// K 为 3x3 内参矩阵。三维坐标与平移量使用相同长度单位。
// 深度非正（点位于相机后方或落在成像平面上）时不产生实投影像，valid 置为 false。
Reprojection Reproject(const cv::Mat_<double>& Pw,
                       const cv::Mat_<double>& K,
                       const cv::Mat_<double>& R,
                       const cv::Mat_<double>& t);

// 两个像素坐标之间的欧氏距离，单位: 像素
double PixelDistance(const cv::Mat_<double>& uv, const cv::Mat_<double>& uv_obs);
