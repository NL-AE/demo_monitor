
// Includes
#include <string.h>
#include <math.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Logo_w.h"               // logo must be R5G6B5, 16bit

// Display
#define SPI_SCLK 4   // SLK
#define SPI_MOSI 6   // MOSI
#define SPI_MISO 5   // MISO
#define DISP_CSL 7   // chip select
#define DISP_DCP 21  // direction control pin
#define DISP_LED 10  // LED PWM pin
#define DISP_POW 20  // display power pin
#define DISP_RST 3

// Adafruit_ILI9341 tft = Adafruit_ILI9341(DISP_CSL, DISP_DCP);
Adafruit_ILI9341 tft = Adafruit_ILI9341(DISP_CSL, DISP_DCP, DISP_RST);
// Adafruit_ILI9341 tft = Adafruit_ILI9341(DISP_CSL, DISP_DCP, SPI_MOSI, SPI_SCLK, -1, SPI_MISO);   // much slower for some reason (it bitbangs it i think)

#include "AvenirNextLTPro_Regular16pt7b.h"
#include "AvenirNextLTPro_Regular12pt7b.h"
#include "AvenirNextLTPro_Regular8pt7b.h"

#define ILI9341_Custom_B tft.color565(39,47,83)

// Variables
enum ScreenState{
  WELCOME,
  WELCOME_MENU,
  TUTORIAL_1,
  TUTORIAL_2,
  TUTORIAL_3,
  TUTORIAL_4,
  MEASURING,
  RESULTS,
  PREV_RESULTS
};

volatile int ScreenState = WELCOME;
int dispDrawn = 0;
volatile int cursor_pos = 0;

void fadePWM(int pin, bool fadeIn, int fadeTimeMillis) {
  const int pwmMax = 255;
  const int pwmMin = 0;

  int startValue = fadeIn ? pwmMin : pwmMax;
  int endValue = fadeIn ? pwmMax : pwmMin;

  int stepCount = abs(endValue - startValue); // Number of steps (usually 255)
  float delayPerStep = (float)fadeTimeMillis / stepCount;

  for (int value = startValue; 
       fadeIn ? (value <= endValue) : (value >= endValue); 
       fadeIn ? value++ : value--) {
    
    analogWrite(pin, value);
    delay(delayPerStep); // Wait a bit before the next step
  }
}

/**************************************************************************/
/*                                  SETUP                                 */
/**************************************************************************/

void setup(void) {
  // Serial
  // Serial.begin(115200);

  pinMode(DISP_LED, OUTPUT);
  pinMode(DISP_POW, OUTPUT);
  digitalWrite(DISP_LED, 0);
  digitalWrite(DISP_POW, 0);
  delay(5);

  // Display
  digitalWrite(DISP_POW,1);
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_WHITE);
  fadePWM(DISP_LED, true, 500);

  ScreenState = TUTORIAL_1;
}

/**************************************************************************/
/*                                  LOOP                                  */
/**************************************************************************/

void loop(void) {
  switch(ScreenState){
    case WELCOME:
      // draw screen
      if(dispDrawn==0){
        tft.fillScreen(ILI9341_WHITE);

        Serial.print("Logo print (ms): ");
        unsigned long start = millis();
        tft.drawRGBBitmap(46, 40, LogoBitmap_w, LogoWid_w, LogoHei_w);
        Serial.println(millis() - start);

        tft.setFont(&AvenirNextLTPro_Regular16pt7b);
        tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
        tft.setCursor( 50,227);   tft.print("Welcome");

        tft.setFont(&AvenirNextLTPro_Regular8pt7b);
        tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
        tft.setCursor( 52,258);   tft.print("Click to get started");

        fadePWM(DISP_LED, true, 500);
      }
      dispDrawn = 1;

      // wait
      delay(1000);

      // transition to next screen
      ScreenState = WELCOME_MENU;
      dispDrawn = 0;

      break;

    case WELCOME_MENU:
      // draw screen
      if(dispDrawn==0){
        // remove prev text
        tft.fillRect(0,181,240,133,ILI9341_WHITE);

        tft.setFont(&AvenirNextLTPro_Regular8pt7b);
        tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
        tft.setCursor( 46,209);   tft.print("Take measurements");
        tft.setCursor( 46,234);   tft.print("See previous results");
        tft.setCursor( 46,258);   tft.print("Connect to phone");
        tft.setCursor( 46,282);   tft.print("Settings");
      }
      dispDrawn = 1;

      switch(cursor_pos){
        case 0:
          tft.fillTriangle(29, 198, 29, 208, 34, 203, ILI9341_BLACK);
          tft.fillTriangle(29, 223, 29, 233, 34, 228, ILI9341_WHITE);
          tft.fillTriangle(29, 248, 29, 258, 34, 253, ILI9341_WHITE);
          tft.fillTriangle(29, 273, 29, 283, 34, 278, ILI9341_WHITE);
          break;

        case 1:
          tft.fillTriangle(29, 198, 29, 208, 34, 203, ILI9341_WHITE);
          tft.fillTriangle(29, 223, 29, 233, 34, 228, ILI9341_BLACK);
          tft.fillTriangle(29, 248, 29, 258, 34, 253, ILI9341_WHITE);
          tft.fillTriangle(29, 273, 29, 283, 34, 278, ILI9341_WHITE);
          break;

        case 2:
          tft.fillTriangle(29, 198, 29, 208, 34, 203, ILI9341_WHITE);
          tft.fillTriangle(29, 223, 29, 233, 34, 228, ILI9341_WHITE);
          tft.fillTriangle(29, 248, 29, 258, 34, 253, ILI9341_BLACK);
          tft.fillTriangle(29, 273, 29, 283, 34, 278, ILI9341_WHITE);
          break;

        case 3:
          tft.fillTriangle(29, 198, 29, 208, 34, 203, ILI9341_WHITE);
          tft.fillTriangle(29, 223, 29, 233, 34, 228, ILI9341_WHITE);
          tft.fillTriangle(29, 248, 29, 258, 34, 253, ILI9341_WHITE);
          tft.fillTriangle(29, 273, 29, 283, 34, 278, ILI9341_BLACK);
          break;
      }

      delay(1000);
      cursor_pos = cursor_pos+1;
      if(cursor_pos == 4){
        cursor_pos = 0;
        ScreenState = TUTORIAL_1;
        dispDrawn = 0;
      }

      break;

    case TUTORIAL_1:
      if(dispDrawn==0){
        tft.fillScreen(ILI9341_WHITE);
        tft.fillRect(0,0,240,45,ILI9341_Custom_B);

        tft.setFont(&AvenirNextLTPro_Regular8pt7b);
        tft.setTextColor(ILI9341_WHITE,ILI9341_Custom_B);
        tft.setCursor( 23, 26);   tft.print("To take your measurement");

        // draw image using primitives
        tft.drawRoundRect(59, 80,122,61, 8, ILI9341_Custom_B);
        tft.drawRoundRect(60, 81,120,59, 7, ILI9341_Custom_B);
        tft.fillRect(0, 80, 240, 10, ILI9341_WHITE);

        tft.drawRect(80, 139, 29, 61, ILI9341_Custom_B);
        tft.drawRect(81, 140, 27, 59, ILI9341_Custom_B);

        tft.drawRect(85, 182, 19, 13, ILI9341_Custom_B);

        tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
        tft.setCursor( 34,245);   tft.print("1. Insert strip into device");

        tft.setCursor( 60,290);   tft.print("Click to continue");
      }
      dispDrawn = 1;
      break;
  }

  delay(10);
}