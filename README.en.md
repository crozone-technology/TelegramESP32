# TelegramESP32 Library

ESP32 library for sending and receiving Telegram messages.

## Installation

1. Copy `TelegramESP32.h` and `TelegramESP32.cpp` to your project
2. Install required libraries:
   - ArduinoJson
   - WiFiClientSecure
   - HTTPClient

## Basic Usage

```cpp
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "TelegramESP32.h"

const char* deviceName = "WeatherStation#1";
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* BOT_TOKEN = "123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11";
const char* CHAT_ID = "-100xxxxxxxxxxxx";

TelegramESP32 telegram(BOT_TOKEN);

void handleMessage(String& message) {
  if (message == "/temp") {
    float temp = 25.5;
    telegram.sendMessage(String(deviceName) +"\nTemperature: " + String(temp) + "°C");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  telegram.begin();
  telegram.setMessageCallback(handleMessage);
  telegram.addChat(CHAT_ID, TelegramESP32::CHANNEL, "Notification"); 
  // telegram.addChat("123456", TelegramESP32::PRIVATE, "Admin");
  // telegram.addChat("-200xxx", TelegramESP32::GROUP, "Team");

  char wifiIP[50];
  snprintf(wifiIP, sizeof(wifiIP), "Device Name: %s\nIP: %s", deviceName, WiFi.localIP().toString().c_str());
  telegram.sendMessageToChat("Notification",wifiIP);
}

void loop() {
  telegram.loop();
}
```

## Methods

- `addChat(id, name)`: Add chat for sending messages
- `sendMessage(message)`: Send to first added chat  
- `sendMessageToChat(name, message)`: Send to specific chat
- `broadcast(message)`: Send to all chats
- `setMessageCallback(callback)`: Handle incoming messages
- `setMessageInterval(ms)`: Set minimum interval between messages

## Configuration

- `MAX_CHATS`: 5 chats maximum
- `BUFFER_SIZE`: 256 bytes message buffer
- Default message interval: 1000ms