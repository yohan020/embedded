#include <stdio.h>
#include <pthread.h>
#include <wiringPi.h>
// 🚨 수정: #include "bluetooth.c" 대신 헤더 파일 포함
#include "bluetooth.h" 
#include "sensors.h"

// 공유 변수 정의 (다른 파일에서 extern으로 참조함)
// 1. 모터 제어 변수
int MOTOR_MOVEMENT;
int MOTOR_POWER;
int AUTO_MODE; // 0 : 수동, 1 : 자동

// 2. 센서 거리 변수
float SHARED_DIST_LEFT;
float SHARED_DIST_CENTER;
float SHARED_DIST_RIGHT;
float SHARED_DIST_HYPOTENUSE;

// 뮤텍스 정의
pthread_mutex_t shared_lock = PTHREAD_MUTEX_INITIALIZER; 

int main() {
    // wiringPi 초기화는 스레드 내부가 아닌 main에서 먼저 호출하는 것이 일반적입니다.
    // 하지만 일단 bluetooth 스레드 내부에 wiringPiSetupGpio()가 있으므로 유지합니다.
    
    // 블루투스 스레드 하나만 생성
    pthread_t threads[1];
    
    // Pthread 표준 시그니처를 따르는 bluetooth 함수를 호출합니다.
    if (pthread_create(&threads[0], NULL, bluetooth, NULL) != 0) {
        perror("Failed to create thread");
        return 1;
    }
    
    while(1) {
        pthread_mutex_lock(&shared_lock);
        printf("%d", MOTOR_MOVEMENT);
        pthread_mutex_unlock(&shared_lock);
    }
    printf("Main thread started, waiting for Bluetooth thread to join...\n");
    // 스레드 종료 대기
    pthread_join(threads[0], NULL);
    
    printf("Bluetooth thread terminated. Exiting program.\n");
    
    // 뮤텍스 해제
    pthread_mutex_destroy(&shared_lock);
    
    return 0;
}