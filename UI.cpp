#include "UI.h"

UI::UI() {
}

uint16_t UI::getLabelColor(const String& colorName) {
  if (colorName == "red") return COLOR_RED;
  if (colorName == "green") return COLOR_GREEN;
  if (colorName == "blue") return COLOR_BLUE;
  if (colorName == "yellow") return COLOR_YELLOW;
  if (colorName == "orange") return COLOR_ORANGE;
  if (colorName == "purple") return COLOR_PURPLE;
  if (colorName == "pink") return COLOR_RED;
  if (colorName == "lime") return COLOR_GREEN;
  if (colorName == "sky") return COLOR_BLUE;
  return COLOR_GRAY; // Default for unknown colors
}

void UI::clearScreen() {
  M5Cardputer.Display.fillScreen(COLOR_BLACK);
}

void UI::drawHeader(const String& title, const String& subtitle) {
  M5Cardputer.Display.setTextColor(COLOR_WHITE);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setCursor(MARGIN, MARGIN);
  M5Cardputer.Display.print(title);
  
  if (subtitle.length() > 0) {
    M5Cardputer.Display.setCursor(MARGIN, MARGIN + LINE_HEIGHT);
    M5Cardputer.Display.setTextColor(COLOR_GRAY);
    M5Cardputer.Display.print(subtitle);
  }
}

void UI::drawFooter(const String& leftText, const String& rightText) {
  int footerY = SCREEN_HEIGHT - LINE_HEIGHT - MARGIN;
  
  if (leftText.length() > 0) {
    M5Cardputer.Display.setTextColor(COLOR_GRAY);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(MARGIN, footerY);
    M5Cardputer.Display.print(leftText);
  }
  
  if (rightText.length() > 0) {
    M5Cardputer.Display.setTextColor(COLOR_GRAY);
    M5Cardputer.Display.setTextSize(1);
    int textWidth = rightText.length() * 6; // Approximate char width
    M5Cardputer.Display.setCursor(SCREEN_WIDTH - textWidth - MARGIN, footerY);
    M5Cardputer.Display.print(rightText);
  }
}

void UI::drawScrollIndicator(int currentPage, int totalPages) {
  if (totalPages <= 1) return;
  
  String pageText = "Pg " + String(currentPage + 1) + "/" + String(totalPages);
  int textWidth = pageText.length() * 6;
  int x = SCREEN_WIDTH - textWidth - MARGIN;
  int y = MARGIN + LINE_HEIGHT + 2;
  
  M5Cardputer.Display.setTextColor(COLOR_GRAY);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setCursor(x, y);
  M5Cardputer.Display.print(pageText);
}

void UI::drawStatusBar(bool online, bool cacheMode) {
  int statusY = MARGIN + LINE_HEIGHT * 2;
  
  // Online/offline indicator
  M5Cardputer.Display.fillCircle(SCREEN_WIDTH - 20, statusY + 3, 2, 
                                 online ? COLOR_GREEN : COLOR_RED);
  
  if (cacheMode) {
    M5Cardputer.Display.setTextColor(COLOR_YELLOW);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(SCREEN_WIDTH - 50, statusY);
    M5Cardputer.Display.print("CACHE");
  }
}

String UI::truncateText(const String& text, int maxChars) {
  if (text.length() <= maxChars) {
    return text;
  }
  return text.substring(0, maxChars - 3) + "...";
}

