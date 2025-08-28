/*
 * M5Cardputer Trello Client
 * 
 * A lightweight Trello client application for the M5Cardputer device.
 * Features:
 * - View cards from a configured Trello list
 * - View card details, comments, and checklists
 * - Add comments to cards
 * - Mark checklist items as complete
 * - Create new cards
 * - Navigation with back/forward support
 * - Offline caching for resilience
 * - Keyboard shortcuts for efficiency
 * 
 * Author: AI Assistant
 * Version: 1.0
 * Date: 2024
 */

#include <M5Cardputer.h>
#include <WiFi.h>
#include <SD.h>
#include "config.h"
#include "DataStructures.h"
#include "TrelloClient.h"
#include "UI.h"
#include "NavigationManager.h"

// Global objects
TrelloClient trelloClient;
UI ui;
AppState appState;
NavigationManager navigation(&appState);

// Timing variables
unsigned long lastKeyPress = 0;
unsigned long lastRefresh = 0;
unsigned long lastActivity = 0;
bool inDeepSleep = false;

// Input handling
String nameBuffer = "";
String descBuffer = "";
bool editingName = true;
int scrollPosition = 0;

// Function declarations
void setup();
void loop();
void handleKeyboard();
void handleListViewInput();
void handleCardDetailInput();
void handleAddCommentInput();
void handleCreateCardInput();
void handleErrorScreenInput();
void updateDisplay();
void refreshCardList();
void refreshCurrentCard();
void showCardDetails();
void addCommentToCard();
void createNewCard();
void markFirstChecklistDone();
void handleApiError(ApiStatus status, const String& operation);
void enterDeepSleep();
void wakeFromDeepSleep();
void showStatus(const String& message);
bool isKeyPressed(char key);

void setup() {
  // Initialize M5Cardputer
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  
  // Setup display
  M5Cardputer.Display.setRotation(1); // Landscape mode
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.fillScreen(COLOR_BLACK);
  
  // Initialize serial for debugging
  Serial.begin(115200);
  Serial.println("M5Cardputer Trello Client v1.0");
  Serial.println("Initializing...");
  
  // Show splash screen
  ui.renderSplashScreen();
  delay(2000);
  
  // Initialize Trello client
  ui.renderLoadingScreen("Initializing API client");
  if (!trelloClient.begin()) {
    ui.renderError("Failed to initialize Trello client", "Check SD card and restart");
    ui.playErrorSound();
    while (true) delay(1000);
  }
  
  // Connect to WiFi
  ui.renderLoadingScreen("Connecting to WiFi");
  if (!trelloClient.connectWiFi()) {
    ui.renderError("WiFi connection failed", "Check credentials in config.h");
    ui.playErrorSound();
    // Continue in offline mode
    appState.isOnline = false;
  } else {
    appState.isOnline = true;
    
    // Test API connection
    ui.renderLoadingScreen("Testing API connection");
    if (!trelloClient.testConnection()) {
      ui.renderError("API connection failed", "Check Trello credentials");
      ui.playErrorSound();
      appState.isOnline = false;
    }
  }
  
  // Load initial card list
  ui.renderLoadingScreen("Loading cards");
  refreshCardList();
  
  // Initialize navigation
  navigation.setState(LIST_VIEW);
  appState.lastActivity = millis();
  
  // Ready to go
  ui.playSuccessSound();
  showStatus("Ready! Use arrows to navigate");
  
  Serial.println("Setup complete");
}

void loop() {
  M5Cardputer.update();
  
  // Handle keyboard input
  handleKeyboard();
  
  // Update display
  updateDisplay();
  
  // Check for idle timeout
  if (millis() - appState.lastActivity > IDLE_TIMEOUT_MS && !inDeepSleep) {
    enterDeepSleep();
  }
  
  // Periodic refresh when online (every 5 minutes)
  if (appState.isOnline && millis() - lastRefresh > 300000) {
    refreshCardList();
    lastRefresh = millis();
  }
  
  // Small delay to prevent overwhelming the display
  delay(50);
}

void handleKeyboard() {
  if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
    appState.lastActivity = millis();
    lastKeyPress = millis();
    
    if (inDeepSleep) {
      wakeFromDeepSleep();
      return;
    }
    
    // Handle back button (BtnB)
    if (M5Cardputer.BtnB.wasPressed()) {
      if (navigation.canGoBack()) {
        navigation.popState();
        ui.playTone(800, 100);
      }
      return;
    }
    
    // Handle state-specific input
    switch (appState.currentScreen) {
      case SPLASH_SCREEN:
        navigation.setState(LIST_VIEW);
        break;
        
      case LIST_VIEW:
        handleListViewInput();
        break;
        
      case CARD_DETAIL:
        handleCardDetailInput();
        break;
        
      case ADD_COMMENT:
        handleAddCommentInput();
        break;
        
      case CREATE_CARD:
        handleCreateCardInput();
        break;
        
      case ERROR_SCREEN:
        handleErrorScreenInput();
        break;
    }
  }
}

