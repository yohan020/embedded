#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "motors.h"

// 커밋 테스트

// ================= PIN 정의 (BCM GPIO 번호) =================
// 사용하시는 모터 드라이버 배선에 맞춰 번호를 수정하세요.

// 1. 바퀴 (기어박스) 모터 - L298N 같은 드라이버 사용 가정
#define LEFT_MOTOR_PIN1  14
#define LEFT_MOTOR_PIN2  15
#define RIGHT_MOTOR_PIN1 3
#define RIGHT_MOTOR_PIN2 2

// 2. 브러쉬 모터 (2개) - 각각 2핀으로 제어 (바퀴 모터와 동일 방식)
#define BRUSH_PIN_L1     20
#define BRUSH_PIN_L2     21
#define BRUSH_PIN_R1     7
#define BRUSH_PIN_R2     8

// 3. 흡입 모터 (1개) - MOSFET 모듈 등을 사용 가정
// #define SUCTION_PIN      26
#define SUCTION_PIN      12

// ================= 설정 상수 =================
#define SPEED_MOVE     70   // 바퀴 이동 속도 (0~100)
#define SPEED_TURN     70   // 회전 시 속도 (0~100)
#define SPEED_BRUSH    50  // 브러쉬 속도 (일정하게 돎)
#define SPEED_SUCTION  99  // 흡입 모터 속도 (최대)

// ================= 초기화 함수 =================
void motor_Init() {
    // 1. 핀 모드 설정 불필요 (softPwmCreate가 출력으로 자동 설정함)

    // 2. 소프트웨어 PWM 생성 (핀번호, 초기값, 범위)
    // 범위는 100으로 설정 (0~100% 듀티비)
    softPwmCreate(LEFT_MOTOR_PIN1, 0, 100);
    softPwmCreate(LEFT_MOTOR_PIN2, 0, 100);
    softPwmCreate(RIGHT_MOTOR_PIN1, 0, 100);
    softPwmCreate(RIGHT_MOTOR_PIN2, 0, 100);
    
    softPwmCreate(BRUSH_PIN_L1, 0, 100);
    softPwmCreate(BRUSH_PIN_L2, 0, 100);
    softPwmCreate(BRUSH_PIN_R1, 0, 100);
    softPwmCreate(BRUSH_PIN_R2, 0, 100);
    
    //softPwmCreate(SUCTION_PIN, 0, 100);
    pinMode(SUCTION_PIN, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(738);  // 세밀한 제어 및 고주파 PWM을 위해 1000으로 설정
    pwmSetClock(2);    // 19.2MHz ÷ 96 ÷ 1000 = 20kHz (모터 최적 주파수)
    pwmWrite(SUCTION_PIN, 0);
    
    printf("[모터 준비]\n");
}

// ================= 기능별 함수 구현 =================

// 1. 바퀴 제어 함수들

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
    // 우회전 전 정지
    // stop_Moving();
    // 제자리 우회전 (탱크 턴): 왼쪽 전진, 오른쪽 후진
    softPwmWrite(LEFT_MOTOR_PIN1, SPEED_TURN);
    softPwmWrite(LEFT_MOTOR_PIN2, 0);
    
    softPwmWrite(RIGHT_MOTOR_PIN1, 0);
    softPwmWrite(RIGHT_MOTOR_PIN2, SPEED_TURN);
    printf("[우회전중...]\n");
}

// 좌회전
void turn_Left() {
    // 좌회전 전 정지
    // stop_Moving();
    // 제자리 좌회전: 왼쪽 후진, 오른쪽 전진
    softPwmWrite(LEFT_MOTOR_PIN1, 0);
    softPwmWrite(LEFT_MOTOR_PIN2, SPEED_TURN);
    
    softPwmWrite(RIGHT_MOTOR_PIN1, SPEED_TURN);
    softPwmWrite(RIGHT_MOTOR_PIN2, 0);
    printf("[좌회전중...]\n");
}

// 2. 브러쉬 제어 함수 (On/Off)
// state: 1 = 켜기, 0 = 끄기
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

// 3. 흡입 모터 제어 함수 (On/Off)
// state: 1 = 켜기, 0 = 끄기
void control_Suction(int state) {
    if (state == 1) {
        // 흡입 모터는 초기 기동 전류가 높으므로 서서히 올리는 것(Soft Start)이 좋을 수 있음
        // 여기서는 단순 On으로 구현
        //softPwmWrite(SUCTION_PIN, SPEED_SUCTION);
        pwmWrite(SUCTION_PIN, 738);  // 100% 듀티비 (range 1000 기준)
        printf("[흡입기] ON (Full Power 20kHz)\n");
    } else {
        //softPwmWrite(SUCTION_PIN, 0);
        pwmWrite(SUCTION_PIN, 0);
        printf("[흡입기] OFF\n");
    }
}

void force_Stop_All() {
    // 1. 바퀴 모터 0 설정
    softPwmWrite(LEFT_MOTOR_PIN1, 0);
    softPwmWrite(LEFT_MOTOR_PIN2, 0);
    softPwmWrite(RIGHT_MOTOR_PIN1, 0);
    softPwmWrite(RIGHT_MOTOR_PIN2, 0);

    // 2. 브러쉬 모터 0 설정
    softPwmWrite(BRUSH_PIN_L1, 0);
    softPwmWrite(BRUSH_PIN_L2, 0);
    softPwmWrite(BRUSH_PIN_R1, 0);
    softPwmWrite(BRUSH_PIN_R2, 0);

    // 3. 흡입 모터 0 설정
    //softPwmWrite(SUCTION_PIN, 0);
    pwmWrite(SUCTION_PIN, 0);

    printf("[모두 정지]\n");
}

void force_Stop_All_Init() {
    // softPwm 스레드를 먼저 정지시킨 후 LOW 설정
    
    // 1. 바퀴 모터 PWM 정지 후 LOW
    softPwmStop(LEFT_MOTOR_PIN1);
    softPwmStop(LEFT_MOTOR_PIN2);
    softPwmStop(RIGHT_MOTOR_PIN1);
    softPwmStop(RIGHT_MOTOR_PIN2);
    
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);

    // 2. 브러쉬 모터 PWM 정지 후 LOW
    softPwmStop(BRUSH_PIN_L1);
    softPwmStop(BRUSH_PIN_L2);
    softPwmStop(BRUSH_PIN_R1);
    softPwmStop(BRUSH_PIN_R2);
    
    digitalWrite(BRUSH_PIN_L1, LOW);
    digitalWrite(BRUSH_PIN_L2, LOW);
    digitalWrite(BRUSH_PIN_R1, LOW);
    digitalWrite(BRUSH_PIN_R2, LOW);

    // 3. 흡입 모터 PWM 정지 후 LOW
    //softPwmStop(SUCTION_PIN);
    pinMode(SUCTION_PIN, OUTPUT);
    digitalWrite(SUCTION_PIN, LOW);

    printf("[모두 정지]\n");
}

void* motors(void *arg) {
    /*
    if (wiringPiSetupGpio() == -1) {
        printf("WiringPi setup failed!\n");
        return 1;
    }
    */
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
        // MOTOR_POWER가 0일 때(작동 중이 아닐 때) 명령이 들어오더라도 패스
        else{
            force_Stop_All();
            prev_motor_movement = -1;  // 상태 리셋
            delay(10);
            continue;
        }
        delay(10);
    }
}