void UI::drawWrappedText(const String& text, int x, int y, int maxWidth, int maxLines) {
  int charsPerLine = maxWidth / 6; // Approximate chars per line
  int currentLine = 0;
  int textLength = text.length();
  int startIndex = 0;
  
  M5Cardputer.Display.setTextSize(1);
  
  while (startIndex < textLength && (maxLines == -1 || currentLine < maxLines)) {
    int endIndex = min(startIndex + charsPerLine, textLength);
    
    // Try to break at word boundary
    if (endIndex < textLength) {
      int lastSpace = text.lastIndexOf(' ', endIndex);
      if (lastSpace > startIndex) {
        endIndex = lastSpace;
      }
    }
    
    String line = text.substring(startIndex, endIndex);
    M5Cardputer.Display.setCursor(x, y + currentLine * LINE_HEIGHT);
    M5Cardputer.Display.print(line);
    
    startIndex = endIndex;
    if (startIndex < textLength && text[startIndex] == ' ') {
      startIndex++; // Skip the space
    }
    currentLine++;
  }
}

void UI::drawProgressIndicator(int current, int total, int y) {
  int barWidth = SCREEN_WIDTH - 2 * MARGIN;
  int progressWidth = (current * barWidth) / total;
  
  // Background bar
  M5Cardputer.Display.drawRect(MARGIN, y, barWidth, 4, COLOR_GRAY);
  
  // Progress bar
  if (progressWidth > 0) {
    M5Cardputer.Display.fillRect(MARGIN, y, progressWidth, 4, COLOR_GREEN);
  }
}

void UI::renderSplashScreen() {
  clearScreen();
  
  // Title
  M5Cardputer.Display.setTextColor(COLOR_WHITE);
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setCursor(30, 30);
  M5Cardputer.Display.print("Trello Client");
  
  // Version
  M5Cardputer.Display.setTextColor(COLOR_GRAY);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setCursor(80, 55);
  M5Cardputer.Display.print("v1.0");
  
  // Loading message
  M5Cardputer.Display.setTextColor(COLOR_YELLOW);
  M5Cardputer.Display.setCursor(60, 80);
  M5Cardputer.Display.print("Connecting...");
  
  // Progress dots animation
  static int dotCount = 0;
  String dots = "";
  for (int i = 0; i < (dotCount % 4); i++) {
    dots += ".";
  }
  M5Cardputer.Display.setCursor(140, 80);
  M5Cardputer.Display.print("    "); // Clear previous dots
  M5Cardputer.Display.setCursor(140, 80);
  M5Cardputer.Display.print(dots);
  dotCount++;
}

void UI::renderListView(const std::vector<CardSummary>& cards, int selectedIndex, 
                       int currentPage, int totalPages, bool isOnline) {
  clearScreen();
  
  // Header
  drawHeader("Trello Cards");
  drawScrollIndicator(currentPage, totalPages);
  drawStatusBar(isOnline);
  
  // Cards list
  int startY = MARGIN + LINE_HEIGHT * 3;
  int startIndex = currentPage * CARDS_PER_PAGE;
  int endIndex = min(startIndex + CARDS_PER_PAGE, (int)cards.size());
  
  for (int i = startIndex; i < endIndex; i++) {
    int itemY = startY + (i - startIndex) * CARD_ITEM_HEIGHT;
    bool isSelected = (i == selectedIndex);
    
    // Selection highlight
    if (isSelected) {
      M5Cardputer.Display.fillRect(0, itemY - 1, SCREEN_WIDTH, CARD_ITEM_HEIGHT, COLOR_GRAY);
    }
    
    // Selection arrow
    M5Cardputer.Display.setTextColor(isSelected ? COLOR_BLACK : COLOR_WHITE);
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setCursor(MARGIN, itemY);
    M5Cardputer.Display.print(isSelected ? ">" : " ");
    
    // Card name
    String displayName = truncateText(cards[i].name, 30);
    M5Cardputer.Display.setCursor(MARGIN + 10, itemY);
    M5Cardputer.Display.print(displayName);
    
    // Label indicators
    int labelX = SCREEN_WIDTH - 30;
    for (int j = 0; j < min(3, (int)cards[i].labelColors.size()); j++) {
      uint16_t color = getLabelColor(cards[i].labelColors[j]);
      M5Cardputer.Display.fillRect(labelX + j * 4, itemY + 2, 
                                  LABEL_INDICATOR_SIZE, LABEL_INDICATOR_SIZE, color);
    }
    
    // Done indicator
    if (cards[i].isDone) {
      M5Cardputer.Display.fillCircle(SCREEN_WIDTH - 10, itemY + 5, 3, COLOR_GREEN);
    }
    
    // Due date indicator
    if (cards[i].hasDueDate) {
      M5Cardputer.Display.fillRect(SCREEN_WIDTH - 8, itemY, 2, CARD_ITEM_HEIGHT - 2, COLOR_YELLOW);
    }
  }
  
  // Footer
  drawFooter("ENTER:Open C:Comment N:New", "B:Back");
  
  // Empty list message
  if (cards.empty()) {
    M5Cardputer.Display.setTextColor(COLOR_GRAY);
    M5Cardputer.Display.setCursor(60, 60);
    M5Cardputer.Display.print("No cards found");
  }
}

