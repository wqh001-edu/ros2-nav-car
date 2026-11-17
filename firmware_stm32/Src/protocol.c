#include "protocol.h"
#include <string.h>

static uint8_t rx_buf[64];
static uint8_t rx_idx = 0;

void Protocol_Init(void) {
    rx_idx = 0;
}

// 状态机逐字节解析接收到的串口数据
bool Protocol_Parse_Byte(uint8_t byte, float *target_vx, float *target_wz) {
    rx_buf[rx_idx++] = byte;

    if (rx_idx == 1 && rx_buf[0] != FRAME_HEADER1) {
        rx_idx = 0;
        return false;
    }
    if (rx_idx == 2 && rx_buf[1] != FRAME_HEADER2) {
        rx_idx = 0;
        return false;
    }

    if (rx_idx >= sizeof(Cmd_Vel_Frame_t)) {
        Cmd_Vel_Frame_t *frame = (Cmd_Vel_Frame_t *)rx_buf;
        if (frame->tail1 == FRAME_TAIL1 && frame->tail2 == FRAME_TAIL2) {
            // 计算校验和
            uint8_t sum = 0;
            uint8_t *p = (uint8_t *)frame;
            for (size_t i = 0; i < sizeof(Cmd_Vel_Frame_t) - 3; i++) {
                sum += p[i];
            }
            if (sum == frame->checksum) {
                *target_vx = frame->target_vx;
                *target_wz = frame->target_wz;
                rx_idx = 0;
                return true;
            }
        }
        rx_idx = 0;
    }

    if (rx_idx >= sizeof(rx_buf)) {
        rx_idx = 0;
    }
    return false;
}

void Protocol_Pack_Odom(Odom_Feedback_Frame_t *frame, float vx, float wz, float x, float y, float yaw) {
    frame->header1 = FRAME_HEADER1;
    frame->header2 = FRAME_HEADER2;
    frame->type = 0x02;
    frame->actual_vx = vx;
    frame->actual_wz = wz;
    frame->pos_x = x;
    frame->pos_y = y;
    frame->heading_yaw = yaw;

    uint8_t sum = 0;
    uint8_t *p = (uint8_t *)frame;
    for (size_t i = 0; i < sizeof(Odom_Feedback_Frame_t) - 3; i++) {
        sum += p[i];
    }
    frame->checksum = sum;
    frame->tail1 = FRAME_TAIL1;
    frame->tail2 = FRAME_TAIL2;
}
