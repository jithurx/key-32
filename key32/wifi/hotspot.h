#pragma once
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "../local_qrcode.h"
#include "../config.h"

extern Adafruit_SSD1306 display;
extern const GFXfont Picopixel;

void openHotspot() {
  bool inMenu = true;
  bool prevSel = HIGH;
  
  // Wait for SELECT release
  while(digitalRead(BTN_SELECT) == LOW) { delay(10); }

  // Start AP
  WiFi.mode(WIFI_AP);
  delay(100); // Give WiFi driver a moment to allocate its resources
  WiFi.softAP(ap_ssid, ap_password);
  
  // Standard WIFI QR Code string
  String qrData = "WIFI:T:WPA;S:";
  qrData += ap_ssid;
  qrData += ";P:";
  qrData += ap_password;
  qrData += ";;";

  QRCode qrcode;
  // Version 5 creates a 37x37 pixel QR matrix, which safely holds 65 bytes
  // of Lowercase Byte string data (Version 3 only holds 32 bytes and buffer overflows!)
  
  // CRITICAL: We cannot use qrcode_getBufferSize(5) as a Variable-Length-Array (VLA) 
  // along with C++ objects like String in the same scope, or the ESP32 stack frame pointer
  // becomes completely misaligned during destructor execution and triggers a hardware panic!
  // Version 5 requires exactly 172 bytes, so 256 bytes is a very comfortable fixed buffer.
  uint8_t qrcodeData[256];
  qrcode_initText(&qrcode, qrcodeData, 5, 0, qrData.c_str());

  display.clearDisplay();
  
  // We must use scale = 1 since 37 * 2 = 74px, which exceeds the 64px display.
  // 37x37 centered on a 128x64 gives a clean 13px screen border.
  int scale = 1;
  int offsetX = (128 - (qrcode.size * scale)) / 2;
  int offsetY = (64 - (qrcode.size * scale)) / 2;

  // Render QR module blocks iteratively onto the OLED
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        display.fillRect(offsetX + x * scale, offsetY + y * scale, scale, scale, 1);
      }
    }
  }
  
  // Overlay accompanying text instructions along the edges
  display.setTextColor(1);
  display.setFont();
  display.setCursor(0, 0);
  display.print("AP:");
  display.setCursor(0, 10);
  display.print(ap_ssid);
  
  display.setCursor(0, 45);
  display.print("< Back");
  display.setCursor(0, 55);
  display.print("(SELECT)");
  
  display.display();

  // Trap user in display state until they press SELECT
  while(inMenu) {
    bool currSel = digitalRead(BTN_SELECT);
    
    if (prevSel == HIGH && currSel == LOW) {
      WiFi.softAPdisconnect(true);
      WiFi.mode(WIFI_OFF);
      inMenu = false;
    }
    prevSel = currSel;
    delay(50);
  }
  
  // Wait for SELECT release before dispatching to menu layer to prevent phantom touches
  while(digitalRead(BTN_SELECT) == LOW) { delay(10); }
}
