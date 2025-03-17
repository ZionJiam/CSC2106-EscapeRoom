#include <M5StickCPlus.h>
#include <WiFi.h>
#include "mqtt_handler.h"

const char* ssid = "Zion iPhone";
const char* password = "Just123abc";

void setup() {
    M5.begin();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }

    // LED pin SETUP and reset
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);

    setupMQTT();  // Initialize MQTT
}

void loop() {
    loopButtonListener();
    loopMQTT();   // Handle MQTT events
}
