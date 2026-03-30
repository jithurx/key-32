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

// Subsystem Modules
#include "games/pong.h"
#include "games/snake.h"
#include "games/tetris.h"
#include "games/invaders.h"
#include "actions/actions.h"
#include "keys/keys.h"
#include "settings/settings.h"
#include "wifi/wifi.h"
#include "wifi/hotspot.h"

// Application State
enum AppState { STATE_BOOTING, STATE_MENU, STATE_PLAYING };
AppState currentState = STATE_BOOTING;
unsigned long bootStartTime = 0;
const unsigned long BOOT_DURATION = 3000;

// Menu State
enum MenuPage { PAGE_MAIN, PAGE_ACTIONS, PAGE_GAMES, PAGE_KEY, PAGE_SETTINGS, PAGE_WIFI, PAGE_HOTSPOT };
MenuPage currentMenu = PAGE_MAIN;
int currentSelection = 0;

const char* menuMain[] = {"ACTIONS", "GAMES", "KEY", "SETTINGS", "WIFI", "HOTSPOT"};
const int numMain = 6;

const char* menuGames[] = {"< Back", "Pong", "Snake", "Tetris", "Invaders"};
const int numGames = 5;

const char* menuDefaultSub[] = {"< Back"};
const int numDefaultSub = 1;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//display functions

void boot_logo() {
  display.drawBitmap(50, 25, image_download_bits, 29, 14, 1);
  display.display();
  delay(2000);
}

void wait_spin(unsigned long duration_ms) {
  unsigned long start = millis();
  while(millis() - start < duration_ms) {
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
  static bool prevUp = HIGH, prevDown = HIGH, prevSel = HIGH;
  
  bool currUp = digitalRead(BTN_UP);
  bool currDown = digitalRead(BTN_DOWN);
  bool currSel = digitalRead(BTN_SELECT);
  
  int maxOptions = (currentMenu == PAGE_MAIN) ? numMain : 
                   (currentMenu == PAGE_GAMES) ? numGames : numDefaultSub;
  
  if (prevUp == HIGH && currUp == LOW) {
    currentSelection--;
    if (currentSelection < 0) currentSelection = maxOptions - 1;
  }
  if (prevDown == HIGH && currDown == LOW) {
    currentSelection++;
    if (currentSelection >= maxOptions) currentSelection = 0;
  }
  
  if (prevSel == HIGH && currSel == LOW) {
    if (currentMenu == PAGE_MAIN) {
      if (currentSelection == 1) {
        currentMenu = PAGE_GAMES;
        currentSelection = 0;
      } else {
        // Direct Action Subsystems dispatch immediately
        if (currentSelection == 0) currentMenu = PAGE_ACTIONS;
        else if (currentSelection == 2) currentMenu = PAGE_KEY;
        else if (currentSelection == 3) currentMenu = PAGE_SETTINGS;
        else if (currentSelection == 4) currentMenu = PAGE_WIFI;
        else if (currentSelection == 5) currentMenu = PAGE_HOTSPOT;
        currentState = STATE_PLAYING;
      }
    } else {
      if (currentSelection == 0) {
        // Back selected
        int prevSelection = 0;
        if (currentMenu == PAGE_ACTIONS) prevSelection = 0;
        else if (currentMenu == PAGE_GAMES) prevSelection = 1;
        else if (currentMenu == PAGE_KEY) prevSelection = 2;
        else if (currentMenu == PAGE_SETTINGS) prevSelection = 3;
        else if (currentMenu == PAGE_WIFI) prevSelection = 4;
        
        currentMenu = PAGE_MAIN;
        currentSelection = prevSelection;
      } else {
        // Dispatch to target Module
        currentState = STATE_PLAYING;
      }
    }
  }
  
  prevUp = currUp;
  prevDown = currDown;
  prevSel = currSel;
}

void drawMenu() {
  display.clearDisplay();
  display.setFont(&Picopixel);
  display.setTextWrap(false);
  
  display.setTextColor(1);
  
  const char** optionsList;
  int maxOptions;
  
  if (currentMenu == PAGE_MAIN) {
    display.setCursor(55, 10);
    display.print("MENU");
    optionsList = menuMain;
    maxOptions = numMain;
  } else if (currentMenu == PAGE_GAMES) {
    display.setCursor(50, 10);
    display.print("GAMES");
    optionsList = menuGames;
    maxOptions = numGames;
  } else if (currentMenu == PAGE_ACTIONS) {
    display.setCursor(45, 10);
    display.print("ACTIONS");
    optionsList = menuDefaultSub;
    maxOptions = numDefaultSub;
  } else if (currentMenu == PAGE_KEY) {
    display.setCursor(55, 10);
    display.print("KEY");
    optionsList = menuDefaultSub;
    maxOptions = numDefaultSub;
  } else if (currentMenu == PAGE_SETTINGS) {
    display.setCursor(40, 10);
    display.print("SETTINGS");
    optionsList = menuDefaultSub;
    maxOptions = numDefaultSub;
  } else if (currentMenu == PAGE_WIFI) {
    display.setCursor(50, 10);
    display.print("WIFI");
    optionsList = menuDefaultSub;
    maxOptions = numDefaultSub;
  }
  
  for (int i = 0; i < maxOptions; i++) {
    int y = 20 + (i * 8);
    
    if (i == currentSelection) {
      display.fillRect(0, y - 6, 128, 8, 1);
      display.setTextColor(0);
    } else {
      display.setTextColor(1);
    }
    
    display.setCursor(10, y);
    display.print(optionsList[i]);
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
  } else if (currentState == STATE_PLAYING) {
    
    // Show spinning star loading animation before game runs
    wait_spin(1000);
    
    // Route to particular subsystem execution logic
    if (currentMenu == PAGE_GAMES) {
      if (currentSelection == 1) playPong();
      else if (currentSelection == 2) playSnake();
      else if (currentSelection == 3) playTetris();
      else if (currentSelection == 4) playInvaders();
    } else if (currentMenu == PAGE_ACTIONS) {
      openActions();
    } else if (currentMenu == PAGE_KEY) {
      openKeys();
    } else if (currentMenu == PAGE_SETTINGS) {
      openSettings();
    } else if (currentMenu == PAGE_WIFI) {
      openWifi();
    } else if (currentMenu == PAGE_HOTSPOT) {
      openHotspot();
    }
    
    // Subsystem execution has finished (return to menu)
    if (currentMenu != PAGE_GAMES) {
      // Return to Main Menu for full screen apps
      if (currentMenu == PAGE_ACTIONS) currentSelection = 0;
      else if (currentMenu == PAGE_KEY) currentSelection = 2;
      else if (currentMenu == PAGE_SETTINGS) currentSelection = 3;
      else if (currentMenu == PAGE_WIFI) currentSelection = 4;
      else if (currentMenu == PAGE_HOTSPOT) currentSelection = 5;
      currentMenu = PAGE_MAIN;
    }
    currentState = STATE_MENU;
  }
}