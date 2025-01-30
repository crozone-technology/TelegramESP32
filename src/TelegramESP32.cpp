#include "TelegramESP32.h"

TelegramESP32::TelegramESP32(const char* botToken) {
  token = botToken;
  lastUpdateId = 0;
  lastMessageTime = 0;
  MESSAGE_MIN_INTERVAL = 1000;
  messageCallback = nullptr;
  chatCount = 0;
}

bool TelegramESP32::begin() {
  if (WiFi.status() != WL_CONNECTED) return false;
  client.setInsecure();
  return true;
}

void TelegramESP32::addChat(String id, ChatType type, String name) {
  if (chatCount < MAX_CHATS) {
    ChatConfig chat = {id, type, name};
    chats[chatCount++] = chat;
  }
}

bool TelegramESP32::sendMessage(const String& message) {
  if (chatCount > 0) {
    return sendToChat(chats[0], message);
  }
  return false;
}

bool TelegramESP32::sendMessageToChat(const String& chatName, const String& message) {
  for (int i = 0; i < chatCount; i++) {
    if (chats[i].name == chatName) {
      return sendToChat(chats[i], message);
    }
  }
  return false;
}

bool TelegramESP32::broadcast(const String& message) {
  bool success = true;
  for (int i = 0; i < chatCount; i++) {
    if (!sendToChat(chats[i], message)) {
      success = false;
    }
  }
  return success;
}

bool TelegramESP32::sendToChat(ChatConfig& chat, const String& message) {
  if (WiFi.status() != WL_CONNECTED) return false;
  
  if (millis() - lastMessageTime < MESSAGE_MIN_INTERVAL) return false;
  lastMessageTime = millis();

  snprintf(urlBuffer, sizeof(urlBuffer), 
    "https://api.telegram.org/bot%s/sendMessage", token);
  snprintf(payloadBuffer, sizeof(payloadBuffer),
    "chat_id=%s&text=%s", chat.id.c_str(), message.c_str());
  
  return sendRequest(payloadBuffer);
}

bool TelegramESP32::sendRequest(const char* payload) {
  http.begin(client, urlBuffer);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(payload);
  bool success = (httpCode > 0);
  http.end();
  return success;
}

String TelegramESP32::receiveMessage() {
  if (!getUpdates()) return "";
  
  String payload = http.getString();
  String message;
  
  if (parseMessage(payload, message)) {
    return message;
  }
  
  return "";
}

bool TelegramESP32::parseMessage(String& payload, String& message) {
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payload);
  
  if (error || !doc["ok"].as<bool>()) return false;
  
  JsonArray result = doc["result"].as<JsonArray>();
  if (result.size() == 0) return false;

  JsonVariant lastMsg = result[result.size()-1];
  lastUpdateId = lastMsg["update_id"].as<unsigned long>();
  message = lastMsg["message"]["text"] | lastMsg["channel_post"]["text"] | "";
  
  return message != "";
}

bool TelegramESP32::getUpdates() {
  if (WiFi.status() != WL_CONNECTED) return false;
  
  snprintf(urlBuffer, sizeof(urlBuffer), 
    "https://api.telegram.org/bot%s/getUpdates?offset=%lu", 
    token, lastUpdateId + 1);
    
  http.begin(client, urlBuffer);
  int httpCode = http.GET();
  return (httpCode > 0);
}

void TelegramESP32::setMessageCallback(MessageCallback callback) {
  messageCallback = callback;
}

void TelegramESP32::setMessageInterval(unsigned long interval) {
  MESSAGE_MIN_INTERVAL = interval;
}

void TelegramESP32::loop() {
  String message = receiveMessage();
  if (message != "" && messageCallback) {
    messageCallback(message);
  }
}
