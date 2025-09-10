#include "JSONManager.h"

void JSONManager::jsonSerialize(const char* equipo, uint8_t contador, const String& rssi) {
  StaticJsonDocument<200> doc;
  doc["Equipo"] = equipo;
  doc["contador"] = contador;
  doc["RSSI"] = rssi;

  char jsonBuffer[256];
  size_t len = serializeJson(doc, jsonBuffer);
  
  Serial.print("JSON generado: ");
  Serial.println(jsonBuffer);

  mqttManager.publish(jsonBuffer, len);
}

void JSONManager::jsonDeserialize(const char* jsonString, const String& rssi) {
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, jsonString);
  
  if (error) {
    Serial.print("Error deserializando JSON: ");
    Serial.println(error.c_str());
    return;
  }
  
  const char* equipo = doc["Equipo"];
  uint8_t contador = doc["contador"];

  // Volver a serializar con el RSSI actualizado
  jsonSerialize(equipo, contador, rssi);
}