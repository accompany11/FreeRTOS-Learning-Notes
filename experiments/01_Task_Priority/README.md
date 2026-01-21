🧪 实验报告：FreeRTOS 任务抢占机制验证
1. 实验目的
验证 FreeRTOS 的抢占式调度 (Preemptive Scheduling) 特性。

观察高优先级任务如何剥夺低优先级任务的 CPU 使用权。

理解 vTaskDelay 如何引发任务状态切换。

2. 实验原理
在 FreeRTOS 中，调度器始终确保处于 Ready (就绪态) 的最高优先级任务获得 CPU。

当 TaskHigh (Priority 3) 调用 vTaskDelay 时，它进入 Blocked (阻塞态)，此时 TaskLow (Priority 2) 获得运行机会。

一旦 vTaskDelay 计时结束，TaskHigh 回到 Ready 状态。由于其优先级更高，内核会立即触发 PendSV 中断进行上下文切换，强行打断 TaskLow。

3. 实验步骤与现象
代码实现：创建两个任务，优先级分别为 3 和 2。

现象观察：通过串口调试助手查看输出。

预期输出：

Plaintext
TaskLow is Running...
TaskLow is Running...
TaskHigh is Running... [High Priority]  <-- 每500ms准确出现一次
TaskLow is Running...
TaskLow is Running...
关键发现：无论 TaskLow 是否在执行复杂逻辑，TaskHigh 都能准时“插入”。

4. 深度思考
如果去掉 vTaskDelay 会怎样？ 如果 TaskHigh 不调用阻塞函数，它将永远占据 CPU，TaskLow 会进入“饥饿”状态（Starvation），永远无法运行。

中断的作用： SysTick 中断负责计时，判断延时是否到期；而上下文切换的动作是在 PendSV 中断中完成的。
