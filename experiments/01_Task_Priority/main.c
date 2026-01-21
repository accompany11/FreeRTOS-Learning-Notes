/**
 * @file    main.c
 * @author  Accompany (GitHub: 你的用户名)
 * @brief   FreeRTOS 任务抢占实验
 * 演示内容：高优先级任务如何通过抢占机制剥夺低优先级任务的 CPU 使用权。
 */

#include "FreeRTOS.h"
#include "task.h"

/* 任务句柄 */
TaskHandle_t TaskHigh_Handler;
TaskHandle_t TaskLow_Handler;

/* 任务函数声明 */
void vTaskHigh(void *pvParameters);
void vTaskLow(void *pvParameters);

int main(void)
{
    // 1. 硬件初始化 (如串口、LED、时钟等)
    prvSetupHardware(); 

    // 2. 创建高优先级任务 (Priority = 3)
    xTaskCreate((TaskFunction_t )vTaskHigh,     
                (const char* )"TaskHigh",   
                (uint16_t       )128,           
                (void* )NULL,          
                (UBaseType_t    )3,             // 优先级最高
                (TaskHandle_t* )&TaskHigh_Handler);   

    // 3. 创建低优先级任务 (Priority = 2)
    xTaskCreate((TaskFunction_t )vTaskLow,     
                (const char* )"TaskLow",    
                (uint16_t       )128,           
                (void* )NULL,          
                (UBaseType_t    )2,             // 优先级较低
                (TaskHandle_t* )&TaskLow_Handler);  

    // 4. 开启任务调度器
    vTaskStartScheduler();          

    while(1); // 正常情况下不会执行到这里
}

/**
 * @brief 高优先级任务
 * @note  每 500ms 打印一次。当它调用 vTaskDelay 时，会进入 Blocked 状态，释放 CPU。
 */
void vTaskHigh(void *pvParameters)
{
    for(;;)
    {
        printf("TaskHigh is Running... [High Priority]\r\n");
        // vTaskDelay 会引起任务切换，此时 TaskLow 才有机会运行
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}

/**
 * @brief 低优先级任务
 * @note  这是一个“打工人”任务，只有在高优先级任务休息（阻塞）时才能运行。
 */
void vTaskLow(void *pvParameters)
{
    for(;;)
    {
        // 模拟高负载工作
        printf("TaskLow is Running... [Low Priority]\r\n");
        
        /* 注意：如果这里没有延时或者阻塞操作，
           且 TaskHigh 延时结束，TaskHigh 会立刻抢占此任务 */
    }
}
