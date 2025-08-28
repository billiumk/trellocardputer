#!/bin/bash
# Git commands to create a PR for the M5Cardputer Trello Client fixes

# Create and switch to a new branch
git checkout -b fix/compilation-errors

# Add all the modified files
git add config.h
git add DataStructures.h
git add TrelloClient.h
git add TrelloClient.cpp
git add UI.h
git add UI.cpp
git add NavigationManager.h
git add NavigationManager.cpp
git add M5Cardputer_Trello_Client.ino
git add README.md
git add libraries.txt
git add platformio.ini
git add TROUBLESHOOTING.md

# Commit the changes with a descriptive message
git commit -m "Fix compilation errors and library compatibility

- Fix ArduinoJson v7 compatibility by reverting to v6 API
- Update keyboard input handling for M5Cardputer API changes
- Fix String.trim() usage to work with Arduino String class
- Add proper text input handling with individual key checking
- Update library dependencies to specify ArduinoJson v6.21.5
- Add comprehensive troubleshooting guide
- Map arrow keys to specific characters (;/,. for down/up/left/right)

Resolves compilation errors:
- JsonDocument API incompatibility with ArduinoJson v7
- KeysState missing members (down, up, left, right)
- String method return type issues
- Text input handling problems"

# Push the branch to origin
git push origin fix/compilation-errors

echo "Branch pushed! Now create a PR from fix/compilation-errors to main"
echo "PR Title: Fix compilation errors and library compatibility"
echo "Use the commit message above as the PR description"