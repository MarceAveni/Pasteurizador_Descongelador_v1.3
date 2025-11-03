#ifndef GLOBAL_UTILS_H
#define GLOBAL_UTILS_H

#include <EEPROM.h>
#include <Arduino.h>

//Definición de pines para sensor NIVEL --------------
#define NIVEL 6

//Definición de pines para botones -------------------------
#define MENU  3
#define MAS   4
#define MENOS 5

//Definición de pines de salida ----------------------------
#define BOMBA   16
#define RELE_ON 12
#define BUZZER  17
#define SNivel_C 14

//Definición de pines no usados ---------------------------
#define GPIO0   7
#define GPIO1   8
#define GPIO2   9
#define GPIO3   10
#define GPIO4   11
#define LED_BI  13
#define BUZZER2 15  //Buzzer en la placa del display

#define DelayMENU 10000   //Es el tiempo [ms] que tarda en salir automáticamente de algún menú hacia la pantalla principal
#define TempMAX 65.0      //Temperatura máxima que se puede configurar para descongelar o pasteurizar
#define TempMIN 10.0      //Temperatura mínima que se puede configurar para descongelar o pasteurizar

#define TimeMAX 120.0     //Tiempo máximo configurable luego que se alcanza la temperatura seteada de pasteurización
#define TimeMIN 0.0       //Tiempo mínimo configurable luego que se alcanza la temperatura seteada de pasteurización

// #define N_FILAS 6         //Cantidad de filas en el menu principal




/* Variables Globales */
extern boolean EnBomba;              //1=Bomba siempre encendida ; 0=Bomba siempre apagada (Encendida por defecto)
extern boolean EnResistencia;        //1=Resistencia automática ; 0=Resistencia siempre apagada (Apagada por defecto)
extern boolean EnPasteurizar;
extern float Temperatura;
extern boolean ResistenciaOn;
extern float SetTemp;
extern float SetTime;
extern boolean ExcedeTemp;
extern int CuentaExcesos;
/* Fin Variables Globales */

#endif