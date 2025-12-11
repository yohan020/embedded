#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <pthread.h>

// main.c에 정의된 공유 변수 선언
extern int MOTOR_MOVEMENT;
extern int MOTOR_POWER;
extern int AUTO_MODE; // 0 : 수동, 1 : 자동

// main.c에 정의된 뮤텍스 선언
extern pthread_mutex_t shared_lock;

// 스레드 함수 프로토타입 선언 (Pthread 표준 시그니처 준수)
// void *bluetooth(void *arg) 형태로 수정
void *bluetooth(void *arg);

#endif // BLUETOOTH_H