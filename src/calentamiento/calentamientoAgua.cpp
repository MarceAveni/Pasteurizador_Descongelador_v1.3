#include "calentamiento/calentamientoAgua.h"
#include "global/global_utils.h"
#include "lecturaTemp/lecturaTemp.h"

//-------------------------- Calculo de temperatura y accionamiento de resistencia --------------------------------------------------------------
void CalentamientoAgua (float Histeresis) { 
  Temperatura = leerTemp();
  if(ResistenciaOn)
  SetTemp = EEPROM.get(10,SetTemp);                   //Si está encendida la resistencia, el set de temperatura es el seteado
  else
  SetTemp = EEPROM.get(10,SetTemp) - Histeresis;        //Si está apagada la resistnecia, el set de temperatura es el seteado menos la histeresis
  
  if((Temperatura < SetTemp) && EnResistencia){
    digitalWrite(RELE_ON, HIGH);
    if(ResistenciaOn == false){
      bool buzzerState = false;
      for(int i = 0; i < 1000; i++){ // hace un poco de ruido
        buzzerState = !buzzerState;
        digitalWrite(BUZZER, !buzzerState);
        delayMicroseconds(150);
      }
      digitalWrite(BUZZER, LOW);
    }
    ResistenciaOn = true;
  }
  else {
    digitalWrite(RELE_ON, LOW);
    if(ResistenciaOn == true && EnResistencia){
      bool buzzerState = false;
      for(int i = 0; i < 700; i++){ // hace un poco de ruido
        buzzerState = !buzzerState;
        digitalWrite(BUZZER, !buzzerState);
        delayMicroseconds(150);
      }
      delay(250);
      for(int i = 0; i < 700; i++){ // hace un poco de ruido
        buzzerState = !buzzerState;
        digitalWrite(BUZZER, !buzzerState);
        delayMicroseconds(150);
      }
      digitalWrite(BUZZER, LOW);
    }
    ResistenciaOn = false;
  }
}
