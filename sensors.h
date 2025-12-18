// sensors.h
#ifndef SENSORS_H
#define SENSORS_H

#include <pthread.h> // 뮤텍스 사용을 위해 필요

// 1. 핀 정의
#define TRIG_LEFT   18
#define ECHO_LEFT   17
#define TRIG_CENTER 22
#define ECHO_CENTER 27
#define TRIG_RIGHT  9
#define ECHO_RIGHT  10

// 2. 공유 변수 선언 (extern을 붙여야 main에서 접근 가능)
extern float SHARED_DIST_LEFT;
extern float SHARED_DIST_CENTER;
extern float SHARED_DIST_RIGHT;
extern float SHARED_DIST_HYPOTENUSE;

extern pthread_mutex_t shared_lock;

// 4. 스레드 함수 선언
void *sensor_thread(void *arg);

#endif