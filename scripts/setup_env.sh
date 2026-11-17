#!/bin/bash
# ==============================================================
# ROS2 自主导航小车大脑环境一键部署脚本 (适用于 Ubuntu 22.04 LTS)
# ==============================================================

set -e

echo "[+] 1. 更新系统软件包源..."
sudo apt update && sudo apt upgrade -y

echo "[+] 2. 安装基础开发工具与编译依赖..."
sudo apt install -y curl gnupg2 lsb-release build-essential git cmake python3-pip

echo "[+] 3. 配置 ROS2 官方源 (Humble LTS)..."
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null

sudo apt update
echo "[+] 4. 安装 ROS2 Humble 桌面版及导航相关依赖..."
sudo apt install -y ros-humble-desktop \
                    ros-humble-navigation2 \
                    ros-humble-nav2-bringup \
                    ros-humble-slam-toolbox \
                    ros-humble-robot-localization \
                    ros-humble-tf2-tools \
                    ros-humble-rplidar-ros

echo "[+] 5. 配置串口 udev 规则..."
sudo cp ../ros2_ws/src/car_bringup/udev/99-car-serial.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger
sudo usermod -aG dialout $USER

echo "[+] 6. 初始化工作空间环境..."
echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc
echo "source $(pwd)/../ros2_ws/install/setup.bash" >> ~/.bashrc

echo "[✓] 环境安装完成！请重新打开终端或执行 source ~/.bashrc"
