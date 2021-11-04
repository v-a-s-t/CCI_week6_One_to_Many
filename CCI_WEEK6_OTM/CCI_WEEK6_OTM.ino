/*Week 6 CCI Diploma - One to many by Andy Sheen */

//MAKE SURE YOU HAVE CHANGED YOU BOARD TO ESP32 DEV MODULE in Tools > Board.
//https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html


#include <esp_now.h>
#include <WiFi.h>

//MAKE SURE YOU DOWNLOAD THE ADAFRUIT NEOPIXEL LIBRARY FROM LIBRARY MANAGER
#include <Adafruit_NeoPixel.h>


// Neopixel definitions
#define LEDPIN  5
#define NUMPIXELS 1

// Touch sensor definitions
#define TOUCHPIN 4
#define TOUCHTHRESHOLD 30

bool isTouched = false;

// Program variables
int CHANNEL = 0;
int incomingID;
int incomingHue;
int sizeOfClass = 42;//update to class size

int myHue = 0; //pick a hue colour between 0 - 65535
int myID = 20; // change this to be your ID number (between 0 and class size)

//initialise Leds
Adafruit_NeoPixel pixels(NUMPIXELS, LEDPIN, NEO_RGB + NEO_KHZ800);


// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int id;
  int hue;
} struct_message;

//Create a struct_message called myData
struct_message sendData;
//Create a struct_message called myData
struct_message receiveData;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Sent to ID: ");
  Serial.println(sendData.id);
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receiveData, incomingData, sizeof(receiveData));
  incomingID = receiveData.id;
  incomingHue = receiveData.hue;
  processData();
}

void processData() {
  //if received data is my ID
  if (incomingID == myID) {
    Serial.println("I've been sent a message!");
    //set led to incoming colour
    pixels.setPixelColor(0, pixels.ColorHSV(incomingHue));
    pixels.show();   // Send the updated color to the led.
    delay(300);
    //turn off after 300ms
    pixels.clear();
    pixels.show();
  }
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

  // Once ESPNow is successfully Init, we will register for Send CallBack to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Once ESPNow is successfully Init, we will register for recv CallBack to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = CHANNEL;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

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

  // if capacitive touch is triggered
  if (touchRead(TOUCHPIN) > TOUCHTHRESHOLD && isTouched == false) {
    Serial.println("I'm going to send a message!");

    //set our debouncing boolean to true
    isTouched = true;

    // Set values to send. ID to send will be random
    sendData.id = random(sizeOfClass + 1);
    sendData.hue = myHue;

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sendData, sizeof(sendData));
    delay(30);
  }

  // capacitive touch debouncing
  if (touchRead(TOUCHPIN) < TOUCHTHRESHOLD && isTouched == true) {
    //set out debouncing boolean to false
    isTouched = false;
  }


}
