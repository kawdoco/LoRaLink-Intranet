#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#define LORA_NSS_PIN 5
#define LORA_RESET_PIN 14
#define LORA_DIO0_PIN 2
#define LORA_FREQUENCY 433E6 // Change to your region's frequency (e.g. 868E6 or 433E6)

// Peripherals pins
#define BUZZER_PIN 12
#define RGB_RED_PIN 25
#define RGB_GREEN_PIN 26
#define RGB_BLUE_PIN 27

#define LORA_RECEIVER

void setRGBColor(int redValue, int greenValue, int blueValue) {
  // Using digitalWrite instead of analogWrite to avoid ESP32 LEDC PWM bugs
  digitalWrite(RGB_RED_PIN, redValue > 0 ? HIGH : LOW);
  digitalWrite(RGB_GREEN_PIN, greenValue > 0 ? HIGH : LOW);
  digitalWrite(RGB_BLUE_PIN, blueValue > 0 ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize Peripherals
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  
  // Starting: RED double blink
  for(int i = 0; i < 2; i++) {
    setRGBColor(255, 0, 0);
    delay(400);
    setRGBColor(0, 0, 0);
    delay(400);
  }
  
  LoRa.setPins(LORA_NSS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    setRGBColor(255, 0, 0); // RED: Error status
    while (1);
  }

  // Force matching RF settings
  LoRa.setFrequency(433000000);
  LoRa.setSpreadingFactor(9);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setTxPower(17, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setSyncWord(0x12); // Default sync word

  // Print startup messages sequentially
  Serial.println("Greetings from LoRaLink Intranet");
  delay(1000);
  Serial.println("LoRa Receiver Node");

  // Set RGB LED to steady Green indicating System Ready to Receive
  setRGBColor(0, 255, 0); 
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String receivedText = "";
    while (LoRa.available()) {
      receivedText += (char)LoRa.read();
    }
    
    // Log to serial explicitly formatting as strings to avoid gibberish
    Serial.print("Rx: ");
    for(int i = 0; i < receivedText.length(); i++) {
        Serial.print(receivedText[i]);
    }
    Serial.println();
    
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());

    // Receiving: YELLOW double blink (SLOW) and sound buzzer
    for(int i = 0; i < 2; i++) {
      setRGBColor(255, 255, 0); // Yellow (Red + Green ON)
      
      if(i == 0) {
        // First yellow blink: Buzzer for 200ms, then delay 300ms
        for (int j = 0; j < 200; j++) {
          digitalWrite(BUZZER_PIN, HIGH);
          delayMicroseconds(500);
          digitalWrite(BUZZER_PIN, LOW);
          delayMicroseconds(500);
        }
        delay(300);
      } else {
        // Second yellow blink: Just wait 500ms
        delay(500);
      }
      
      setRGBColor(0, 0, 0); // Turn LED off between blinks limit
      delay(500);
    }
    
    setRGBColor(0, 255, 0); // Return to Green (Ready to receive)
  }
}