#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>

class MQTTManager {
public:
    void setup();
    void loop();
    void publish(const char* message, size_t length);
    void setCallback(void (*callback)(char*, byte*, unsigned int));
    
private:
    void reconnect();
    
    const char* ssid = "INFINITUMD8C8";
    const char* password = "Uq7Rn4Ge4m";
    const char* mqtt_server = "test.mosquitto.org";
    const int mqtt_port = 1883;
    const char* mqtt_topic = "LMRMP";
    
    WiFiClient espClient;
    PubSubClient client = PubSubClient(espClient);
};

#endif