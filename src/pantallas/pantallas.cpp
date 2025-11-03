#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

#include "pantallas/pantallas_menus.h"
#include "timer/timerCalc.h"
#include "global/global_utils.h"

/* Dimensiones de la pantalla OLED */
#define OLED_RESET -1
Adafruit_SH1106 display(OLED_RESET);
#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif


/* Setup del display */
void displaySetup() {
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  display.clearDisplay(); 
  display.display();
}

/* Logo de Bienvenida */
void logoBienvenida() {
  display.invertDisplay(false);
  display.clearDisplay();
  display.drawBitmap(0, 0, logo_edeys, 128, 64, WHITE); 
  display.display();
  delay(1000);
  display.invertDisplay(true);
  delay(500);
  display.invertDisplay(false);
  delay(500);
}

//----------------------------------------------- Pantalla principal ----------------------------------------------------------------
void PantPrincipal() {
  display.clearDisplay();
  
  // Muestra Resistencia On/Off y TO (temperatura Objetivo)
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  if (ResistenciaOn)
    display.print("Res:ON ");
  else
    display.print("Res:OFF");
  display.print("  -");
  display.setCursor(70,0);
  display.print("TO:");
  display.print(EEPROM.get(10, SetTemp), 1);
  display.write(167);
  display.print("C");
  
  // Indicadores de estado
  display.setCursor(20, 15);
  display.fillRect(10, 12, 110, 13, BLACK);
  if (ResistenciaOn && !EnTimer) {
    display.setCursor(20, 15);
    display.fillRect(10, 12, 110, 13, WHITE);
    display.setTextColor(BLACK, WHITE);
    display.print("Calentando...");
  }
  else if (EnTimer) {
    display.setCursor(4, 15);
    display.fillRect(0, 12, 127, 13, WHITE);
    display.setTextColor(BLACK, WHITE);
    display.print("Pasteurizando: ");
    if (minLeft < 10) display.print("0");
    display.print(minLeft, 0);
    display.print(":");
    if (secLeft < 10) display.print("0");
    display.print(secLeft, 0);
  }
  else if (EnResistencia && !EnPasteurizar) {
    display.setCursor(20, 15);
    display.fillRect(10, 12, 110, 13, WHITE);
    display.setTextColor(BLACK, WHITE);
    display.print("Descongelando");
  }
  else {
    display.setTextColor(WHITE, BLACK);
    display.print("SISTEMA APAGADO");
  }
  
  // Muestra la temperatura actual
  display.setCursor(5, 28);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Temperatura actual:");
  display.setCursor(0, 40);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.print(" ");
  display.print(Temperatura, 1);
  display.setTextSize(2);
  display.write(167);
  display.setTextSize(3);
  display.print("C");
  
  display.display();
}

