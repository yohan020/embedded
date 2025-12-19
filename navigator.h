#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <pthread.h>

extern float SHARED_DIST_LEFT;
extern float SHARED_DIST_CENTER;
extern float SHARED_DIST_RIGHT;
extern float SHARED_DIST_HYPOTENUSE;
extern int AUTO_MODE;
extern int MOTOR_MOVEMENT;

extern pthread_mutex_t shared_lock;

void* navigate(void* arg);

#endif