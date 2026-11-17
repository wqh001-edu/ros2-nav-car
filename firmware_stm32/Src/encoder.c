#include "encoder.h"

// 映射到硬件 TIM2 (左轮正交解码) 与 TIM3 / TIM4 (右轮正交解码)
void Encoder_Init(void) {
    // 启动定时器正交解码计数模式
    // __HAL_TIM_SET_COUNTER(&htim2, 0);
    // __HAL_TIM_SET_COUNTER(&htim3, 0);
}

void Encoder_Update(Encoder_Data_t *data, float dt_seconds) {
    if (dt_seconds <= 0.0001f) return;

    // 假设读取定时器寄存器差值并清零计数器
    // int16_t left_cnt = (short)__HAL_TIM_GET_COUNTER(&htim2);
    // __HAL_TIM_SET_COUNTER(&htim2, 0);
    // int16_t right_cnt = (short)__HAL_TIM_GET_COUNTER(&htim3);
    // __HAL_TIM_SET_COUNTER(&htim3, 0);
    
    int16_t left_cnt = 0;  // 示例模拟值
    int16_t right_cnt = 0;

    data->left_pulse += left_cnt;
    data->right_pulse += right_cnt;

    // 计算单个周期左右轮行进距离 (米)
    float left_dist = ((float)left_cnt / TOTAL_PULSES_PER_REV) * WHEEL_CIRCUMFERENCE;
    float right_dist = ((float)right_cnt / TOTAL_PULSES_PER_REV) * WHEEL_CIRCUMFERENCE;

    // 计算实时线速度 (m/s)
    data->left_speed_m_s = left_dist / dt_seconds;
    data->right_speed_m_s = right_dist / dt_seconds;

    // 差速运动学模型解算
    data->linear_speed = (data->right_speed_m_s + data->left_speed_m_s) / 2.0f;
    data->angular_speed = (data->right_speed_m_s - data->left_speed_m_s) / WHEEL_TRACK_METERS;
}
