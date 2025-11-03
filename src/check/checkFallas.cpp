#include <Adafruit_SH1106.h>
#include "pantallas/pantallas_menus.h"
#include "timer/timerCalc.h"
#include "lecturaTemp/lecturaTemp.h"
#include "check/checkFallas.h"
#include "global/global_utils.h"

void apagarTodo(){
    digitalWrite(RELE_ON,LOW);
    digitalWrite(BOMBA, LOW);
    EnTimer = false;              // Por las dudas apago todo.
    EnResistencia = false;
    EnPasteurizar = false;
    ResistenciaOn = false;  
}

/* Se verifica que el agua este encima del nivel minimo */
void checkNivelAgua(){  // Agua por debajo del nivel mínimo ----------------
  while(digitalRead(SNivel_C) != 0){     
    apagarTodo();
    PantallaNivelMinimo ();
    EIFR = bit(INTF1);
  }
}


/* Se verifica que el sensor de temperatura arroje medidas correctas */
void checkSensorTemp(){
  while(Temperatura < 0 || Temperatura > 99.9){   
    apagarTodo();
    PantallaFallaSensorT ();
    EIFR = bit(INTF1);
    Temperatura = leerTemp();
  }
}


/* Se verifica el exceso de temperatura del agua */
void checkExcesoTemp(){
  while(Temperatura >= 70){       
    apagarTodo();
    PantallaExcesoTemp ();
    EIFR = bit(INTF1);
    if(!ExcedeTemp){                                    // Si detecto un exceso de T, y el flag está apagado,
      CuentaExcesos = EEPROM.get(40, CuentaExcesos);    // incremento el contador de excesos y lo guardo en memoria.
      CuentaExcesos++;
      EEPROM.put(40, CuentaExcesos);
      ExcedeTemp = true;                                // Enciendo el flag para no entrar de nuevo al if, hasta que no baje la T.
    }
    bool buzzerState = false;
    for(int i = 0; i < 2500; i++){ // hace un poco de ruido
      buzzerState = !buzzerState;
      digitalWrite(BUZZER, !buzzerState);
      delayMicroseconds(200);
    }
    digitalWrite(BUZZER, LOW);
    delay(500);
    Temperatura = leerTemp();
  }
  ExcedeTemp = false;              // Reseteo el flag, siempre que no supere los 70°C.
}