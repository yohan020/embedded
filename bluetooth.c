#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

// ★ 주의: 블루투스 모듈 초기값은 보통 9600입니다. 
// 통신이 안 되면 이 값을 9600으로 변경해보세요.
#define BAUD_RATE 115200

// 요청하신 UART2 장치 파일
static const char *UART2_DEV = "/dev/ttyAMA2"; 

unsigned char serialRead(const int fd);
void serialWrite(const int fd, const unsigned char c);
void serialWriteBytes(const int fd, const char *s);

// 1바이트 데이터를 읽음 (기존 함수 유지)
unsigned char serialRead(const int fd) {
    unsigned char x;
    if (read(fd, &x, 1) != 1) 
        return -1;
    return x; 
}

// 1바이트 데이터를 씀 (기존 함수 유지)
void serialWrite(const int fd, const unsigned char c) {
    write(fd, &c, 1); 
}

// 여러 바이트의 데이터를 씀 (기존 함수 유지)
void serialWriteBytes(const int fd, const char *s) {
    write(fd, s, strlen(s));
}

int main() {
    int fd_serial;
    unsigned char dat;

    // 1. GPIO 초기화
    if (wiringPiSetupGpio() < 0) return 1;

    // 2. UART2 포트 오픈
    // 만약 여기서 에러가 나면 config.txt에서 dtoverlay=uart2 설정이 필요할 수 있습니다.
    if ((fd_serial = serialOpen(UART2_DEV, BAUD_RATE)) < 0) {
        printf("Unable to open serial device (%s).\n", UART2_DEV);
        return 1;
    }

    printf("UART2 Bluetooth Control Mode Started.\n");
    printf("Waiting for commands (0~4)...\n");

    while (1) {
        // 블루투스에서 데이터가 들어왔는지 확인 (Non-blocking)
        if (serialDataAvail(fd_serial)) {
            
            // 데이터 1바이트 읽기
            dat = serialRead(fd_serial);

            // 읽은 값에 따라 로봇 동작 분기
            switch (dat) {
                case '0':
                    printf("[CMD 0] Stop Cleaning\n");
                    // 여기에 정지 함수 추가 (예: motor_stop();)
                    break;

                case '1':
                    printf("[CMD 1] Start Cleaning\n");
                    // 여기에 청소 시작 함수 추가
                    break;

                case '3':
                    printf("[CMD 3] Move Forward\n");
                    // 여기에 전진 함수 추가
                    break;

                case '2':
                    printf("[CMD 2] Turn Left\n");
                    // 여기에 좌회전 함수 추가
                    break;

                case '4':
                    printf("[CMD 4] Turn Right\n");
                    // 여기에 우회전 함수 추가
                    break;
                case '5':
                    printf("[CMD 5] Auto Mode\n");
                    // 여기에 자동 모드 함수 추가
                    break;
                case '6':
                    printf("[CMD 6] Manual Mode\n");
                    // 여기에 수동 모드 함수 추가
                    break;
                // 줄바꿈 문자 등은 무시
                case '\r': 
                case '\n': 
                    break;

                default:
                    // 정의되지 않은 문자가 오면 출력해봄 (디버깅용)
                    // printf("Unknown: %c\n", dat); 
                    break;
            }
            fflush(stdout); // 즉시 화면에 출력
        }
        
        // CPU 점유율을 낮추기 위한 약간의 대기
        delay(10);
    }

    return 0;
}