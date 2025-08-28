#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <Arduino.h>
#include <vector>

// Forward declarations
struct ChecklistItem {
  String id;
  String name;
  bool isComplete;
  
  ChecklistItem() : isComplete(false) {}
  ChecklistItem(String _id, String _name, bool _complete = false) 
    : id(_id), name(_name), isComplete(_complete) {}
};

struct CardSummary {
  String id;
  String name;
  std::vector<String> labelColors;
  bool hasDueDate;
  bool isDone;
  
  CardSummary() : hasDueDate(false), isDone(false) {}
};

struct FullCard {
  CardSummary summary;
  String description;
  String dueDate;
  std::vector<String> comments;
  std::vector<ChecklistItem> checklists;
  
  FullCard() {}
  FullCard(const CardSummary& _summary) : summary(_summary) {}
};

// Screen states for navigation
enum ScreenState {
  SPLASH_SCREEN,
  LIST_VIEW,
  CARD_DETAIL,
  ADD_COMMENT,
  CREATE_CARD,
  ERROR_SCREEN
};

// Navigation context
struct NavigationContext {
  ScreenState state;
  int selectedIndex;
  int currentPage;
  String cardId;
  String inputBuffer;
  
  NavigationContext(ScreenState _state = LIST_VIEW, int _selected = 0, int _page = 0) 
    : state(_state), selectedIndex(_selected), currentPage(_page) {}
};

// API Response status
enum ApiStatus {
  API_SUCCESS,
  API_ERROR_NETWORK,
  API_ERROR_AUTH,
  API_ERROR_RATE_LIMIT,
  API_ERROR_NOT_FOUND,
  API_ERROR_PARSE,
  API_ERROR_UNKNOWN
};

// Application state
struct AppState {
  ScreenState currentScreen;
  std::vector<NavigationContext> navigationStack;
  std::vector<CardSummary> cardList;
  FullCard currentCard;
  int selectedCardIndex;
  int currentPage;
  String inputBuffer;
  bool isOnline;
  unsigned long lastActivity;
  bool needsRefresh;
  
  AppState() : currentScreen(SPLASH_SCREEN), selectedCardIndex(0), 
               currentPage(0), isOnline(false), lastActivity(0), 
               needsRefresh(true) {}
};

#endif // DATA_STRUCTURES_H