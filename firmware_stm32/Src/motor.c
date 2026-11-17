#include "motor.h"
#include <stdlib.h>

// 模拟硬件寄存器或 HAL 库调用接口
// 实际硬件工程中映射到 TIM1 / TIM8 PWM 输出引脚 (如 PA8/PA9, TIM1_CH1/CH2)
#define PWM_MAX_LIMIT 1000

void Motor_Init(void) {
    // 初始化 TIM PWM 通道与方向控制 GPIO
    Motor_Stop_All();
}

void Motor_Set_Left(int16_t pwm) {
    if (pwm > PWM_MAX_LIMIT) pwm = PWM_MAX_LIMIT;
    if (pwm < -PWM_MAX_LIMIT) pwm = -PWM_MAX_LIMIT;

    if (pwm > 0) {
        // 设置正转 GPIO 引脚电平 (IN1=1, IN2=0)
        // 设置 TIM 通道比较值 (CCR1 = pwm)
    } else if (pwm < 0) {
        // 设置反转 GPIO 引脚电平 (IN1=0, IN2=1)
        // 设置 TIM 通道比较值 (CCR1 = -pwm)
    } else {
        // 刹车或空闲 (IN1=0, IN2=0)
    }
}

void Motor_Set_Right(int16_t pwm) {
    if (pwm > PWM_MAX_LIMIT) pwm = PWM_MAX_LIMIT;
    if (pwm < -PWM_MAX_LIMIT) pwm = -PWM_MAX_LIMIT;

    if (pwm > 0) {
        // 设置右轮正转
    } else if (pwm < 0) {
        // 设置右轮反转
    } else {
        // 停止
    }
}

void Motor_Stop_All(void) {
    Motor_Set_Left(0);
    Motor_Set_Right(0);
}
