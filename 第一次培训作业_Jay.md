# 27赛季算法组视觉侧第一次培训作业报告_林义杰

## 一、开发环境、坐标系约定和重投影原理

**开发环境**

| 项目 | 版本 / 说明 |
| --- | --- |
| 操作系统 | Ubuntu 22.04（VMware 虚拟机） |
| 编译器 | g++ 11.4.0 |
| 构建工具 | CMake 3.22.1 + GNU Make 4.3（`Unix Makefiles` 生成器） |
| 依赖库 | OpenCV 4.5.4，由 apt 安装（**没使用 snap**） |
| 编辑器 | VS Code + clangd |

选择 OpenCV 而非手写矩阵运算，是因为 `cv::Mat_<double>` 能直接把 `R * Pw + t` 写成与公式一一对应的表达式，方便。

**坐标系约定**

- 世界坐标系 $O_w$ 与相机坐标系 $O_c$ 均为右手系，相机系 $x$ 轴向右、$y$ 轴向下、$z$ 轴沿光轴指向前方。
- 外参约定为**世界到相机**的变换：

$$P_c = R\,P_w + t$$

其中 $R$ 为 $3\times3$ 旋转矩阵，$t$ 为 $3\times1$ 平移向量，$P_w$、$P_c$ 分别为空间点在世界系与相机系下的坐标。三维点坐标与平移量均以毫米（mm）为单位。

**重投影原理**

在理想针孔模型下，相机系下的点 $P_c=(X_c,Y_c,Z_c)$ 经透视投影落到归一化平面，再经内参矩阵 $K$ 缩放到像素平面，公式大概长这样：

$$
x=\frac{X_c}{Z_c},\quad y=\frac{Y_c}{Z_c}
\qquad\Longrightarrow\qquad
\begin{cases} u = f_x x + c_x \\[2pt] v = f_y y + c_y \end{cases}
$$

$$
K=\begin{bmatrix} f_x & 0 & c_x \\ 0 & f_y & c_y \\ 0 & 0 & 1 \end{bmatrix}
$$

$Z_c$ 是深度，只有 $Z_c>0$（点位于相机前方）时才有真实投影像；如果 $Z_c <= 0$ 说明点位于相机后方或恰好落在成像平面上，归一化会除零或得到无物理意义的东西，属于异常情况，需要特殊处理。

## 二、项目结构、构建运行方法、测试结果及误差分析

**项目结构**

```
CMakeLists.txt               构建脚本（C++17、OpenCV、编译警告）
CMakePresets.json            平台 preset（linux / windows）
include/reprojection.hpp     接口：Reprojection 结构体 + 两个函数声明
src/reprojection.cpp         实现：重投影算法与非正深度处理
src/main.cpp                 程序入口：构造数据、调用并打印结果
```

头文件与实现文件分离：`reprojection.hpp` 只声明数据结构与函数，`reprojection.cpp` 只包含算法实现，`main.cpp` 只负责构造数据与打印。CMake 中编译为独立静态库 `reprojection`，`include/` 目录与 OpenCV 均以 `PUBLIC` 方式传递，链接它的目标自动获得头文件搜索路径。

```cpp
add_library(reprojection src/reprojection.cpp)
target_include_directories(reprojection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(reprojection PUBLIC ${OpenCV_LIBS})
add_executable(reproject src/main.cpp)
target_link_libraries(reproject PRIVATE reprojection)
```

**构建运行方法（Linux）**

```bash
cmake --preset linux
cmake --build --preset linux
./build/reproject
```

**测试结果**

测试数据由程序构造：内参 $K$ 取 $f_x=f_y=800$ px、主点 $(c_x,c_y)=(320,240)$ px；空间点 $P_w=(1,0,0)$ mm；外参取 $R=I$、$t=(0.1,0.2,0.3)$ mm；对应观测像素坐标 $uv_{obs}=(3250,780)$。程序实际输出为：

```
重投影得到的像素坐标 uv =
[3253.333333333333;
 773.3333333333334]
观测到的像素坐标 uv_obs =
[3250;
 780]
像素欧氏距离 err = 7.45356 px
```

退出码为 0。

**结果复核（解析手算）**

为验证程序输出确实由公式推出而非巧合，下面用解析式独立手算一遍：

| 步骤 | 计算式 | 结果 |
| --- | --- | --- |
| 相机系坐标 | $P_c=P_w+t$ | $(1.1,\ 0.2,\ 0.3)$ |
| 深度 | $Z_c = 0.3$（因 $P_w$ 的 $z$ 分量为 0、$R=I$，故 $Z_c=t_z$） | $0.3$ mm |
| 归一化坐标 | $x = 1.1/0.3,\ y = 0.2/0.3$ | $(3.666667,\ 0.666667)$ |
| 像素坐标 | $u=800\times1.1/0.3+320$，$v=800\times0.2/0.3+240$ | $(3253.333333,\ 773.333333)$ |
| 残差 | $\Delta u=u-3250,\ \Delta v=v-780$ | $(+3.333333,\ -6.666667)$ px |
| 像素欧氏距离 | $\sqrt{\Delta u^2+\Delta v^2}=\sqrt{55.555556}$ | $7.453560$ px |

