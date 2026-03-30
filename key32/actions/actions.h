#pragma once
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>

extern Adafruit_SSD1306 display;
extern const GFXfont Picopixel;

void openActions() {
  int currentSelection = 0;
  bool inMenu = true;
  const int numActions = 1;
  const char* actList[] = {"Ping Google"};
  
  bool prevUp = HIGH, prevDown = HIGH, prevSel = HIGH;
  
  // Wait to ensure SELECT from previous menu is fully released
  while(digitalRead(BTN_SELECT) == LOW) { delay(10); }

  while(inMenu) {
    display.clearDisplay();
    display.setFont(&Picopixel);
    display.setTextWrap(false);
    display.setTextColor(1);
    
    display.setCursor(45, 10);
    display.print("ACTIONS");
    
    for (int i = 0; i <= numActions; i++) {
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
        display.print(actList[i-1]);
      }
    }
    display.display();
    
    // Input Handling
    bool currUp = digitalRead(BTN_UP);
    bool currDown = digitalRead(BTN_DOWN);
    bool currSel = digitalRead(BTN_SELECT);

    if (prevUp == HIGH && currUp == LOW) {
      currentSelection--;
      if (currentSelection < 0) currentSelection = numActions;
    }
    if (prevDown == HIGH && currDown == LOW) {
      currentSelection++;
      if (currentSelection > numActions) currentSelection = 0;
    }
    if (prevSel == HIGH && currSel == LOW) {
      if (currentSelection == 0) {
        inMenu = false; // Back
      } else if (currentSelection == 1) {
        // Ping Google
        display.clearDisplay();
        display.setTextColor(1);
        display.setFont(); // Uses default readable font
        
        if (WiFi.status() != WL_CONNECTED) {
          display.setCursor(10, 20);
          display.print("Error: No WiFi!");
          display.setCursor(10, 30);
          display.print("Connect in Menu.");
        } else {
          display.setCursor(0, 10);
          display.print("Pinging google.com..");
          display.display();
          
          HTTPClient http;
          unsigned long start = millis();
          http.begin("http://www.google.com");
          int httpCode = http.GET();
          unsigned long duration = millis() - start;
          
          display.clearDisplay();
          display.setCursor(0, 10);
          if (httpCode > 0) {
            display.print("HTTP Code: ");
            display.print(httpCode);
            display.setCursor(0, 25);
            display.print("Time: ");
            display.print(duration);
            display.print(" ms");
          } else {
            display.print("Ping Failed:");
            display.setCursor(0, 25);
            display.print(http.errorToString(httpCode).c_str());
          }
          http.end();
        }
        display.display();
        delay(3000); // Wait 3 seconds to let user read
        inMenu = false; // return
        while(digitalRead(BTN_SELECT) == LOW) { delay(10); } // Wait for release
      }
    }
    
    prevUp = currUp;
    prevDown = currDown;
    prevSel = currSel;
    
    delay(50);
  }
}