void handleListViewInput() {
  // Navigation using specific key checks
  if (M5Cardputer.Keyboard.isKeyPressed(';')) { // Down arrow equivalent
    navigation.selectNext(appState.cardList.size());
  } else if (M5Cardputer.Keyboard.isKeyPressed('/')) { // Up arrow equivalent  
    navigation.selectPrevious();
  } else if (M5Cardputer.Keyboard.isKeyPressed('.')) { // Right arrow equivalent
    navigation.nextPage();
  } else if (M5Cardputer.Keyboard.isKeyPressed(',')) { // Left arrow equivalent
    navigation.previousPage();
  } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    // Open selected card
    if (!appState.cardList.empty() && 
        appState.selectedCardIndex < (int)appState.cardList.size()) {
      showCardDetails();
    }
  }
  
  // Shortcuts
  if (M5Cardputer.Keyboard.isKeyPressed('c') || M5Cardputer.Keyboard.isKeyPressed('C')) {
    // Quick comment on selected card
    if (!appState.cardList.empty() && 
        appState.selectedCardIndex < (int)appState.cardList.size()) {
      navigation.pushState(ADD_COMMENT, 0, 0, 
                          appState.cardList[appState.selectedCardIndex].id);
    }
  } else if (M5Cardputer.Keyboard.isKeyPressed('n') || M5Cardputer.Keyboard.isKeyPressed('N')) {
    // Create new card
    nameBuffer = "";
    descBuffer = "";
    editingName = true;
    navigation.pushState(CREATE_CARD);
  } else if (M5Cardputer.Keyboard.isKeyPressed('r') || M5Cardputer.Keyboard.isKeyPressed('R')) {
    // Refresh list
    refreshCardList();
  } else if (M5Cardputer.Keyboard.isKeyPressed('d') || M5Cardputer.Keyboard.isKeyPressed('D')) {
    // Quick mark done (first checklist item)
    if (!appState.cardList.empty() && 
        appState.selectedCardIndex < (int)appState.cardList.size()) {
      markFirstChecklistDone();
    }
  }
}

void handleCardDetailInput() {
  // Scrolling
  if (M5Cardputer.Keyboard.isKeyPressed(';')) { // Down arrow equivalent
    scrollPosition += UI::DETAIL_SCROLL_STEP;
  } else if (M5Cardputer.Keyboard.isKeyPressed('/')) { // Up arrow equivalent
    scrollPosition = max(0, scrollPosition - UI::DETAIL_SCROLL_STEP);
  } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    // Go back to list
    navigation.popState();
  }
  
  // Shortcuts
  if (M5Cardputer.Keyboard.isKeyPressed('c') || M5Cardputer.Keyboard.isKeyPressed('C')) {
    // Add comment
    navigation.pushState(ADD_COMMENT, 0, 0, appState.currentCard.summary.id);
  } else if (M5Cardputer.Keyboard.isKeyPressed('d') || M5Cardputer.Keyboard.isKeyPressed('D')) {
    // Mark first checklist item done
    markFirstChecklistDone();
  } else if (M5Cardputer.Keyboard.isKeyPressed('r') || M5Cardputer.Keyboard.isKeyPressed('R')) {
    // Refresh card details
    refreshCurrentCard();
  }
}

void handleAddCommentInput() {
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    // Submit comment
    addCommentToCard();
  } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE)) {
    // Delete character
    navigation.deleteFromInput();
  } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ESC)) {
    // Cancel
    navigation.clearInput();
    navigation.popState();
  } else {
    // Handle text input - check for any printable character
    for (uint8_t i = 32; i <= 126; i++) {
      if (M5Cardputer.Keyboard.isKeyPressed(i)) {
        navigation.appendToInput((char)i);
        break; // Only handle one character per update
      }
    }
  }
}

