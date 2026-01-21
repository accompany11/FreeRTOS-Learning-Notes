/**
 * @file    elevator_fsm.c
 * @author  accompany11
 * @brief   基于有限状态机 (FSM) 的机器人自动进出电梯控制逻辑
 */

#include <stdio.h>

/* 定义电梯交互的各个状态 */
typedef enum {
    STATE_IDLE,             // 空闲：等待任务
    STATE_APPROACHING,      // 接近：行驶至电梯口
    STATE_WAITING_BTN,      // 呼梯：蓝牙控制舵机按键
    STATE_ENTERING,         // 进梯：检测门开后驶入
    STATE_IN_ELEVATOR,      // 驻留：电梯运行中
    STATE_EXITING,          // 出梯：到达目标层驶出
    STATE_ERROR             // 故障：异常处理
} ElevatorState_t;

/* 全局变量：当前状态 */
ElevatorState_t g_current_state = STATE_IDLE;

/**
 * @brief 电梯控制状态机主循环
 * @note  实际项目中此函数会被 FreeRTOS 任务周期性调用
 */
void Elevator_FSM_Process(void) {
    switch (g_current_state) {
        
        case STATE_IDLE:
            if (get_navigation_target() == TARGET_ELEVATOR) {
                printf("[FSM] 收到指令，开始接近电梯...\n");
                g_current_state = STATE_APPROACHING;
            }
            break;

        case STATE_APPROACHING:
            // 调用底盘驱动，配合激光雷达避障
            if (is_at_elevator_door()) {
                printf("[FSM] 到达门口，准备呼梯。\n");
                g_current_state = STATE_WAITING_BTN;
            }
            break;

        case STATE_WAITING_BTN:
            // 通过蓝牙向 STM32 发送指令：舵机按压
            if (send_bluetooth_cmd(CMD_PRESS_BUTTON)) {
                // 判断电梯门是否开启（视觉或红外传感器）
                if (is_elevator_door_open()) {
                    printf("[FSM] 电梯门已开启，正在进入...\n");
                    g_current_state = STATE_ENTERING;
                }
            }
            break;

        case STATE_ENTERING:
            // 切换为短距离高精度纠偏模式（结合高精度陀螺仪）
            if (is_robot_inside()) {
                printf("[FSM] 已安全进入电梯。\n");
                g_current_state = STATE_IN_ELEVATOR;
            }
            break;

        case STATE_IN_ELEVATOR:
            // 监听上位机发送的楼层到达信号
            if (is_target_floor_reached()) {
                printf("[FSM] 到达目标楼层，准备驶出。\n");
                g_current_state = STATE_EXITING;
            }
            break;

        case STATE_EXITING:
            if (is_robot_outside()) {
                printf("[FSM] 任务完成，回归空闲态。\n");
                g_current_state = STATE_IDLE;
            }
            break;

        case STATE_ERROR:
            // 异常恢复逻辑，例如超时回退或上报错误
            handle_fsm_error();
            break;

        default:
            g_current_state = STATE_ERROR;
            break;
    }
}
