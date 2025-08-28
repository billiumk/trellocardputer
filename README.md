# M5Cardputer Trello Client

A lightweight Trello client application for the M5Cardputer ESP32-S3 device. This application provides a compact, embedded interface for interacting with Trello boards, optimized for the M5Cardputer's 240x135 display and 56-key keyboard.

## Features

### Core Functionality
- **View Cards**: Browse cards from a configured Trello list with pagination
- **Card Details**: View card descriptions, comments, and checklists
- **Add Comments**: Add new comments to cards using the keyboard
- **Mark Tasks Done**: Complete checklist items to mark tasks as finished
- **Create Cards**: Add new cards with name and description
- **Navigation Stack**: Full back/forward navigation support

### Quality of Life Features
- **Keyboard Shortcuts**: Quick actions without deep navigation
- **Offline Caching**: Continue working when WiFi is unavailable
- **Visual Indicators**: Colored dots for labels and status indicators
- **Pagination**: Navigate through long lists efficiently
- **Error Handling**: Robust error handling with user-friendly messages
- **Audio Feedback**: Sound confirmation for actions
- **Power Management**: Automatic sleep mode to preserve battery

## Hardware Requirements

- **M5Cardputer**: ESP32-S3 based device with:
  - 1.14" TFT Display (240x135 pixels)
  - 56-key QWERTY keyboard
  - MicroSD card slot
  - WiFi connectivity
  - Built-in speaker

## Software Requirements

### Arduino IDE Setup
1. Install Arduino IDE 2.0 or later
2. Add ESP32 board package URL: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
3. Install the ESP32 board package (version 2.0.11 or later)
4. Select board: "M5Stamp-S3"

### Required Libraries
Install these libraries through the Arduino Library Manager:

- **M5Cardputer** (v1.0.2 or later)
- **M5GFX** (v0.1.11 or later)  
- **M5Unified** (v0.1.11 or later)
- **ArduinoJson** (v6.21.0 or later)
- **WiFi** (built-in with ESP32)
- **SD** (built-in with ESP32)

## Configuration

### 1. Trello API Setup
1. Visit [Trello Developer Portal](https://trello.com/app-key)
2. Get your API Key
3. Generate a Token with read/write permissions
4. Note your Board ID and List ID (can be found in URLs or via API)

### 2. Configure the Application
Edit `config.h` with your credentials:

```cpp
// Trello API Configuration
#define TRELLO_API_KEY "your_trello_api_key_here"
#define TRELLO_API_TOKEN "your_trello_api_token_here" 
#define TRELLO_BOARD_ID "your_board_id_here"
#define TRELLO_LIST_ID "your_list_id_here"

// WiFi Configuration
const char* WIFI_SSID = "your_wifi_ssid";
const char* WIFI_PASSWORD = "your_wifi_password";
```

### 3. Upload to Device
1. Connect M5Cardputer via USB
2. Hold the G0 button while connecting for download mode
3. Select the correct port in Arduino IDE
4. Upload the sketch

## Usage

### Navigation
- **Arrow Keys**: Navigate through lists and menus
- **Enter**: Select items or confirm actions
- **B Button**: Go back to previous screen
- **Page Up/Down**: Navigate between pages

### Keyboard Shortcuts
- **C**: Add comment to selected card
- **N**: Create new card
- **D**: Mark first incomplete checklist item as done
- **R**: Refresh current view
- **ESC**: Cancel current action
- **TAB**: Switch between input fields (when creating cards)

### Screen Layout
```
┌─────────────────────────────────────┐
│ Header: Screen Title       [Status] │
│                                     │
│ > Card 1: Task name        [●●○]    │
│   Card 2: Another task     [●○○]    │
│   Card 3: Third task       [○○○]    │
│                                     │
│ Footer: Shortcuts         Page 1/3  │
└─────────────────────────────────────┘
```

### Status Indicators
- **Colored Dots**: Represent Trello label colors
- **Green Circle**: Task is complete (all checklist items done)
- **Yellow Bar**: Card has a due date
- **Connection Dot**: Green (online) / Red (offline)

## Offline Mode

The application automatically caches data to the SD card:
- Card lists are cached for offline browsing
- Card details are cached when viewed
- Cache is automatically refreshed when online
- Offline operations are queued and synced when reconnected

## Troubleshooting

### Common Issues

**WiFi Connection Failed**
- Check SSID and password in config.h
- Ensure WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Try moving closer to the router

**API Authentication Failed**
- Verify API key and token in config.h
- Check that token has proper permissions
- Ensure board and list IDs are correct

**SD Card Issues**
- Format SD card as FAT32
- Ensure card is properly inserted
- Try a different SD card if problems persist

**Display Issues**
- Check that M5GFX library is properly installed
- Ensure correct board selection in Arduino IDE

### Debug Information
Enable Serial Monitor (115200 baud) for detailed logging:
- Connection status
- API request/response information
- Error messages and stack traces
- Navigation state changes

## Development

### Project Structure
```
M5Cardputer_Trello_Client/
├── M5Cardputer_Trello_Client.ino  # Main Arduino sketch
├── config.h                       # Configuration constants
├── DataStructures.h               # Data type definitions
├── TrelloClient.h/.cpp           # Trello API interface
├── UI.h/.cpp                     # Display rendering
├── NavigationManager.h/.cpp      # Navigation logic
└── README.md                     # This file
```

### Architecture
- **State Machine**: Manages different application screens
- **Navigation Stack**: Enables back/forward navigation
- **Event-Driven**: Keyboard events trigger state transitions
- **Caching Layer**: Transparent offline/online data access
- **Modular Design**: Separate concerns for maintainability

### Adding Features
The modular design makes it easy to extend:
- Add new screens by extending `ScreenState` enum
- Add new keyboard shortcuts in respective input handlers
- Extend UI with new rendering functions
- Add new API endpoints in `TrelloClient` class

## License

This project is provided as-is for educational and personal use. Please respect Trello's API terms of service and rate limits.

## Contributing

This is an AI-generated project. Feel free to:
- Report issues or suggestions
- Fork and modify for your needs
- Share improvements with the community

## Version History

- **v1.0**: Initial release with core functionality
  - Card viewing and navigation
  - Comment addition
  - Card creation
  - Offline caching
  - Keyboard shortcuts

---

**Note**: This project demonstrates embedded application development for the M5Cardputer platform, showcasing integration of WiFi, SD storage, display management, and API interactions in a resource-constrained environment.