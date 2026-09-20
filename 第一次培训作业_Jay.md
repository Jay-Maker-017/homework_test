# 27赛季算法组视觉侧第一次培训作业报告_林义杰

## 一、开发环境、坐标系约定和重投影原理

**开发环境**

| 项目 | 版本 / 说明 |
| --- | --- |
| 操作系统 | Ubuntu 22.04（VMware 虚拟机） |
| 编译器 | g++ 11.4.0 |
| 构建工具 | CMake 3.22.1 + GNU Make 4.3 |
| 依赖库 | OpenCV 4.5.4，由 apt 安装（**没使用 snap**） |
| 编辑器 | VS Code + clangd |

选了用 OpenCV 而非手写矩阵运算，是因为 `cv::Mat_<double>` 能直接把 `R * Pw + t` 写成与公式对应的表达式，很方便。

**坐标系约定**

- 世界坐标系 $O_w$ 与相机坐标系 $O_c$ 都是右手系，相机系 $x$ 轴向右、$y$ 轴向下、$z$ 轴沿光轴指向前方。
- 外参约定为**世界到相机**的变换：

$$P_c = R\,P_w + t$$

其中 $R$ 为 $3\times3$ 旋转矩阵，$t$ 为 $3\times1$ 平移向量，$P_w$、$P_c$ 分别为空间点在世界系与相机系下的坐标。三维点坐标与平移量均以毫米为单位。

**重投影原理**

在理想针孔模型下，相机系下的点 $P_c=(X_c,Y_c,Z_c)$ 经透视投影落到"归一化平面"，再经内参矩阵 $K$ 缩放到像素平面：

$$x = Xc / Zc , y = Yc / Zc$$
$$u = fx * x + cx ; v = fy * y + cy$$
$$K = [
fx  0   cx
0   fy  cy
0   0   1
]$$


$Z_c$ 是深度，只有 $Z_c>0$（点位于相机前方）时才合理；如果 $Z_c <= 0$ 说明点位于相机后方或恰好落在成像平面上，运算过程中会除零或得到无物理意义的东西，属于异常情况，需要特殊处理。

## 二、项目结构、构建运行方法、测试结果及误差分析

**项目结构**

```
CMakeLists.txt               构建脚本（C++17、OpenCV 等等）
CMakePresets.json            平台 preset（linux / windows）(但实测 windows 貌似不一定行)
include/reprojection.hpp     Reprojection 结构体 + 两个函数声明
src/reprojection.cpp         重投影算法与非正深度处理
src/main.cpp                 程序入口：构造数据、调用并打印结果
```

头文件与实现文件分离：`reprojection.hpp` 来声明数据结构与函数，`reprojection.cpp` 来包含算法实现，`main.cpp` 来负责构造数据与打印。
CMake 把算法实现单独打包成静态库 reprojection，并用 "PUBLIC" 把 include 目录和 OpenCV 一并"继承"给它的所有使用方。


**测试结果**

测试数据提前输入程序：内参 $K$ 取 $f_x=f_y=800$ px、主点 $(c_x,c_y)=(320,240)$ px；空间点 $P_w=(1,0,0)$ mm；外参取 $R=I$、$t=(0.1,0.2,0.3)$ mm；对应观测像素坐标 $uv_{obs}=(3250,780)$。
程序实际输出为：

```
重投影得到的像素坐标 uv =
[3253.333333333333;
 773.3333333333334]
观测到的像素坐标 uv_obs =
[3250;
 780]
像素欧氏距离 err = 7.45356 px
```
感觉没什么问题


**误差分析**

1. **误差定义**。这里的"误差"是重投影点到观测点的像素欧氏距离,主要用于衡量投影残差的大小。
2. **7.45356 px 是我人为构造的，不是算法误差**。因为观测点 $(3250,780)$ 是自行构造的，与实际值有偏差。
3.好像也没有别的误差了……吧……


## 三、遇到的问题与解决办法

**主要都是配环境遇到的问题，和红色波浪线斗争了一个上午 www……**
1. **`#include <opencv2/core.hpp>` 报文件不存在。** Ubuntu 把 OpenCV 4 的头文件装在 `/usr/include/opencv4`，该目录一开始找不到。最后在AI的帮助下用 `find_package(OpenCV REQUIRED)` 解决了。
2. **编辑器里所有 OpenCV 头文件和相关的东西都标红。** 原因好像是 clangd 找不到 `compile_commands.json`，无法解析头文件路径。解决办法：在 `CMakeLists.txt` 中打开 `CMAKE_EXPORT_COMPILE_COMMANDS`，然后根据AI的说法把 Linux 的 `binaryDir` 定为仓库根下的 `build/`，这样 clangd 会自动向上级目录查找 `build/compile_commands.json`。

## 四、代码仓库链接、PR 链接和最终提交的 commit hash

- 代码仓库：`<待填写：仓库 URL>`
- PR 链接：`<待填写：PR URL>`
- 最终提交 hash：`<待填写：commit hash>`