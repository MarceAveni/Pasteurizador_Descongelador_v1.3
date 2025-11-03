#include <Adafruit_SH1106.h>
#include "pantallas/pantallas_menus.h"
#include "global/global_utils.h"
#include "timer/timerCalc.h"

volatile boolean EntroMENU = false;       //Bandera que avisa si entró al menú seleccionado o no
byte FilaMENU = 0;                        //Variable que indica la fila seleccionada en el menú
volatile unsigned long TiempoMENU = 0;    //Mide el tiempo que transcurre dentro de un menú sin acciones por parte del usuario
byte N_FILAS = 5;

/* Funcion ISR 
 > Queda aca porque es utilizada, en la mayoria de los casos, por las pantallas.
 > Ademas, esta asociada a el funcionamiento de los botones del menu/pantallas.
*/

/* Interrupción externa para botón de menú */
void ISR_botonMenu  () {
  detachInterrupt(digitalPinToInterrupt(3));
  EntroMENU = true;
  bool buzzerState = false;
  for(int i = 0; i < 500; i++){ // hace un Beep
    buzzerState = !buzzerState;
    digitalWrite(BUZZER, !buzzerState);
    delayMicroseconds(150);
  }
  digitalWrite(BUZZER,LOW);
  EIFR = bit(INTF1);
  attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
}
/* FIN rutina ISR */


/* Pantalla Menu Principal */
void MenuPrincipal(){
  detachInterrupt(digitalPinToInterrupt(3));
  EntroMENU = false;
  PantallaMenuPrincipal(&FilaMENU);
  TiempoMENU = millis() + DelayMENU;
  if (EnResistencia) N_FILAS = 4;
  else N_FILAS = 6;
  
  while(!EntroMENU && (TiempoMENU > millis()) ){
    EIFR = bit(INTF1); // estaba comentado
    attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
    if (digitalRead(MENOS) == HIGH) {
      detachInterrupt(digitalPinToInterrupt(3));
      FilaMENU = (FilaMENU+1) % N_FILAS; // Roll-over de fila
      PantallaMenuPrincipal(&FilaMENU);
      delay(500);
      TiempoMENU = millis() + DelayMENU;
      EIFR = bit(INTF0);
      attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
    }
    if (digitalRead(MAS) == HIGH) {
      detachInterrupt(digitalPinToInterrupt(3));
      FilaMENU--;
      if (FilaMENU == 255) FilaMENU = N_FILAS-1;  // (255 por el OverFlow del byte)  
      else FilaMENU = FilaMENU % N_FILAS; // Roll-over de fila  
      PantallaMenuPrincipal(&FilaMENU);
      delay(500);
      TiempoMENU = millis() + DelayMENU;
      EIFR = bit(INTF0);
      attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
    }
  }
    detachInterrupt(digitalPinToInterrupt(3)); //Desactivo la interrupción porque entro al switch de los submenú
    if(EntroMENU){                             //Entra a los menús secundarios
      EntroMENU=false;
      switch(FilaMENU){
        case 0:
        break;
        
        case 1:              
        MenuSetTemp();     //Menú de seteo de temperatura objetivo
        break;

        case 2:
        MenuTimer();   //Menú de selección de Bomba de Agua (Auto o Apagada)
        break; 
                               
        case 3:         // Pasteurizar.
        if (!EnResistencia){
          EnResistencia = true;
          EnPasteurizar = true;
        }
        else {          // Parar - Stop
          EnResistencia = false;
          EnPasteurizar = false;
          EnTimer = false;
        }
        break; 
                                      
        case 4:         // Descongelar
        EnResistencia = true;
        break;
        
        case 5:        
        MenuBombaAgua();   //Menú de selección de Bomba de Agua (Auto o Apagada)
        break;
      }
  }
  EntroMENU=false;
  FilaMENU=0;  
}

