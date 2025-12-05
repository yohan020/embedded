import socket
import struct
import cv2
import numpy as np

# === 테스트용 임시 서버 설정 ===
HOST = '0.0.0.0'  # 모든 인터페이스에서 접속 허용
PORT = 9999

def run_server():
    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        server_sock.bind((HOST, PORT))
        server_sock.listen(1)
        print(f"Server listening on {HOST}:{PORT}...")
        print("이제 클라이언트(카메라) 코드를 실행해보세요.")

        conn, addr = server_sock.accept()
        print(f"Connected by {addr}")

        data = b""
        payload_size = struct.calcsize(">L")  # 4 bytes (Big-endian unsigned long)

        while True:
            # 1. 메시지 길이 수신 (4바이트)
            while len(data) < payload_size:
                packet = conn.recv(4096)
                if not packet: 
                    break
                data += packet
            
            if not data:
                break

            packed_msg_size = data[:payload_size]
            data = data[payload_size:]
            msg_size = struct.unpack(">L", packed_msg_size)[0]

            # 2. 실제 이미지 데이터 수신
            while len(data) < msg_size:
                data += conn.recv(4096)

            frame_data = data[:msg_size]
            data = data[msg_size:]

            # (옵션) 수신된 이미지 디코딩 확인
            # frame = cv2.imdecode(np.frombuffer(frame_data, dtype=np.uint8), cv2.IMREAD_COLOR)
            print(f"Received frame size: {msg_size} bytes")

    except Exception as e:
        print(f"Error: {e}")
    finally:
        server_sock.close()
        print("Server closed")

if __name__ == "__main__":
    run_server()