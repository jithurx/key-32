#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Picopixel.h"
#include "image_data.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//display functions

void boot_logo() {
  display.drawBitmap(50, 25, image_download_bits, 29, 14, 1);
  display.display();
  delay(2000);
}

void boot_wait_spin() {
  display.clearDisplay();
  display.drawBitmap(60, 28, image_Pin_star_bits, 7, 7, 1);
  display.drawBitmap(60, 28, image_Layer_2_bits, 7, 7, 0);
  display.display();
  delay(50);
  display.clearDisplay();
  display.drawBitmap(60, 28, image_Pin_star_bits, 7, 7, 1);
  display.drawBitmap(60, 28, image_Layer_3_bits, 7, 7, 0);
  display.display();
  delay(50);
}

void booting_up() {
    static int dots = 1;   // keeps value between calls

    display.clearDisplay();
    display.setTextColor(1);
    display.setTextWrap(false);
    display.setFont(&Picopixel);
    display.setCursor(43, 60);

    display.print("booting up ");

    // print dots
    for (int i = 0; i < dots; i++) {
        display.print(".");
    }

    display.display();

    // update dots (1 → 4 → 1 loop)
    dots++;
    if (dots > 4) dots = 1;

    delay(300);  // controls animation speed
}

//-------------------


void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  boot_logo();
}

void loop() {
  booting_up();
}