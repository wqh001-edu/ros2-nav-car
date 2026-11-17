# ROS2 自主导航智能小车 (ROS2 Autonomous Navigation Car)

本项目是一个采用**“算力中枢、车载交互与硬实时运动控制”物理三层解耦架构**的开源移动机器人系统。  
专为高校嵌入式竞赛、ROS2 移动机器人实训以及个人极客自主研发打造，提供全栈可直接复现的软硬件工程。

---

## 📸 系统架构与核心分工

```mermaid
flowchart TD
    subgraph S1["感知层 (Perception)"]
        LiDAR["激光雷达 (RPLIDAR/LD19)\n环境扫描与测距"]
        Cam["深度相机 (可选)\n三维点云避障"]
    end

    subgraph S2["大脑中枢 (泰山派 RK3576)"]
        ROS2["Ubuntu 22.04 LTS + ROS2 Humble"]
        SLAM["Slam-Toolbox 异步在线建图"]
        Nav2["Nav2 路径规划与自主避障"]
    end

    subgraph S3["HMI 车机中控 (瑞芯微 RK3566)"]
        Screen["触摸液晶中控屏"]
        LVGL["LVGL 8.3/9.x 车机界面 (56 FPS)"]
    end

    subgraph S4["下位机运动控制 (STM32F407)"]
        PID["速度双闭环增量式 PID"]
        Encoder["正交编码器测速与里程计解算"]
        Motor["双路减速电机驱动底盘"]
    end

    LiDAR -->|USB 串口| ROS2
    ROS2 <-->|ROS2 DDS 话题 /map, /odom| LVGL
    ROS2 <-->|自定义二进制串口帧 /cmd_vel, /odom| STM32F407
    STM32F407 --> Motor
    LVGL --> Screen
```

### 1. 为什么采用三层解耦架构？
* **大脑层（泰山派 RK3576）**：四核 A72 + 四核 A53，集成 6 TOPS NPU。负责计算密集型的激光雷达 SLAM 建图、膨胀代价地图与 Nav2 路径寻优算法，保障导航响应迅速。
* **交互中控层（瑞芯微 RK3566）**：搭载 Mali-G52 GPU，运行轻量级图形引擎 **LVGL**。专职渲染类似新能源汽车的中控仪表盘与实时 2D 栅格地图，界面帧率稳定在 **50+ FPS**，完全不消耗大脑导航的 CPU 资源。
* **底层运动控制（STM32F407）**：ARM Cortex-M4 硬实时微控制器，负责高频 100Hz 编码器测速、电机增量式 PID 闭环调速，确保小车走直线平稳、转角精确。

---

## 🛠️ 硬件清单 (BOM)

| 部件名称 | 推荐型号/规格 | 数量 | 作用说明 |
| :--- | :--- | :---: | :--- |
| **主控大脑** | 泰山派 RK3576 开发板 (4G/8G RAM) | 1 | 运行 ROS2、SLAM 及 Nav2 |
| **交互车机** | 瑞芯微 RK3566 开发板 + 7寸/10.1寸触摸屏 | 1 | 驱动 LVGL 车载中控与地图展示 |
| **下位机主板** | STM32F407ZGT6 / VET6 核心板 | 1 | 电机双闭环控制与编码器采集 |
| **激光雷达** | 思岚 RPLIDAR A1/C1 或 乐动 LD19 | 1 | 360 度室内环境扫描建图 |
| **电机底盘** | 12V 1:30 霍尔编码器减速电机 + 两轮差速机械底盘 | 1 | 动力驱动与底盘运动学支撑 |
| **电机驱动板** | TB6612FNG 或 A4950 驱动模块 | 1 | 驱动双路直流电机 |
| **动力电源** | 3S 航模动力锂电池 (11.1V~12.6V) + DC-DC 降压模块 | 1 | 全系统稳压与动力供给 |

---

## 📂 项目工程目录

```text
ros2-nav-car/
├── README.md                      # 项目主文档
├── docs/                          # 详细硬件接线图与标定手册
│   └── hardware_wiring.md         # 硬件引脚分配与供电方案
├── firmware_stm32/                # 下位机固件源码 (STM32F407)
│   ├── Inc/                       # 头文件 (pid.h, encoder.h, motor.h, protocol.h)
│   └── Src/                       # 电机驱动、正交编码器、PID与串口通信实现
├── ros2_ws/                       # 大脑主控 ROS2 Humble 工作空间
│   └── src/
│       ├── car_bringup/           # 底盘串口驱动节点、一键启动脚本、udev 规则
│       ├── car_description/       # 机器人 URDF 物理模型与 TF 树
│       ├── car_slam/              # Slam-Toolbox 建图配置文件
│       └── car_navigation/        # Nav2 代价地图、路径规划器配置
├── hmi_lvgl/                      # 车机交互工程 (RK3566 / LVGL)
│   ├── src/                       # LVGL 车载仪表盘与 2D 栅格地图绘制组件
│   └── CMakeLists.txt             # Linux 平台构建脚本
└── scripts/                       # 自动化脚本
    └── setup_env.sh               # Ubuntu 22.04 一键安装依赖脚本
```

---

## 🚀 快速上手复现指南

### 1. 下位机固件烧录 (STM32)
1. 使用 Keil MDK 或 STM32CubeIDE 打开 `firmware_stm32/` 工程。
2. 按照 [docs/hardware_wiring.md](docs/hardware_wiring.md) 完成电机驱动与编码器引脚接线。
3. 编译并使用 ST-Link / J-Link 将程序烧录至 STM32F407 核心板。

### 2. 泰山派大脑环境配置
在泰山派 RK3576 (Ubuntu 22.04) 终端执行一键部署脚本：
```bash
cd scripts
chmod +x setup_env.sh
./setup_env.sh
```

### 3. 编译大脑 ROS2 工作空间
```bash
cd ros2_ws
colcon build --symlink-install
source install/setup.bash
```

### 4. 运行建图与自主导航
* **终端 1：唤醒小车底盘与激光雷达**
  ```bash
  ros2 launch car_bringup bringup.launch.py
  ```
* **终端 2：启动 SLAM 在线建图**
  ```bash
  ros2 launch car_slam online_async_slam.launch.py
  ```
* **终端 3：保存地图并启动自主避障导航**
  ```bash
  # 保存建好的地图
  ros2 run nav2_map_server map_saver_cli -f ~/my_map
  # 启动 Nav2 自主导航
  ros2 launch car_navigation navigation.launch.py map:=~/my_map.yaml
  ```

---

## 📄 开源许可证

本项目基于 [Apache License 2.0](LICENSE) 协议开源，欢迎 Star、Fork 并提交 Pull Request！
