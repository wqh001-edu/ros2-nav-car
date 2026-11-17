#ifndef __UI_DASHBOARD_H
#define __UI_DASHBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

// 初始化车载 HMI 整体界面 (基于 LVGL 8.3 / 9.x)
void UI_Dashboard_Create(void);

// 更新左侧车速仪表数据 (km/h)
void UI_Dashboard_Update_Speed(float speed_kmh);

// 更新电量与 Wi-Fi 状态
void UI_Dashboard_Update_Battery(int percentage);
void UI_Dashboard_Update_Wifi(const char *ssid, int signal_strength);

// 更新右侧 2D 栅格地图 (从 ROS2 /map 话题接收的数据点阵)
void UI_Dashboard_Update_Map(const int8_t *map_data, int width, int height, float resolution);

#ifdef __cplusplus
}
#endif

#endif
