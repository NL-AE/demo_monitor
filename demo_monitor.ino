
// Includes
#include <string.h>
#include <math.h>
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Display
#define SPI_SCLK 4   // SLK
#define SPI_MOSI 6   // MOSI
#define SPI_MISO 5   // MISO
#define DISP_CSL 7   // chip select
#define DISP_DCP 21  // direction control pin
#define DISP_LED 10  // LED PWM pin
#define DISP_POW 20  // display power pin
#define DISP_RST 3

Adafruit_ILI9341 tft = Adafruit_ILI9341(DISP_CSL, DISP_DCP, DISP_RST);

// generated with lcd-image-converter.exe
// output must be R5G6B5, 16bit
#include "Logo_w.h"               
#include "Drop_img.h"
#include "Logo_w_small.h"

// generated with https://rop.nl/truetype2gfx/
#include "AvenirNextLTPro_Regular6pt7b.h"
#include "AvenirNextLTPro_Regular8pt7b.h"
#include "AvenirNextLTPro_Regular12pt7b.h"
#include "AvenirNextLTPro_Regular16pt7b.h"
#include "AvenirNextLTPro_Regular24pt7b.h"
#include "AvenirNextLTPro_Regular28pt7b.h"

#define ILI9341_Custom_B tft.color565(39,47,83)

// Sleep
volatile unsigned long lastActivityTime = 0; 
const unsigned long sleepDelay = 20000;   // ms before going to sleep

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

// Buttons
#define Button_Pow  0
#define Button_Up   1
#define Button_Dn   2

volatile uint32_t last_interrupt_time_pow = 0;
volatile uint32_t last_interrupt_time_dn = 0;
volatile uint32_t last_interrupt_time_up = 0;
const uint32_t debounce_delay_ms = 300; // debounce time

void Button_Pow_ISR() {
  lastActivityTime = millis();
  uint32_t current_time = millis();

  if (current_time - last_interrupt_time_pow > debounce_delay_ms) {
      Serial.println("power button pressed");

      dispDrawn = 0;

      ScreenState = ScreenState+1;

      if(ScreenState >= 8){
        ScreenState = 0;
      }

      last_interrupt_time_pow = current_time;
  }
}

void Button_Up_ISR() {
  lastActivityTime = millis();
  uint32_t current_time = millis();

  if (current_time - last_interrupt_time_dn > debounce_delay_ms) {
      Serial.println("up button pressed");

      cursor_pos = cursor_pos-1;    // n.b. highest option is cursor_pos = 0. next one down is cursor_pos = 1 so need to -1 to move upwards
      if(cursor_pos < 0){
        cursor_pos = 3;
      }
      
      last_interrupt_time_dn = current_time;
  }
}

void Button_Dn_ISR() {
  lastActivityTime = millis();
  uint32_t current_time = millis();

  if (current_time - last_interrupt_time_up > debounce_delay_ms) {
      Serial.println("down button pressed");

      cursor_pos = cursor_pos+1;
      if(cursor_pos >= 4){
        cursor_pos = 0;
      }

      last_interrupt_time_up = current_time;
  }
}

/**************************************************************************/
/*                                  SETUP                                 */
/**************************************************************************/

