# LoRa Ra-02 & ESP32-U Testing Guide

This project contains a setup and testing guide for the **Ai-Thinker Ra-02 (SX1278)** LoRa module using an **ESP32-U**.

## Hardware Requirements
* 1x (or 2x for full testing) ESP32-U Development Board
* 1x (or 2x for full testing) Ra-02 LoRa Module
* Breadboard & Jumper Wires
* 6x 10 µF Capacitors (for power stabilization)
* USB Cable

## Wiring Guide

| Ra-02 Pin | Purpose | ESP32-U GPIO Pin |
| :--- | :--- | :--- |
| Pin 3 (3.3V) | Power (VCC) | 3V3 |
| Pin 1, 2, 9, or 16 (GND)| Ground | GND |
| Pin 15 (NSS) | Chip Select (CS) | GPIO 5 |
| Pin 14 (MOSI) | SPI Data Out | GPIO 23 |
| Pin 13 (MISO) | SPI Data In | GPIO 19 |
| Pin 12 (SCK) | SPI Clock | GPIO 18 |
| Pin 4 (RESET) | Reset | GPIO 14 |
| Pin 5 (DIO0) | Interrupt Request | GPIO 2 |

**Important Power Note:** The Ra-02 module can draw significant current, particularly during transmission. Place the six 10 µF capacitors in parallel across the 3.3V and GND rails on your breadboard to stabilize the power from the ESP32's 3.3V pin.

### Wiring Diagram

```mermaid
graph LR
    subgraph ESP32-U
        3V3[3V3]
        GND1[GND]
        D5[GPIO 5]
        D23[GPIO 23]
        D19[GPIO 19]
        D18[GPIO 18]
        D14[GPIO 14]
        D2[GPIO 2]
    end

    subgraph Breadboard Power
        VCC_Rail[3.3V Rail]
        GND_Rail[GND Rail]
        CAP[6x 10µF Capacitors]
        VCC_Rail ---|Parallel| CAP --- GND_Rail
    end

    subgraph Ra-02 LoRa Module
        VCC2[Pin 3: 3.3V]
        GND2[Pin: GND]
        NSS[Pin 15: NSS]
        MOSI[Pin 14: MOSI]
        MISO[Pin 13: MISO]
        SCK[Pin 12: SCK]
        RST[Pin 4: RESET]
        DIO0[Pin 5: DIO0]
    end

    3V3 ==>|Power| VCC_Rail
    GND1 ==>|Ground| GND_Rail

    VCC_Rail ==>|Power| VCC2
    GND_Rail ==>|Ground| GND2

    D5 -->|SPI CS| NSS
    D23 -->|SPI MOSI| MOSI
    D19 -->|SPI MISO| MISO
    D18 -->|SPI SCK| SCK
    D14 -->|Reset| RST
    D2 -->|Interrupt| DIO0
```

## Software Setup
This project is built using **PlatformIO** in VS Code.

Dependencies (automatically managed in `platformio.ini`):
* Framework: Arduino
* Library: `sandeepmistry/LoRa @ ^0.8.0`

## Testing Phases

### Phase 1: Single Module Hardware Test
Before testing over-the-air transmission, verify that the ESP32 can successfully communicate with the Ra-02 module over the SPI bus.

1. Wire up a single ESP32-U and Ra-02 according to the wiring guide.
2. Ensure the hardware test code is flashed to your ESP32-U.
3. Open the Serial Monitor at `115200` baud rate.
4. If the wiring and power are correct, you will see:
   ```
   LoRa Single Module Test
   LoRa Initialized OK!
   Success! SPI communication with LoRa module is working.
   ```
   *If it says "Starting LoRa failed! Check your wiring.", triple-check your SPI pins and ensure the module has stable power.*

### Phase 2: Sender & Receiver Test (Two Modules)
Once your single-module hardware test passes successfully on both sets of hardware, you can test communication between two modules.

Replace your `main.cpp` code with the following Sender/Receiver script:

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#define LORA_NSS_PIN 5
#define LORA_RESET_PIN 14
#define LORA_DIO0_PIN 2
#define LORA_FREQUENCY 915E6 // Change to your region's frequency (e.g. 868E6 or 433E6)

// Uncomment one of the following lines to select the board's role
#define LORA_SENDER 
// #define LORA_RECEIVER

void setup() {
  Serial.begin(115200);
  while (!Serial);

  LoRa.setPins(LORA_NSS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  #ifdef LORA_SENDER
  Serial.println("LoRa Sender Node");
  #else
  Serial.println("LoRa Receiver Node");
  LoRa.onReceive(onReceive);
  LoRa.receive();
  #endif
}

#ifdef LORA_SENDER
int counter = 0;
void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;
  delay(5000);
}
#endif

#ifdef LORA_RECEIVER
void onReceive(int packetSize) {
  if (packetSize == 0) return;

  String receivedText = "";
  while (LoRa.available()) {
    receivedText += (char)LoRa.read();
  }

  Serial.print("Received packet '");
  Serial.print(receivedText);
  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());
}

void loop() {
  // onReceive handles incoming packets
}
#endif
```

#### Steps to run Phase 2:
1. Copy the code above into your `src/main.cpp`.
2. Connect **Board A** to your PC. Leave `#define LORA_SENDER` uncommented. Build and Upload.
3. Connect **Board B** to your PC. Comment out `#define LORA_SENDER` and uncomment `#define LORA_RECEIVER`. Build and Upload.
4. Power up both boards. 
5. Open Serial Monitors for both COM ports at `115200` baud. You should see Board A printing "Sending packet..." and Board B printing "Received packet..." with the signal strength (RSSI).
