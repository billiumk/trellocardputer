#include "TrelloClient.h"

// Trello's root CA certificate (DigiCert Global Root CA)
const char* trello_root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
"-----END CERTIFICATE-----";

TrelloClient::TrelloClient() : secureClient(nullptr), httpClient(nullptr), 
                               lastApiCall(0), isInitialized(false) {
}

TrelloClient::~TrelloClient() {
  if (httpClient) {
    delete httpClient;
  }
  if (secureClient) {
    delete secureClient;
  }
}

bool TrelloClient::begin() {
  if (isInitialized) {
    return true;
  }
  
  // Initialize secure client
  secureClient = new WiFiClientSecure();
  if (!secureClient) {
    Serial.println("Failed to create secure client");
    return false;
  }
  
  // Set root CA certificate
  secureClient->setCACert(trello_root_ca);
  
  // Initialize HTTP client
  httpClient = new HTTPClient();
  if (!httpClient) {
    Serial.println("Failed to create HTTP client");
    return false;
  }
  
  // Initialize SD card for caching
  if (!SD.begin()) {
    Serial.println("Warning: SD card initialization failed - caching disabled");
  }
  
  isInitialized = true;
  return true;
}

bool TrelloClient::connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println(" Failed!");
    return false;
  }
}

void TrelloClient::disconnect() {
  WiFi.disconnect();
}

bool TrelloClient::isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String TrelloClient::buildUrl(const String& endpoint, const String& params) {
  String url = TRELLO_BASE_URL + endpoint;
  url += "?key=" + String(TRELLO_API_KEY);
  url += "&token=" + String(TRELLO_API_TOKEN);
  if (params.length() > 0) {
    url += "&" + params;
  }
  return url;
}

ApiStatus TrelloClient::makeRequest(const String& url, const String& method, 
                                   const String& payload, JsonDocument& response) {
  if (!isConnected()) {
    if (!connectWiFi()) {
      return API_ERROR_NETWORK;
    }
  }
  
  enforceRateLimit();
  
  httpClient->begin(*secureClient, url);
  httpClient->addHeader("Content-Type", "application/json");
  httpClient->setConnectTimeout(10000);
  httpClient->setTimeout(10000);
  
  int httpCode = -1;
  
  if (method == "GET") {
    httpCode = httpClient->GET();
  } else if (method == "POST") {
    httpCode = httpClient->POST(payload);
  } else if (method == "PUT") {
    httpCode = httpClient->PUT(payload);
  }
  
  lastApiCall = millis();
  
  if (httpCode > 0) {
    String responseStr = httpClient->getString();
    
    if (httpCode == 200 || httpCode == 201) {
      DeserializationError error = deserializeJson(response, responseStr);
      if (error) {
        Serial.println("JSON parse error: " + String(error.c_str()));
        httpClient->end();
        return API_ERROR_PARSE;
      }
      httpClient->end();
      return API_SUCCESS;
    } else if (httpCode == 401) {
      Serial.println("API authentication error");
      httpClient->end();
      return API_ERROR_AUTH;
    } else if (httpCode == 429) {
      Serial.println("API rate limit exceeded");
      httpClient->end();
      return API_ERROR_RATE_LIMIT;
    } else if (httpCode == 404) {
      Serial.println("API resource not found");
      httpClient->end();
      return API_ERROR_NOT_FOUND;
    }
  }
  
  Serial.println("HTTP error: " + String(httpCode));
  httpClient->end();
  return API_ERROR_NETWORK;
}

void TrelloClient::enforceRateLimit() {
  unsigned long timeSinceLastCall = millis() - lastApiCall;
  if (timeSinceLastCall < API_RATE_LIMIT_DELAY_MS) {
    delay(API_RATE_LIMIT_DELAY_MS - timeSinceLastCall);
  }
}

ApiStatus TrelloClient::fetchCardList(std::vector<CardSummary>& cards, bool useCache) {
  cards.clear();
  
  // Try cache first if requested or if offline
  if (useCache || !isConnected()) {
    JsonDocument doc;
    if (loadFromCache(CACHE_LIST_FILE, doc)) {
      return parseCardList(doc, cards);
    }
  }
  
  // Fetch from API
  String url = buildUrl("/lists/" + String(TRELLO_LIST_ID) + "/cards", 
                       "fields=name,id,labels,due,badges");
  
  JsonDocument doc;
  ApiStatus status = makeRequest(url, "GET", "", doc);
  
  if (status == API_SUCCESS) {
    saveToCache(CACHE_LIST_FILE, doc);
    return parseCardList(doc, cards);
  }
  
  return status;
}

ApiStatus TrelloClient::parseCardList(const JsonDocument& doc, std::vector<CardSummary>& cards) {
  if (doc.is<JsonArray>()) {
    JsonArray cardsArray = doc.as<JsonArray>();
    for (JsonObject card : cardsArray) {
      CardSummary summary;
      summary.id = card["id"].as<String>();
      summary.name = card["name"].as<String>();
      
      // Parse labels
      JsonArray labels = card["labels"];
      for (JsonObject label : labels) {
        String color = label["color"].as<String>();
        if (color.length() > 0) {
          summary.labelColors.push_back(color);
        }
      }
      
      // Check due date
      if (card.containsKey("due") && !card["due"].isNull()) {
        summary.hasDueDate = true;
      }
      
      // Check if done (based on badges or checklists)
      if (card.containsKey("badges")) {
        JsonObject badges = card["badges"];
        if (badges.containsKey("checkItems")) {
          int checkItems = badges["checkItems"];
          int checkItemsChecked = badges["checkItemsChecked"];
          summary.isDone = (checkItems > 0) && (checkItems == checkItemsChecked);
        }
      }
      
      cards.push_back(summary);
    }
    return API_SUCCESS;
  }
  return API_ERROR_PARSE;
}

