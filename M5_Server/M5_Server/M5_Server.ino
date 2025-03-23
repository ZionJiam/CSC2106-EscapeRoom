#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>
#include <M5StickCPlus.h>

#define SERVER_NAME "Room_A"

BLEServer* pServer = nullptr;
std::string connectedDevice = "No device connected";

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) override {
        Serial.println("A central device connected!");
        connectedDevice = BLEAddress(param->connect.remote_bda).toString();
    }

    void onDisconnect(BLEServer* pServer) override {
        Serial.println("Central device disconnected!");
        connectedDevice = "No device connected";
        pServer->getAdvertising()->start();
    }
};

void setup() {
    Serial.begin(115200);
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 2);
    M5.Lcd.println("Room_A (Server)");

    BLEDevice::init(SERVER_NAME);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService* pService = pServer->createService("12345678-1234-1234-1234-1234567890ab");
    pService->start();

    BLEDevice::startAdvertising();
}

void loop() {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 2);
    M5.Lcd.printf("ROOM A \n\nConnected:\n%s", connectedDevice.c_str());
    
    Serial.printf("Connected Client: %s\n", connectedDevice.c_str());
    
    delay(1000);
}
