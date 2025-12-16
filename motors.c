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
#define BRUSH_PIN_R1     8
#define BRUSH_PIN_R2     9

// 3. 흡입 모터 (1개) - MOSFET 모듈 등을 사용 가정
#define SUCTION_PIN      26

// ================= 설정 상수 =================
#define SPEED_MOVE     70   // 바퀴 이동 속도 (0~100)
#define SPEED_TURN     70   // 회전 시 속도 (0~100)
#define SPEED_BRUSH    13  // 브러쉬 속도 (일정하게 돎)
#define SPEED_SUCTION  80  // 흡입 모터 속도 (최대)

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
    
    softPwmCreate(SUCTION_PIN, 0, 100);
    
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
        softPwmWrite(SUCTION_PIN, SPEED_SUCTION);
        printf("[흡입기] ON\n");
    } else {
        softPwmWrite(SUCTION_PIN, 0);
        printf("[흡입기] OFF\n");
    }
}

void force_Stop_All() {
    // 1. 바퀴 모터 강제 LOW
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);

    // 2. 브러쉬 모터 강제 LOW
    digitalWrite(BRUSH_PIN_L1, LOW);
    digitalWrite(BRUSH_PIN_L2, LOW);
    digitalWrite(BRUSH_PIN_R1, LOW);
    digitalWrite(BRUSH_PIN_R2, LOW);

    // 3. 흡입 모터 강제 LOW
    digitalWrite(SUCTION_PIN, LOW);

    printf("[모두 정지]\n");
}

/* 모터 테스트용 코드
int main() {
    if (wiringPiSetupGpio() == -1) {
        //printf("WiringPi setup failed!\n");
        return 1;
    }

    motor_Init();
    force_Stop_All();
    //printf("Cleaning Robot Control Ready.\n");
    //printf("Keys: w(Forward), a(Left), d(Right), s(Stop), b(Brush Toggle), v(Suction Toggle), q(Quit)\n");

    char cmd;
    int brush_state = 0;
    int suction_state = 0;

    while (1) {
        //printf("Command: ");
        scanf(" %c", &cmd);

        switch (cmd) {
            case 'w': move_Forward(); break;
            case 'a': turn_Left(); break;
            case 'd': turn_Right(); break;
            case 's': stop_Moving(); break;
            
            case 'b': // 브러쉬 토글
                brush_state = !brush_state;
                control_Brush(brush_state);
                break;

            case 'v': // 흡입 토글
                suction_state = !suction_state;
                control_Suction(suction_state);
                break;

            case 'q': // 종료
                stop_Moving();
                control_Brush(0);
                control_Suction(0);
                //printf("Program Exit.\n");
                return 0;

            default:
                //printf("Invalid Command.\n");
                break;
        }
    }

    return 0;
}
*/

void* motors(void *arg) {
    /*
    if (wiringPiSetupGpio() == -1) {
        printf("WiringPi setup failed!\n");
        return 1;
    }
    */
    int motor_power;
    int motor_movement;

    motor_Init();
    force_Stop_All();
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
        }
        // MOTOR_POWER가 0일 때(작동 중이 아닐 때) 명령이 들어오더라도 패스
        else{
            force_Stop_All();
            delay(10);
            continue;
        }
        delay(10);
    }
}