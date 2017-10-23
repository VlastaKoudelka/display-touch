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
    displej.drawCircle(coordX[i], coordY[i], radius, ILI9340_YELLOW); 
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
  cal.scaleX = XRes / (Xmax-Xmin); //input data are shifted first and than scaled
  cal.scaleY = YRes / (Ymax-Ymin);

  displej.fillScreen(ILI9340_BLACK);  
  displej.setCursor( 0, 12);
  displej.print("posun X: ");
  displej.println(cal.shiftX);
  displej.print("posun Y: ");
  displej.println(cal.shiftY);
  displej.print("skalovani X: ");
  displej.println(cal.scaleX,10);
  displej.print("skalovani Y: ");
  displej.print(cal.scaleY,10);
}

void setup() {
  cal.shiftX = 141; //defaultni kalibrace
  cal.shiftY = 240;
  cal.scaleX = 0.08966;
  cal.scaleY = 0.06552;
  
  // zahájení komunikace s displejem a dotykovou vrstvou
  displej.begin();
  dotyk.begin();
  // pro otočení displeje stačí změnit číslo
  // v závorce v rozmezí 0-3
  displej.setRotation(1);
  // vyplnění displeje černou barvou
  displej.setFont(&FreeMono9pt7b);
  displej.fillScreen(ILI9340_BLACK);
  
  //DisplayCal(320.0, 240.0);
}

void loop() {
  TS_Point data;
  uint16_t realX,realY, lastX, lastY;
  int painColor = ILI9340_BLUE, radius = 1, diffX, diffY;
  boolean dragFlag = false;
  unsigned int colors[8] = {0x001F, 0xF800, 0x07E0, 0x07FF, 0xF81F, 0xFFE0, 0xFFFF, 0x0000};
  displej.drawRect(300,0,20,20,painColor);
  displej.drawRect(300,220,20,20,int(random(0x0000,0xFFFF)));
  displej.drawRect(0,220,20,20,int(random(0x0000,0xFFFF)));
  delay(1);
  
  while (true) {
    if (dotyk.touched()){
      if (!dragFlag){ //if this is the first touch after some interuption        
        delay(20);
        data = dotyk.getPoint();
        lastX = int((data.x - cal.shiftX)*cal.scaleX);
        lastY = int((data.y - cal.shiftY)*cal.scaleY);        
        delay(1);    //wait for appropriate preasure
        dragFlag = true;
      }      
      data = dotyk.getPoint();
      realX = int((data.x - cal.shiftX)*cal.scaleX);
      realY = int((data.y - cal.shiftY)*cal.scaleY);
      diffX = lastX - realX;
      diffY = lastY - realY;  
      
      if ((realX > 300) && (realY > 220)){ //mazani displeje
        displej.fillScreen(ILI9340_BLACK);     
        delay(50);
          displej.drawRect(300,220,20,20,int(random(0x0000,0xFFFF)));
          displej.drawRect(300,0,20,20,int(random(0x0000,0xFFFF)));
          displej.drawRect(0,220,20,20,int(random(0x0000,0xFFFF)));          
      }
      else if ((realX > 300) && (realY < 20)){ //nahodna zmena barvy
        painColor = int(random(0x0000,0xFFFF));
        displej.drawRect(300,0,20,20,painColor); 
        radius = 1;
      }
      else if ((realX < 20) && (realY > 220)){ //zapnuti gumy    
        painColor = ILI9340_BLACK;
        radius = 4;
      }   
      else if ((abs(diffX) < 10)&&(abs(diffY) < 10)){
        displej.fillCircle(realX,realY,radius,painColor);
        lastX = realX;
        lastY = realY;
      }
      else{
        delay(1);
      }
    }
    else {
      dragFlag = false;  //new touch will be the first touch
    }    
    delay(1);
  }
}