void UI::renderCardDetail(const FullCard& card, int scrollPosition) {
  clearScreen();
  
  // Header with card name
  String headerName = truncateText(card.summary.name, 35);
  drawHeader("Card Details", headerName);
  
  int contentY = MARGIN + LINE_HEIGHT * 3;
  int scrollY = contentY - scrollPosition;
  
  // Description
  if (card.description.length() > 0) {
    M5Cardputer.Display.setTextColor(COLOR_WHITE);
    M5Cardputer.Display.setCursor(MARGIN, scrollY);
    M5Cardputer.Display.print("Description:");
    scrollY += LINE_HEIGHT;
    
    M5Cardputer.Display.setTextColor(COLOR_GRAY);
    drawWrappedText(card.description, MARGIN, scrollY, SCREEN_WIDTH - 2 * MARGIN, 4);
    scrollY += LINE_HEIGHT * 4;
  }
  
  // Due date
  if (card.summary.hasDueDate && card.dueDate.length() > 0) {
    M5Cardputer.Display.setTextColor(COLOR_YELLOW);
    M5Cardputer.Display.setCursor(MARGIN, scrollY);
    M5Cardputer.Display.print("Due: " + card.dueDate.substring(0, 10));
    scrollY += LINE_HEIGHT;
  }
  
  // Checklists
  if (!card.checklists.empty()) {
    M5Cardputer.Display.setTextColor(COLOR_WHITE);
    M5Cardputer.Display.setCursor(MARGIN, scrollY);
    M5Cardputer.Display.print("Checklist:");
    scrollY += LINE_HEIGHT;
    
    for (const auto& item : card.checklists) {
      if (scrollY >= SCREEN_HEIGHT - LINE_HEIGHT * 2) break;
      
      M5Cardputer.Display.setTextColor(item.isComplete ? COLOR_GREEN : COLOR_GRAY);
      M5Cardputer.Display.setCursor(MARGIN, scrollY);
      String checkbox = item.isComplete ? "[x] " : "[ ] ";
      String itemText = checkbox + truncateText(item.name, 30);
      M5Cardputer.Display.print(itemText);
      scrollY += LINE_HEIGHT;
    }
    scrollY += LINE_HEIGHT / 2;
  }
  
  // Comments
  if (!card.comments.empty()) {
    M5Cardputer.Display.setTextColor(COLOR_WHITE);
    M5Cardputer.Display.setCursor(MARGIN, scrollY);
    M5Cardputer.Display.print("Comments:");
    scrollY += LINE_HEIGHT;
    
    for (const auto& comment : card.comments) {
      if (scrollY >= SCREEN_HEIGHT - LINE_HEIGHT * 3) break;
      
      M5Cardputer.Display.setTextColor(COLOR_BLUE);
      drawWrappedText(comment, MARGIN, scrollY, SCREEN_WIDTH - 2 * MARGIN, 2);
      scrollY += LINE_HEIGHT * 2;
    }
  }
  
  // Labels display
  if (!card.summary.labelColors.empty()) {
    int labelY = MARGIN + LINE_HEIGHT;
    for (int i = 0; i < min(5, (int)card.summary.labelColors.size()); i++) {
      uint16_t color = getLabelColor(card.summary.labelColors[i]);
      M5Cardputer.Display.fillRect(SCREEN_WIDTH - 25 + i * 4, labelY, 
                                  LABEL_INDICATOR_SIZE, LABEL_INDICATOR_SIZE, color);
    }
  }
  
  // Footer
  drawFooter("C:Comment D:Done", "UP/DN:Scroll B:Back");
}

