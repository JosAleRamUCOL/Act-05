#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define SS 5
#define RST 14
#define DIO0 2

const char* ssid = "INFINITUMD8C8";
const char* password = "Uq7Rn4Ge4m";

const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "LMRMP";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a WiFi");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void enviarMQTT(String datos, int rssi) {
  if (!client.connected()) {
    reconnect();
  }
  
  StaticJsonDocument<200> doc;
  
  DeserializationError error = deserializeJson(doc, datos);
  
  if (error) {
    Serial.print("Error al parsear JSON: ");
    Serial.println(error.c_str());
    
    doc["datos"] = datos;
    doc["rssi"] = rssi;
  } else {
    doc["rssi"] = rssi;
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  client.publish(mqtt_topic, jsonString.c_str());
  Serial.print("Mensaje enviado a MQTT: ");
  Serial.println(jsonString);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  Serial.println("Iniciando receptor LoRa");

  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(433E6)) {
    Serial.println("Error al iniciar LoRa!");
    while (1);
  }

  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  LoRa.setSyncWord(0x12);
  LoRa.enableCrc();

  Serial.println("Receptor LoRa listo!");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Paquete recibido de tamaño: ");
    Serial.println(packetSize);

    String receivedData = "";
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }

    Serial.print("Datos recibidos: ");
    Serial.println(receivedData);

    int rssi = LoRa.packetRssi();
    Serial.print("RSSI: ");
    Serial.println(rssi);

    enviarMQTT(receivedData, rssi);
  }
  
  delay(10);
}
