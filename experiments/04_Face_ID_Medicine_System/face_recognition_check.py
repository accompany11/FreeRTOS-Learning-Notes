import cv2
import time

# 模拟患者人脸特征数据库
known_patient_db = {"Patient_A": "ID_001", "Patient_B": "ID_002"}

def run_face_pharmacy_system():
    print("[System] 护理机器人人脸比对系统启动...")
    
    # 1. 模拟开启摄像头并捕获人脸
    # 在实际项目中，这里会调用 cv2.CascadeClassifier 或深度学习模型
    print("[Vision] 正在扫描人脸...")
    time.sleep(2) 
    
    # 2. 模拟识别成功（匹配到 Patient_A）
    detected_user = "Patient_A"
    
    if detected_user in known_patient_db:
        print(f"[Success] 身份确认: {detected_user}，准备下发取药指令。")
        
        # 3. 跨平台通信：通过串口或话题向底盘/药仓控制端发送指令
        send_command_to_stm32("OPEN_PHARMACY_DRAWER_01")
    else:
        print("[Error] 身份验证失败，拒绝开启药仓。")

def send_command_to_stm32(command):
    # 这里对应你简历中提到的“自动运行相关代码，自动连接通信模块”
    print(f"[Comm] 已向 STM32 控制端发送信号: {command}")

if __name__ == "__main__":
    run_face_pharmacy_system()
