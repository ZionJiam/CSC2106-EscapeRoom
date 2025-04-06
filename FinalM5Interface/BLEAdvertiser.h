#ifndef BLE_ADVERTISER_H
#define BLE_ADVERTISER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <M5StickCPlus.h>

class BLEAdvertiser {
public:
    void begin(const std::string& deviceName);
    void stop(); // <-- Add stop function declaration
};

#endif
