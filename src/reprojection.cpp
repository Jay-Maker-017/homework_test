#include "reprojection.hpp"

Reprojection Reproject(const cv::Mat_<double>& Pw,
                       const cv::Mat_<double>& K,
                       const cv::Mat_<double>& R,
                       const cv::Mat_<double>& t) {
    // 世界坐标系 -> 相机坐标系
    const cv::Mat_<double> Pc = R * Pw + t;

    Reprojection result;
    result.depth = Pc(2);

    // 深度非正异常情况检查
    if (result.depth <= 0.0) {
        return result;
    }

    // 归一化平面坐标 (理想针孔模型, 忽略畸变)
    const double x = Pc(0) / result.depth;
    const double y = Pc(1) / result.depth;

    // 像素坐标: u = fx * x + cx, v = fy * y + cy
    result.uv = (cv::Mat_<double>(2, 1) << K(0, 0) * x + K(0, 2),
                    K(1, 1) * y + K(1, 2));
    result.valid = true;
    return result;
}

double PixelDistance(const cv::Mat_<double>& uv, const cv::Mat_<double>& uv_obs) {
    // cv::norm 默认即 L2 范数, 对 (Δu, Δv) 求模得到像素欧氏距离
    return cv::norm(uv - uv_obs);
}
