
// Includes
#include <string.h>
#include <math.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Logo_w.h"               // logo must be R5G6B5, 16bit
#include "Drop_img.h"
#include "Logo_w_small.h"

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
  MEASURING,
  RESULTS,
  PREV_RESULTS
};

volatile int ScreenState = WELCOME;
int dispDrawn = 0;
volatile int cursor_pos = 0;
volatile int MeasuringState = 0;

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

        tft.drawRGBBitmap(46, 40, LogoBitmap_w, LogoWid_w, LogoHei_w);

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
      cursor_pos = cursor_pos + 1;
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
        // tft.drawRoundRect(60, 81,120,59, 7, ILI9341_Custom_B);
        tft.fillRect(0, 80, 240, 10, ILI9341_WHITE);

        // tft.drawRect(80, 139, 29, 61, ILI9341_Custom_B);
        tft.drawRect(81, 140, 27, 59, ILI9341_Custom_B);

        tft.drawRect(85, 182, 19, 13, ILI9341_Custom_B);

        tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
        tft.setCursor( 34,245);   tft.print("1. Insert strip into device");

        tft.setCursor( 60,290);   tft.print("Click to continue");
      }
      dispDrawn = 1;
      delay(1000);
      ScreenState = TUTORIAL_2;
      dispDrawn = 0;
      break;

    case TUTORIAL_2:
      if(dispDrawn==0){
        // clear previous drawing
        tft.fillRect(0, 45, 240, 220, ILI9341_WHITE);

        // draw image using primitives
        tft.drawLine( 67, 130, 196, 171, ILI9341_Custom_B);   // strip top
        tft.drawLine(196, 171, 196, 180, ILI9341_Custom_B);   // strip right corner vertical
        tft.drawLine(196, 171, 158, 195, ILI9341_Custom_B);   // strip front up
        tft.drawLine(196, 180, 158, 204, ILI9341_Custom_B);   // strip front bottom
        tft.drawLine(158, 195, 158, 204, ILI9341_Custom_B);   // strip bottom corner vertical
        tft.drawLine(158, 195,  41, 161, ILI9341_Custom_B);   // strip left top
        tft.drawLine(158, 204,  41, 170, ILI9341_Custom_B);   // strip left bottom

        // collection area
        tft.drawLine(157, 164, 184, 173, ILI9341_Custom_B);   // top right
        tft.drawLine(184, 173, 156, 190, ILI9341_Custom_B);   // buttom right
        tft.drawLine(156, 190, 129, 182, ILI9341_Custom_B);   // bottom left
        tft.drawLine(129, 182, 157, 164, ILI9341_Custom_B);   // top left
        tft.drawLine(157, 164, 157, 170, ILI9341_Custom_B);   // inside corner vertical
        tft.drawLine(157, 170, 134, 184, ILI9341_Custom_B);   // inside corner top left
        tft.drawLine(157, 170, 177, 176, ILI9341_Custom_B);   // inside corner top right

        // fill in rectangle
        for (int y = 170; y <= 190; y++) {
            float leftX, rightX;

            if (y <= 184) { // left edge is AB
                leftX = 157 + (134 - 157) * (float)(y - 170) / (184 - 170);
            } else { // left edge is BC
                leftX = 134 + (156 - 134) * (float)(y - 184) / (190 - 184);
            }

            if (y <= 176) { // right edge is CA
                rightX = 177 + (157 - 177) * (float)(y - 176) / (170 - 176);
            } else { // right edge is CD
                rightX = 156 + (177 - 156) * (float)(y - 190) / (176 - 190);
            }

            // Draw horizontal line
            tft.drawLine((int)ceil(leftX), y, (int)ceil(rightX), y, ILI9341_Custom_B);
        }

        tft.drawRGBBitmap(145, 90, Drop_img_bitmap, Drop_img_Wid_b, Drop_img_Hei_b);
        
        // text
        tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
        tft.setCursor( 37,245);   tft.print("2. Drop sample into area");

        tft.setCursor( 60,290);   tft.print("Click to continue");
      }
      dispDrawn = 1;
      delay(1000);
      ScreenState = TUTORIAL_3;
      dispDrawn = 0;
      break;

    case TUTORIAL_3:
      if(dispDrawn==0){
        // clear previous drawing
        tft.fillRect(0, 45, 240, 220, ILI9341_WHITE);

        // drawing
        tft.drawRoundRect( 52, 90, 136, 136, 8, ILI9341_Custom_B);
        tft.drawRoundRect( 72,112,  96, 112, 6, ILI9341_Custom_B);

        tft.drawRoundRect(182,114,  9,  20, 3, ILI9341_Custom_B);
        tft.fillRect(170,110,17,30,ILI9341_WHITE);

        tft.drawLine(200,109,217, 93,ILI9341_Custom_B);
        tft.drawLine(202,118,222,110,ILI9341_Custom_B);
        tft.drawLine(203,128,225,126,ILI9341_Custom_B);

        tft.fillRect(0,200,240,100,ILI9341_WHITE);

        tft.drawRGBBitmap(88,128, LogoBitmap_w_small, LogoWid_w_small, LogoHei_w_small);

        // text
        tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
        tft.setCursor( 30,245);   tft.print("3. Click button to measure");
      }
      dispDrawn = 1;

      delay(1000);
      dispDrawn = 0;
      ScreenState = MEASURING;
      break;
    
    case MEASURING:
      if(dispDrawn==0){
        tft.fillScreen(ILI9341_WHITE);

        tft.drawRGBBitmap(46, 40, LogoBitmap_w, LogoWid_w, LogoHei_w);

        tft.setFont(&AvenirNextLTPro_Regular16pt7b);
        tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
      }
      dispDrawn = 1;

      switch(MeasuringState){
        case 0:
          tft.setCursor( 35,235);   tft.print("Measuring.  ");
          break;
        case 1:
          tft.setCursor( 35,235);   tft.print("Measuring.. ");
          break;
        case 2:
          tft.setCursor( 35,235);   tft.print("Measuring...");
          break;
      }
      delay(1000);
      MeasuringState = MeasuringState + 1;
      if(MeasuringState == 3){
        MeasuringState = 0;
        ScreenState = RESULTS;
        dispDrawn = 0;
      }
      break;
  }

  delay(10);
}