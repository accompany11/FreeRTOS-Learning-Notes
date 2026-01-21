/**
 * @file    imu_diff_correction.c
 * @brief   基于高精度 IMU 反馈的重载底盘差速纠偏算法
 * @details 利用工业级陀螺仪的 Yaw 角反馈，解决病床拖行时的物理偏移复位
 */

#include <math.h>
#include "chassis_config.h" 

// 纠偏 PID 参数定义
#define Kp 1.5f
#define Ki 0.01f
#define Kd 0.2f

float target_yaw = 0.0f;    // 目标航向角
float current_yaw = 0.0f;   // 工业级陀螺仪实时输出的 Yaw 角度
float error_integral = 0.0f;
float last_error = 0.0f;

/**
 * @brief  高精度纠偏算法核心逻辑
 * @param  base_speed 上位机给出的基础前进速度
 * @return 左右电机的差速补偿值
 */
float Calculate_Differential_Compensation(float base_speed) {
    // 1. 获取高精度 IMU 数据（假设通过串口/CAN 已更新至 current_yaw）
    float error = target_yaw - current_yaw;
    
    // 2. 角度环 PID 计算
    error_integral += error;
    float derivative = error - last_error;
    float compensation = Kp * error + Ki * error_integral + Kd * derivative;
    
    last_error = error;
    
    // 3. 输出差速补偿值量值
    return compensation; 
}

/**
 * @brief  下发电机控制指令
 * @param  linear_vel 线性速度 (m/s)
 */
void Apply_Chassis_Control(float linear_vel) {
    float diff = Calculate_Differential_Compensation(linear_vel);
    
    // 差速模型：左轮减速，右轮加速（实现航向纠偏）
    float left_motor_speed  = linear_vel - diff;
    float right_motor_speed = linear_vel + diff;
    
    // 调用底盘通信接口，将数据发送至电机驱动器
    Motor_Drive_Send(left_motor_speed, right_motor_speed);
}
