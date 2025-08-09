
// Includes
#include <string.h>
#include <math.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Logo.h"

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
  tft.fillScreen(ILI9341_BLACK);;
  // fadePWM(DISP_LED, true, 500);
}

/**************************************************************************/
/*                                  LOOP                                  */
/**************************************************************************/

void loop(void) {
  switch(ScreenState){
    case WELCOME:
      // draw screen
      if(dispDrawn==0){
        tft.fillScreen(ILI9341_BLACK);

        Serial.print("Logo print (ms): ");
        unsigned long start = millis();
        tft.drawRGBBitmap(46, 40, LogoBitmap, LogoWid, LogoHei);
        Serial.println(millis() - start);

        tft.setFont(&AvenirNextLTPro_Regular16pt7b);
        tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
        tft.setCursor( 50,227);   tft.print("Welcome");

        tft.setFont(&AvenirNextLTPro_Regular8pt7b);
        tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
        tft.setCursor( 52,258);   tft.print("Click to get started");

        fadePWM(DISP_LED, true, 500);
      }
      dispDrawn = 1;

      // // wait
      // delay(1000);

      // // transition to next screen
      // ScreenState = WELCOME_MENU;
      // dispDrawn = 0;

      break;

    case WELCOME_MENU:
      // draw screen
      if(dispDrawn==0){
        // remove prev text
        tft.fillRect(0,181,240,133,ILI9341_BLACK);

        tft.setFont(&AvenirNextLTPro_Regular16pt7b);
        tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
        tft.setCursor( 46,214);   tft.print("Take measurements");
        tft.setCursor( 46,238);   tft.print("See previous results");
        tft.setCursor( 46,262);   tft.print("Connect to phone");
        tft.setCursor( 46,286);   tft.print("Settings");
      }
      dispDrawn = 1;

      switch(cursor_pos){
        case 0:
          break;
        case 1:
          break;
        case 2:
          break;
        case 3:
          break;
      }

      break;
  }

  delay(10);
}