void setup(void) {
  // Serial
  Serial.begin(115200);

  pinMode(Button_Pow, INPUT_PULLUP);
  pinMode(Button_Up, INPUT_PULLUP);
  pinMode(Button_Dn, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(Button_Pow), Button_Pow_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button_Up), Button_Up_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button_Dn), Button_Dn_ISR, FALLING);

  pinMode(DISP_LED, OUTPUT);
  pinMode(DISP_POW, OUTPUT);
  digitalWrite(DISP_LED, 1);
  digitalWrite(DISP_POW, 1);
  delay(5);

  // Display
  // digitalWrite(DISP_POW,1);
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(ILI9341_WHITE);
  // fadePWM(DISP_LED, true, 500);

  ScreenState = WELCOME;
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
      }
      dispDrawn = 1;
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

        cursor_pos = 0;   // reset cursor position so it always 0
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
      delay(500);
      MeasuringState = MeasuringState + 1;
      if(MeasuringState == 3){
        MeasuringState = 0;
        delay(500);
        ScreenState = RESULTS;
        dispDrawn = 0;
      }
      break;

    case RESULTS:
      if(dispDrawn==0){
        tft.fillScreen(ILI9341_WHITE);
        tft.fillRect(0,0,240,45,ILI9341_Custom_B);

        tft.setFont(&AvenirNextLTPro_Regular8pt7b);
        tft.setTextColor(ILI9341_WHITE,ILI9341_Custom_B);
        tft.setCursor( 35, 26);   tft.print("Measurement complete!");

        tft.fillCircle( 32,200, 6,tft.color565(211,126,136));
        tft.fillCircle( 57,200, 8,tft.color565(229,190,135));
        tft.fillCircle( 84,200,10,tft.color565(166,196,153));
        tft.fillCircle(120,200,12,tft.color565(160,188,176));
        tft.fillCircle(156,200,10,tft.color565(166,196,153));
        tft.fillCircle(183,200, 8,tft.color565(229,190,135));
        tft.fillCircle(208,200, 6,tft.color565(211,126,136));

        tft.setFont(&AvenirNextLTPro_Regular8pt7b);
        tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
        tft.setCursor(158,127);   tft.print("mmol/L");
        tft.setCursor( 40,249);   tft.print("Click to save and finish");

        // now draw the variable parts
        float measured_result = 2.0 + ((float)random(0, 10000) / 10000.0) * (5.0 - 2.0);
        String str = String(measured_result,2);

        tft.setFont(&AvenirNextLTPro_Regular28pt7b);
        tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
        tft.setCursor(30,152);   tft.print(str);

        int triangle_pos = map(measured_result, 1,6,32,208);
        tft.fillTriangle(triangle_pos-4, 178, triangle_pos+4, 178, triangle_pos, 184, ILI9341_BLACK);

      }
      dispDrawn = 1;
      break;

    case PREV_RESULTS:
      if(dispDrawn==0){
        // tft.fillScreen(ILI9341_WHITE);
        tft.fillRect(0,0,240,45,ILI9341_Custom_B);
        tft.fillRect(0,45,240,275,ILI9341_WHITE);

        tft.setFont(&AvenirNextLTPro_Regular8pt7b);
        tft.setTextColor(ILI9341_WHITE,ILI9341_Custom_B);
        tft.setCursor( 35, 26);   tft.print("Previous Measurements");

        // graph
        tft.setFont(&AvenirNextLTPro_Regular6pt7b);
        tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
        tft.setCursor(20, 71);   tft.print("7");
        tft.setCursor(20, 83);   tft.print("6");
        tft.setCursor(20, 95);   tft.print("5");
        tft.setCursor(20,107);   tft.print("4");
        tft.setCursor(20,119);   tft.print("3");
        tft.setCursor(20,131);   tft.print("2");
        tft.setCursor(20,143);   tft.print("1");
        tft.setCursor(20,157);   tft.print("0");

        tft.drawLine(30, 62, 30, 154, ILI9341_BLACK);
        tft.drawLine(31, 62, 31, 154, ILI9341_BLACK);

        tft.drawLine(30, 153, 210, 153, ILI9341_BLACK);
        tft.drawLine(30, 154, 210, 154, ILI9341_BLACK);

        // colored background
        // tft.fillRect(32, 68,178,14,tft.color565(229,190,194));
        // tft.fillRect(32, 82,178, 7,tft.color565(240,218,194));
        // tft.fillRect(32, 89,178, 7,tft.color565(207,221,201));
        // tft.fillRect(32, 96,178,21,tft.color565(204,217,211));
        // tft.fillRect(32,117,178, 7,tft.color565(207,221,201));
        // tft.fillRect(32,124,178, 7,tft.color565(240,218,194));
        // tft.fillRect(32,131,178,22,tft.color565(229,190,194));

        // grey background
        tft.fillRect(32, 92,178, 18,tft.color565(220,220,220));
        tft.drawLine(32, 92,210, 92,tft.color565(190,190,190));
        tft.drawLine(32,110,210,110,tft.color565(190,190,190));

        // plot dots and lines on graph
        uint16_t plot_results[] = {320,480,470,365,514,454,326,432};
        int plot_y;
        int plot_x;

        for(int i=0; i<8; i++){
          int plot_y_prev = plot_y;
          int plot_x_prev = plot_x;

          // calculate x/y position
          plot_y = map(plot_results[i],0,700,153,68);
          plot_x = 40 + i*23;

          // draw circle
          tft.fillCircle(plot_x, plot_y, 2, ILI9341_Custom_B);

          // draw line
          if(i>0 && i<8){
            tft.drawLine(plot_x_prev, plot_y_prev, plot_x, plot_y, ILI9341_Custom_B);
          }
        }

        // table
        tft.drawLine(18, 170, 222, 170, ILI9341_BLACK);
        tft.drawLine(18, 171, 222, 171, ILI9341_BLACK);

        tft.fillRect(18,195,204,20,tft.color565(220,220,220));
        tft.fillRect(18,235,204,20,tft.color565(220,220,220));
        tft.fillRect(18,275,204,20,tft.color565(220,220,220));

        tft.fillTriangle(110,302,126,302,118,310,ILI9341_BLACK);

        tft.setFont(&AvenirNextLTPro_Regular8pt7b);
        tft.setTextColor(ILI9341_BLACK);
        tft.setCursor( 21, 190);   tft.print("Jul. 22 2025");
        tft.setCursor( 21, 210);   tft.print("Jul. 24 2025");
        tft.setCursor( 21, 230);   tft.print("Jul. 28 2025");
        tft.setCursor( 21, 250);   tft.print("Aug. 01 2025");
        tft.setCursor( 21, 270);   tft.print("Aug. 03 2025");
        tft.setCursor( 21, 290);   tft.print("Aug. 04 2025");

        tft.setCursor(180, 190);   tft.print("4.70");
        tft.setCursor(180, 210);   tft.print("3.65");
        tft.setCursor(180, 230);   tft.print("5.14");
        tft.setCursor(180, 250);   tft.print("4.54");
        tft.setCursor(180, 270);   tft.print("3.26");
        tft.setCursor(180, 290);   tft.print("4.32");
      }
      dispDrawn = 1;
      break;
  }

  delay(10);

  // go to sleep
  if (millis() - lastActivityTime > sleepDelay) {
    digitalWrite(DISP_LED,0);
    tft.fillScreen(ILI9341_BLACK);
    digitalWrite(DISP_POW,0);

    esp_deep_sleep_enable_gpio_wakeup(((1ULL << GPIO_NUM_0) | 
                                       (1ULL << GPIO_NUM_1) | 
                                       (1ULL << GPIO_NUM_2)), 
                                       ESP_GPIO_WAKEUP_GPIO_LOW);

    Serial.println("sleepy time");
    esp_deep_sleep_start(); 
  }
}