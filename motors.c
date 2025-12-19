#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "motors.h"

// 바퀴 (기어박스) 모터
#define LEFT_MOTOR_PIN1  14
#define LEFT_MOTOR_PIN2  15
#define RIGHT_MOTOR_PIN1 3
#define RIGHT_MOTOR_PIN2 2

// 브러쉬 모터 (2개) - 각각 2핀으로 제어
#define BRUSH_PIN_L1     20
#define BRUSH_PIN_L2     21
#define BRUSH_PIN_R1     7
#define BRUSH_PIN_R2     8

// 흡입 모터 (1개)
// #define SUCTION_PIN      26
#define SUCTION_PIN      12

// 설정 상수
#define SPEED_MOVE     70   // 바퀴 이동 속도 (0~100)
#define SPEED_TURN     70   // 회전 시 속도 (0~100)
#define SPEED_BRUSH    50   // 브러쉬 속도 (일정하게 돎)
#define SPEED_SUCTION  99   // 흡입 모터 속도 (최대)

// 초기화 함수
void motor_Init() {
    // 모터
    softPwmCreate(LEFT_MOTOR_PIN1, 0, 100);
    softPwmCreate(LEFT_MOTOR_PIN2, 0, 100);
    softPwmCreate(RIGHT_MOTOR_PIN1, 0, 100);
    softPwmCreate(RIGHT_MOTOR_PIN2, 0, 100);
    // 브러쉬
    softPwmCreate(BRUSH_PIN_L1, 0, 100);
    softPwmCreate(BRUSH_PIN_L2, 0, 100);
    softPwmCreate(BRUSH_PIN_R1, 0, 100);
    softPwmCreate(BRUSH_PIN_R2, 0, 100);
    // 흡입 모터
    pinMode(SUCTION_PIN, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(738);
    pwmSetClock(2);
    pwmWrite(SUCTION_PIN, 0);
    
    printf("[모터 준비]\n");
}

// 정지
void stop_Moving() {
    softPwmWrite(LEFT_MOTOR_PIN1, 0);
    softPwmWrite(LEFT_MOTOR_PIN2, 0);
    softPwmWrite(RIGHT_MOTOR_PIN1, 0);
    softPwmWrite(RIGHT_MOTOR_PIN2, 0);
    printf("[정지]\n");
}

// 전진
void move_Forward() {
    // 왼쪽 전진
    softPwmWrite(LEFT_MOTOR_PIN1, SPEED_MOVE);
    softPwmWrite(LEFT_MOTOR_PIN2, 0);
    // 오른쪽 전진
    softPwmWrite(RIGHT_MOTOR_PIN1, SPEED_MOVE);
    softPwmWrite(RIGHT_MOTOR_PIN2, 0);
    printf("[전진중...]\n");
}

// 우회전
void turn_Right() {
    // 제자리 우회전 : 왼쪽 전진, 오른쪽 후진
    softPwmWrite(LEFT_MOTOR_PIN1, SPEED_TURN);
    softPwmWrite(LEFT_MOTOR_PIN2, 0);
    
    softPwmWrite(RIGHT_MOTOR_PIN1, 0);
    softPwmWrite(RIGHT_MOTOR_PIN2, SPEED_TURN);
    printf("[우회전중...]\n");
}

// 좌회전
void turn_Left() {
    // 제자리 좌회전: 왼쪽 후진, 오른쪽 전진
    softPwmWrite(LEFT_MOTOR_PIN1, 0);
    softPwmWrite(LEFT_MOTOR_PIN2, SPEED_TURN);
    
    softPwmWrite(RIGHT_MOTOR_PIN1, SPEED_TURN);
    softPwmWrite(RIGHT_MOTOR_PIN2, 0);
    printf("[좌회전중...]\n");
}

// 브러쉬 제어 함수 (On/Off)
void control_Brush(int state) {
    if (state == 1) {
        // 왼쪽 브러쉬 회전
        softPwmWrite(BRUSH_PIN_L1, SPEED_BRUSH);
        softPwmWrite(BRUSH_PIN_L2, 0);
        // 오른쪽 브러쉬 회전
        softPwmWrite(BRUSH_PIN_R1, 0);
        softPwmWrite(BRUSH_PIN_R2, SPEED_BRUSH);
        printf("[브러쉬] ON\n");
    } else {
        softPwmWrite(BRUSH_PIN_L1, 0);
        softPwmWrite(BRUSH_PIN_L2, 0);
        softPwmWrite(BRUSH_PIN_R1, 0);
        softPwmWrite(BRUSH_PIN_R2, 0);
        printf("[브러쉬] OFF\n");
    }
}

// 흡입 모터 제어 함수 (On/Off)
// state: 1 = 켜기, 0 = 끄기
void control_Suction(int state) {
    if (state == 1) {
        //softPwmWrite(SUCTION_PIN, SPEED_SUCTION);
        pwmWrite(SUCTION_PIN, 738);
        printf("[흡입기] ON (Full Power 20kHz)\n");
    } else {
        //softPwmWrite(SUCTION_PIN, 0);
        pwmWrite(SUCTION_PIN, 0);
        printf("[흡입기] OFF\n");
    }
}

// 정지
void force_Stop_All() {
    // 바퀴 모터 정지
    softPwmWrite(LEFT_MOTOR_PIN1, 0);
    softPwmWrite(LEFT_MOTOR_PIN2, 0);
    softPwmWrite(RIGHT_MOTOR_PIN1, 0);
    softPwmWrite(RIGHT_MOTOR_PIN2, 0);

    // 브러쉬 모터 정지
    softPwmWrite(BRUSH_PIN_L1, 0);
    softPwmWrite(BRUSH_PIN_L2, 0);
    softPwmWrite(BRUSH_PIN_R1, 0);
    softPwmWrite(BRUSH_PIN_R2, 0);

    // 흡입 모터 정지
    //softPwmWrite(SUCTION_PIN, 0);
    pwmWrite(SUCTION_PIN, 0);

    printf("[모두 정지]\n");
}

// 초기 강제정지
void force_Stop_All_Init() {
    // 바퀴 모터
    softPwmStop(LEFT_MOTOR_PIN1);
    softPwmStop(LEFT_MOTOR_PIN2);
    softPwmStop(RIGHT_MOTOR_PIN1);
    softPwmStop(RIGHT_MOTOR_PIN2);
    
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);

    // 브러쉬 모터
    softPwmStop(BRUSH_PIN_L1);
    softPwmStop(BRUSH_PIN_L2);
    softPwmStop(BRUSH_PIN_R1);
    softPwmStop(BRUSH_PIN_R2);
    
    digitalWrite(BRUSH_PIN_L1, LOW);
    digitalWrite(BRUSH_PIN_L2, LOW);
    digitalWrite(BRUSH_PIN_R1, LOW);
    digitalWrite(BRUSH_PIN_R2, LOW);

    // 흡입 모터
    //softPwmStop(SUCTION_PIN);
    pinMode(SUCTION_PIN, OUTPUT);
    digitalWrite(SUCTION_PIN, LOW);

    printf("[모두 정지]\n");
}

