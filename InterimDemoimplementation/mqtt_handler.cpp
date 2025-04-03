#include "mqtt_handler.h"
#include "zombie_handler.h"
#include "theme_handler.h"
#include "theme_type.h"



// Check for HomeAssistant Network if Using Hotspot IP Address
const char* mqtt_server = "192.168.1.115";
const char* client_id = "m5stick";
const char* mqtt_user = "zionjiam";
const char* mqtt_password = "98323646";

WiFiClientSecure espClient;
PubSubClient client(espClient);

EscapeRoomTheme currentThemeGlobal = THEME_NONE;

void setupMQTT(EscapeRoomTheme theme) {
    espClient.setInsecure();  // For self-signed or unverified certs
    client.setServer(mqtt_server, 8883);
    client.setCallback(mqttCallback);
    currentThemeGlobal = theme;
    reconnectMQTT(currentThemeGlobal);
}

void loopMQTT() {
    if (!client.connected()) {
      reconnectMQTT(currentThemeGlobal);
    }
    client.loop();
}

void reconnectMQTT(EscapeRoomTheme theme) {
  while (!client.connected()) {
    if (client.connect("m5stick", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT Broker!");

      switch (theme) {
        case THEME_ZOMBIE:
          client.subscribe("m5stick/zombie/cube");
          client.subscribe("m5stick/zombie/shake");
          client.subscribe("m5stick/zombie/endscreen");
          break;

        case THEME_HAUNTED:
          client.subscribe("m5stick/haunted/tap");
          break;

        case THEME_PRISON:
          client.subscribe("m5stick/prison/rotate");
          client.subscribe("m5stick/prison/slide");
          break;
      }
    } else {
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(0, 10);
      M5.Lcd.printf("MQTT connect failed, state= %d", client.state());
      Serial.println("MQTT connection failed, retrying...");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) message += (char)payload[i];
  String topicStr = String(topic);

  switch (currentThemeGlobal) {
    case THEME_ZOMBIE:
      handleZombieMQTT(topicStr, message);
      break;
    case THEME_HAUNTED:
      handleZombieMQTT(topicStr, message);
      //handleHauntedMQTT(topicStr, message);
      break;
    case THEME_PRISON:
      handleZombieMQTT(topicStr, message);
      //handlePrisonMQTT(topicStr, message);
      break;
  }
}

void loopButtonListener(){

  //  if (M5.BtnA.wasPressed()) {
  //   currentTheme = (EscapeRoomTheme)((currentTheme + 1) % 3); // Cycle between 0,1,2
  //   M5.Lcd.fillScreen(BLACK);
  //   M5.Lcd.setCursor(0, 10);
  //   M5.Lcd.printf("Switched Theme: %d", currentTheme);

  //   // Force re-subscribe to new topics
  //   client.disconnect(); // Trigger reconnectMQTT()
  // }
    switch (currentThemeGlobal) {
        case THEME_ZOMBIE:
          loopZombieButtonListener();
          break;

        case THEME_HAUNTED:
          break;

        case THEME_PRISON:
          break;
      }
}
