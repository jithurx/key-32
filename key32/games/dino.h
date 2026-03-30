#pragma once
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

#define DINO_W 8
#define DINO_H 12
#define CACTUS_W 6

void playDino() {
  bool playing = true;
  bool gameOver = false;
  
  float dinoY = 50;
  float dinoVY = 0.0;
  bool isJumping = false;
  
  float cactusX = 128;
  int cactusH = 10;
  float gameSpeed = 3.0;
  
  int score = 0;
  unsigned long lastFrame = millis();
  
  while(playing) {
    display.clearDisplay();
    display.setTextColor(1);
    display.setFont(); // Force standard text for UI resets
    
    // Process Inputs
    if (digitalRead(BTN_SELECT) == LOW) {
      playing = false;
      delay(200); // Menu toggle debounce
    }
    
    if (digitalRead(BTN_UP) == LOW && !isJumping && !gameOver) {
      dinoVY = -6.0; // Initial burst velocity upwards
      isJumping = true;
    }
    
    if (gameOver) {
      // Allow rapid replay if they hit UP after crashing
      if (digitalRead(BTN_UP) == LOW) {
        gameOver = false;
        dinoY = 50;
        dinoVY = 0;
        isJumping = false;
        cactusX = 128;
        gameSpeed = 3.0; // Reset momentum
        score = 0;
        delay(200);
      }
    }
    
    if (!gameOver) {
      // Simulate Physics (Gravity vector)
      dinoVY += 0.5; // Constant downward acceleration
      dinoY += dinoVY;
      
      // Stop plunging endlessly through the ground floor
      if (dinoY >= 50) {
        dinoY = 50;
        dinoVY = 0;
        isJumping = false;
      }
      
      // Simulate Global Scroll
      cactusX -= gameSpeed;
      if (cactusX < -CACTUS_W) {
        cactusX = 128 + random(10, 60); // Random horizon staggering
        cactusH = random(8, 16); // Procedurally alter cactus heights
        score += 10;
        
        // Progressively ramp up difficulty speed
        gameSpeed += 0.15; 
        if (gameSpeed > 8.0) gameSpeed = 8.0; // Terminal velocity clamping
      }
      
      // Compute Hitbox Overlaps dynamically
      float dLeft = 20;
      float dRight = 20 + DINO_W;
      float cLeft = cactusX;
      float cRight = cactusX + CACTUS_W;
      
      if (dRight > cLeft && dLeft < cRight) { // If X intersects
        if (dinoY + DINO_H > 62 - cactusH) {  // AND Y intersects
          gameOver = true;
          
          // Flash the screen visually to indicate crash heavily!
          display.invertDisplay(true);
          delay(50);
          display.invertDisplay(false);
        }
      }
    }
    
    // Rasterize Graphics Elements
    if (gameOver) {
      display.setCursor(35, 20);
      display.print("GAME OVER");
      display.setCursor(35, 30);
      display.print("Score:");
      display.print(score);
    } else {
      // Core scenery
      display.drawLine(0, 62, 128, 62, 1); // Floor Horizon
      
      // Dino Body
      display.fillRect(20, (int)dinoY, DINO_W, DINO_H, 1);
      display.drawPixel(20 + 5, (int)dinoY + 2, 0); // Singular cut-out eye
      
      // Dino Leg Animatics (Pseudo tick interpolation)
      if (!isJumping && (score % 4) < 2) {
        display.drawLine(20 + 2, (int)dinoY + DINO_H, 20 + 2, (int)dinoY + DINO_H + 2, 1); // Left leg drop
      } else {
        display.drawLine(20 + 6, (int)dinoY + DINO_H, 20 + 6, (int)dinoY + DINO_H + 2, 1); // Right leg drop
      }
      
      // Obstacle Geometry
      display.fillRect((int)cactusX, 62 - cactusH, CACTUS_W, cactusH, 1);
      
      // HUD Number Tracker
      display.setCursor(0, 0);
      display.print(score);
    }
    
    display.display();
    
    // Engine Tick Locking (~30FPS) to prevent physics runaway
    unsigned long wait = millis() - lastFrame;
    if (wait < 30) delay(30 - wait);
    lastFrame = millis();
  }
}
