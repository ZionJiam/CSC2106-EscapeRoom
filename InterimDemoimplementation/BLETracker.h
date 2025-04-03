#ifndef BLE_TRACKER_H
#define BLE_TRACKER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <M5StickCPlus.h>

// Constants
#define ROOM_PREFIX "Room_"
#define RSSI_THRESHOLD 3   // Minimum RSSI improvement to switch rooms
#define SCAN_INTERVAL 3     // Scan duration (seconds)
#define SCAN_DELAY 2000     // Delay between scans (milliseconds)

// BLE Tracking Class
class BLETracker {
public:
    BLETracker();
    void begin();               // Initialize BLE and display
    void scanForRooms();        // Scan for BLE devices
    bool connectToBestRoom();   // Connect to the best available BLE room
    int getCurrentRSSI();       // Get the current room's RSSI value
    std::string getCurrentRoom(); // Get the current room name

private:
    BLEScan* pBLEScan;
    BLEClient* pClient;
    BLEAddress* pBestServerAddress;
    int bestRssi;
    bool doConnect;
    std::string currentRoom;

    // Internal BLE scanning callback class
    class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
        void onResult(BLEAdvertisedDevice advertisedDevice) override;
    };
};

#endif
