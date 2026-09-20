#include "reprojection.hpp"
#include <iostream>

//歪比巴卜

// 构造数据验证重投影: 输入三维点 Pw、相机内参 K、世界到相机的已知外参 (Pc = R * Pw + t),
// 输出二维像素坐标, 并计算与对应观测点之间的像素欧氏距离。
// 重投影算法与非正深度的异常处理见 include/reprojection.hpp 与 src/reprojection.cpp。
int main() {
    std::cout << "OpenCV 版本: " << CV_VERSION << '\n';

    // 相机内参 K, fx = fy = 800, 主点 (cx, cy) = (320, 240)
    const cv::Mat_<double> K =
        (cv::Mat_<double>(3, 3) << 800.0, 0.0, 320.0, 0.0, 800.0, 240.0, 0.0, 0.0, 1.0);
    std::cout << "内参矩阵 K =\n" << K << '\n';

    // 世界坐标系下的三维点 (x, y, z)
    const cv::Mat_<double> Pw = (cv::Mat_<double>(3, 1) << 1.0, 0.0, 0.0);
    std::cout << "世界坐标系下的三维空间点 Pw =\n" << Pw << '\n';

    // 世界到相机的旋转矩阵 R
    const cv::Mat_<double> R =
        (cv::Mat_<double>(3, 3) << 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
    std::cout << "世界到相机的旋转矩阵 R =\n" << R << '\n';

    // 世界到相机的平移向量 t
    const cv::Mat_<double> t = (cv::Mat_<double>(3, 1) << 0.1, 0.2, 0.3);
    std::cout << "世界到相机的平移向量 t =\n" << t << '\n';

    // 重投影求像素坐标
    const Reprojection reproj = Reproject(Pw, K, R, t);
    if (!reproj.valid) {
        std::cerr << "错误: 深度 Z = " << reproj.depth << " 非正, 该点无法重投影\n";
        return 1;
    }
    std::cout << "重投影得到的像素坐标 uv =\n" << reproj.uv << '\n';

    // 对应的观测像素坐标
    const cv::Mat_<double> uv_obs = (cv::Mat_<double>(2, 1) << 3250.0, 780.0);
    std::cout << "观测到的像素坐标 uv_obs =\n" << uv_obs << '\n';

    // 与对应观测点之间的像素欧氏距离
    const double err = PixelDistance(reproj.uv, uv_obs);
    std::cout << "像素欧氏距离 err = " << err << " px\n";

    return 0;
}