# Troubleshooting Guide - M5Cardputer Trello Client

## Compilation Errors and Solutions

### 1. ArduinoJson Version Issues

**Error**: `'ArduinoJson::V6215PB2::JsonDocument::JsonDocument()' is protected within this context` or `invalid use of incomplete type`

**Solution**: 
- Uninstall ArduinoJson v7.x.x from Library Manager
- Install ArduinoJson v6.21.5 instead
- The code now uses `DynamicJsonDocument` instead of `JsonDocument` for v6 compatibility
- ArduinoJson v7 has breaking API changes that are incompatible with this code

**Steps**:
1. Go to Arduino IDE > Tools > Manage Libraries
2. Search for "ArduinoJson"
3. If version 7.x.x is installed, click "Remove"
4. Install version 6.21.5 specifically

### 2. M5Cardputer API Issues

**Errors**: 
- `'struct Keyboard_Class::KeysState' has no member named 'down'`
- `'class m5::M5_CARDPUTER' has no member named 'BtnB'`
- `'KEY_ESC' was not declared in this scope`

**Solutions**: 
- The code has been updated to use `M5Cardputer.Keyboard.isKeyPressed()` instead of KeysState
- BtnB doesn't exist - using double-press BtnA for back navigation
- KEY_ESC is replaced with backtick (`` ` ``) key
- Arrow key equivalents are mapped to specific keys:
  - Down: `;` (semicolon)
  - Up: `/` (forward slash)  
  - Left: `,` (comma)
  - Right: `.` (period)
  - Cancel: `` ` `` (backtick)
  - Back: Double-press BtnA

### 3. Library Dependencies

**Required Libraries** (exact versions):
```
M5Cardputer@^1.0.2
M5GFX@^0.1.11
M5Unified@^0.1.11
ArduinoJson@^6.21.5
```

**Installation Order**:
1. Install ESP32 board package v2.0.11+ first
2. Install M5Cardputer library (will auto-install M5GFX and M5Unified)
3. Install ArduinoJson v6.21.5 specifically

### 4. Board Configuration

**Arduino IDE Settings**:
- Board: "M5Stamp-S3" or "ESP32S3 Dev Module"
- Port: Select the appropriate COM port
- Upload Speed: 921600
- USB CDC On Boot: Enabled

### 5. Memory Issues

**Error**: Memory allocation failures

**Solutions**:
- Ensure PSRAM is enabled: `Tools > PSRAM: "OPI PSRAM"`
- Use partition scheme with more app space: `Tools > Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"`

### 6. SD Card Issues

**Error**: SD card initialization failed

**Solutions**:
- Format SD card as FAT32
- Use a good quality SD card (Class 10 recommended)
- Insert card properly before powering on
- Check if SD card is required for your use case (can be disabled for testing)

### 7. WiFi Connection Issues

**Common problems**:
- Wrong SSID/password in config.h
- 5GHz network (ESP32 only supports 2.4GHz)
- Special characters in WiFi credentials

**Solutions**:
- Double-check credentials in config.h
- Use 2.4GHz network only
- Escape special characters in strings

### 8. Trello API Issues

**Error**: Authentication failed

**Solutions**:
- Get fresh API key from https://trello.com/app-key
- Generate new token with proper permissions
- Verify board ID and list ID are correct
- Check API rate limits (max 300 requests per 10 seconds)

### 9. Compilation Environment

**Recommended Setup**:
- Arduino IDE 2.0+ (preferred) or 1.8.19+
- ESP32 Arduino Core v2.0.11+
- Fresh installation of libraries (remove old versions)

### 10. Debug Mode

**Enable verbose output**:
```cpp
// In setup(), add:
Serial.setDebugOutput(true);
```

**Monitor Serial Output**:
- Baud rate: 115200
- Monitor connection status, API responses, and errors

## Common Quick Fixes

1. **Clean Rebuild**: Delete build cache and recompile
2. **Library Refresh**: Restart Arduino IDE after installing libraries
3. **Port Reset**: Unplug/replug USB cable if upload fails
4. **Factory Reset**: Hold G0 button while connecting for download mode

## Getting Help

If issues persist:
1. Check the Serial Monitor output at 115200 baud
2. Verify all library versions match requirements
3. Try a minimal example first to test hardware
4. Check M5Stack community forums for similar issues

## Test Code

Minimal test to verify hardware and libraries:

```cpp
#include <M5Cardputer.h>

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  Serial.begin(115200);
  M5Cardputer.Display.fillScreen(0x0000);
  M5Cardputer.Display.setTextColor(0xFFFF);
  M5Cardputer.Display.setCursor(10, 10);
  M5Cardputer.Display.print("Hardware Test OK");
}

void loop() {
  M5Cardputer.update();
  if (M5Cardputer.Keyboard.isChange()) {
    if (M5Cardputer.Keyboard.isPressed()) {
      Serial.println("Key pressed");
    }
  }
  delay(50);
}
```

This should compile and run successfully if all dependencies are correctly installed.