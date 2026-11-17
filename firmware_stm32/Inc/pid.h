#ifndef __PID_H
#define __PID_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float kp;
    float ki;
    float kd;
    
    float target_val;
    float actual_val;
    
    float err;
    float err_last;
    float err_prev;
    
    float integral;
    float max_integral;
    float max_output;
    float output;
} PID_Controller;

void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float max_integral, float max_output);
float PID_Incremental_Calc(PID_Controller *pid, float target, float actual);
float PID_Positional_Calc(PID_Controller *pid, float target, float actual);
void PID_Reset(PID_Controller *pid);

#ifdef __cplusplus
}
#endif

#endif
