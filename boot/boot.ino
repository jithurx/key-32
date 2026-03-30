#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Picopixel.h"
#include "image_data.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// ESP32 I2C Pins for SSD1306 OLED
#define I2C_SDA 21
#define I2C_SCL 22

// ESP32 Switch Pins
#define BTN_UP 32
#define BTN_DOWN 33
#define BTN_LEFT 25
#define BTN_RIGHT 26
#define BTN_SELECT 27

// Application State
enum AppState { STATE_BOOTING, STATE_MENU };
AppState currentState = STATE_BOOTING;
unsigned long bootStartTime = 0;
const unsigned long BOOT_DURATION = 3000;

// Menu State
const char* gameOptions[] = {"Pong", "Snake", "Tetris", "Invaders", "Settings"};
const int numOptions = 5;
int currentSelection = 0;

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
  display.drawBitmap(60, 28, image_star_spin_frame1_bits, 7, 7, 0);
  display.display();
  delay(50);
  display.clearDisplay();
  display.drawBitmap(60, 28, image_Pin_star_bits, 7, 7, 1);
  display.drawBitmap(60, 28, image_star_spin_frame2_bits, 7, 7, 0);
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

void handleMenuInput() {
  static bool prevUp = true;
  static bool prevDown = true;
  
  bool currUp = digitalRead(BTN_UP);
  bool currDown = digitalRead(BTN_DOWN);
  
  if (prevUp == HIGH && currUp == LOW) {
    currentSelection--;
    if (currentSelection < 0) currentSelection = numOptions - 1;
  }
  if (prevDown == HIGH && currDown == LOW) {
    currentSelection++;
    if (currentSelection >= numOptions) currentSelection = 0;
  }
  
  prevUp = currUp;
  prevDown = currDown;
}

void drawMenu() {
  display.clearDisplay();
  display.setFont(&Picopixel);
  display.setTextWrap(false);
  
  display.setTextColor(1);
  display.setCursor(55, 10);
  display.print("MENU");
  
  for (int i = 0; i < numOptions; i++) {
    int y = 20 + (i * 8);
    
    // Draw inverted background for selected item
    if (i == currentSelection) {
      display.fillRect(0, y - 6, 128, 8, 1);
      display.setTextColor(0);
    } else {
      display.setTextColor(1);
    }
    
    // Center text slightly over to the right
    display.setCursor(10, y);
    display.print(gameOptions[i]);
  }
  display.display();
}

//-------------------


void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  boot_logo();
  bootStartTime = millis();
}

void loop() {
  if (currentState == STATE_BOOTING) {
    booting_up();
    if (millis() - bootStartTime > BOOT_DURATION) {
      currentState = STATE_MENU;
    }
  } else if (currentState == STATE_MENU) {
    handleMenuInput();
    drawMenu();
    delay(20);
  }
}