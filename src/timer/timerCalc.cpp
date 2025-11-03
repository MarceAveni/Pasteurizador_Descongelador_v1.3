#include "timer/timerCalc.h"
#include "global/global_utils.h"

boolean EnTimer = 0 ;  // 1=Timer funcionando; 0=Timer apagado
float timeLeft_ms = 0;
float finishTime = 0;
float minLeft = 0;
float secLeft = 0;


/* Se calcula el tiempo restante del pasteurizador */
void timerPasteurizador(){
  if((EnPasteurizar && (Temperatura >= EEPROM.get(10,SetTemp))) || EnTimer == true ){ // Temperatura supera el setpoint - Comienzo a contar
    EnTimer = true;
    timeLeft_ms = finishTime - millis();    // En finishTime se termina el tiempo.
    secLeft = int(timeLeft_ms/1000) % 60;
    minLeft = floor((timeLeft_ms/1000.0) / 60.0);
    if (minLeft < 0) minLeft = 0; 
    if(timeLeft_ms <= 0){
      EnTimer = false;
      EnResistencia = false;
      EnPasteurizar = false;
      ResistenciaOn = false;        // No es necesario, pero así el display cambia de estado rapidamente.
      digitalWrite(RELE_ON, LOW);   // No es necesario, solo me aseguro que se apague rápido la resisitencia.
      EEPROM.put(1,EnResistencia);
      bool buzzerState = false;
      for(int i = 0; i < 2500; i++){ // hace un poco de ruido
        buzzerState = !buzzerState;
        digitalWrite(BUZZER, !buzzerState);
        delayMicroseconds(200);
      }
      delay(500);
      for(int i = 0; i < 2500; i++){ // hace un poco de ruido
        buzzerState = !buzzerState;
        digitalWrite(BUZZER, !buzzerState);
        delayMicroseconds(210);
      }
      delay(500);
      for(int i = 0; i < 2500; i++){ // hace un poco de ruido
        buzzerState = !buzzerState;
        digitalWrite(BUZZER, !buzzerState);
        delayMicroseconds(220);
      }
      digitalWrite(BUZZER, LOW);
    } 
  }
  else{
    timeLeft_ms = SetTime*1000*60; // queda todo el tiempo restante
    secLeft = int(timeLeft_ms/1000.0) % 60;
    minLeft = floor((timeLeft_ms/1000.0) / 60);    
    finishTime = millis() + SetTime*60*1000; // el "finishTime" se actualiza periodicamente
  }
}
