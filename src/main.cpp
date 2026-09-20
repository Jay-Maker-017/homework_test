#include <opencv2/opencv.hpp>
#include <iostream>

//   输入 三维点 Pw、相机内参 K、世界到相机的外参 (Pc = R * Pw + t)，
//   输出 二维像素坐标，并计算与对应观测点之间的像素欧氏距离；
//   需处理非正深度等异常情况。
int main() {

    // 相机内参 K, fx = fy = 800, 主点 (cx, cy) = (320, 240)
    const cv::Mat_<double> K =
        (cv::Mat_<double>(3, 3) << 800.0, 0.0, 320.0, 0.0, 800.0, 240.0, 0.0, 0.0, 1.0);
    std::cout << "内参矩阵 K =\n" << K << '\n';


    const cv::Mat_<double> Pw =
        (cv::Mat_<double>(3, 1) << 1.0, 0.0, 0.0);  // 世界坐标系下的三维点 (x, y, z)
    std::cout << "世界坐标系下的三维空间点 Pw =\n" << Pw << '\n';

    const cv::Mat_<double> R =
        (cv::Mat_<double>(3, 3) << 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
    std::cout << "世界到相机的旋转矩阵 R =\n" << R << '\n';

    const cv::Mat_<double> t =
        (cv::Mat_<double>(3, 1) << 0.1, 0.2, 0.3);
    std::cout << "世界到相机的平移向量 t =\n" << t << '\n';

    const cv::Mat_<double> Pc = R * Pw + t;
    std::cout << "相机坐标系下的三维点 Pc =\n" << Pc << '\n';

    // 对应的观测像素坐标
    const cv::Mat_<double> uv_obs =
        (cv::Mat_<double>(2, 1) << 3250.0, 780.0);
    std::cout << "观测到的像素坐标 uv_obs =\n" << uv_obs << '\n';

    // 异常情况: 深度非正说明点位于相机后方或落在成像平面上, 不存在实的投影像
    const double Z = Pc(2);
    if (Z <= 0.0) {
        std::cerr << "错误: 深度 Z = " << Z << " 非正, 该点无法重投影\n";
        return 1;
    }

    // 归一化平面坐标 (理想针孔模型, 忽略畸变)
    const double x = Pc(0) / Z;
    const double y = Pc(1) / Z;

    // 像素坐标: u = fx * x + cx, v = fy * y + cy
    const cv::Mat_<double> uv =
        (cv::Mat_<double>(2, 1) << K(0, 0) * x + K(0, 2), K(1, 1) * y + K(1, 2));
    std::cout << "重投影得到的像素坐标 uv =\n" << uv << '\n';

    // 与观测点的像素欧氏距离
    const cv::Mat_<double> duv = uv - uv_obs;
    const double err = cv::norm(duv);
    std::cout << "像素欧氏距离 err = " << err << " px\n";

    return 0;
}