ApiStatus TrelloClient::fetchCardDetails(const String& cardId, FullCard& card, bool useCache) {
  // Try cache first if requested or if offline
  String cacheFile = CACHE_DETAILS_PREFIX + cardId + ".json";
  if (useCache || !isConnected()) {
    JsonDocument doc;
    if (loadFromCache(cacheFile, doc)) {
      return parseCardDetails(doc, card);
    }
  }
  
  // Fetch from API
  String url = buildUrl("/cards/" + cardId, 
                       "fields=name,desc,due,labels,badges&actions=commentCard&actions_limit=50&checklists=all");
  
  JsonDocument doc;
  ApiStatus status = makeRequest(url, "GET", "", doc);
  
  if (status == API_SUCCESS) {
    saveToCache(cacheFile, doc);
    return parseCardDetails(doc, card);
  }
  
  return status;
}

ApiStatus TrelloClient::parseCardDetails(const JsonDocument& doc, FullCard& card) {
  if (!doc.is<JsonObject>()) {
    return API_ERROR_PARSE;
  }
  
  JsonObject cardObj = doc.as<JsonObject>();
  
  // Parse basic info
  card.summary.id = cardObj["id"].as<String>();
  card.summary.name = cardObj["name"].as<String>();
  card.description = cardObj["desc"].as<String>();
  
  if (cardObj.containsKey("due") && !cardObj["due"].isNull()) {
    card.dueDate = cardObj["due"].as<String>();
    card.summary.hasDueDate = true;
  }
  
  // Parse labels
  card.summary.labelColors.clear();
  JsonArray labels = cardObj["labels"];
  for (JsonObject label : labels) {
    String color = label["color"].as<String>();
    if (color.length() > 0) {
      card.summary.labelColors.push_back(color);
    }
  }
  
  // Parse comments
  card.comments.clear();
  JsonArray actions = cardObj["actions"];
  for (JsonObject action : actions) {
    if (action["type"].as<String>() == "commentCard") {
      JsonObject data = action["data"];
      String text = data["text"].as<String>();
      String memberName = action["memberCreator"]["fullName"].as<String>();
      card.comments.push_back(memberName + ": " + text);
    }
  }
  
  // Parse checklists
  card.checklists.clear();
  JsonArray checklists = cardObj["checklists"];
  for (JsonObject checklist : checklists) {
    JsonArray checkItems = checklist["checkItems"];
    for (JsonObject item : checkItems) {
      ChecklistItem checkItem;
      checkItem.id = item["id"].as<String>();
      checkItem.name = item["name"].as<String>();
      checkItem.isComplete = item["state"].as<String>() == "complete";
      card.checklists.push_back(checkItem);
    }
  }
  
  return API_SUCCESS;
}

ApiStatus TrelloClient::addComment(const String& cardId, const String& comment) {
  String url = buildUrl("/cards/" + cardId + "/actions/comments");
  
  JsonDocument payload;
  payload["text"] = comment;
  
  String payloadStr;
  serializeJson(payload, payloadStr);
  
  JsonDocument response;
  return makeRequest(url, "POST", payloadStr, response);
}

ApiStatus TrelloClient::markChecklistItemDone(const String& cardId, const String& checklistId, const String& itemId) {
  String url = buildUrl("/cards/" + cardId + "/checklist/" + checklistId + "/idChecklist/" + itemId);
  
  JsonDocument payload;
  payload["value"]["state"] = "complete";
  
  String payloadStr;
  serializeJson(payload, payloadStr);
  
  JsonDocument response;
  return makeRequest(url, "PUT", payloadStr, response);
}

ApiStatus TrelloClient::createCard(const String& name, const String& description) {
  String url = buildUrl("/cards");
  
  JsonDocument payload;
  payload["name"] = name;
  payload["desc"] = description;
  payload["idList"] = TRELLO_LIST_ID;
  
  String payloadStr;
  serializeJson(payload, payloadStr);
  
  JsonDocument response;
  return makeRequest(url, "POST", payloadStr, response);
}

bool TrelloClient::saveToCache(const String& filename, const JsonDocument& doc) {
  if (!SD.begin()) {
    return false;
  }
  
  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    return false;
  }
  
  size_t bytesWritten = serializeJson(doc, file);
  file.close();
  
  return bytesWritten > 0;
}

bool TrelloClient::loadFromCache(const String& filename, JsonDocument& doc) {
  if (!SD.begin()) {
    return false;
  }
  
  File file = SD.open(filename, FILE_READ);
  if (!file) {
    return false;
  }
  
  DeserializationError error = deserializeJson(doc, file);
  file.close();
  
  return error == DeserializationError::Ok;
}

bool TrelloClient::testConnection() {
  String url = buildUrl("/members/me", "fields=username");
  JsonDocument response;
  return makeRequest(url, "GET", "", response) == API_SUCCESS;
}

String TrelloClient::getLastError() {
  // This would store the last error message
  return "Check serial output for details";
}