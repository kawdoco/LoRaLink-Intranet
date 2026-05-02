#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#define LORA_NSS_PIN 5
#define LORA_RESET_PIN 14
#define LORA_DIO0_PIN 2
#define LORA_FREQUENCY 915E6 // Change to your region's frequency (e.g. 868E6 or 433E6)

#define LORA_SENDER 

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(LORA_NSS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa Sender Node");
}

int counter = 0;
void loop() {
  String customMessage = "Hello from LoRa_Sender! ";
  
  Serial.print("Sending packet: ");
  Serial.print(customMessage);
  Serial.println(counter);

  LoRa.beginPacket();
  LoRa.print(customMessage);
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;
  delay(5000);
}