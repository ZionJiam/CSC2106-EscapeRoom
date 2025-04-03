#include <M5StickCPlus.h>
#include <WiFi.h>
#include "mqtt_handler.h"
#include "theme_handler.h"


const char* ssid = "Linksys12197";
const char* password = "e3sneffpkf";


void setup() {
  M5.begin();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setRotation(3);
  M5.Lcd.setCursor(30, 10);
  M5.Lcd.println("Connecting...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }

  // LED pin setup
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  displayThemeSelection();  // Show theme list first
}

void loop() {
  M5.update();

  if (!themeLocked) {
    handleThemeSelection();  // Button B to scroll, A to confirm
  } else {
    loopButtonListener();
    handleThemeReset();      // Hold A to reset theme
    loopMQTT();              // Maintain MQTT connection and loop
  }
}
