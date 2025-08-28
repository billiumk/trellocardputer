#ifndef TRELLO_CLIENT_H
#define TRELLO_CLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SD.h>
#include "config.h"
#include "DataStructures.h"

class TrelloClient {
private:
  WiFiClientSecure* secureClient;
  HTTPClient* httpClient;
  unsigned long lastApiCall;
  bool isInitialized;
  
  // Helper methods
  String buildUrl(const String& endpoint, const String& params = "");
  ApiStatus makeRequest(const String& url, const String& method, const String& payload = "", JsonDocument& response = *(new JsonDocument));
  ApiStatus parseCardList(const JsonDocument& doc, std::vector<CardSummary>& cards);
  ApiStatus parseCardDetails(const JsonDocument& doc, FullCard& card);
  String getColorFromLabel(const String& color);
  void enforceRateLimit();
  bool saveToCache(const String& filename, const JsonDocument& doc);
  bool loadFromCache(const String& filename, JsonDocument& doc);
  
public:
  TrelloClient();
  ~TrelloClient();
  
  // Initialization
  bool begin();
  bool connectWiFi();
  void disconnect();
  bool isConnected();
  
  // API Methods
  ApiStatus fetchCardList(std::vector<CardSummary>& cards, bool useCache = false);
  ApiStatus fetchCardDetails(const String& cardId, FullCard& card, bool useCache = false);
  ApiStatus addComment(const String& cardId, const String& comment);
  ApiStatus markChecklistItemDone(const String& cardId, const String& checklistId, const String& itemId);
  ApiStatus createCard(const String& name, const String& description = "");
  ApiStatus refreshCard(const String& cardId, FullCard& card);
  
  // Cache Management
  bool clearCache();
  bool isCacheValid(const String& filename, unsigned long maxAge = 300000); // 5 minutes default
  
  // Utility
  String getLastError();
  bool testConnection();
};

#endif // TRELLO_CLIENT_H