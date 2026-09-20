#include <opencv2/opencv.hpp>
#include <iostream>

//   输入 三维点 Pw、相机内参 K、世界到相机的外参 (Pc = R * Pw + t)，
//   输出 二维像素坐标，并计算与对应观测点之间的像素欧氏距离；
//   需处理非正深度等异常情况。
int main() {
    std::cout << "OpenCV 版本: " << CV_VERSION << '\n';

    const cv::Mat_<double> K =
            (cv::Mat_<double>(3, 3) << 800.0, 0.0, 320.0, 0.0, 800.0, 240.0, 0.0, 0.0, 1.0);
        std::cout << "内参矩阵 K =\n" << K << '\n';


    cv::Mat_<double> Pw = 
        (cv::Mat_<double>(3, 1) << 1.0, 0.0, 0.0);  //x,y,z
    std::cout << "世界坐标系下的三维空间点 Pw =\n" << Pw << '\n';

    const cv::Mat_<double> R =
        (cv::Mat_<double>(3, 3) << 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
    std::cout << "世界到相机的旋转矩阵 R =\n" << R << '\n';

    const cv::Mat_<double> t =
        (cv::Mat_<double>(3, 1) << 0.1, 0.2, 0.3);
    std::cout << "世界到相机的平移向量 t =\n" << t << '\n';

    const cv::Mat_<double> Pc = R * Pw + t ;
    std::cout << "相机坐标系下的三维点 Pc =\n" << Pc << '\n';
    


    return 0;
}