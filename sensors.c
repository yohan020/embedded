// sensors.c
#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include "sensors.h"

// 내부 거리 계산 함수 (외부로 노출 안 함)
float measure_distance(int trigPin, int echoPin)
{
    long startTime, endTime;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    int timeout = 30000;
    while (digitalRead(echoPin) == LOW)
    {
        if (timeout-- <= 0)
            return -1.0;
    }
    startTime = micros();

    timeout = 30000;
    while (digitalRead(echoPin) == HIGH)
    {
        if (timeout-- <= 0)
            return -1.0;
    }
    endTime = micros();

    return (endTime - startTime) / 58.0;
}

// 센서 스레드 (백그라운드에서 계속 값 갱신)
void *sensor_thread(void *arg)
{
    pinMode(TRIG_LEFT, OUTPUT);
    pinMode(ECHO_LEFT, INPUT);
    pinMode(TRIG_CENTER, OUTPUT);
    pinMode(ECHO_CENTER, INPUT);
    pinMode(TRIG_RIGHT, OUTPUT);
    pinMode(ECHO_RIGHT, INPUT);

    digitalWrite(TRIG_LEFT, LOW);
    digitalWrite(TRIG_CENTER, LOW);
    digitalWrite(TRIG_RIGHT, LOW);

    printf("Sensor Thread Started...\n");

    while (1)
    {
        float l = measure_distance(TRIG_LEFT, ECHO_LEFT);
        float c = measure_distance(TRIG_CENTER, ECHO_CENTER);
        float r = measure_distance(TRIG_RIGHT, ECHO_RIGHT);
        // 빗변 거리 측정, 작아질 수록 사선으로 가까워짐
        
        float hypotenuse;
        if (l > 0 && r > 0 && c > 0)
        {
            // 왼쪽 전방 대각선 빗변 거리
            float left_hyp = sqrt((17.0 + c) * (17.0 + c) + (17.0 + l) * (17.0 + l));

            // 오른쪽 전방 대각선 빗변 거리
            float right_hyp = sqrt((17.0 + c) * (17.0 + c) + (17.0 + r) * (17.0 + r));

            // 둘 중 작은 값 = 더 가까운 대각선 벽
            if (left_hyp < right_hyp) {
                hypotenuse = left_hyp;
            } else {
                hypotenuse = right_hyp;
            }
        }
        else
        {
            hypotenuse = -1.0; // 측정 실패 표시
        }
        // 공유 변수 업데이트 (쓰기)
        // main에서 읽는 도중에 값이 바뀌면 꼬일 수 있으니 lock
        pthread_mutex_lock(&shared_lock);
        SHARED_DIST_LEFT = l;
        SHARED_DIST_CENTER = c;
        SHARED_DIST_RIGHT = r;
        SHARED_DIST_HYPOTENUSE = hypotenuse;
        pthread_mutex_unlock(&shared_lock);

        delay(100);
    }
    return NULL;
}