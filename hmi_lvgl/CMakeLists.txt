cmake_minimum_required(VERSION 3.10)
project(car_hmi_lvgl C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# 支持直接在 Linux / 树莓派 / 瑞芯微 RK3566 / RK3576 上交叉编译与运行
include_directories(
    src
)

add_executable(car_hmi_app
    src/ui_dashboard.c
)

# 目标安装
install(TARGETS car_hmi_app DESTINATION bin)