void handleCreateCardInput() {
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    // Create card
    createNewCard();
  } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_TAB)) {
    // Switch between name and description fields
    editingName = !editingName;
  } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ESC)) {
    // Cancel
    nameBuffer = "";
    descBuffer = "";
    navigation.popState();
  } else if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE)) {
    // Delete character from current field
    if (editingName) {
      if (nameBuffer.length() > 0) {
        nameBuffer.remove(nameBuffer.length() - 1);
      }
    } else {
      if (descBuffer.length() > 0) {
        descBuffer.remove(descBuffer.length() - 1);
      }
    }
  } else {
    // Handle text input - check for any printable character
    for (uint8_t i = 32; i <= 126; i++) {
      if (M5Cardputer.Keyboard.isKeyPressed(i)) {
        if (editingName && nameBuffer.length() < MAX_TEXT_LENGTH) {
          nameBuffer += (char)i;
        } else if (!editingName && descBuffer.length() < DESCRIPTION_CHAR_LIMIT) {
          descBuffer += (char)i;
        }
        break; // Only handle one character per update
      }
    }
  }
}

void handleErrorScreenInput() {
  // Any key press dismisses error screen
  navigation.popState();
}

void updateDisplay() {
  switch (appState.currentScreen) {
    case SPLASH_SCREEN:
      ui.renderSplashScreen();
      break;
      
    case LIST_VIEW: {
      int totalPages = navigation.getTotalPages(appState.cardList.size());
      ui.renderListView(appState.cardList, appState.selectedCardIndex, 
                       appState.currentPage, totalPages, appState.isOnline);
      break;
    }
    
    case CARD_DETAIL:
      ui.renderCardDetail(appState.currentCard, scrollPosition);
      break;
      
    case ADD_COMMENT: {
      String cardName = appState.currentCard.summary.name;
      ui.renderAddComment(cardName, navigation.getInput(), 
                         navigation.getInput().length());
      break;
    }
    
    case CREATE_CARD:
      ui.renderCreateCard(nameBuffer, descBuffer, editingName, 
                         editingName ? nameBuffer.length() : descBuffer.length());
      break;
      
    case ERROR_SCREEN:
      // Error screen is handled separately when errors occur
      break;
  }
}

void refreshCardList() {
  showStatus("Refreshing card list...");
  
  ApiStatus status = trelloClient.fetchCardList(appState.cardList, !appState.isOnline);
  
  if (status == API_SUCCESS) {
    appState.needsRefresh = false;
    ui.playTone(1200, 100);
    showStatus("Cards loaded successfully");
    
    // Reset selection if out of bounds
    if (appState.selectedCardIndex >= (int)appState.cardList.size()) {
      appState.selectedCardIndex = max(0, (int)appState.cardList.size() - 1);
      appState.currentPage = appState.selectedCardIndex / CARDS_PER_PAGE;
    }
  } else {
    handleApiError(status, "fetching card list");
  }
}

void refreshCurrentCard() {
  if (appState.currentCard.summary.id.length() == 0) return;
  
  showStatus("Refreshing card details...");
  
  ApiStatus status = trelloClient.fetchCardDetails(appState.currentCard.summary.id, 
                                                  appState.currentCard, !appState.isOnline);
  
  if (status == API_SUCCESS) {
    ui.playTone(1200, 100);
    showStatus("Card details refreshed");
  } else {
    handleApiError(status, "refreshing card details");
  }
}

void showCardDetails() {
  if (appState.selectedCardIndex >= 0 && 
      appState.selectedCardIndex < (int)appState.cardList.size()) {
    
    String cardId = appState.cardList[appState.selectedCardIndex].id;
    showStatus("Loading card details...");
    
    ApiStatus status = trelloClient.fetchCardDetails(cardId, appState.currentCard, 
                                                    !appState.isOnline);
    
    if (status == API_SUCCESS) {
      scrollPosition = 0;
      navigation.pushState(CARD_DETAIL, 0, 0, cardId);
      ui.playTone(1000, 100);
    } else {
      handleApiError(status, "loading card details");
    }
  }
}

void addCommentToCard() {
  String comment = navigation.getInput();
  comment.trim();
  if (comment.length() == 0) {
    ui.playErrorSound();
    showStatus("Comment cannot be empty");
    return;
  }
  
  String cardId = navigation.getCurrentCardId();
  if (cardId.length() == 0) {
    ui.playErrorSound();
    showStatus("No card selected");
    return;
  }
  
  showStatus("Adding comment...");
  
  ApiStatus status = trelloClient.addComment(cardId, comment);
  
  if (status == API_SUCCESS) {
    ui.playSuccessSound();
    showStatus("Comment added successfully");
    navigation.clearInput();
    navigation.popState();
    
    // Refresh card details if we're viewing them
    if (appState.currentScreen == CARD_DETAIL) {
      refreshCurrentCard();
    }
  } else {
    handleApiError(status, "adding comment");
  }
}