//-------------------------------------- Menú seteo de temperatura objetivo ---------------------------------------------------------------------
void MenuSetTemp() {
  EEPROM.get(10,SetTemp);
  PantallaSetTemp ();
  TiempoMENU = millis()+DelayMENU;
  while(!EntroMENU && (TiempoMENU > millis())){                        //Mira los botones + y - . Sale a los "TiempoMENU" milisegundos.
    byte i = 1;
    attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
    while(digitalRead(MAS)==HIGH && (SetTemp<TempMAX)){
      detachInterrupt(digitalPinToInterrupt(3));  
      SetTemp = SetTemp + 0.1*i;
      i++;
      if(SetTemp >= TempMAX)
      SetTemp = TempMAX;
      PantallaSetTemp();
      delay(250);
      TiempoMENU = millis()+DelayMENU;
      EIFR = bit(INTF1);
      attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
    }
    while(digitalRead(MENOS)==HIGH && (SetTemp>TempMIN)){
      detachInterrupt(digitalPinToInterrupt(3));
      SetTemp = SetTemp - 0.1*i;
      i++;
      if(SetTemp <= TempMIN)
      SetTemp = TempMIN;
      PantallaSetTemp();
      delay(250);
      TiempoMENU = millis()+DelayMENU;
      EIFR = bit(INTF1);
      attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
    }
  }
  detachInterrupt(digitalPinToInterrupt(3));
  if(EntroMENU){
    EntroMENU=false;
    EEPROM.put(10,SetTemp);
  }  
}

//-------------------------------------- Menú Bomba de Agua -------------------------------------------------------------------------------------------
void MenuBombaAgua () {
  EEPROM.get(0,EnBomba);
  PantallaBomba ();
  TiempoMENU = millis()+DelayMENU;
  while( (!EntroMENU && TiempoMENU > millis()) ){                        //Mira los botones + y - . Sale a los "TiempoMENU" milisegundos.
    attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
    if (digitalRead(MAS) == HIGH || digitalRead(MENOS) == HIGH) {
      detachInterrupt(digitalPinToInterrupt(3));
      EnBomba = !EnBomba;
      PantallaBomba();
      delay(250);
      TiempoMENU = millis() + DelayMENU;
    }
  }
  EntroMENU=false;          //Se guarda siempre el estado seleccionado para la Bomba, lo hayan o no aceptado con el botón "MENU"
  EEPROM.put(0,EnBomba);    //por ejemplo si sale solo del menú, porque pasó el "TiempoMENU".
}

//-------------------------------------------------- Menu TIMER ----------------------------------------

void MenuTimer () {
  EEPROM.get(30,SetTime);
  PantallaSetTimer();
  TiempoMENU = millis() + DelayMENU;
  while(!EntroMENU && (TiempoMENU > millis()) ){                        //Mira los botones + y - . Sale a los "TiempoMENU" milisegundos.
    byte i = 1;
    EIFR = bit(INTF1); // estaba comentado
    attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
    while(digitalRead(MAS)==HIGH && (SetTime<TimeMAX)){
      detachInterrupt(digitalPinToInterrupt(3));  
      SetTime = SetTime + i;
      i++;
      if(SetTime >= TimeMAX)
      SetTime = TimeMAX;
      PantallaSetTimer();
      delay(250);
      TiempoMENU = millis()+DelayMENU;
      EIFR = bit(INTF1);
      attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
    }
    while(digitalRead(MENOS)==HIGH && (SetTime>TimeMIN)){
      detachInterrupt(digitalPinToInterrupt(3));
      SetTime = SetTime - i;
      i++;
      if(SetTime <= TimeMIN)
      SetTime = TimeMIN;
      PantallaSetTimer();
      delay(250);
      TiempoMENU = millis()+DelayMENU;
      EIFR = bit(INTF1);
      attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
    }
  }
  detachInterrupt(digitalPinToInterrupt(3));
  if(EntroMENU){
    EntroMENU=false;
    EEPROM.put(30,SetTime);
  }  
}
