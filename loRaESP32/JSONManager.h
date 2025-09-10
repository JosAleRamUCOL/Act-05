#ifndef JSON_MANAGER_H
#define JSON_MANAGER_H

#include <ArduinoJson.h>
#include "MQTTManager.h"

class JSONManager {
public:
    void jsonSerialize(const char* equipo, uint8_t contador, const String& rssi);
    void jsonDeserialize(const char* jsonString, const String& rssi);
    
private:
    MQTTManager mqttManager;
};

#endif