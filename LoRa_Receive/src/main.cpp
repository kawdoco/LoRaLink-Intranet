#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#define LORA_NSS_PIN 5
#define LORA_RESET_PIN 14
#define LORA_DIO0_PIN 2
#define LORA_FREQUENCY 433E6 // Change to your region's frequency (e.g. 868E6 or 433E6)

#define LORA_RECEIVER

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  String receivedText = "";
  while (LoRa.available()) {
    receivedText += (char)LoRa.read();
  }

  Serial.print("Received packet '");
  Serial.print(receivedText);
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(LORA_NSS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Force matching RF settings
  LoRa.setSyncWord(0xF3);           // Unique network ID (0-255)
  LoRa.setTxPower(17);              // Default power

  // Register the receive callback
  LoRa.onReceive(onReceive);
  // Put the radio into continuous receive mode
  LoRa.receive();

  Serial.println("LoRa Receiver Node");
}

void loop() {
  // Do nothing in loop, the interrupt will handle incoming packets
}