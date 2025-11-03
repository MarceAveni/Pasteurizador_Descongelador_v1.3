// Descongelador / Pasteurizador de calostro, usando pantalla OLED 1.3"
// Author: MA
// Collab: LL
// Date:   julio 2025
// TODO: Hacer rutinas de fallas.

#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#include "global/global_utils.h"
#include "lecturaTemp/lecturaTemp.h"
#include "pantallas/pantallas_menus.h"
#include "calentamiento/calentamientoAgua.h"
#include "timer/timerCalc.h"
#include "check/checkFallas.h"

/* Instanciacion de varibles globales */
float SetTemp = 37.5;
float SetTime = 10;
boolean EnBomba = 1;              //1=Bomba siempre encendida ; 0=Bomba siempre apagada (Encendida por defecto)
boolean EnResistencia = 0;        //1=Resistencia automática ; 0=Resistencia siempre apagada (Apagada por defecto)
boolean EnPasteurizar = 0;
float Temperatura = 0;
boolean ResistenciaOn = false;
boolean ExcedeTemp = false;
int CuentaExcesos = 0;
/* Fin Variables Globales */


/* Declaracion e instanciacion de variables locales */
float Histeresis = 0.2;     	    //Valor de Histeresis para el control de Temperatura
/* Fin Variables Locales */

//-----------------------SETUP----------------------
void setup(){

  pinMode(NIVEL,INPUT_PULLUP);
  pinMode(MENU, INPUT);
  pinMode(MAS, INPUT);
  pinMode(MENOS, INPUT);
  pinMode(BOMBA, OUTPUT);
  pinMode(RELE_ON, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(SNivel_C,INPUT_PULLUP);
  
  digitalWrite(BUZZER,LOW);
  digitalWrite(RELE_ON,LOW);
  digitalWrite(BOMBA,LOW);
  
  pinMode(GPIO0, OUTPUT);
  pinMode(GPIO1, OUTPUT);
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO3, OUTPUT);
  pinMode(GPIO4, OUTPUT);
  pinMode(LED_BI, OUTPUT);
  pinMode(BUZZER2, OUTPUT);

  digitalWrite(GPIO0,LOW);
  digitalWrite(GPIO1,LOW);
  digitalWrite(GPIO2,LOW);
  digitalWrite(GPIO3,LOW);
  digitalWrite(GPIO4,LOW);
  digitalWrite(LED_BI,LOW);
  digitalWrite(BUZZER2,LOW);
  
  initSensorTemp();

  displaySetup();
  logoBienvenida();

  EEPROM.put(0,EnBomba);                  // Cuando se enciende el sistema, la bomba siempre está encendidad, si hay agua (EnBomba=1).
  EEPROM.put(1,EnResistencia);            // Cuando se enciende el sistema, la resistencia siempre está apagada (EnResistencia=0).

  byte eepromFlag;
  EEPROM.get(100, eepromFlag);

  if (eepromFlag != 0xAA) {                 // Comprobación de inicialización de EEPROM, Si no tiene flag 0xAA, se cargan valores por defecto
    // Primera vez: valores por defecto
    SetTemp = 37.5;
    SetTime = 10.0;
    CuentaExcesos = 0;

    EEPROM.put(10, SetTemp);
    EEPROM.put(30, SetTime);
    EEPROM.put(40, CuentaExcesos);

    // Guardar flag de inicialización
    eepromFlag = 0xAA;
    EEPROM.put(100, eepromFlag);
  }
  else {
    // Ya estaba inicializado: cargar valores reales
    EEPROM.get(10, SetTemp);
    EEPROM.get(30, SetTime);
    EEPROM.get(40, CuentaExcesos);
  }

  PantallaCodigo0();
  delay(1500);

}

//-----------------------MAIN-----------------------
void loop() {
  digitalWrite(BUZZER,LOW);           // Por si queda sin querer en alto en algún lado.

  /* Verificacion de Fallas */
  detachInterrupt(digitalPinToInterrupt(3));
  checkNivelAgua();
  checkSensorTemp();
  checkExcesoTemp();
  attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
  /* Fin Verificacion de Fallas */

  CalentamientoAgua(Histeresis);  // Función para el calculo de temperatura y encendido de resistencia.
  timerPasteurizador();           // Calcula tiempo restante cuando está pasteurizando.
  PantPrincipal();
  delay(100);
  
  if(EnBomba){                  // Chequea si se deshabilitó la Bomba.
    digitalWrite(BOMBA, HIGH);
  } else{
    digitalWrite(BOMBA, LOW);
  }

  if(!EnResistencia || !ResistenciaOn){ // No es necesario, pero por si falla algún algoritmo.
    digitalWrite(RELE_ON, LOW);
  }

  if(EntroMENU){                  // Entra al menú principal, si se presiona el boton "Menú"
    MenuPrincipal();
  }

}