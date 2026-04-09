// Descongelador de calostro, usando pantalla OLED 1.3"
// Author: MA
// Collab: Chat GPT
// Date:   Abril 2026

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
#include "check/checkFallas.h"

#define TIMEOUT_INACTIVIDAD 7200000UL  // 1 hora = 3600000 ms


/* Instanciacion de varibles globales */
float SetTemp = 37.5;
bool EnBomba = 1;              //1=Bomba habilitada ; 0=Bomba siempre apagada (Habilitada por defecto)
bool EnResistencia = 1;        //1=Resistencia automática ; 0=Resistencia siempre apagada (Automatica por defecto)
float TemperaturaS0 = 0;
float TemperaturaS1 = 0;
float TempFiltro = 0;
bool ResistenciaOn = false;
bool ExcedeTemp = false;
int CuentaExcesos = 0;
int TimeOut = 0;
float Histeresis = 0.2;     	    //Valor de Histeresis para el control de Temperatura
unsigned long Reset_WD = 0;
//unsigned long FallaST = 0;
byte sensorFallado = 2;
float nuevaLectura = 0;
unsigned long lastUserActivity = 0;    // guarda el momento de la última interacción
bool buzzerState = false;

DeviceAddress addr0 = {0};
DeviceAddress addr1 = {0};
/* Fin Variables Globales */


/* Declaracion e instanciacion de variables locales */
/* Fin Variables Locales */

//-----------------------SETUP----------------------
void setup(){

  wdt_disable();

  bool wd_reset = MCUSR & (1 << WDRF);  // Bandera de reset por WDT
  MCUSR = 0; // Limpia las banderas
  EEPROM.get(50, Reset_WD);
  if (wd_reset) {
    Reset_WD++;
    EEPROM.put(50, Reset_WD);
  }

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
  
  initSensorTemp();                       //Inicializacion de bus One Wire y Sensor de Temperatura ---

  displaySetup();
  logoBienvenida();

  EEPROM.put(0,EnBomba);                  // Cuando se enciende el sistema, la bomba siempre está habilitada.
  EEPROM.put(1,EnResistencia);            // Cuando se enciende el sistema, la resistencia siempre está en automático (EnResistencia=1).

  byte eepromFlag;
  EEPROM.get(100, eepromFlag);

  if (eepromFlag != 0xAA) {                 // Comprobación de inicialización de EEPROM, Si no tiene flag 0xAA, se cargan valores por defecto
    // Primera vez: valores por defecto
    SetTemp = 37.5;
    CuentaExcesos = 0;
    Reset_WD = 0;
    //FallaST = 0;
    TimeOut = 0;

    EEPROM.put(10, SetTemp);
    EEPROM.put(40, CuentaExcesos);
    EEPROM.put(50, Reset_WD);
    //EEPROM.put(60, FallaST);
    EEPROM.put(70, TimeOut);

    // Guardar flag de inicialización
    eepromFlag = 0xAA;
    EEPROM.put(100, eepromFlag);
  }
  else {
    // Ya estaba inicializado: cargar valores reales
    EEPROM.get(10, SetTemp);
    EEPROM.get(40, CuentaExcesos);
    EEPROM.get(50, Reset_WD);
    //EEPROM.get(60, FallaST);
    EEPROM.get(70, TimeOut);
  }

  PantallaInfo();
  delay(3000);

  lastUserActivity = millis();

  wdt_enable(WDTO_8S);
}

//-----------------------MAIN-----------------------
void loop() {
  digitalWrite(BUZZER,LOW);           // Por si queda sin querer en alto en algún lado.

  leerTemp();
  delay(100);
  /* Verificacion de Fallas */
  detachInterrupt(digitalPinToInterrupt(3));
  checkNivelAgua();
  checkExcesoTemp();
  attachInterrupt(digitalPinToInterrupt(3), ISR_botonMenu, RISING);
  /* Fin Verificacion de Fallas */
  
  CalentamientoAgua(Histeresis);  // Función para el calculo de temperatura y encendido de resistencia.

  if(EnBomba){                  // Chequea si se deshabilitó la Bomba.
    digitalWrite(BOMBA, HIGH);
  } else{
    digitalWrite(BOMBA, LOW);
  }

  if(!EnResistencia || !ResistenciaOn){ // No es necesario, pero por si falla algún algoritmo.
    digitalWrite(RELE_ON, LOW);
  }

  PantPrincipal();

  if(EntroMENU){                  // Entra al menú principal, si se presiona el boton "Menú"
    MenuPrincipal();
  }

  if ((millis() - lastUserActivity) > TIMEOUT_INACTIVIDAD) {
    apagarTodo();
    TimeOut++;
    EEPROM.put(70, TimeOut);
    bool buzzerState = false;
      for(int i = 0; i < 5000; i++){ // hace un poco de ruido
        buzzerState = !buzzerState;
        digitalWrite(BUZZER, !buzzerState);
        delayMicroseconds(250);
      }
      digitalWrite(BUZZER, LOW);
    lastUserActivity = millis();
  }

  wdt_reset();
}