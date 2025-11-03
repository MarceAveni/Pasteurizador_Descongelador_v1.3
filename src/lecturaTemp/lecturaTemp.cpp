#include <OneWire.h>
#include <DallasTemperature.h>
#include "global/global_utils.h"
#include "lecturaTemp/lecturaTemp.h"

//Inicializacion de bus One Wire y Sensor de Temperatura ---
#define pinDatoTemp 2
OneWire oneWireObjeto(pinDatoTemp);
DallasTemperature sensorDS18B20(&oneWireObjeto);

void initSensorTemp(){
  sensorDS18B20.begin();
}

float leerTemp(){
    sensorDS18B20.requestTemperatures();
    return sensorDS18B20.getTempCByIndex(0);
}