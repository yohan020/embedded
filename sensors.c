#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>

// ★ GPIO 번호 (BCM) 정의 ★
// 핀맵 그림에 적힌 "GPIO xx" 숫자를 그대로 씁니다.
#define TRIG_LEFT   4
#define ECHO_LEFT   17

#define TRIG_CENTER 22
#define ECHO_CENTER 27

#define TRIG_RIGHT  9
#define ECHO_RIGHT  10

float getDistance(int trigPin, int echoPin) {
    long startTime, endTime;
    float distance;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // 타임아웃 추가 (무한대기 방지)
    int timeout = 30000; 
    while(digitalRead(echoPin) == LOW) {
        if(timeout-- <= 0) return -1;
    }
    startTime = micros();

    timeout = 30000;
    while(digitalRead(echoPin) == HIGH) {
        if(timeout-- <= 0) return -1;
    }
    endTime = micros();

    distance = (endTime - startTime) / 58.0;
    return distance;
}

int main(void) {
    // ★ 여기서 GPIO 모드로 초기화합니다 ★
    if (wiringPiSetupGpio() == -1) return 1;

    // 핀 모드 설정
    pinMode(TRIG_LEFT, OUTPUT);   pinMode(ECHO_LEFT, INPUT);
    pinMode(TRIG_CENTER, OUTPUT); pinMode(ECHO_CENTER, INPUT);
    pinMode(TRIG_RIGHT, OUTPUT);  pinMode(ECHO_RIGHT, INPUT);
    
    // 초기화
    digitalWrite(TRIG_LEFT, LOW);
    digitalWrite(TRIG_CENTER, LOW);
    digitalWrite(TRIG_RIGHT, LOW);

    printf("로봇 청소기 센서 테스트 (GPIO 모드)\n");

    while(1) {
        printf("좌: %.1fcm | 중: %.1fcm | 우: %.1fcm\n",
            getDistance(TRIG_LEFT, ECHO_LEFT),
            getDistance(TRIG_CENTER, ECHO_CENTER),
            getDistance(TRIG_RIGHT, ECHO_RIGHT)
        );
        delay(100); // 0.1초 대기
    }
    return 0;
}