cmake_minimum_required(VERSION 3.16)

project(
    homework_test
    VERSION 0.1.0
    DESCRIPTION "27赛季算法组视觉侧第一次培训作业：针孔模型重投影"
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# 语言标准：作业要求 C++17
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 单配置生成器（Makefiles / Ninja）默认不带 CMAKE_BUILD_TYPE，这里给个默认值
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "构建类型" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# ---------------------------------------------------------------------------
# 生成 compile_commands.json，供 clangd / IntelliSense 解析头文件路径
# 只有 Makefile 与 Ninja 生成器支持该选项
# ---------------------------------------------------------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# OpenCV
# 说明：Ubuntu 把 OpenCV4 的头文件装在 /usr/include/opencv4（不在默认搜索路径），
#      OpenCVConfig.cmake 会自动为 ${OpenCV_LIBS} 带上该 include 目录，
#      因此这里不需要手写 include_directories。
# ---------------------------------------------------------------------------
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV 版本: ${OpenCV_VERSION}")
message(STATUS "OpenCV 头文件目录: ${OpenCV_INCLUDE_DIRS}")

# ---------------------------------------------------------------------------
# 可执行目标
# ---------------------------------------------------------------------------
add_executable(reproject src/main.cpp)

target_link_libraries(reproject PRIVATE ${OpenCV_LIBS})

# 只对 GNU/Clang 系编译器打开警告；MSVC 不认这些选项
target_compile_options(
    reproject
    PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
)
