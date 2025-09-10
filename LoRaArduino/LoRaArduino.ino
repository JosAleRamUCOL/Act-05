#include <SPI.h>
#include "JSON.h"

#define SS 10
#define RST 9
#define DIO0 2
#define BTN 3

int contador = 0;
JSON json;

void setup() {
  Serial.begin(9600);
  while (!Serial); // Esperar a que el puerto serial se inicialice

  pinMode(BTN, INPUT_PULLUP);

  Serial.println("Iniciando transmisor LoRa SX1278");

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

  Serial.println("Transmisor LoRa listo!");
}

void loop() {
  // Leer el estado del botón (está en PULLUP, por lo que presionado es LOW)
  if (digitalRead(BTN) == LOW) {
    // Evitar rebotes: esperar un poco y verificar aún está presionado
    delay(50);
    if (digitalRead(BTN) == LOW) {
      contador++;
      Serial.print("Contador: ");
      Serial.println(contador);
      json.enviarJSON(contador); // Enviamos el contador
      // Esperar a que se suelte el botón
      while (digitalRead(BTN) == LOW) delay(10);
    }
  }
  delay(10); // Pequeña pausa para no saturar
}