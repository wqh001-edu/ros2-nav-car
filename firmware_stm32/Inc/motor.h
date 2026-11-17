#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 电机方向枚举
typedef enum {
    MOTOR_FWD = 0,
    MOTOR_REV = 1,
    MOTOR_STOP = 2
} Motor_Dir_t;

void Motor_Init(void);
void Motor_Set_Left(int16_t pwm);
void Motor_Set_Right(int16_t pwm);
void Motor_Stop_All(void);

#ifdef __cplusplus
}
#endif

#endif
