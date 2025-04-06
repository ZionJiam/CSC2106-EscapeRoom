#include "BLEAdvertiser.h"

void BLEAdvertiser::begin(const std::string& deviceName) {
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.println("Advertising...");

    // Initialize BLE with given name
    BLEDevice::init(deviceName);
    BLEServer *pServer = BLEDevice::createServer(); // You can extend for callbacks if needed

    // Configure and start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinInterval(100); // 100ms
    pAdvertising->setMaxInterval(200); // 200ms
    pAdvertising->start();

    Serial.printf("Started advertising as %s\n", deviceName.c_str());
}

void BLEAdvertiser::stop() {
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->stop();
    Serial.println("BLE Advertising stopped.");
}