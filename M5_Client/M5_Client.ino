#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <M5StickCPlus.h>

#define ROOM_PREFIX "Room_"
#define RSSI_THRESHOLD 3   // Only switch if a new room is at least 5 dB stronger
#define SCAN_INTERVAL 3     // Scan duration in seconds (short scan for better updates)
#define SCAN_DELAY 2000     // Delay between scans to avoid BLE overload

BLEClient* pClient = nullptr;
BLEScan* pBLEScan = nullptr;
BLEAddress* pBestServerAddress = nullptr;
int bestRssi = -100;
bool doConnect = false;
std::string currentRoom = "No Room Found";  // Default value

bool connectToServer(BLEAddress pAddress, std::string roomName) {
    if (pClient) {
        pClient->disconnect();
        delete pClient;
    }

    pClient = BLEDevice::createClient();
    Serial.println("Created BLE client");

    if (pClient->connect(pAddress)) {
        Serial.printf("Connected to server: %s\n", roomName.c_str());
        currentRoom = roomName;

        // Update display
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setCursor(0, 0, 2);
        M5.Lcd.printf("Connected to:\n%s\nRSSI: %d", roomName.c_str(), bestRssi);

        return true;
    } else {
        Serial.println("Failed to connect");
        return false;
    }
}

// BLE Scan callback
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        std::string deviceName = advertisedDevice.getName();
        if (!deviceName.empty() && deviceName.find(ROOM_PREFIX) != std::string::npos) {
            int currentRssi = advertisedDevice.getRSSI();
            Serial.printf("Found %s | RSSI: %d\n", deviceName.c_str(), currentRssi);

            // Check if the new device has a significantly stronger signal
            if (currentRssi > bestRssi + RSSI_THRESHOLD) {
                bestRssi = currentRssi;
                if (pBestServerAddress) {
                    delete pBestServerAddress;
                }
                pBestServerAddress = new BLEAddress(advertisedDevice.getAddress());
                currentRoom = deviceName; // Update room name
                Serial.printf("Switching to: %s | RSSI: %d\n", currentRoom.c_str(), bestRssi);
                doConnect = true;
            }
        }
    }
};

void setup() {
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

void loop() {
    Serial.println("Scanning for nearby Room_ devices...");
    pBLEScan->start(SCAN_INTERVAL, false);

    // Attempt to connect if a new best room is found
    if (doConnect && pBestServerAddress) {
        Serial.println("Attempting connection to the best room...");
        connectToServer(*pBestServerAddress, currentRoom);
        doConnect = false; // Reset the flag after connecting
    }

    // **Serial print connection status every loop**
    Serial.printf("Current Room: %s | RSSI: %d\n", currentRoom.c_str(), bestRssi);

    // **Update M5StickCPlus display dynamically**
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 2);
    M5.Lcd.printf("Now in:\n%s\nRSSI: %d", currentRoom.c_str(), bestRssi);

    delay(SCAN_DELAY); // Wait before rescanning
}
