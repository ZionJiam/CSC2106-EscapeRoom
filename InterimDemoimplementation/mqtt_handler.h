#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <M5StickCPlus.h>
#include "theme_type.h" // Use shared enum definition


extern PubSubClient client;

void setupMQTT(EscapeRoomTheme theme);
void loopMQTT();
void reconnectMQTT(EscapeRoomTheme theme);
void loopButtonListener();
void mqttCallback(char* topic, byte* payload, unsigned int length);

#endif

