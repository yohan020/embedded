#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <pthread.h>

extern float SHARED_DIST_LEFT;
extern float SHARED_DIST_CENTER;
extern float SHARED_DIST_RIGHT;
extern float SHARED_DIST_HYPOTENUSE;
extern int AUTO_MODE;
extern int MOTOR_MOVEMENT;

// 공유변수를 읽어들일 때 사용할 mutex
extern pthread_mutex_t shared_lock;

// 스레드용 메인 루프 함수
// main.c에서 pthread_create로 실행할 함수입니다.
void* navigate(void* arg);

#endif