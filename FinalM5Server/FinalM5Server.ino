#include <M5StickCPlus.h> // M5StickC Plus library for device control
#include <BLEDevice.h> // Core BLE functions
#include <BLEUtils.h>  // BLE helper utilities
#include <BLEScan.h> // BLE scanning functionality
#include <WiFi.h>  // WiFi support
#include <PubSubClient.h> // MQTT client library

// --- WiFi and MQTT Setup ---
const char* ssid = "Linksys12197";
const char* password = "e3sneffpkf";
const char* mqtt_server = "192.168.1.113";
const char* topic_publish = "m5stick/data";

WiFiClient espClient; // Handles WiFi connection
PubSubClient client(espClient); // MQTT client using WiFi

// --- BLE Scan Settings ---
BLEScan* pBLEScan; // BLE scanner object
int scanTime = 1; // seconds

// Server identifier (A1,A2,B1,B2,C1,C2,D1,D2)
const char* server_id = "Server-D2"; // Used to identify which M5Stick is sending data

// --- Connect to WiFi ---
void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); // Start WiFi connection
  M5.Lcd.print("Connecting WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.println("\nWiFi Connected");
}

// --- Reconnect to MQTT Broker if disconnected ---
void reconnectMQTT() {
  while (!client.connected()) {
    M5.Lcd.println("Connecting MQTT...");
    String clientId = "M5Stick-";
    clientId += String(random(0xffff), HEX); // Create unique client ID
    if (client.connect(clientId.c_str())) {
      M5.Lcd.println("MQTT Connected");
    } else {
      M5.Lcd.println("Retry MQTT in 5s");
      delay(5000);
    }
  }
}

// --- Main Setup ---
void setup() {
  M5.begin(); // Initialize M5Stick
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Server Ready");

  setupWifi(); // Connect to WiFi
  client.setServer(mqtt_server, 1883); // Set MQTT server IP and port

  BLEDevice::init(""); // Initialize BLE functions
  pBLEScan = BLEDevice::getScan(); // Create BLE scanner
  pBLEScan->setActiveScan(true);
}

// --- Main Loop ---
void loop() {
  if (!client.connected()) reconnectMQTT(); // Make sure MQTT is connected
  client.loop();

  int rssi1 = -999;
  int rssi2 = -999;

  BLEScanResults results = pBLEScan->start(scanTime, false); // Scan for BLE devices

  // Go through each found device
  for (int i = 0; i < results.getCount(); i++) {
    BLEAdvertisedDevice d = results.getDevice(i);
    String name = d.getName().c_str();
    // Check for devices named "Player1" and "Player2"
    if (name == "Player1") {
      rssi1 = d.getRSSI();
    } else if (name == "Player2") {
      rssi2 = d.getRSSI();
    }
  }

  pBLEScan->clearResults(); // Clear results 

  // Display on screen
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);

  M5.Lcd.println("Server:");
  M5.Lcd.println(server_id);

  if (rssi1 != -999) {
    M5.Lcd.println("P1 RSSI: " + String(rssi1));
  } else {
    M5.Lcd.println("P1: No signal");
  }

  if (rssi2 != -999) {
    M5.Lcd.println("P2 RSSI: " + String(rssi2));
  } else {
    M5.Lcd.println("P2: No signal");
  }

  // Format MQTT message
  String payload = "{";
  payload += "\"server_id\":\"" + String(server_id) + "\",";
  payload += "\"player1_rssi\":" + String(rssi1) + ",";
  payload += "\"player2_rssi\":" + String(rssi2);
  payload += "}";

  client.publish(topic_publish, payload.c_str());

  delay(1000);
}
