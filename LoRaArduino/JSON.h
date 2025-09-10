#ifndef JSON_H
#define JSON_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LoRa.h> 

class JSON {
public:
    String construirJSON(uint8_t contador);
    void enviarJSON(uint8_t contador);
};

String JSON::construirJSON(uint8_t contador) {
  StaticJsonDocument<200> doc;
  doc["Equipo"] = "4LR";
  doc["contador"] = contador;

  String output;
  serializeJson(doc, output);
  return output;
}

void JSON::enviarJSON(uint8_t contador) {
  String jsonSalida = construirJSON(contador);
  Serial.print("JSON enviado: ");
  Serial.println(jsonSalida);
  
  LoRa.beginPacket();
  LoRa.print(jsonSalida);
  LoRa.endPacket();

  delay(10);
}


#endif
