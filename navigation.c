#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <pthread.h>
#include "navigator.h"
#include "motors.h"

// 센서에서 읽어들이는 값의 단위는 cm

// 설정 상수
#define DIST_HYPOTENUSE_THRESHOLD 70.0f // 대각선 거리 경계값 (cm) - 이보다 작으면 벽으로 간주
#define TURN_DELAY_MS 1700              // 회전 시 동작 시간 (ms)
#define LOOP_DELAY_MS 50                // 루프 주기 (ms)

// 자율주행
void *navigate(void *arg)
{
    float local_left, local_center, local_right, local_hypotenuse;

    int motor_movement;
    int auto_mode;

    printf("[Navigator] Autonomous Driving Thread Started.\n");

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

        // 센서 값 읽기 (Critical Section)
        pthread_mutex_lock(&shared_lock);
        local_left = SHARED_DIST_LEFT;
        local_center = SHARED_DIST_CENTER;
        local_right = SHARED_DIST_RIGHT;
        local_hypotenuse = SHARED_DIST_HYPOTENUSE;
        pthread_mutex_unlock(&shared_lock);

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
        // 정면에 장애물 감지 (회피)
        if (local_center <= 10.0f)
        {
            // 상태 업데이트
            pthread_mutex_lock(&shared_lock);
            MOTOR_MOVEMENT = 0;
            pthread_mutex_unlock(&shared_lock);

            printf("[Navigator] Front Obstacle Detected! Avoiding...\n");
            delay(100); // 정지 후 이동

            // 넓은 쪽으로 회전
            if (local_left > local_right)
            {
                // 왼쪽 공간이 더 넓음 -> 좌회전
                printf("[Navigator] Turn Left (L:%.1f > R:%.1f)\n", local_left, local_right);

                pthread_mutex_lock(&shared_lock);
                MOTOR_MOVEMENT = 3; // 좌회전 상태
                pthread_mutex_unlock(&shared_lock);

                delay(TURN_DELAY_MS); // 회전할 시간 확보
            }
            else
            {
                // 오른쪽 공간이 더 넓음 -> 우회전
                printf("[Navigator] Turn Right (R:%.1f >= L:%.1f)\n", local_right, local_left);

                pthread_mutex_lock(&shared_lock);
                MOTOR_MOVEMENT = 2; // 우회전 상태
                pthread_mutex_unlock(&shared_lock);

                delay(TURN_DELAY_MS);
            }

            // 회전 후 잠깐 정지하고 출발
            pthread_mutex_lock(&shared_lock);
            MOTOR_MOVEMENT = 0;
            pthread_mutex_unlock(&shared_lock);
            delay(100);
        }
        // 대각선(벽/코너)이 너무 가까움
        else if (local_hypotenuse < DIST_HYPOTENUSE_THRESHOLD && local_hypotenuse > 0)
        {
            printf("[Navigator] Wall Approaching (Hypotenuse: %.1f). Adjusting...\n", local_hypotenuse);

            // 벽이 가까워지면 안전한 쪽(공간이 넓은 쪽)으로 방향 전환
            if (local_left > local_right)
            {
                pthread_mutex_lock(&shared_lock);
                MOTOR_MOVEMENT = 3;
                pthread_mutex_unlock(&shared_lock);

                delay(800); // 약간만 회전하여 경로 수정
            }
            else
            {
                pthread_mutex_lock(&shared_lock);
                MOTOR_MOVEMENT = 2;
                pthread_mutex_unlock(&shared_lock);

                delay(800);
            }

            pthread_mutex_lock(&shared_lock);
            MOTOR_MOVEMENT = 0;
            pthread_mutex_unlock(&shared_lock);
        }
        // 장애물 없음 (전진)
        else
        {
            pthread_mutex_lock(&shared_lock);
            MOTOR_MOVEMENT = 1; // 전진 상태
            pthread_mutex_unlock(&shared_lock);
        }

        // 잠시 딜레이
        delay(LOOP_DELAY_MS);
    }

    return NULL;
}