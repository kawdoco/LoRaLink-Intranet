#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#define LORA_NSS_PIN 5
#define LORA_RESET_PIN 14
#define LORA_DIO0_PIN 2
#define LORA_FREQUENCY 433E6 // Change to your region's frequency (e.g. 868E6 or 433E6)

#define LORA_SENDER 

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(LORA_NSS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Force matching RF settings
  LoRa.setFrequency(433000000);
  LoRa.setSpreadingFactor(9);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setTxPower(17, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setSyncWord(0x12); // Default sync word
  
  Serial.println("LoRa Sender Node");
}

int counter = 0;
void loop() {
  String customMessage = "Test Packet ";
  customMessage += String(counter);
  
  Serial.print("Sending: ");
  Serial.println(customMessage);

  LoRa.beginPacket();
  LoRa.print(customMessage);
  LoRa.endPacket();

  counter++;
  delay(3000);
}