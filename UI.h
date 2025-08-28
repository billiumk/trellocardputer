#ifndef UI_H
#define UI_H

#include <M5Cardputer.h>
#include <M5GFX.h>
#include "config.h"
#include "DataStructures.h"

class UI {
private:
  // Display dimensions
  static const int SCREEN_WIDTH = 240;
  static const int SCREEN_HEIGHT = 135;
  static const int LINE_HEIGHT = 12;
  static const int MARGIN = 4;
  static const int MAX_LINES = 10;
  
  // Color mapping for labels
  uint16_t getLabelColor(const String& colorName);
  
  // Text handling
  void drawWrappedText(const String& text, int x, int y, int maxWidth, int maxLines = -1);
  String truncateText(const String& text, int maxChars);
  void drawProgressIndicator(int current, int total, int y);
  
  // UI Elements
  void drawHeader(const String& title, const String& subtitle = "");
  void drawFooter(const String& leftText = "", const String& rightText = "");
  void drawScrollIndicator(int currentPage, int totalPages);
  void drawStatusBar(bool online, bool cacheMode = false);
  
public:
  UI();
  
  // Screen rendering methods
  void renderSplashScreen();
  void renderListView(const std::vector<CardSummary>& cards, int selectedIndex, 
                     int currentPage, int totalPages, bool isOnline);
  void renderCardDetail(const FullCard& card, int scrollPosition = 0);
  void renderAddComment(const String& cardName, const String& inputBuffer, 
                       int cursorPosition);
  void renderCreateCard(const String& nameBuffer, const String& descBuffer, 
                       bool editingName, int cursorPosition);
  void renderError(const String& errorMessage, const String& suggestion = "");
  void renderLoadingScreen(const String& message);
  
  // Input handling helpers
  void showInputCursor(int x, int y, bool visible = true);
  void highlightSelection(int x, int y, int width, int height);
  
  // Utility methods
  void clearScreen();
  void showMessage(const String& message, int duration = 2000);
  void playTone(int frequency, int duration);
  void playSuccessSound();
  void playErrorSound();
  
  // Constants for UI layout
  static const int CARD_ITEM_HEIGHT = 14;
  static const int DETAIL_SCROLL_STEP = 10;
  static const int INPUT_CURSOR_WIDTH = 1;
  static const int LABEL_INDICATOR_SIZE = 3;
};

#endif // UI_H