// Displej dotykový 240x320px

// připojení knihoven
#include "SPI.h"
#include "Adafruit_ILI9340.h"
#include "XPT2046_Touchscreen.h"
#include "Fonts/FreeMono9pt7b.h"

// nastavení propojovacích pinů
#define LCD_cs 10
#define LCD_dc 9
#define LCD_rst 8
#define DOTYK_cs  7

// inicializace LCD displeje z knihovny
Adafruit_ILI9340 displej = Adafruit_ILI9340(LCD_cs, LCD_dc, LCD_rst);
// inicializace řadiče dotykové vrstvy z knihovny
XPT2046_Touchscreen dotyk(DOTYK_cs);

struct CALIBRATION {
  int shiftX;
  int shiftY;
  float scaleX;
  float scaleY;
};

CALIBRATION cal;

 void DisplayCal(float XRes, float YRes) {
  uint16_t coordX[5] = {0, 320, 320, 0, 160};
  uint16_t coordY[5] = {0, 0, 240, 240, 120};
  unsigned int Xmax, Xmin, Ymax, Ymin;
  uint16_t radius = 10;
  TS_Point data;


  displej.setCursor(0, 20);
  displej.println("Kalibrace dotykove vrstvy");
  displej.println("Dotkni se LCD..");
  while (!dotyk.touched()) {
    delay(10);
  }

  for (int i = 0; i < 5; i++) {
    displej.fillScreen(ILI9340_BLACK);
    displej.drawCircle(coordX[i], coordY[i], radius, ILI9340_BLUE);        
    while (!dotyk.touched()) {
      delay(10);
    }
    data = dotyk.getPoint(); //collect data
    coordX[i] = data.x;
    coordY[i] = data.y;
    while (dotyk.touched()) {     //have to wait till the touch is remaining
      delay(10);
    }
    
  }
  Xmax = max(coordX[1], coordX[2]); //select the best maximal values
  Ymax = max(coordY[2], coordY[3]);
  Xmin = min(coordX[0], coordX[3]); //select the best minima
  Ymin = min(coordY[0], coordY[1]);

  cal.shiftX = Xmin;
  cal.shiftY = Ymin;
  cal.scaleX = XRes / Xmax;
  cal.scaleY = YRes / Ymax;

  displej.fillScreen(ILI9340_BLACK);  
  displej.setCursor( 0, 12);
  displej.print("minimum X: ");
  displej.println(Xmin);
  displej.print("minimum Y: ");
  displej.println(Ymin);
  displej.print("maximum X: ");
  displej.println(Xmax);
  displej.print("maximum Y: ");
  displej.println(Ymax);
  displej.print("skalovani X: ");
  displej.println(cal.scaleX,5);
  displej.print("skalovani Y: ");
  displej.print(cal.scaleY,5);
}



void setup() {

  // zahájení komunikace s displejem a dotykovou vrstvou
  displej.begin();
  dotyk.begin();
  // pro otočení displeje stačí změnit číslo
  // v závorce v rozmezí 0-3
  displej.setRotation(1);
  // vyplnění displeje černou barvou
  displej.setFont(&FreeMono9pt7b);
  displej.fillScreen(ILI9340_BLACK);
  DisplayCal(320.0, 240.0);
}

void loop() {
  TS_Point data;

  unsigned int colors[8] = {0x001F, 0xF800, 0x07E0, 0x07FF, 0xF81F, 0xFFE0, 0xFFFF, 0x0000};
  while (true) {
    for (int i = 0; i < 7; i++) {
      while (!dotyk.touched()) {
        delay(10);
      }
      data = dotyk.getPoint();
      displej.fillScreen(colors[i]);
      displej.setCursor( 0, 12);
      displej.setTextColor(colors[i + 1]);
      displej.print("data x = ");
      displej.println(data.x);
      displej.print("data y = ");
      displej.println(data.y);
      displej.print("data z = ");
      displej.print(data.z);
      delay(500);
    }
  }
  delay(1);

}
