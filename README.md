# 📝 FreeRTOS 内核深度笔记：任务调度机制 (Task Scheduling)

本仓库用于记录我对 FreeRTOS 调度算法的理解、源码分析及实验验证。

---

## 1. 宏观架构：如何实现“伪并行”？
FreeRTOS 采用 **抢占式 (Preemptive)** 调度策略。其核心逻辑是：**最高优先级的就绪任务必须始终获得 CPU 使用权。**

### 核心硬件机制
为了实现任务切换，FreeRTOS 巧妙地利用了 ARM Cortex-M 内核的两个特殊中断：

| 中断名称 | 角色 | 职能 |
| :--- | :--- | :--- |
| **SysTick** | 系统的“心跳” | 每隔 1ms 触发一次，判断是否需要进行任务切换。 |
| **PendSV** | 任务切换的“搬运工” | 执行具体的上下文切换（保存/恢复寄存器状态），确保不会打断高优先级的异常处理。 |



---

## 2. 任务状态机 (State Machine)
理解调度算法的前提是理清任务在内核中的生存状态。

* **Running (运行态)**：当前占用 CPU 的任务。
* **Ready (就绪态)**：在就绪列表中等待调度，优先级最高者将变为 Running。
* **Blocked (阻塞态)**：任务在等待特定事件（如 `vTaskDelay` 或信号量），不占用 CPU 资源。
* **Suspended (挂起态)**：通过 `vTaskSuspend()` 彻底退出调度，直到被手动唤醒。



---

## 3. 源码级深度探索 (Source Code Analysis)

> **"Read the fucking source code."** —— 只有深入内核函数，才能真正掌握 RTOS。

### 🚀 A. 调度器是如何启动的？
在调用 `vTaskStartScheduler()` 后，系统完成了从“裸机控制”到“多任务接管”的跳跃。
* **关键函数**：`xPortStartScheduler()`
* **核心逻辑**：设置 PendSV 优先级为最低、启动 SysTick 计数器、通过汇编触发第一个任务。

### 🔄 B. 上下文切换的底层实现
任务切换的本质是寄存器状态的保存与恢复。
* **关键函数**：`xPortPendSVHandler` (汇编实现)
* **保存过程**：将当前任务的 `R4-R11` 推入堆栈，更新堆栈指针 (PSP)。
* **恢复过程**：从下一个任务的堆栈中弹出 `R4-R11`，切换 PSP，最后跳转执行。

```c
// 伪代码示例：任务切换逻辑
void vTaskSwitchContext( void ) {
    if ( uxSchedulerSuspended != pdFALSE ) {
        // 调度器挂起，记录请求但不切换
    } else {
        // 寻找当前最高优先级的就绪任务
        taskSELECT_HIGHEST_PRIORITY_TASK(); 
    }
}
