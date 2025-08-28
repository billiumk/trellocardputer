#ifndef CONFIG_H
#define CONFIG_H

// Trello API Configuration
// Replace these with your actual Trello API credentials
#define TRELLO_API_KEY "your_trello_api_key_here"
#define TRELLO_API_TOKEN "your_trello_api_token_here"
#define TRELLO_BOARD_ID "your_board_id_here"
#define TRELLO_LIST_ID "your_list_id_here"

// WiFi Configuration
// Replace these with your WiFi credentials
const char* WIFI_SSID = "your_wifi_ssid";
const char* WIFI_PASSWORD = "your_wifi_password";

// API Configuration
#define TRELLO_BASE_URL "https://api.trello.com/1"
#define MAX_RETRIES 3
#define RETRY_DELAY_MS 2000
#define API_RATE_LIMIT_DELAY_MS 5000

// Display Configuration
#define CARDS_PER_PAGE 5
#define MAX_TEXT_LENGTH 100
#define DESCRIPTION_CHAR_LIMIT 500

// Power Management
#define IDLE_TIMEOUT_MS 300000  // 5 minutes

// Cache Configuration
#define CACHE_LIST_FILE "/cache_list.json"
#define CACHE_DETAILS_PREFIX "/cache_detail_"
#define MAX_CACHE_SIZE 4096

// UI Colors (16-bit RGB565)
#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE 0x001F
#define COLOR_YELLOW 0xFFE0
#define COLOR_ORANGE 0xFD20
#define COLOR_PURPLE 0x780F
#define COLOR_GRAY 0x8410

// Audio feedback
#define TONE_SUCCESS 1000
#define TONE_ERROR 500
#define TONE_DURATION_MS 200

#endif // CONFIG_H