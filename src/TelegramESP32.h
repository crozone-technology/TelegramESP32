#ifndef TELEGRAMESP32_H
#define TELEGRAMESP32_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <functional>

class TelegramESP32 {
  public:
    typedef std::function<void(String&)> MessageCallback;
    enum ChatType { PRIVATE, CHANNEL, GROUP };
    
    struct ChatConfig {
      String id;
      ChatType type;
      String name;
    };

    TelegramESP32(const char* botToken);
    bool begin();
    bool sendMessage(const String& message);
    bool sendMessageToChat(const String& chatName, const String& message);
    bool broadcast(const String& message);
    String receiveMessage();
    void setMessageCallback(MessageCallback callback);
    void setMessageInterval(unsigned long interval);
    void loop();
    void addChat(String id, ChatType type, String name = "");
    
  private:
    static const size_t BUFFER_SIZE = 256;
    static const int MAX_CHATS = 5;
    
    char msgBuffer[BUFFER_SIZE];
    char urlBuffer[128];
    char payloadBuffer[256];
    const char* token;
    WiFiClientSecure client;
    HTTPClient http;
    unsigned long lastUpdateId;
    unsigned long lastMessageTime;
    unsigned long MESSAGE_MIN_INTERVAL;
    MessageCallback messageCallback;
    ChatConfig chats[MAX_CHATS];
    int chatCount;
    
    bool getUpdates();
    bool parseMessage(String& payload, String& message);
    bool sendToChat(ChatConfig& chat, const String& message);
    bool sendRequest(const char* payload);
};

#endif
