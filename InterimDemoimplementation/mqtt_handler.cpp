#include "mqtt_handler.h"
#include "cube_handler.h"
#include "vibrationsensor_handler.h"

const char* mqtt_server = "172.20.10.1";
const char* mqtt_cube_topic = "m5stick/cube";
const char* mqtt_vibrationsensor_topic = "m5stick/command";
const char* client_id = "m5stick";
const char* mqtt_user = "zionjiam";
const char* mqtt_password = "98323646";

WiFiClient espClient;
PubSubClient client(espClient);

void setupMQTT() {
    client.setServer(mqtt_server, 1883);
    client.setCallback(mqttCallback);
    reconnectMQTT();
}

void loopMQTT() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();
}

void reconnectMQTT() {
    while (!client.connected()) {
        if (client.connect(client_id, mqtt_user, mqtt_password)) {
            client.subscribe(mqtt_cube_topic);
            client.subscribe(mqtt_vibrationsensor_topic);
        } else {
            Serial.println("MQTT Connection Failed. Retrying...");
            delay(5000);
        }
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    if (String(topic) == mqtt_cube_topic) {
        handleCubeInput(message);
    } else if (String(topic) == mqtt_vibrationsensor_topic) {
        handleVibrationSensor(message);
    }
}


void loopButtonListener(){
   M5.update(); // Update button states

    // **Button A Press - Reset Sequence**
    if (M5.BtnA.wasPressed()) {
        Serial.println("Button A pressed: Resetting sequence.");
        resetSequence();
    }
}