//-------------------------------------------------- Pantalla del Menú principal -------------------------------------------------------------------------------
void PantallaMenuPrincipal(byte *FilaMENU) {
  byte PosXRec = 0;
  byte PosYRec = 0;
  PosYRec = 16*(*FilaMENU);
  if(PosYRec >= 48) PosYRec = 48;
  switch((*FilaMENU)){
    case 0:
    display.clearDisplay();
    display.fillRect(PosXRec, PosYRec, 128, 16, WHITE);
    display.setTextSize(2);
    display.setCursor(1,1);
    display.setTextColor(BLACK);
    display.println("<-----");  //Seleccionado
    display.setTextColor(WHITE);
    display.println("Set Temp.");
    display.println("Set Tiempo");
    if(EnResistencia){ display.println("PARAR-STOP");}
    else{ display.println("PASTEURIZA");}
    display.display();
    break;

    case 1:
    display.clearDisplay();
    display.fillRect(PosXRec, PosYRec, 128, 16, WHITE);
    display.setTextSize(2);
    display.setCursor(1,1);
    display.setTextColor(WHITE);
    display.println("<-----");
    display.setTextColor(BLACK);
    display.println("Set Temp.");  //Seleccionado
    display.setTextColor(WHITE);
    display.println("Set Tiempo");
    if(EnResistencia){ display.println("PARAR-STOP");}
    else{ display.println("PASTEURIZA");}
    display.display();
    break;

    case 2:
    display.clearDisplay();
    display.fillRect(PosXRec, PosYRec, 128, 16, WHITE);
    display.setTextSize(2);
    display.setCursor(1,1);
    display.setTextColor(WHITE);
    display.println("<-----");
    display.println("Set Temp.");
    display.setTextColor(BLACK);
    display.println("Set Tiempo");  //Seleccionado
    display.setTextColor(WHITE);    
    if(EnResistencia){ display.println("PARAR-STOP");}
    else{ display.println("PASTEURIZA");}  
    display.display();
    break;

    case 3:
    display.clearDisplay();
    display.fillRect(PosXRec, PosYRec, 128, 16, WHITE);
    display.setTextSize(2);
    display.setCursor(1,1);
    display.setTextColor(WHITE);
    display.println("<-----");
    display.println("Set Temp.");
    display.println("Set Tiempo");
    display.setTextColor(BLACK);
    if(EnResistencia){ display.println("PARAR-STOP");}  //Seleccionado este o
    else{ display.println("PASTEURIZA");}                  //este.
    display.setTextColor(WHITE);
    display.display();
    break;

    case 4:
    display.clearDisplay();
    display.fillRect(PosXRec, PosYRec, 128, 16, WHITE);
    display.setTextSize(2);
    display.setCursor(1,1);
    display.setTextColor(WHITE);
    display.println("Set Temp.");   
    display.println("Set Tiempo");
    display.println("PASTEURIZA");
    display.setTextColor(BLACK);
    display.println("DESCONGELA");   //Seleccionado
    display.setTextColor(WHITE);
    display.display();
    break;

    case 5:
    display.clearDisplay();
    display.fillRect(PosXRec, PosYRec, 128, 16, WHITE);
    display.setTextSize(2);
    display.setCursor(1,1);
    display.setTextColor(WHITE);  
    display.println("Set Tiempo");
    display.println("PASTEURIZA");
    display.println("DESCONGELA");
    display.setTextColor(BLACK);
    display.println("Bomba-Agua");
    display.setTextColor(WHITE);
    display.display();
    break;
  }
}

//----------------------------------------------- Pantalla de set de temperatura ----------------------------------------------------------------------
void PantallaSetTemp () {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(40,0);
  display.print("TEMP");
  display.setCursor(10, 20);
  display.println("Objetivo:");
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0,40);
  display.print(" ");
  display.print(SetTemp,1);
  display.print("C");
  display.display();
}

//----------------------------------------------- Pantalla de set de timer ----------------------------------------------------------------------
void PantallaSetTimer () {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.println("Tiempo");
  display.println("[min]:");
  display.setCursor(0,40);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.print(" ");
  display.print(SetTime,1);
  display.display();
}

//----------------------------------------------- Pantalla Bomba de Agua ----------------------------------------------------------------------
void PantallaBomba () {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.println("Recircula");
  display.println("   Agua:");
  display.setCursor(6,45);
  display.setTextSize(2);
  if(EnBomba)
  display.print("ENCENDIDO");
  else
  display.print(" APAGADO");
  display.display();
}

//------------------------------------------ Pantalla Nivel Mínimo ----------------------------------------------------------------------------
void PantallaNivelMinimo (){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(6,0);
  display.setTextColor(WHITE);
  display.print("Agua bajo");
  display.setCursor(6,18);
  display.print(" el nivel");
  display.setCursor(12,41);
  display.setTextSize(3);
  display.print("MINIMO");
  display.drawRect(0,39,128,25,WHITE);
  display.display();
}

//------------------------------------------ Pantalla de Falla Sensor de Temperatura ------------------------------------------------------------
void PantallaFallaSensorT (){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(6,0);
  display.setTextColor(WHITE);
  display.print("Sensor de");
  display.setCursor(38,18);
  display.print("TEMP.");
  display.setCursor(14,41);
  display.setTextSize(3);
  display.print("FALLA!");
  display.drawRect(0,39,128,25,WHITE);
  display.display();
}

//------------------------------------------ Pantalla exceso de Temperatura ------------------------------------------------------------
void PantallaExcesoTemp (){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(6,0);
  display.setTextColor(WHITE);
  display.print("EXCESO DE");
  display.setCursor(38,18);
  display.print("TEMP.");
  display.setCursor(14,41);
  display.setTextSize(3);
  display.print("FALLA!");
  display.drawRect(0,39,128,25,WHITE);
  display.display();
}

//------------------------------------------ Pantalla inicial - CODIGO: 0 ---------------------------------------------------------------

void PantallaCodigo0 (){
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(6, 10);
  display.setTextColor(WHITE);
  display.print("CODIGO: 0");
  display.setCursor(50, 35);
  display.setTextSize(3);
  display.print(EEPROM.get(20, CuentaExcesos));
  display.display();
}