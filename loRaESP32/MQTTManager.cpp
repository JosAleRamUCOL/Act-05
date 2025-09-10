#include "MQTTManager.h"

void MQTTManager::setup() {
  // Conectar a WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Configurar MQTT
  client.setServer(mqtt_server, mqtt_port);
}

void MQTTManager::publish(const char* message, size_t length) {
  if (!client.connected()) {
    reconnect();
  }
  
  if (client.publish(mqtt_topic, (const uint8_t*)message, length, false)) {
    Serial.println("Mensaje publicado correctamente");
  } else {
    Serial.println("Error al publicar mensaje");
  }
}

void MQTTManager::reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando a MQTT...");
    
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado");
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando en 5 segundos");
      delay(5000);
    }
  }
}

void MQTTManager::loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void MQTTManager::setCallback(void (*callback)(char*, byte*, unsigned int)) {
  client.setCallback(callback);
}