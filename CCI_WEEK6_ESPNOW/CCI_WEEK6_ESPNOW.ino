/*Week 6 CCI Diploma - ESPNOW by Andy Sheen */

//MAKE SURE YOU HAVE CHANGED YOU BOARD TO ESP32 DEV MODULE in Tools > Board.
//https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html


#include <esp_now.h>
#include <WiFi.h>

//MAKE SURE YOU DOWNLOAD THE ADAFRUIT NEOPIXEL LIBRARY FROM LIBRARY MANAGER
#include <Adafruit_NeoPixel.h>


// Neopixel definitions
#define LEDPIN  5
#define NUMPIXELS 1

// Program variables
#define CHANNEL 0
int incomingHue;

//initialise Leds
Adafruit_NeoPixel pixels(NUMPIXELS, LEDPIN, NEO_RGB + NEO_KHZ800);

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int id;
  int hue;
} struct_message;

//Create a struct_message called myData
struct_message receiveData;

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receiveData, incomingData, sizeof(receiveData));
  incomingHue = receiveData.hue;
  processData();
}

void processData() {
  Serial.println("I've been sent a message!");
  pixels.setPixelColor(0, pixels.ColorHSV(incomingHue));
  pixels.show();   // Send the updated color to the led.
  delay(30);
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CallBack to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  delay(100);

  //set up leds
  pixels.begin();

  //blink led red on start up to see if it's working!
  pixels.setPixelColor(0, pixels.ColorHSV(0));
  pixels.show();   // Send the updated color to the led.
  delay(300);
  pixels.clear();
  pixels.show();
}

void loop() {
  //Nothing to do
}
