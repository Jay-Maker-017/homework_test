#include <opencv2/opencv.hpp>

#include <iostream>

// TODO(作业): 下面是验证「头文件能否找到 + 库能否链接」的占位 main。
// 实现重投影时请替换掉它：
//   输入 三维点 Pw、相机内参 K、世界到相机的外参 (Pc = R * Pw + t)，
//   输出 二维像素坐标，并计算与对应观测点之间的像素欧氏距离；
//   需处理非正深度等异常情况。
int main() {
    std::cout << "OpenCV 版本: " << CV_VERSION << '\n';

    const cv::Mat_<double> K =
        (cv::Mat_<double>(3, 3) << 800.0, 0.0, 320.0, 0.0, 800.0, 240.0, 0.0, 0.0, 1.0);
    std::cout << "内参矩阵 K =\n" << K << '\n';

    return 0;
}