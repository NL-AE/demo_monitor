// Includes
// #include <string.h>
// #include <math.h>

// Variables
enum ScreenState{
  WELCOME,
  CLK_TO_MEASURE,
  MEASURING,
  RESULTS,
  SAVE_TO_PHONE,
  SETTINGS_SELECT,
  SETTINGS_LANG,
  SETTINGS_UNIT,
  SETTINGS_BRIGHTNESS,
  SETTINGS_LIGHT_DARK_MODE,
  SETTINGS_TEXT_SIZE,
  SETTINGS_COLOR
};

volatile int ScreenState = WELCOME;
volatile int ButOnState, ButUpState, ButDnState;
// int dispDrawn = 0;

/**************************************************************************/
/*                                  SETUP                                 */
/**************************************************************************/

void setup(void) {
  // Serial
  Serial.begin(115200);

  // Setup
  // attachInterrupt(digitalPinToInterrupt(BUT_ON), ISR_BUT_ON, FALLING);
  // attachInterrupt(digitalPinToInterrupt(BUT_UP), ISR_BUT_UP, FALLING);
  // attachInterrupt(digitalPinToInterrupt(BUT_DN), ISR_BUT_DN, FALLING);

  // BLE

  // Display
}

/**************************************************************************/
/*                                  LOOP                                  */
/**************************************************************************/

void loop(void) {
  switch(ScreenState){
    case WELCOME:
      // // draw screen
      // if(dispDrawn==0){tft.fillScreen(ILI9341_BLACK);}
      // dispDrawn = 1;

      // tft.drawRGBBitmap(46, 55, LogoBitmap, LogoWid, LogoHei);
      // tft.setFont(&AvenirNextLTPro_Regular16pt7b);
      // tft.setCursor( 50,230);   tft.print("Welcome");
      // tft.setFont(&AvenirNextLTPro_Regular8pt7b);
      // tft.setCursor( 43,255);   tft.print("Please connect to the ");
      // tft.setCursor( 30,275);   tft.print("K Plus app to get started");
      // break;
    case CLK_TO_MEASURE:
      // // draw screen
      // if(dispDrawn==0){tft.fillScreen(ILI9341_BLACK);}
      // dispDrawn = 1;
      
      // tft.drawRGBBitmap(46, 60, LogoBitmap, LogoWid, LogoHei);
      // tft.setFont(&AvenirNextLTPro_Regular12pt7b);
      // tft.setCursor( 30,250);   tft.print("Click to measure");
      break;
    case MEASURING:
      break;
    case SETTINGS_SELECT:
      break;
    case SETTINGS_LANG:
      break;
    case SETTINGS_UNIT:
      break;
    case SETTINGS_BRIGHTNESS:
      break;
    case SETTINGS_LIGHT_DARK_MODE:
      break;
    case SETTINGS_TEXT_SIZE:
      break;
    case SETTINGS_COLOR:
      break;
  }
}

/**************************************************************************/
/*                           Button Interrupts                            */
/**************************************************************************/

// void ISR_BUT_ON(void){
//   ButOnState++;
// }

// void ISR_BUT_UP(void){
//   ButUpState++;
// }

// void ISR_BUT_DN(void){
//   ButDnState++;
// }
