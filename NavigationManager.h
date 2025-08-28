#ifndef NAVIGATION_MANAGER_H
#define NAVIGATION_MANAGER_H

#include <Arduino.h>
#include <vector>
#include "DataStructures.h"
#include "config.h"

class NavigationManager {
private:
  std::vector<NavigationContext> navigationStack;
  AppState* appState;
  
  // Navigation helpers
  void saveCurrentContext();
  void restoreContext(const NavigationContext& context);
  
public:
  NavigationManager(AppState* state);
  
  // Stack management
  void pushState(ScreenState newState, int selectedIndex = 0, int page = 0, const String& cardId = "");
  bool popState();
  void clearStack();
  bool canGoBack();
  
  // State management
  void setState(ScreenState newState);
  ScreenState getCurrentState();
  
  // Page navigation
  void nextPage();
  void previousPage();
  int getTotalPages(int totalItems, int itemsPerPage = CARDS_PER_PAGE);
  bool isValidPage(int page, int totalItems, int itemsPerPage = CARDS_PER_PAGE);
  
  // Selection management
  void selectNext(int maxItems);
  void selectPrevious();
  void setSelection(int index);
  int getSelection();
  
  // Input buffer management
  void appendToInput(char c);
  void deleteFromInput();
  void clearInput();
  String getInput();
  void setInput(const String& text);
  
  // Context getters
  String getCurrentCardId();
  void setCurrentCardId(const String& cardId);
  
  // Debug
  void printStack();
  int getStackSize();
};

#endif // NAVIGATION_MANAGER_H