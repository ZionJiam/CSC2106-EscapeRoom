#include "BLETracker.h"

// Constructor
BLETracker::BLETracker()
    : pBLEScan(nullptr), pClient(nullptr), pBestServerAddress(nullptr), 
      bestRssi(-100), doConnect(false), currentRoom("No Room Found") {}

// Initialize BLE and display
void BLETracker::begin() {
    Serial.begin(115200);
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 2);
    M5.Lcd.println("BLE Tracker (Client)");

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(50);
}

// Scan for BLE rooms
void BLETracker::scanForRooms() {
    Serial.println("Scanning for nearby Room_ devices...");
    pBLEScan->start(SCAN_INTERVAL, false);

    if (doConnect && pBestServerAddress) {
        Serial.println("Attempting connection to the best room...");
        connectToBestRoom();
        doConnect = false;
    }
}

// BLE Device Scan Callback
void BLETracker::MyAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice) {
    std::string deviceName = advertisedDevice.getName();
    if (!deviceName.empty() && deviceName.find(ROOM_PREFIX) != std::string::npos) {
        int currentRssi = advertisedDevice.getRSSI();
        Serial.printf("Found %s | RSSI: %d\n", deviceName.c_str(), currentRssi);

        BLETracker* tracker = static_cast<BLETracker*>(BLEDevice::getScan()->getAdvertisedDeviceCallbacks());

        if (currentRssi > tracker->bestRssi + RSSI_THRESHOLD) {
            tracker->bestRssi = currentRssi;
            if (tracker->pBestServerAddress) {
                delete tracker->pBestServerAddress;
            }
            tracker->pBestServerAddress = new BLEAddress(advertisedDevice.getAddress());
            tracker->currentRoom = deviceName;
            Serial.printf("Switching to: %s | RSSI: %d\n", tracker->currentRoom.c_str(), tracker->bestRssi);
            tracker->doConnect = true;
        }
    }
}

// Connect to the best available room
bool BLETracker::connectToBestRoom() {
    if (pClient) {
        pClient->disconnect();
        delete pClient;
    }

    pClient = BLEDevice::createClient();
    Serial.println("Created BLE client");

    if (pClient->connect(*pBestServerAddress)) {
        Serial.printf("Connected to server: %s\n", currentRoom.c_str());

        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(0, 0, 2);
        M5.Lcd.printf("Connected to:\n%s\nRSSI: %d", currentRoom.c_str(), bestRssi);

        return true;
    } else {
        Serial.println("Failed to connect");
        return false;
    }
}

// Getter for RSSI value
int BLETracker::getCurrentRSSI() {
    return bestRssi;
}

// Getter for current room name
std::string BLETracker::getCurrentRoom() {
    return currentRoom;
}
