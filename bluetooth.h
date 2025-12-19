#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <pthread.h>

extern int MOTOR_MOVEMENT;
extern int MOTOR_POWER;
extern int AUTO_MODE; // 0 : 수동, 1 : 자동

extern pthread_mutex_t shared_lock;

void *bluetooth(void *arg);

#endif // BLUETOOTH_H