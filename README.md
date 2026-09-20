# 视觉第一次培训作业（针孔模型重投影）

## 环境要求

- CMake ≥ 3.21（使用 CMakePresets；Ubuntu 22.04 自带 3.22.1）
- 支持 C++17 的编译器
- OpenCV 4.x

**Linux（Ubuntu 22.04）**

```bash
sudo apt install build-essential cmake libopencv-dev
```

Ubuntu 把 OpenCV4 头文件装在 `/usr/include/opencv4`（不在默认搜索路径），
但 `OpenCVConfig.cmake` 会自动把该目录带给目标，因此 CMake 里无需手写 `include_directories`。
不要把 OpenCV 装在非标准位置，也**不要用 snap 安装**。

**Windows（MinGW）**

需要 MinGW-w64 GCC 与 **MinGW 版 OpenCV**。
OpenCV 官方 Windows 预编译包里 `build/x64/vc16` 是 MSVC 库，GCC 链接不了，
必须用 `x64/mingw` 那一份（或 `vcpkg install opencv4[core]:x64-mingw-dynamic`）。

## 构建与运行

**Linux**

```bash
cmake --preset linux
cmake --build --preset linux
./build/reproject
```

**Windows（MinGW）**

```bat
cmake --preset windows
cmake --build --preset windows
build-windows\reproject.exe
```

查看可用 preset：`cmake --list-presets`。
`--list-presets` 只列出当前主机可用的 preset —— 在 Linux 上不会显示 `windows`，
在 Windows 上可能不会显示 `linux`，用 `cmake --list-presets=all` 可以看到全部。

## Windows 上的 OpenCV 路径

仓库里不写死别人机器上的绝对路径。若 MinGW 版 OpenCV 不在标准位置，
在仓库根目录新建 `CMakeUserPresets.json`（已被 `.gitignore` 忽略，各人各写）：

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "windows-local",
      "inherits": "windows",
      "cacheVariables": {
        "OpenCV_DIR": "D:/opencv/build/x64/mingw"
      }
    }
  ],
  "buildPresets": [
    { "name": "windows-local", "configurePreset": "windows-local" }
  ]
}
```

然后 `cmake --preset windows-local`。
用下面这条命令定位 `OpenCVConfig.cmake` 的实际位置：

```powershell
Get-ChildItem -Path D:\ -Recurse -Filter OpenCVConfig.cmake -ErrorAction SilentlyContinue
```

## 编辑器（VS Code）

- **Linux**：无需任何额外配置。CMake 配置后会在 `build/` 生成 `compile_commands.json`，
  clangd 会自动发现它，`#include <opencv2/opencv.hpp>` 的 “file not found” 随之消失。
- **Windows**：构建目录是 `build-windows/`，clangd 不会自动发现，需要在本机（Windows）的
  **用户设置**里加一行（不要写进仓库里的 `.vscode/settings.json`，那个文件会随共享目录
  带到 Linux 上，写死 Windows 路径会反过来破坏 Linux）：

  ```json
  "clangd.arguments": ["--compile-commands-dir=build-windows"]
  ```

- 改动构建配置后，诊断不会自动刷新，需在命令面板执行
  **Clangd: Restart language server**。

## 目录结构

```
CMakeLists.txt      构建脚本
CMakePresets.json   linux / windows 两套 preset
src/main.cpp        程序入口（占位 main，待实现重投影）
build/              Linux 构建目录（生成物，已忽略）
build-windows/      Windows 构建目录（生成物，已忽略）
```
