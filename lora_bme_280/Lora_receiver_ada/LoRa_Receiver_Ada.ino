/*********
  This is designed to work with TTGO ESP32 + SX1276 LoRa chip + SSD1306 OLED board
  Part of this code is based on Rui Santos tutorials (https://randomnerdtutorials.com)
*********/

//include credencials
#include "config.h"

// Import Wi-Fi library
#include "AdafruitIO_WiFi.h"
//#include <WiFi.h>
//#include "ESPAsyncWebServer.h"

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 915E6

//OLED pins
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels



AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
AdafruitIO_Feed *feedT = io.feed("temp1020");
AdafruitIO_Feed *feedH = io.feed("humi1020");
AdafruitIO_Feed *feedR = io.feed("debi1020");
AdafruitIO_Feed *feedG = io.feed("general");
AdafruitIO_Feed *feedCounter = io.feed("counter");


// Initialize variables to get and save LoRa data
bool debug = false;
int rssi = 1;
String loRaMessage = "1";
String temperature = "1";
String humidity = "1";
String pressure = "1";
String readingID = "1";
String buf = "1";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);


//Initialize OLED display
void startOLED() {
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  } else {
    Serial.println("oled ok");
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(13, 0);
  display.print("Lora Receiver");
}

//Initialize LoRa module
void startLoRA() {
  int counter;
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  display.setCursor(0, 10);
  while (!LoRa.begin(BAND) && counter < 10) {
    display.print(".");
    counter++;
    delay(500);
  }

  display.setCursor(0, 10);
  if (counter == 10) {
    // Increment readingID on every new reading
    display.print("LoRa failed!");
  } else {
    display.print("LoRa  init OK!");
  }

  display.display();
  delay(2000);
}

// Read LoRa packet and get the sensor readings
void getLoRaData() {
  Serial.print("Lora packet received: ");
  // Read packet
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    // LoRaData format: readingID/temperature&soilMoisture#batterylevel
    // String example: 1/27.43&654#95.34
    Serial.print(LoRaData);

    // Get readingID, temperature and soil moisture
    int pos1 = LoRaData.indexOf('/');
    int pos2 = LoRaData.indexOf('&');
    int pos3 = LoRaData.indexOf('#');
    readingID = LoRaData.substring(0, pos1);
    temperature = LoRaData.substring(pos1 + 1, pos2);
    humidity = LoRaData.substring(pos2 + 1, pos3);
    pressure = LoRaData.substring(pos3 + 1, LoRaData.length());
  }
  // Get RSSI
  rssi = LoRa.packetRssi();
  Serial.print(" with RSSI ");
  Serial.println(rssi);

}


//display
void updateDisplay() {
  debug = !debug;
  display.clearDisplay();
  display.setCursor(10, 20);
  display.print(temperature);
  display.print("  C");
  display.setCursor(10, 30);
  display.print(humidity);
  display.print("  %");
  display.setCursor(10, 40);
  display.print(rssi);
  display.print("  db");
  display.setCursor(10, 50);
  float time_old = millis() / 1000;
  display.print(time_old);
  display.setCursor(0, 1);
  display.print(debug);
  display.display();
}


void setup() {
  Serial.begin(115200);
  Serial.print("Serial OK\n");
  startOLED();
  startLoRA();

  //display.clearDisplay();
  display.setCursor(0, 20);
  display.print("Cloud");
  display.display();

  //connect to io.adafruit.com
  io.connect();
  io.run();
  feedCounter->save(1);

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    display.print(".");
    delay(500);

  }
  display.setCursor(0, 20);
  display.print("                ");
  display.setCursor(0, 20);
  display.print("Connect to Cloud");
  display.display();

}

void loop() {
  // Check if there are LoRa packets available
  int packetSize = LoRa.parsePacket();

  //delay(1000);
  //updateDisplay();

  if (packetSize) {

    getLoRaData();
    updateDisplay();
    io.run();
    feedT->save(temperature.toInt());
    delay(2000);
    feedH->save(humidity.toInt());
    delay(2000);
    feedR->save(rssi);
    delay(2000);
    //buf = temperature + "*" + humidity + "*" + rssi;-
    //feedG->save(buf);
    feedCounter->save(0);
  }

  if (millis() > 600000) ESP.restart();
}
