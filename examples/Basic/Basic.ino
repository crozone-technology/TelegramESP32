/*
 sendDataWhenReceived.ino
 Example for TelegramESP32 library
 
 Demonstrates:
 - Connecting to Telegram bot
 - Sending device status
 - Handling commands (/temp)
 - Broadcasting messages
 
 Setup:
 1. Create Telegram bot via @BotFather
 2. Get chat ID
 3. Set WiFi credentials and bot token below
 4. Upload and monitor Serial

    Created 26 January 2025
    By YuTTYL
    Modified 26 January 2025
    By YuTTYL
    
 Library: https://github.com/crozone-technology/TelegramESP32
*/
#include <TelegramESP32.h>
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