void UI::renderAddComment(const String& cardName, const String& inputBuffer, int cursorPosition) {
  clearScreen();
  
  // Header
  String headerName = truncateText(cardName, 25);
  drawHeader("Add Comment", headerName);
  
  // Input area
  int inputY = MARGIN + LINE_HEIGHT * 3;
  M5Cardputer.Display.setTextColor(COLOR_WHITE);
  M5Cardputer.Display.setCursor(MARGIN, inputY);
  M5Cardputer.Display.print("Comment:");
  
  // Input box
  M5Cardputer.Display.drawRect(MARGIN, inputY + LINE_HEIGHT, 
                              SCREEN_WIDTH - 2 * MARGIN, LINE_HEIGHT * 4, COLOR_GRAY);
  
  // Input text
  M5Cardputer.Display.setTextColor(COLOR_WHITE);
  drawWrappedText(inputBuffer, MARGIN + 2, inputY + LINE_HEIGHT + 2, 
                 SCREEN_WIDTH - 2 * MARGIN - 4, 3);
  
  // Cursor
  int cursorX = MARGIN + 2 + (cursorPosition % 35) * 6;
  int cursorY = inputY + LINE_HEIGHT + 2 + (cursorPosition / 35) * LINE_HEIGHT;
  showInputCursor(cursorX, cursorY);
  
  // Footer
  drawFooter("ENTER:Send ESC:Cancel", "DEL:Delete");
}

void UI::renderCreateCard(const String& nameBuffer, const String& descBuffer, 
                         bool editingName, int cursorPosition) {
  clearScreen();
  
  // Header
  drawHeader("Create New Card");
  
  int inputY = MARGIN + LINE_HEIGHT * 2;
  
  // Name field
  M5Cardputer.Display.setTextColor(editingName ? COLOR_WHITE : COLOR_GRAY);
  M5Cardputer.Display.setCursor(MARGIN, inputY);
  M5Cardputer.Display.print("Name:");
  
  M5Cardputer.Display.drawRect(MARGIN, inputY + LINE_HEIGHT, 
                              SCREEN_WIDTH - 2 * MARGIN, LINE_HEIGHT + 4, 
                              editingName ? COLOR_WHITE : COLOR_GRAY);
  
  M5Cardputer.Display.setTextColor(COLOR_WHITE);
  M5Cardputer.Display.setCursor(MARGIN + 2, inputY + LINE_HEIGHT + 2);
  M5Cardputer.Display.print(truncateText(nameBuffer, 35));
  
  if (editingName) {
    int cursorX = MARGIN + 2 + (cursorPosition * 6);
    showInputCursor(cursorX, inputY + LINE_HEIGHT + 2);
  }
  
  // Description field
  inputY += LINE_HEIGHT * 3;
  M5Cardputer.Display.setTextColor(!editingName ? COLOR_WHITE : COLOR_GRAY);
  M5Cardputer.Display.setCursor(MARGIN, inputY);
  M5Cardputer.Display.print("Description:");
  
  M5Cardputer.Display.drawRect(MARGIN, inputY + LINE_HEIGHT, 
                              SCREEN_WIDTH - 2 * MARGIN, LINE_HEIGHT * 3, 
                              !editingName ? COLOR_WHITE : COLOR_GRAY);
  
  M5Cardputer.Display.setTextColor(COLOR_WHITE);
  drawWrappedText(descBuffer, MARGIN + 2, inputY + LINE_HEIGHT + 2, 
                 SCREEN_WIDTH - 2 * MARGIN - 4, 2);
  
  if (!editingName) {
    int cursorX = MARGIN + 2 + (cursorPosition % 35) * 6;
    int cursorDescY = inputY + LINE_HEIGHT + 2 + (cursorPosition / 35) * LINE_HEIGHT;
    showInputCursor(cursorX, cursorDescY);
  }
  
  // Footer
  drawFooter("TAB:Switch Field", "ENTER:Create ESC:Cancel");
}

