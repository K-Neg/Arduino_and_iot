#include "WiFi.h"
#include <Wire.h>
#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
#include "FS.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "RTClib.h"

//BME280 i2c PINS
#define SDA_1 21
#define SCL_1 22

#define SDA_2 33
#define SCL_2 32

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP 10

RTC_DS3231 rtc;
TwoWire I2Cone = TwoWire(0);
TwoWire I2Ctwo = TwoWire(1);
Adafruit_BME280 bme;
Adafruit_BME280 bme_2;
AsyncWebServer server(80);

String lastfile = "/a.txt";
String dataline();
//bool print_wakeup_reason();
bool modo = false;

//sensor values
float temperature = 1;
float humidity = 1;

void setup()
{
    Serial.begin(9600);
    startSPIFFS();
    modo = print_wakeup_reason();

    if (modo)
    {
        I2Cone.begin(SDA_1, SCL_1, 100000);
        I2Ctwo.begin(SDA_2, SCL_2, 100000);
        startRTC();
        startBME();
        deleteFile(SPIFFS, "/a.txt");
        writeFile(SPIFFS, "/a.txt", "Start");
    }
    else
    {
        startWifi();
        delay(2000);
        //home page
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/index.html", String(), false);
        });
        //tries to direct send the txt
        server.on("/direct", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/b.txt");
        });
        //send txt as a file
        server.on("/filehard", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, lastfile, String(), true);
        });
        server.begin();
    }
}

void loop()
{
    if (modo)
    {
        printValues();
        //delay(3000);
        String buf = dataline();
        char outPutArray[sizeof(buf)];
        buf.toCharArray(outPutArray, sizeof(outPutArray));
        appendFile(SPIFFS, "/a.txt", outPutArray);
        startSleep();
    }

    if (millis() >= 30000)
        startSleep();
}
