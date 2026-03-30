#pragma once
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "../config.h"

extern Adafruit_SSD1306 display;
extern const GFXfont Picopixel;

void openWifi() {
  int currentSelection = 0;
  bool inMenu = true;
  
  bool prevUp = HIGH, prevDown = HIGH, prevSel = HIGH;
  
  // Wait to ensure SELECT from previous menu is fully released
  while(digitalRead(BTN_SELECT) == LOW) { delay(10); }

  while(inMenu) {
    display.clearDisplay();
    display.setFont(&Picopixel);
    display.setTextWrap(false);
    display.setTextColor(1);
    
    display.setCursor(35, 10);
    display.print("CONNECT WIFI");
    
    // Draw networks (+1 for Back button)
    for (int i = 0; i <= numNetworks; i++) {
      int y = 20 + (i * 8);
      
      if (currentSelection == i) {
        display.fillRect(0, y - 6, 128, 8, 1);
        display.setTextColor(0);
      } else {
        display.setTextColor(1);
      }
      
      display.setCursor(10, y);
      if (i == 0) {
        display.print("< Back");
      } else {
        display.print(networks[i-1].ssid);
      }
    }
    display.display();
    
    // Input Handling
    bool currUp = digitalRead(BTN_UP);
    bool currDown = digitalRead(BTN_DOWN);
    bool currSel = digitalRead(BTN_SELECT);

    if (prevUp == HIGH && currUp == LOW) {
      currentSelection--;
      if (currentSelection < 0) currentSelection = numNetworks;
    }
    if (prevDown == HIGH && currDown == LOW) {
      currentSelection++;
      if (currentSelection > numNetworks) currentSelection = 0;
    }
    if (prevSel == HIGH && currSel == LOW) {
      if (currentSelection == 0) {
        inMenu = false; // Go back
      } else {
        // Connect to network
        int netIdx = currentSelection - 1;
        display.clearDisplay();
        display.setTextColor(1);
        display.setFont(); // standard font for connection logs
        display.setCursor(0, 10);
        display.print("Connecting to:");
        display.setCursor(0, 20);
        display.print(networks[netIdx].ssid);
        display.display();
        
        WiFi.begin(networks[netIdx].ssid, networks[netIdx].password);
        
        unsigned long startAttempt = millis();
        bool connected = false;
        int dotCounter = 0;
        
        display.setCursor(0, 30);
        while(millis() - startAttempt < 8000) { // 8 second timeout
          if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            break;
          }
          delay(500);
          display.print(".");
          display.display();
          dotCounter++;
          if (dotCounter > 20) {
            display.setCursor(0, 40);
            dotCounter = 0;
          }
        }
        
        display.clearDisplay();
        display.setCursor(0, 10);
        if (connected) {
          display.print("Connected!");
          display.setCursor(0, 20);
          display.print("IP: ");
          display.print(WiFi.localIP());
        } else {
          display.print("Connection Failed.");
        }
        display.display();
        delay(3000);
        inMenu = false; // Go back to main menu after attempt
        while(digitalRead(BTN_SELECT) == LOW) { delay(10); } // Wait for release
      }
    }
    
    prevUp = currUp;
    prevDown = currDown;
    prevSel = currSel;
    
    delay(50);
  }
}