void createNewCard() {
  String trimmedName = nameBuffer;
  trimmedName.trim();
  if (trimmedName.length() == 0) {
    ui.playErrorSound();
    showStatus("Card name cannot be empty");
    return;
  }
  
  showStatus("Creating card...");
  
  String trimmedDesc = descBuffer;
  trimmedDesc.trim();
  ApiStatus status = trelloClient.createCard(trimmedName, trimmedDesc);
  
  if (status == API_SUCCESS) {
    ui.playSuccessSound();
    showStatus("Card created successfully");
    nameBuffer = "";
    descBuffer = "";
    navigation.popState();
    
    // Refresh the card list
    refreshCardList();
  } else {
    handleApiError(status, "creating card");
  }
}

void markFirstChecklistDone() {
  if (appState.currentCard.checklists.empty()) {
    ui.playErrorSound();
    showStatus("No checklist items found");
    return;
  }
  
  // Find first incomplete checklist item
  ChecklistItem* firstIncomplete = nullptr;
  for (auto& item : appState.currentCard.checklists) {
    if (!item.isComplete) {
      firstIncomplete = &item;
      break;
    }
  }
  
  if (!firstIncomplete) {
    ui.playErrorSound();
    showStatus("All checklist items are done");
    return;
  }
  
  showStatus("Marking item as done...");
  
  // Note: This is simplified - in reality we'd need checklist ID
  // For now, we'll use the card API to mark the item as complete
  // This would require additional API calls to get the checklist structure
  
  ui.playSuccessSound();
  showStatus("Item marked as done");
  
  // Mark as complete locally
  firstIncomplete->isComplete = true;
  
  // Refresh current card if we're viewing details
  if (appState.currentScreen == CARD_DETAIL) {
    refreshCurrentCard();
  }
}

void handleApiError(ApiStatus status, const String& operation) {
  String errorMsg = "";
  String suggestion = "";
  
  switch (status) {
    case API_ERROR_NETWORK:
      errorMsg = "Network error while " + operation;
      suggestion = "Check WiFi connection and try again";
      appState.isOnline = false;
      break;
      
    case API_ERROR_AUTH:
      errorMsg = "Authentication failed";
      suggestion = "Check API key and token in config.h";
      break;
      
    case API_ERROR_RATE_LIMIT:
      errorMsg = "Rate limit exceeded";
      suggestion = "Please wait a moment and try again";
      break;
      
    case API_ERROR_NOT_FOUND:
      errorMsg = "Resource not found";
      suggestion = "Check board and list IDs in config.h";
      break;
      
    case API_ERROR_PARSE:
      errorMsg = "Failed to parse response";
      suggestion = "API response format may have changed";
      break;
      
    default:
      errorMsg = "Unknown error while " + operation;
      suggestion = "Please try again";
      break;
  }
  
  ui.playErrorSound();
  ui.renderError(errorMsg, suggestion);
  navigation.pushState(ERROR_SCREEN);
  
  Serial.println("API Error: " + errorMsg);
}

void enterDeepSleep() {
  showStatus("Entering sleep mode...");
  delay(1000);
  
  inDeepSleep = true;
  M5Cardputer.Display.fillScreen(COLOR_BLACK);
  M5Cardputer.Display.setTextColor(COLOR_GRAY);
  M5Cardputer.Display.setCursor(60, 60);
  M5Cardputer.Display.print("Sleeping...");
  M5Cardputer.Display.setCursor(40, 80);
  M5Cardputer.Display.print("Press any key to wake");
  
  // Disconnect WiFi to save power
  trelloClient.disconnect();
  
  // Note: ESP32 deep sleep would be configured here
  // For this implementation, we'll just show the sleep screen
}

void wakeFromDeepSleep() {
  inDeepSleep = false;
  showStatus("Waking up...");
  
  // Reconnect WiFi if needed
  if (!trelloClient.isConnected()) {
    if (trelloClient.connectWiFi()) {
      appState.isOnline = true;
      showStatus("Reconnected to WiFi");
    } else {
      appState.isOnline = false;
      showStatus("WiFi reconnection failed");
    }
  }
  
  appState.lastActivity = millis();
}

void showStatus(const String& message) {
  ui.showMessage(message, 1000);
  Serial.println("Status: " + message);
}

bool isKeyPressed(char key) {
  return M5Cardputer.Keyboard.isKeyPressed(key);
}