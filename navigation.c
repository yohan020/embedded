#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // usleep 사용을 위해
#include <wiringPi.h> // delay 함수 사용
#include <pthread.h>
#include "navigator.h"
#include "motors.h"
// #include "motors.h"

// 일단 함수 직접 호출은 막아놓음
// [수정 필요]
// 센서에서 읽어들이는 값의 단위는 cm
// 전면 센서 값의 기준 설정 필요

// ================= 설정 상수 =================
#define DIST_HYPOTENUSE_THRESHOLD 55.0f // 대각선 거리 경계값 (cm) - 이보다 작으면 벽으로 간주
#define TURN_DELAY_MS 1700              // 회전 시 동작 시간 (ms)
#define LOOP_DELAY_MS 50                // 루프 주기 (ms)

// ================= 외부 변수 참조 =================
// 1. 센서 데이터 (main.c에 정의됨)
// extern float SHARED_DIST_LEFT;
// extern float SHARED_DIST_CENTER;       // 0: 없음, 1: 있음
// extern float SHARED_DIST_RIGHT;
// extern float SHARED_DIST_HYPOTENUSE;

// 2. 모터 상태 변수 (motors.c에 정의됨 - 상태 동기화용)
// extern int MOTOR_MOVE;

// 3. 동기화 객체
// extern pthread_mutex_t shared_lock;

// ================= 자율주행 스레드 함수 =================
void *navigate(void *arg)
{
    float local_left, local_center, local_right, local_hypotenuse;

    int motor_movement;
    int auto_mode;

    printf("[Navigator] Autonomous Driving Thread Started.\n");

    // 초기화: 안전을 위해 일단 정지
    // stop_Moving();
    pthread_mutex_lock(&shared_lock);
    MOTOR_MOVEMENT = 0; // 정지 상태
    pthread_mutex_unlock(&shared_lock);

    while (1)
    {
        pthread_mutex_lock(&shared_lock);
        auto_mode = AUTO_MODE; // 자동 모드인지 확인하기 위해 공유변수 확인
        pthread_mutex_unlock(&shared_lock);
        // 수동 모드라면 작동시키지 않음
        if (auto_mode == 0)
        {
            delay(10);
            continue;
        }
        // --------------------------------------------------------
        // 1단계: 센서 값 읽기 (Critical Section)
        // --------------------------------------------------------
        pthread_mutex_lock(&shared_lock);

        local_left = SHARED_DIST_LEFT;
        local_center = SHARED_DIST_CENTER;
        local_right = SHARED_DIST_RIGHT;
        local_hypotenuse = SHARED_DIST_HYPOTENUSE;

        pthread_mutex_unlock(&shared_lock);

        // --------------------------------------------------------
        // 2단계: 주행 알고리즘
        // --------------------------------------------------------

        // 센서 실패 시 안전하게 정지
        if (local_center < 0 || local_left < 0 || local_right < 0)
        {
            // 센서 오류 - 정지
            pthread_mutex_lock(&shared_lock);
            MOTOR_MOVEMENT = 0;
            pthread_mutex_unlock(&shared_lock);
            delay(100);
            continue;
        }
        // [상황 1] 정면에 장애물 감지 (긴급 회피)
        // 추후 수정 필요 : 센서 값 기준, 단위(현재 cm)
        if (local_center <= 10.0f)
        {
            // 1. 일단 정지
            // stop_Moving();

            // 상태 업데이트 (디버깅/모니터링용)
            pthread_mutex_lock(&shared_lock);
            MOTOR_MOVEMENT = 0;
            pthread_mutex_unlock(&shared_lock);

            printf("[Navigator] Front Obstacle Detected! Avoiding...\n");
            delay(100); // 관성 제어

            // 2. 넓은 쪽으로 회전
            if (local_left > local_right)
            {
                // 왼쪽 공간이 더 넓음 -> 좌회전
                printf("[Navigator] Turn Left (L:%.1f > R:%.1f)\n", local_left, local_right);

                pthread_mutex_lock(&shared_lock);
                MOTOR_MOVEMENT = 3; // 좌회전 상태
                pthread_mutex_unlock(&shared_lock);

                // turn_Left();
                delay(TURN_DELAY_MS); // 회전할 시간 부여
            }
            else
            {
                // 오른쪽 공간이 더 넓음 -> 우회전
                printf("[Navigator] Turn Right (R:%.1f >= L:%.1f)\n", local_right, local_left);

                pthread_mutex_lock(&shared_lock);
                MOTOR_MOVEMENT = 2; // 우회전 상태
                pthread_mutex_unlock(&shared_lock);

                // turn_Right();
                delay(TURN_DELAY_MS);
            }

            // 회전 후 잠깐 정지하여 센서 안정화
            // stop_Moving();
            pthread_mutex_lock(&shared_lock);
            MOTOR_MOVEMENT = 0;
            pthread_mutex_unlock(&shared_lock);
            delay(100);
        }
        // [상황 2] 대각선(벽/코너)이 너무 가까움 (경로 보정)
        else if (local_hypotenuse < DIST_HYPOTENUSE_THRESHOLD && local_hypotenuse > 0)
        {
            // Hypotenuse가 0인 경우는 센서 오류일 수 있으므로 제외
            printf("[Navigator] Wall Approaching (Hypotenuse: %.1f). Adjusting...\n", local_hypotenuse);

            // 벽이 가까워지면 안전한 쪽(공간이 넓은 쪽)으로 방향 전환
            if (local_left > local_right)
            {
                pthread_mutex_lock(&shared_lock);
                MOTOR_MOVEMENT = 3;
                pthread_mutex_unlock(&shared_lock);

                // turn_Left();
                delay(800); // 약간만 회전하여 경로 수정
            }
            else
            {
                pthread_mutex_lock(&shared_lock);
                MOTOR_MOVEMENT = 2;
                pthread_mutex_unlock(&shared_lock);

                // turn_Right();
                delay(800);
            }

            // 다시 전진 준비
            // stop_Moving();
            pthread_mutex_lock(&shared_lock);
            MOTOR_MOVEMENT = 0;
            pthread_mutex_unlock(&shared_lock);
        }
        // [상황 3] 장애물 없음 (전진)
        else
        {
            // 이미 전진 중이라면 불필요한 호출을 줄일 수도 있으나,
            // 확실한 제어를 위해 지속적으로 호출
            // printf("[Navigator] Path Clear. Moving Forward.\n"); // 로그 과다 방지 주석처리

            pthread_mutex_lock(&shared_lock);
            MOTOR_MOVEMENT = 1; // 전진 상태
            pthread_mutex_unlock(&shared_lock);

            // move_Forward();
        }

        // --------------------------------------------------------
        // 3단계: 루프 지연 (CPU 점유율 관리)
        // --------------------------------------------------------
        delay(LOOP_DELAY_MS);
    }

    return NULL;
}