void* motors(void *arg) {
    int motor_power;
    int motor_movement;
    int prev_motor_movement = -1;  // 이전 상태 저장 (초기값 -1)

    force_Stop_All_Init();
    motor_Init();
    printf("Cleaning Robot Control Ready.\n");
    // 공유변수를 계속 읽어들임
    while (1) {
        // MOTOR_POWER 확인
        pthread_mutex_lock(&shared_lock);
        motor_power = MOTOR_POWER;
        pthread_mutex_unlock(&shared_lock);

        // MOTOR_POWER가 1일 때(작동 중) 명령 받기
        if (motor_power == 1)
        {
            control_Brush(1);
            control_Suction(1);
            
            // MOTOR_MOVE 확인
            pthread_mutex_lock(&shared_lock);
            motor_movement = MOTOR_MOVEMENT;
            pthread_mutex_unlock(&shared_lock);
            
            // 상태가 변경되었을 때만 동작 함수 호출
            if (motor_movement != prev_motor_movement)
            {
                switch (motor_movement)
                {
                case 0:
                    stop_Moving();
                    break;
                case 1:
                    move_Forward();
                    break;
                case 2:
                    turn_Right();
                    break;
                case 3:
                    turn_Left();
                    break;
                default:
                    break;
                }
                prev_motor_movement = motor_movement;  // 상태 업데이트
            }
        }
        // MOTOR_POWER가 0일 때(작동 중이 아닐 때) 명령이 들어오더라도 패스함
        else{
            force_Stop_All();
            prev_motor_movement = -1;  // 상태 리셋
            delay(10);
            continue;
        }
        delay(10);
    }
}