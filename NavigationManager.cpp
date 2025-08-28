#include "NavigationManager.h"

NavigationManager::NavigationManager(AppState* state) : appState(state) {
  navigationStack.clear();
}

void NavigationManager::saveCurrentContext() {
  NavigationContext context;
  context.state = appState->currentScreen;
  context.selectedIndex = appState->selectedCardIndex;
  context.currentPage = appState->currentPage;
  context.inputBuffer = appState->inputBuffer;
  
  // Only save if it's different from the last context
  if (navigationStack.empty() || 
      navigationStack.back().state != context.state ||
      navigationStack.back().selectedIndex != context.selectedIndex ||
      navigationStack.back().currentPage != context.currentPage) {
    navigationStack.push_back(context);
  }
}

void NavigationManager::restoreContext(const NavigationContext& context) {
  appState->currentScreen = context.state;
  appState->selectedCardIndex = context.selectedIndex;
  appState->currentPage = context.currentPage;
  appState->inputBuffer = context.inputBuffer;
}

void NavigationManager::pushState(ScreenState newState, int selectedIndex, int page, const String& cardId) {
  // Save current context before changing state
  saveCurrentContext();
  
  // Set new state
  appState->currentScreen = newState;
  appState->selectedCardIndex = selectedIndex;
  appState->currentPage = page;
  appState->inputBuffer = "";
  
  // Store card ID in the navigation context if provided
  if (!navigationStack.empty() && cardId.length() > 0) {
    navigationStack.back().cardId = cardId;
  }
}

bool NavigationManager::popState() {
  if (navigationStack.empty()) {
    return false;
  }
  
  // Restore previous context
  NavigationContext previousContext = navigationStack.back();
  navigationStack.pop_back();
  
  restoreContext(previousContext);
  
  return true;
}

void NavigationManager::clearStack() {
  navigationStack.clear();
}

bool NavigationManager::canGoBack() {
  return !navigationStack.empty();
}

void NavigationManager::setState(ScreenState newState) {
  appState->currentScreen = newState;
}

ScreenState NavigationManager::getCurrentState() {
  return appState->currentScreen;
}

void NavigationManager::nextPage() {
  int totalPages = getTotalPages(appState->cardList.size());
  if (appState->currentPage < totalPages - 1) {
    appState->currentPage++;
    appState->selectedCardIndex = appState->currentPage * CARDS_PER_PAGE;
  }
}

void NavigationManager::previousPage() {
  if (appState->currentPage > 0) {
    appState->currentPage--;
    appState->selectedCardIndex = appState->currentPage * CARDS_PER_PAGE;
  }
}

int NavigationManager::getTotalPages(int totalItems, int itemsPerPage) {
  if (totalItems == 0) return 1;
  return (totalItems + itemsPerPage - 1) / itemsPerPage;
}

bool NavigationManager::isValidPage(int page, int totalItems, int itemsPerPage) {
  int totalPages = getTotalPages(totalItems, itemsPerPage);
  return page >= 0 && page < totalPages;
}

void NavigationManager::selectNext(int maxItems) {
  if (maxItems == 0) return;
  
  int currentPageStart = appState->currentPage * CARDS_PER_PAGE;
  int currentPageEnd = min(currentPageStart + CARDS_PER_PAGE, maxItems);
  
  if (appState->selectedCardIndex < currentPageEnd - 1) {
    appState->selectedCardIndex++;
  } else {
    // Move to next page if available
    if (appState->currentPage < getTotalPages(maxItems) - 1) {
      nextPage();
    } else {
      // Wrap to first item on current page
      appState->selectedCardIndex = currentPageStart;
    }
  }
}

void NavigationManager::selectPrevious() {
  int currentPageStart = appState->currentPage * CARDS_PER_PAGE;
  
  if (appState->selectedCardIndex > currentPageStart) {
    appState->selectedCardIndex--;
  } else {
    // Move to previous page if available
    if (appState->currentPage > 0) {
      previousPage();
      // Select last item on previous page
      int prevPageStart = appState->currentPage * CARDS_PER_PAGE;
      int prevPageEnd = min(prevPageStart + CARDS_PER_PAGE, (int)appState->cardList.size());
      appState->selectedCardIndex = prevPageEnd - 1;
    } else {
      // Wrap to last page and last item
      int totalPages = getTotalPages(appState->cardList.size());
      appState->currentPage = totalPages - 1;
      int lastPageStart = appState->currentPage * CARDS_PER_PAGE;
      int lastPageEnd = min(lastPageStart + CARDS_PER_PAGE, (int)appState->cardList.size());
      appState->selectedCardIndex = lastPageEnd - 1;
    }
  }
}

void NavigationManager::setSelection(int index) {
  if (index >= 0 && index < (int)appState->cardList.size()) {
    appState->selectedCardIndex = index;
    appState->currentPage = index / CARDS_PER_PAGE;
  }
}

int NavigationManager::getSelection() {
  return appState->selectedCardIndex;
}

void NavigationManager::appendToInput(char c) {
  if (appState->inputBuffer.length() < MAX_TEXT_LENGTH) {
    appState->inputBuffer += c;
  }
}

void NavigationManager::deleteFromInput() {
  if (appState->inputBuffer.length() > 0) {
    appState->inputBuffer.remove(appState->inputBuffer.length() - 1);
  }
}

void NavigationManager::clearInput() {
  appState->inputBuffer = "";
}

String NavigationManager::getInput() {
  return appState->inputBuffer;
}

void NavigationManager::setInput(const String& text) {
  appState->inputBuffer = text;
}

String NavigationManager::getCurrentCardId() {
  if (!navigationStack.empty()) {
    return navigationStack.back().cardId;
  }
  if (appState->selectedCardIndex >= 0 && 
      appState->selectedCardIndex < (int)appState->cardList.size()) {
    return appState->cardList[appState->selectedCardIndex].id;
  }
  return "";
}

void NavigationManager::setCurrentCardId(const String& cardId) {
  if (!navigationStack.empty()) {
    navigationStack.back().cardId = cardId;
  }
}

void NavigationManager::printStack() {
  Serial.println("Navigation Stack:");
  for (int i = 0; i < navigationStack.size(); i++) {
    Serial.printf("  [%d] State: %d, Index: %d, Page: %d, CardID: %s\n", 
                  i, navigationStack[i].state, navigationStack[i].selectedIndex, 
                  navigationStack[i].currentPage, navigationStack[i].cardId.c_str());
  }
  Serial.printf("Current: State: %d, Index: %d, Page: %d\n", 
                appState->currentScreen, appState->selectedCardIndex, appState->currentPage);
}

int NavigationManager::getStackSize() {
  return navigationStack.size();
}