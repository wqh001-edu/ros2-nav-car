#include "pid.h"

void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float max_integral, float max_output) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->max_integral = max_integral;
    pid->max_output = max_output;
    
    PID_Reset(pid);
}

void PID_Reset(PID_Controller *pid) {
    pid->target_val = 0.0f;
    pid->actual_val = 0.0f;
    pid->err = 0.0f;
    pid->err_last = 0.0f;
    pid->err_prev = 0.0f;
    pid->integral = 0.0f;
    pid->output = 0.0f;
}

// 增量式 PID，适用于电机速度环控制
float PID_Incremental_Calc(PID_Controller *pid, float target, float actual) {
    pid->target_val = target;
    pid->actual_val = actual;
    pid->err = target - actual;

    float delta_u = pid->kp * (pid->err - pid->err_last)
                  + pid->ki * pid->err
                  + pid->kd * (pid->err - 2.0f * pid->err_last + pid->err_prev);

    pid->output += delta_u;

    // 限幅
    if (pid->output > pid->max_output) {
        pid->output = pid->max_output;
    } else if (pid->output < -pid->max_output) {
        pid->output = -pid->max_output;
    }

    pid->err_prev = pid->err_last;
    pid->err_last = pid->err;

    return pid->output;
}

// 位置式 PID，适用于小车航向与位置追踪
float PID_Positional_Calc(PID_Controller *pid, float target, float actual) {
    pid->target_val = target;
    pid->actual_val = actual;
    pid->err = target - actual;

    pid->integral += pid->err;
    // 抗积分饱和
    if (pid->integral > pid->max_integral) {
        pid->integral = pid->max_integral;
    } else if (pid->integral < -pid->max_integral) {
        pid->integral = -pid->max_integral;
    }

    pid->output = pid->kp * pid->err
                + pid->ki * pid->integral
                + pid->kd * (pid->err - pid->err_last);

    if (pid->output > pid->max_output) {
        pid->output = pid->max_output;
    } else if (pid->output < -pid->max_output) {
        pid->output = -pid->max_output;
    }

    pid->err_last = pid->err;
    return pid->output;
}
