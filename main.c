#include <stdio.h>
#include <pthread.h>
#include <wiringPi.h>
#include "bluetooth.h" 
#include "sensors.h"
#include "motors.h"
#include "navigator.h"

// 공유 변수 정의 (다른 파일에서 extern으로 참조함)
// 모터 제어 변수
int MOTOR_MOVEMENT = 0;
int MOTOR_POWER = 0;
int AUTO_MODE = 0; // 0 : 수동, 1 : 자동, 기본값은 수동으로 설정

// 센서 거리 변수
float SHARED_DIST_LEFT;
float SHARED_DIST_CENTER;
float SHARED_DIST_RIGHT;
float SHARED_DIST_HYPOTENUSE;

// 뮤텍스
pthread_mutex_t shared_lock = PTHREAD_MUTEX_INITIALIZER; 

int main() {
    if (wiringPiSetupGpio() < 0) {
        perror("wiringPiSetupGpio failed");
        return 0;
    }
    
    // 블루투스 스레드, 모터, 네비게이션, 센서 쓰레드 생성
    pthread_t threads[4];
    
    // bluetooth 함수 호출
    if (pthread_create(&threads[0], NULL, bluetooth, NULL) != 0) {
        perror("Failed to create thread");
        return 1;
    }
    // sensor_thread 함수 호출
    if (pthread_create(&threads[1], NULL, sensor_thread, NULL) != 0) {
        perror("Failed to create thread");
        return 1;
    }
    // navigate 함수 호출
    if (pthread_create(&threads[2], NULL, navigate, NULL) != 0) {
        perror("Failed to create thread");
        return 1;
    }
    // motors 함수 호출
    if (pthread_create(&threads[3], NULL, motors, NULL) != 0) {
        perror("Failed to create thread");
        return 1;
    }

    printf("Main thread started, waiting for Bluetooth thread to join...\n");
    // 스레드 종료 대기
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);
    pthread_join(threads[3], NULL);
    
    printf("All thread terminated. Exiting program.\n");
    
    // 뮤텍스 해제
    pthread_mutex_destroy(&shared_lock);
    
    return 0;
}