#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "motor.h"
#include "encoder.h"
#include "pid.h"
#include "protocol.h"

// 速度 PID 控制器与小车全局状态
static PID_Controller pid_left;
static PID_Controller pid_right;
static Encoder_Data_t encoder_data;

// 目标速度 (由 ROS2 通过串口下发)
static float target_linear_v = 0.0f;
static float target_angular_w = 0.0f;

// 累积全局位姿
static float odom_x = 0.0f;
static float odom_y = 0.0f;
static float odom_yaw = 0.0f;

void System_Init(void) {
    Motor_Init();
    Encoder_Init();
    Protocol_Init();

    // 初始化增量式速度 PID 参数 (需根据实车质量与减速比精调)
    PID_Init(&pid_left, 150.0f, 12.0f, 2.0f, 300.0f, 1000.0f);
    PID_Init(&pid_right, 150.0f, 12.0f, 2.0f, 300.0f, 1000.0f);
}

// 100Hz 定时器中断回调函数 (每 10ms 触发一次)
void Control_Loop_10ms_ISR(void) {
    const float dt = 0.010f; // 10ms

    // 1. 采集编码器反馈，解算当前实际轮速
    Encoder_Update(&encoder_data, dt);

    // 2. 积分计算小车航向与平面坐标
    float delta_dist = encoder_data.linear_speed * dt;
    float delta_yaw = encoder_data.angular_speed * dt;
    odom_yaw += delta_yaw;
    odom_x += delta_dist * cosf(odom_yaw);
    odom_y += delta_dist * sinf(odom_yaw);

    // 3. 差速逆运动学解算：将目标 v, w 拆分为左右轮线速度
    float target_v_left = target_linear_v - (target_angular_w * WHEEL_TRACK_METERS / 2.0f);
    float target_v_right = target_linear_v + (target_angular_w * WHEEL_TRACK_METERS / 2.0f);

    // 4. 双闭环 PID 计算电机驱动 PWM 输出
    float left_pwm = PID_Incremental_Calc(&pid_left, target_v_left, encoder_data.left_speed_m_s);
    float right_pwm = PID_Incremental_Calc(&pid_right, target_v_right, encoder_data.right_speed_m_s);

    // 5. 输出到底层电机 H 桥
    Motor_Set_Left((int16_t)left_pwm);
    Motor_Set_Right((int16_t)right_pwm);

    // 6. 按照 50Hz (每 2 次控制循环) 向 ROS2 上报里程计数据帧
    static uint8_t report_div = 0;
    if (++report_div >= 2) {
        report_div = 0;
        Odom_Feedback_Frame_t odom_frame;
        Protocol_Pack_Odom(&odom_frame, encoder_data.linear_speed, encoder_data.angular_speed,
                           odom_x, odom_y, odom_yaw);
        // 发送 odom_frame 二进制流至串口 (如 USART1_SendBytes((uint8_t*)&odom_frame, sizeof(odom_frame)))
    }
}

// 串口接收中断调用
void USART_Rx_Callback(uint8_t ch) {
    float vx, wz;
    if (Protocol_Parse_Byte(ch, &vx, &wz)) {
        // 收到合法速度指令，更新设定值
        target_linear_v = vx;
        target_angular_w = wz;
    }
}

int main(void) {
    System_Init();

    while (1) {
        // 主任务调度或低优先级状态机
    }
}