手算值与程序输出在全部有效数位内一致（末位差异不超过 1 ULP，源于除法与乘法先后顺序不同），说明实现与前述公式相符。

**误差分析**

1. **误差定义**。这里的"误差"是重投影点到观测点的像素欧氏距离，即 $\sqrt{\Delta u^2+\Delta v^2}$，等价于 $\|\Delta\|_2$ 范数，量纲为像素；它衡量的是一次投影残差的大小，而不是相机模型本身的精度。
2. **7.45356 px 的来源是人为构造的观测偏差，不是算法误差**。观测点 $(3250,780)$ 是自行构造的，与真值 $(3253.333333,\ 773.333333)$ 相差约 $(3.33,-6.67)$ px。该值只用来检验 `PixelDistance` 的残差计算与欧氏距离定义是否正确，不能反过来证明重投影精度。要评估真实精度，需要用标定得到的 $K$ 与真实观测点，并对噪声做统计。
3. **数值上的巧合需要留意**。$\Delta v$ 恰好为 $\Delta u$ 的 $-2$ 倍、误差恰为 $\sqrt{5}\times(1/0.3)\approx7.4536$，这源于 $f_x=f_y$、$R=I$ 且 $P_w$ 的 $z$ 分量为 0 等对称设定，并非普适结论。若换了 $R$ 或非零的 $P_w.z$，残差方向与模长都会改变。
4. **该点实际落在视场之外**。$x=3.666667$ 远大于 $1$，对应的 $u=3253$ px 已超出常见 $640\times480$ 像面，$Z_c=0.3$ mm 也远小于真实景距。这说明本组数据是**纯算术校核用例**，目的是让每个中间量都能手算复现，而不是模拟真实成像场景。
5. **浮点精度**。输出中 $u$ 打印为 `3253.333333333333`、$v$ 打印为 `773.3333333333334`，末位取整来自二进制浮点表示；独立手算得到的 $u$ 末位为 `...334`，与程序相差 1 ULP，是乘法与除法的先后顺序不同所致，量级约 $10^{-13}$ px，对像素级结果没有影响。

**非正深度的异常处理**

`reprojection.cpp` 中在求出深度后立即判断：

```cpp
result.depth = Pc(2);
if (result.depth <= 0.0) {
    return result;   // valid 保持 false，uv 不被赋值
}
```

即不抛异常、不返回 NaN，而是通过 `Reprojection::valid` 标志把"该点不可投影"这一语义显式上传给调用方；此时 `uv` 无意义，调用方必须自行检查 `valid`。`main.cpp` 中对应分支会向 `stderr` 打印深度值并以非 0 退出码结束，避免把无效结果当作有效值继续参与后续计算。

## 三、遇到的问题与解决办法

1. **`#include <opencv2/core.hpp>` 报文件不存在。** Ubuntu 把 OpenCV 4 的头文件装在 `/usr/include/opencv4`，该目录不在编译器默认搜索路径中。解决办法是不手写 `include_directories`，而是依赖 `find_package(OpenCV REQUIRED)` 导入的目标：`OpenCVConfig.cmake` 会自动把 `${OpenCV_INCLUDE_DIRS}` 随 `${OpenCV_LIBS}` 一起传播给链接它的目标，`CMakeLists.txt` 因此保持干净。
2. **编辑器里所有 OpenCV 头文件都标红。** 原因是 clangd 找不到 `compile_commands.json`，无法解析头文件路径。解决办法有两点：在 `CMakeLists.txt` 中打开 `CMAKE_EXPORT_COMPILE_COMMANDS`，并把 Linux 的 `binaryDir` 定为仓库根下的 `build/`——clangd 会自动向上级目录查找 `build/compile_commands.json`，因此无需任何配置即可生效（Windows 侧构建目录名不同，需通过本机用户设置指定 `--compile-commands-dir`）。另外，修改构建配置后诊断不会自动刷新，需重启 clangd 语言服务。
3. **非正深度该怎么处理。** 最初想直接输出 NaN 或抛异常，但前者会污染后续所有运算且难以定位，后者会让调用方必须写 `try/catch`，对视觉流水线太重。最终选择返回带 `valid` 标志的结构体，把异常情况变成返回值的一部分，调用方用 `if (!reproj.valid)` 一步判断即可，同时保留 `depth` 便于日志输出。

## 四、代码仓库链接、PR 链接和最终提交的 commit hash

- 代码仓库：`<待填写：仓库 URL>`
- PR 链接：`<待填写：PR URL>`
- 最终提交 hash：`<待填写：commit hash>`

**从零复现（Linux）**

```bash
git clone <仓库 URL>
cd <仓库目录>
cmake --preset linux
cmake --build --preset linux
./build/reproject
```

依赖仅需 `build-essential`、`cmake`、`libopencv-dev`（`sudo apt install build-essential cmake libopencv-dev`），环境要求与构建步骤详见仓库 `README.md`。
