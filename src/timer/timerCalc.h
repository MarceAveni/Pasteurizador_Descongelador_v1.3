#ifndef TIMERCALC_H
#define TIMERCALC_H

#include <Arduino.h>

extern boolean EnTimer;  // 1=Timer funcionando; 0=Timer apagado
extern float timeLeft_ms;
extern float finishTime;
extern float minLeft;
extern float secLeft;

// Se calcula el tiempo restante
void timerPasteurizador();

#endif

