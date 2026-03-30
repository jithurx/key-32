#pragma once
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

void playPong() {
  display.clearDisplay();
  display.setTextColor(1);
  display.setCursor(10, 30);
  display.print("Pong Loading...");
  display.display();
  delay(2000);
}
