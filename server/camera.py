import cv2
import socket
import struct
import time
import os

# === 설정 ===
SERVER_IP = '127.0.0.1'  # 톰캣이 같은 라즈베리파이에 있음
SERVER_PORT = 9999
SIGNAL_FILE = "/home/pi/embedded/record.txt"
SAVE_PATH = "/home/pi/embedded/video/" # 영상 저장 경로 (미리 폴더 생성 필요)

# === 소켓 연결 ===
def connect_socket():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    while True:
        try:
            sock.connect((SERVER_IP, SERVER_PORT))
            print("Connected to Tomcat Server!")
            return sock
        except:
            print("Connection failed. Retrying in 3s...")
            time.sleep(3)

client_socket = connect_socket()
cap = cv2.VideoCapture(0, cv2.CAP_V4L2)
cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))

cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
cap.set(cv2.CAP_PROP_FPS, 30)

# 녹화 관련 변수
is_recording = False
out = None
fourcc = cv2.VideoWriter_fourcc(*'MJPG')

# === 메인 루프 ===
try:
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        # 1. 제어 신호 파일 읽기 (JSP와의 통신)
        try:
            if os.path.exists(SIGNAL_FILE):
                with open(SIGNAL_FILE, 'r') as f:
                    cmd = f.read().strip()
                
                # 녹화 시작 명령 ('1') 이고 현재 녹화중이 아닐 때
                if cmd == '1' and not is_recording:
                    print(">>> Start Recording")
                    is_recording = True
                    filename = f"{SAVE_PATH}rec_{int(time.time())}.avi"
                    out = cv2.VideoWriter(filename, fourcc, 20.0, (640, 480))
                
                # 녹화 중지 명령 ('0') 이고 현재 녹화중일 때
                elif cmd == '0' and is_recording:
                    print(">>> Stop Recording")
                    is_recording = False
                    if out:
                        out.release()
                        out = None
        except Exception as e:
            print(f"File Read Error: {e}")

        # 2. 녹화 중이면 파일 쓰기
        if is_recording and out:
            out.write(frame)

        # 3. 톰캣으로 스트리밍 전송
        try:
            # 프레임 크기 줄이기 (전송 최적화)
            # frame_resized = cv2.resize(frame, (320, 240)) 
            _, img_encoded = cv2.imencode('.jpg', frame)
            data = img_encoded.tobytes()
            
            # 패킷 전송: [길이(4byte)][데이터]
            client_socket.sendall(struct.pack(">L", len(data)) + data)
        except:
            # 소켓 끊기면 재연결 시도 or 종료
            print("Socket Error")
            break
            
        time.sleep(0.04) # CPU 점유율 조절

finally:
    cap.release()
    if out:
        out.release()
    client_socket.close()