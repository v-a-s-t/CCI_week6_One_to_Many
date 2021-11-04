/*Week 6 CCI Diploma - Neopixel test by Andy Sheen */

//MAKE SURE YOU HAVE CHANGED YOU BOARD TO ESP32 DEV MODULE in Tools > Board.
//https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html


//MAKE SURE YOU DOWNLOAD THE ADAFRUIT NEOPIXEL LIBRARY FROM LIBRARY MANAGER
#include <Adafruit_NeoPixel.h>


// Neopixel definitions
#define LEDPIN  5
#define NUMPIXELS 1

// Touch sensor definitions
#define TOUCHPIN 4
#define TOUCHTHRESHOLD 30

bool isTouched = false;

//initialise Leds
Adafruit_NeoPixel pixels(NUMPIXELS, LEDPIN, NEO_RGB + NEO_KHZ800);

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  //set up leds
  pixels.begin();

}

void loop() {

  // if capacitive touch is triggered
  if (touchRead(TOUCHPIN) > TOUCHTHRESHOLD && isTouched == false) {
    isTouched = true;
    pixels.setPixelColor(0, pixels.ColorHSV(random(65535)));
    pixels.show();   // Send the updated color to the led.
    delay(30);
  }

  // capacitive touch debouncing
  if (touchRead(TOUCHPIN) < TOUCHTHRESHOLD && isTouched == true) {
    //set out debouncing boolean to false
    isTouched = false;
  }


}
