// Displej dotykový 240x320px

// připojení knihoven
#include "SPI.h"
#include "Adafruit_ILI9340.h"
#include "XPT2046_Touchscreen.h"
#include "Fonts/FreeSansBold24pt7b.h"

// nastavení propojovacích pinů
#define LCD_cs 10
#define LCD_dc 9
#define LCD_rst 8
#define DOTYK_cs  7

// inicializace LCD displeje z knihovny
Adafruit_ILI9340 displej = Adafruit_ILI9340(LCD_cs, LCD_dc, LCD_rst);
// inicializace řadiče dotykové vrstvy z knihovny
XPT2046_Touchscreen dotyk(DOTYK_cs);

void setup() {
  // zahájení komunikace s displejem a dotykovou vrstvou
  displej.begin();
  dotyk.begin();
  // pro otočení displeje stačí změnit číslo
  // v závorce v rozmezí 0-3
  displej.setRotation(0);
  // vyplnění displeje černou barvou
  displej.setFont(&FreeSansBold24pt7b);
  displej.fillScreen(ILI9340_BLACK);
}

void loop() {
  //TS_Point data = dotyk.getPoint();
  //displej.drawPixel(data.x,data.y,ILI9340_YELLOW); 
  unsigned int colors[8] = {0x001F,0xF800,0x07E0,0x07FF,0xF81F,0xFFE0,0xFFFF,0x0000};  
  while (true){
    for (int i = 0; i<8; i++){
      while (!dotyk.touched()){
        delay(10);
      }
      displej.fillScreen(colors[i]);
      delay(500);    
    }
  }
  delay(1);

}
