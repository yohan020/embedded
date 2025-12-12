#ifndef MOTORS_H
#define MOTORS_H

#include <pthread.h>
#include <wiringPi.h>

// 공유변수를 읽어들일 때 사용할 mutex
extern pthread_mutex_t shared_lock;

extern int MOTOR_MOVEMENT;
// 0 : 정지
// 1 : 전진
// 2 : 우회전
// 3 : 좌회전
extern int MOTOR_POWER;
// 0 : 종료, 흡입 모터 off, 브러쉬 off
// 1 : 시작, 흡입 모터 on, 브러쉬 on

// 초기화 및 정지 함수
void motor_Init();
void force_Stop_All();

// 개별 동작 제어 함수 (필요 시 외부에서 직접 호출 가능)
void stop_Moving();
void move_Forward();
void turn_Right();
void turn_Left();
void control_Brush(int state);
void control_Suction(int state);

// 스레드용 메인 루프 함수
// main.c에서 pthread_create로 실행할 함수입니다.
void* motors(void* arg);

#endif