#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 协议帧头帧尾
#define FRAME_HEADER1 0x5A
#define FRAME_HEADER2 0xA5
#define FRAME_TAIL1   0x0D
#define FRAME_TAIL2   0x0A

#pragma pack(1)
// 下发控制帧 (ROS2 -> STM32)
typedef struct {
    uint8_t header1;     // 0x5A
    uint8_t header2;     // 0xA5
    uint8_t type;        // 0x01: 速度控制
    float target_vx;     // 目标线速度 m/s
    float target_wz;     // 目标角速度 rad/s
    uint8_t checksum;    // 校验和
    uint8_t tail1;       // 0x0D
    uint8_t tail2;       // 0x0A
} Cmd_Vel_Frame_t;

// 上报里程计数据帧 (STM32 -> ROS2)
typedef struct {
    uint8_t header1;     // 0x5A
    uint8_t header2;     // 0xA5
    uint8_t type;        // 0x02: 里程计反馈
    float actual_vx;     // 实时线速度 m/s
    float actual_wz;     // 实时角速度 rad/s
    float pos_x;         // 累积位移 x (m)
    float pos_y;         // 累积位移 y (m)
    float heading_yaw;   // 航向角 theta (rad)
    uint8_t checksum;    // 校验和
    uint8_t tail1;       // 0x0D
    uint8_t tail2;       // 0x0A
} Odom_Feedback_Frame_t;
#pragma pack()

void Protocol_Init(void);
bool Protocol_Parse_Byte(uint8_t byte, float *target_vx, float *target_wz);
void Protocol_Pack_Odom(Odom_Feedback_Frame_t *frame, float vx, float wz, float x, float y, float yaw);

#ifdef __cplusplus
}
#endif

#endif