void UI::renderError(const String& errorMessage, const String& suggestion) {
  clearScreen();
  
  // Header
  drawHeader("Error", "");
  
  // Error icon (simple X)
  M5Cardputer.Display.setTextColor(COLOR_RED);
  M5Cardputer.Display.setTextSize(3);
  M5Cardputer.Display.setCursor(SCREEN_WIDTH / 2 - 15, 40);
  M5Cardputer.Display.print("X");
  
  // Error message
  M5Cardputer.Display.setTextColor(COLOR_WHITE);
  M5Cardputer.Display.setTextSize(1);
  drawWrappedText(errorMessage, MARGIN, 70, SCREEN_WIDTH - 2 * MARGIN, 3);
  
  // Suggestion
  if (suggestion.length() > 0) {
    M5Cardputer.Display.setTextColor(COLOR_YELLOW);
    drawWrappedText(suggestion, MARGIN, 100, SCREEN_WIDTH - 2 * MARGIN, 2);
  }
  
  // Footer
  drawFooter("", "Press any key to continue");
}

void UI::renderLoadingScreen(const String& message) {
  clearScreen();
  
  // Header
  drawHeader("Loading...");
  
  // Loading message
  M5Cardputer.Display.setTextColor(COLOR_YELLOW);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setCursor(MARGIN, 60);
  M5Cardputer.Display.print(message);
  
  // Animated dots
  static unsigned long lastDotUpdate = 0;
  static int dotCount = 0;
  
  if (millis() - lastDotUpdate > 500) {
    dotCount = (dotCount + 1) % 4;
    lastDotUpdate = millis();
  }
  
  String dots = "";
  for (int i = 0; i < dotCount; i++) {
    dots += ".";
  }
  M5Cardputer.Display.setCursor(MARGIN + message.length() * 6, 60);
  M5Cardputer.Display.print("    "); // Clear previous dots
  M5Cardputer.Display.setCursor(MARGIN + message.length() * 6, 60);
  M5Cardputer.Display.print(dots);
}

void UI::showInputCursor(int x, int y, bool visible) {
  if (visible && (millis() % 1000) < 500) { // Blink every 500ms
    M5Cardputer.Display.drawLine(x, y, x, y + LINE_HEIGHT - 2, COLOR_WHITE);
  }
}

void UI::highlightSelection(int x, int y, int width, int height) {
  M5Cardputer.Display.drawRect(x, y, width, height, COLOR_WHITE);
}

void UI::showMessage(const String& message, int duration) {
  // Simple toast-like message at bottom
  int msgY = SCREEN_HEIGHT - LINE_HEIGHT * 2;
  M5Cardputer.Display.fillRect(0, msgY - 2, SCREEN_WIDTH, LINE_HEIGHT + 4, COLOR_GRAY);
  M5Cardputer.Display.setTextColor(COLOR_BLACK);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setCursor(MARGIN, msgY);
  M5Cardputer.Display.print(truncateText(message, 35));
  
  delay(duration);
}

void UI::playTone(int frequency, int duration) {
  M5Cardputer.Speaker.tone(frequency, duration);
}

void UI::playSuccessSound() {
  playTone(TONE_SUCCESS, TONE_DURATION_MS);
}

void UI::playErrorSound() {
  playTone(TONE_ERROR, TONE_DURATION_MS);
}