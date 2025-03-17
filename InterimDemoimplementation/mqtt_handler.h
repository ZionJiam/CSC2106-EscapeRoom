#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <M5StickCPlus.h>


extern PubSubClient client;  // Externally accessible MQTT client

void setupMQTT();
void loopMQTT();
void loopButtonListener();
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);

#endif
