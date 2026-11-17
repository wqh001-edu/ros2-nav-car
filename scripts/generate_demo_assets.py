import os
import math
import numpy as np
from PIL import Image, ImageDraw, ImageFont

OUTPUT_DIR = "d:/douyin project/ros2-nav-car/docs/images"
os.makedirs(OUTPUT_DIR, exist_ok=True)

# 基础配色定义 (深色科技车机风)
BG_COLOR = (15, 18, 25)
PANEL_BG = (23, 28, 38)
ACCENT_BLUE = (0, 180, 255)
ACCENT_GREEN = (40, 210, 120)
ACCENT_ORANGE = (255, 140, 0)
TEXT_WHITE = (240, 245, 255)
TEXT_MUTED = (140, 150, 170)
WALL_COLOR = (70, 85, 110)
GRID_COLOR = (28, 34, 48)

def draw_single_frame(t_ratio=0.5):
    """
    绘制单帧车机 UI 与自主导航状态 (t_ratio 从 0.0 到 1.0)
    """
    width, height = 1200, 700
    img = Image.new('RGB', (width, height), BG_COLOR)
    draw = ImageDraw.Draw(img)

    # 1. 顶部状态栏
    draw.rectangle([0, 0, width, 45], fill=(18, 22, 30))
    draw.text((25, 12), "ROS2 AUTONOMOUS ROBOT // HMI COCKPIT v2.4", fill=TEXT_MUTED)
    draw.text((650, 12), "IP: 192.168.12.105 (STA)", fill=TEXT_MUTED)
    draw.text((880, 12), "RK3566: 56.2 FPS | 28% CPU", fill=ACCENT_GREEN)
    draw.text((1100, 12), "12.4V [88%]", fill=ACCENT_BLUE)

    # 2. 分屏：左侧仪表中控面板 (0 ~ 460)
    left_w = 460
    draw.rounded_rectangle([20, 65, left_w, height - 25], radius=12, fill=PANEL_BG)

    # 车速表盘背景圆弧
    center_x, center_y = 240, 210
    radius = 110
    draw.arc([center_x - radius, center_y - radius, center_x + radius, center_y + radius], 
             start=135, end=405, fill=(45, 55, 75), width=14)

    # 动态车速计算 (随着路径前进而变化)
    speed = 0.32 + 0.08 * math.sin(t_ratio * math.pi * 2)
    speed_deg = 135 + (speed / 0.6) * 270
    draw.arc([center_x - radius, center_y - radius, center_x + radius, center_y + radius], 
             start=135, end=int(speed_deg), fill=ACCENT_BLUE, width=14)

    # 表盘文字
    draw.text((center_x - 55, center_y - 35), f"{speed:.2f}", fill=TEXT_WHITE)
    draw.text((center_x - 20, center_y + 15), "m/s", fill=TEXT_MUTED)
    draw.text((center_x - 35, center_y + 35), f"{speed*3.6:.1f} km/h", fill=ACCENT_BLUE)

    # 车辆状态卡片
    card_y = 350
    draw.rectangle([40, card_y, left_w - 20, card_y + 80], fill=(29, 36, 49))
    draw.text((55, card_y + 12), "MOTOR & PID STATUS", fill=TEXT_MUTED)
    draw.text((55, card_y + 35), "Left Wheel:  425 PWM | 0.31 m/s", fill=TEXT_WHITE)
    draw.text((55, card_y + 55), "Right Wheel: 422 PWM | 0.32 m/s", fill=TEXT_WHITE)

    # 导航核心节点指示灯
    nodes_y = 450
    draw.rectangle([40, nodes_y, left_w - 20, nodes_y + 130], fill=(29, 36, 49))
    draw.text((55, nodes_y + 12), "SYSTEM NODES STATUS", fill=TEXT_MUTED)
    
    statuses = [
        ("Slam-Toolbox (Async)", "ONLINE", ACCENT_GREEN),
        ("Nav2 Controller (DWB)", "ACTIVE", ACCENT_GREEN),
        ("RPLIDAR Laser (10Hz)", "STREAMING", ACCENT_GREEN),
        ("STM32 Base (100Hz PID)", "CONNECTED", ACCENT_BLUE)
    ]
    for idx, (name, st, col) in enumerate(statuses):
        y_pos = nodes_y + 38 + idx * 22
        draw.ellipse([55, y_pos + 3, 63, y_pos + 11], fill=col)
        draw.text((72, y_pos), name, fill=TEXT_WHITE)
        draw.text((360, y_pos), st, fill=col)

    # 底部快捷按钮
    btn_y = 605
    draw.rounded_rectangle([40, btn_y, 160, btn_y + 45], radius=6, fill=(35, 120, 240))
    draw.text((70, btn_y + 14), "START NAV", fill=TEXT_WHITE)
    draw.rounded_rectangle([180, btn_y, 300, btn_y + 45], radius=6, fill=(45, 55, 75))
    draw.text((215, btn_y + 14), "PAUSE", fill=TEXT_WHITE)
    draw.rounded_rectangle([320, btn_y, 440, btn_y + 45], radius=6, fill=(180, 50, 50))
    draw.text((350, btn_y + 14), "E-STOP", fill=TEXT_WHITE)

    # 3. 分屏：右侧 SLAM 栅格地图与导航区域 (480 ~ 1180)
    map_x1, map_y1 = 490, 65
    map_x2, map_y2 = width - 20, height - 25
    draw.rounded_rectangle([map_x1, map_y1, map_x2, map_y2], radius=12, fill=(18, 22, 30))

    # 地图网格线
    for gx in range(map_x1, map_x2, 40):
        draw.line([(gx, map_y1), (gx, map_y2)], fill=GRID_COLOR, width=1)
    for gy in range(map_y1, map_y2, 40):
        draw.line([(map_x1, gy), (map_x2, gy)], fill=GRID_COLOR, width=1)

    # 室内障碍物与墙壁 (黑色/灰蓝墙体)
    # 外墙
    draw.rectangle([530, 110, 1140, 125], fill=WALL_COLOR) # 顶墙
    draw.rectangle([530, 610, 1140, 625], fill=WALL_COLOR) # 底墙
    draw.rectangle([530, 110, 545, 625], fill=WALL_COLOR)  # 左墙
    draw.rectangle([1125, 110, 1140, 625], fill=WALL_COLOR) # 右墙

    # 室内隔断墙与柱子
    draw.rectangle([750, 125, 765, 380], fill=WALL_COLOR)
    draw.rectangle([900, 320, 915, 610], fill=WALL_COLOR)
    # 矩形障碍物 (桌椅/箱子)
    draw.rectangle([620, 240, 670, 300], fill=(85, 100, 130))
    draw.rectangle([980, 200, 1030, 260], fill=(85, 100, 130))
    draw.rectangle([780, 480, 840, 540], fill=(85, 100, 130))

    # 导航全局路径 (S 型路径点)
    path_points = [
        (590, 540), (600, 450), (630, 360), (700, 340), 
        (820, 350), (840, 240), (950, 230), (1060, 380), (1050, 520)
    ]
    # 绘制全局绿色轨迹线
    for i in range(len(path_points) - 1):
        draw.line([path_points[i], path_points[i+1]], fill=(30, 160, 90), width=3)

    # 终点旗帜 Goal Pose
    goal_x, goal_y = path_points[-1]
    draw.polygon([(goal_x, goal_y), (goal_x + 18, goal_y - 12), (goal_x, goal_y - 24)], fill=ACCENT_ORANGE)
    draw.line([(goal_x, goal_y), (goal_x, goal_y - 28)], fill=TEXT_WHITE, width=2)
    draw.text((goal_x - 15, goal_y + 8), "GOAL", fill=ACCENT_ORANGE)

    # 计算小车当前动态位置 (在路径上插值)
    total_segments = len(path_points) - 1
    cur_pos_float = t_ratio * total_segments
    seg_idx = min(int(cur_pos_float), total_segments - 1)
    seg_t = cur_pos_float - seg_idx
    
    p1 = path_points[seg_idx]
    p2 = path_points[seg_idx + 1]
    car_x = p1[0] + (p2[0] - p1[0]) * seg_t
    car_y = p1[1] + (p2[1] - p1[1]) * seg_t
    car_angle = math.atan2(p2[1] - p1[1], p2[0] - p1[0])

    # 绘制局部避障规划线 (从当前小车前方延伸出的橙色轨迹)
    local_p1 = (car_x, car_y)
    local_p2 = (car_x + math.cos(car_angle) * 45, car_y + math.sin(car_angle) * 45)
    draw.line([local_p1, local_p2], fill=ACCENT_ORANGE, width=4)

    # 激光雷达扫描扇区光束 (LiDAR Scan Rays)
    for ray_i in range(16):
        ray_ang = car_angle - math.pi/2 + (ray_i / 15.0) * math.pi
        ray_len = 65 + 25 * math.sin(ray_i * 0.8 + t_ratio * 10)
        rx = car_x + math.cos(ray_ang) * ray_len
        ry = car_y + math.sin(ray_ang) * ray_len
        draw.line([(car_x, car_y), (rx, ry)], fill=(0, 200, 255, 60), width=1)
        # 点云反射点
        draw.ellipse([rx - 2, ry - 2, rx + 2, ry + 2], fill=ACCENT_BLUE)

    # 绘制小车本体 (定向多边形)
    c_r = 16
    p_front = (car_x + math.cos(car_angle) * (c_r + 4), car_y + math.sin(car_angle) * (c_r + 4))
    p_left = (car_x + math.cos(car_angle + 2.4) * c_r, car_y + math.sin(car_angle + 2.4) * c_r)
    p_right = (car_x + math.cos(car_angle - 2.4) * c_r, car_y + math.sin(car_angle - 2.4) * c_r)
    draw.polygon([p_front, p_left, (car_x, car_y), p_right], fill=(240, 60, 60))
    draw.ellipse([car_x - 4, car_y - 4, car_x + 4, car_y + 4], fill=TEXT_WHITE)

    # 右上角地图图例
    draw.rectangle([map_x2 - 190, map_y1 + 15, map_x2 - 15, map_y1 + 85], fill=(25, 32, 45))
    draw.line([(map_x2 - 175, map_y1 + 30), (map_x2 - 140, map_y1 + 30)], fill=(30, 160, 90), width=3)
    draw.text((map_x2 - 130, map_y1 + 23), "Global Path", fill=TEXT_WHITE)
    draw.line([(map_x2 - 175, map_y1 + 50), (map_x2 - 140, map_y1 + 50)], fill=ACCENT_ORANGE, width=3)
    draw.text((map_x2 - 130, map_y1 + 43), "Local DWB", fill=TEXT_WHITE)
    draw.ellipse([map_x2 - 170, map_y1 + 67, map_x2 - 162, map_y1 + 75], fill=ACCENT_BLUE)
    draw.text((map_x2 - 130, map_y1 + 63), "LiDAR Points", fill=TEXT_WHITE)

    return img

def main():
    print("[*] 正在生成自主原创的高保真车机中控与 SLAM 静态效果图...")
    static_img = draw_single_frame(t_ratio=0.35)
    static_path = os.path.join(OUTPUT_DIR, "demo_system.png")
    static_img.save(static_path, "PNG")
    print(f"[✓] 静态展示图已生成: {static_path}")

    print("[*] 正在生成 24 帧平滑自主导航与避障演示动态动图 (GIF)...")
    frames = []
    total_frames = 24
    for f_idx in range(total_frames):
        ratio = f_idx / float(total_frames)
        frame_img = draw_single_frame(t_ratio=ratio)
        frames.append(frame_img)
    
    gif_path = os.path.join(OUTPUT_DIR, "navigation_demo.gif")
    # 保存为循环 GIF，每帧 120ms
    frames[0].save(
        gif_path,
        save_all=True,
        append_images=frames[1:],
        duration=120,
        loop=0
    )
    print(f"[✓] 动态演示 GIF 已生成: {gif_path}")

if __name__ == "__main__":
    main()
