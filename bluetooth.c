#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "bluetooth.h" 

#define BAUD_RATE 115200

// 타임아웃 설정 (밀리초 단위, 예: 500ms = 0.5초)
// 0.5초 동안 블루투스 신호가 없으면 정지합니다. 4번 5번에 꼽혀있음

#define SAFETY_TIMEOUT_MS 250 

static const char *UART2_DEV = "/dev/ttyAMA2"; 

unsigned char serialRead(const int fd);
void serialWrite(const int fd, const unsigned char c);
void serialWriteBytes(const int fd, const char *s);

unsigned char serialRead(const int fd) {
    unsigned char x;
    if (read(fd, &x, 1) != 1) 
        return 0; 
    return x; 
}

void serialWrite(const int fd, const unsigned char c) {
    write(fd, &c, 1); 
}

void serialWriteBytes(const int fd, const char *s) {
    write(fd, s, strlen(s));
}

void *bluetooth(void *arg) {
    int fd_serial;
    unsigned char dat;
    
    // ★ 추가: 마지막 데이터 수신 시간을 기록할 변수
    unsigned int last_received_time; 

    if (wiringPiSetupGpio() < 0) {
        perror("wiringPiSetupGpio failed");
        return (void *)-1;
    }

    if ((fd_serial = serialOpen(UART2_DEV, BAUD_RATE)) < 0) {
        printf("Unable to open serial device (%s).\n", UART2_DEV);
        return (void *)-2;
    }

    printf("UART2 Bluetooth Control Mode Started.\n");
    
    // 초기 시간 설정
    last_received_time = millis(); 

    while (1) {
        if (serialDataAvail(fd_serial)) {
            // ★ 중요: 데이터가 들어오면 수신 시간을 현재 시간으로 갱신
            last_received_time = millis(); 

            dat = serialRead(fd_serial);

            switch (dat) {
                case '0':
                    printf("[CMD 0] Stop Power\n");
                    pthread_mutex_lock(&shared_lock);
                    MOTOR_POWER = 0;
                    pthread_mutex_unlock(&shared_lock);
                    break;
                case '1':
                    printf("[CMD 1] Start Power\n");
                    pthread_mutex_lock(&shared_lock);
                    MOTOR_POWER = 1;
                    pthread_mutex_unlock(&shared_lock);
                    break;
                case '3':
                    printf("[CMD 3] Move Forward\n");
                    pthread_mutex_lock(&shared_lock);
                    MOTOR_MOVEMENT = 1;
                    pthread_mutex_unlock(&shared_lock);
                    break;
                case '2':
                    printf("[CMD 2] Turn Left\n");
                    pthread_mutex_lock(&shared_lock);
                    MOTOR_MOVEMENT = 3;
                    pthread_mutex_unlock(&shared_lock);
                    break;
                case '4':
                    printf("[CMD 4] Turn Right\n");
                    pthread_mutex_lock(&shared_lock);
                    MOTOR_MOVEMENT = 2;
                    pthread_mutex_unlock(&shared_lock);
                    break;
                case '5':
                    printf("[CMD 5] Auto Mode\n");
                    break;
                case '6':
                    printf("[CMD 6] Manual Mode\n");
                    break;
                case '\r': 
                case '\n': 
                    break;
                default:
                    // 정의되지 않은 문자가 와도 일단 멈춤 처리
                    pthread_mutex_lock(&shared_lock);
                    MOTOR_MOVEMENT = 0;
                    pthread_mutex_unlock(&shared_lock);
                    break;
            }
            fflush(stdout);
        } 
        else {
            // ★ 추가: 데이터가 수신되지 않을 때 타임아웃 체크
            // (현재 시간 - 마지막 수신 시간)이 설정된 타임아웃보다 크면
            if (millis() - last_received_time > SAFETY_TIMEOUT_MS) {
                
                // 불필요한 Mutex 잠금을 막기 위해 현재 상태가 0이 아닐 때만 실행
                if (MOTOR_MOVEMENT != 0) {
                    printf("[Safety] No signal for %dms. Auto-Stop!\n", SAFETY_TIMEOUT_MS);
                    pthread_mutex_lock(&shared_lock);
                    MOTOR_MOVEMENT = 0; // 강제 정지
                    pthread_mutex_unlock(&shared_lock);
                    fflush(stdout);
                }
                
                // 타임아웃이 계속 발생할 때 last_received_time을 계속 갱신할지,
                // 아니면 한 번 멈추고 다음 데이터가 올 때까지 기다릴지는 선택 사항입니다.
                // 여기서는 다음 데이터가 올 때까지 기다리는 구조입니다.
            }
        }
        
        delay(10);
    }

    return NULL;
}