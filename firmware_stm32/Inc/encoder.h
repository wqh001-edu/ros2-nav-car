#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 机器人底盘物理参数
#define WHEEL_DIAMETER_METERS   0.065f   // 轮径 65mm
#define WHEEL_TRACK_METERS      0.180f   // 左右两轮间距 (轮距) 180mm
#define ENCODER_RESOLUTION      13.0f    // 霍尔编码器原始线数 13
#define GEAR_REDUCTION_RATIO    30.0f    // 电机减速比 1:30
#define ENCODER_QUADRATURE      4.0f     // 正交解码倍频 (4倍频)

// 一圈总脉冲数 = 13 * 30 * 4 = 1560
#define TOTAL_PULSES_PER_REV    (ENCODER_RESOLUTION * GEAR_REDUCTION_RATIO * ENCODER_QUADRATURE)
#define WHEEL_CIRCUMFERENCE     (3.14159265f * WHEEL_DIAMETER_METERS)

typedef struct {
    int32_t left_pulse;
    int32_t right_pulse;
    float left_speed_m_s;    // 左轮实时线速度 m/s
    float right_speed_m_s;   // 右轮实时线速度 m/s
    float linear_speed;      // 小车中心线速度 v
    float angular_speed;     // 小车角速度 w
} Encoder_Data_t;

void Encoder_Init(void);
void Encoder_Update(Encoder_Data_t *data, float dt_seconds);

#ifdef __cplusplus
}
#endif

#endif
