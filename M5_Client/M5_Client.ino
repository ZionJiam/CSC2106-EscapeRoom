#include <M5StickCPlus.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Client Ready");

  BLEDevice::init("Player2"); // Unique name
  BLEServer *pServer = BLEDevice::createServer();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);  // Faster advertising
  pAdvertising->setMinInterval(100);    // 100ms
  pAdvertising->setMaxInterval(200);    // 200ms
  pAdvertising->start();
}

void loop() {
  // Just keep broadcasting
  delay(1000);
}
