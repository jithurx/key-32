#pragma once
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

#define PONG_PADDLE_H 12
#define PONG_PADDLE_W 2
#define PONG_BALL_SIZE 2

void playPong() {
  int playerY = 26;
  float aiY = 26;
  float ballX = 64;
  float ballY = 32;
  float ballSpeedX = 2.0;
  float ballSpeedY = 1.0;
  
  int playerScore = 0;
  int aiScore = 0;
  
  bool playing = true;
  
  while(playing) {
    // 1. Read Inputs
    if (digitalRead(BTN_UP) == LOW) {
      playerY -= 2;
    }
    if (digitalRead(BTN_DOWN) == LOW) {
      playerY += 2;
    }
    if (digitalRead(BTN_SELECT) == LOW) {
      playing = false;
      delay(200); // debounce exit
    }
    
    // Bounds check player paddle
    if (playerY < 0) playerY = 0;
    if (playerY > 64 - PONG_PADDLE_H) playerY = 64 - PONG_PADDLE_H;
    
    // 2. AI logic
    float aiCenter = aiY + (PONG_PADDLE_H / 2.0);
    if (aiCenter < ballY - 2) aiY += 1.5;
    if (aiCenter > ballY + 2) aiY -= 1.5;
    if (aiY < 0) aiY = 0;
    if (aiY > 64 - PONG_PADDLE_H) aiY = 64 - PONG_PADDLE_H;
    
    // 3. Ball movement
    ballX += ballSpeedX;
    ballY += ballSpeedY;
    
    // Bounce off top/bottom
    if (ballY <= 0 || ballY >= 64 - PONG_BALL_SIZE) {
      ballSpeedY = -ballSpeedY;
    }
    
    // Bounce off player paddle
    if (ballX <= 10 + PONG_PADDLE_W && ballX >= 10 && ballY >= playerY - PONG_BALL_SIZE && ballY <= playerY + PONG_PADDLE_H) {
      ballSpeedX = -ballSpeedX;
      ballX = 10 + PONG_PADDLE_W; // push out
    }
    
    // Bounce off AI paddle
    if (ballX >= 116 - PONG_BALL_SIZE && ballX <= 116 + PONG_PADDLE_W && ballY >= aiY - PONG_BALL_SIZE && ballY <= aiY + PONG_PADDLE_H) {
      ballSpeedX = -ballSpeedX;
      ballX = 116 - PONG_BALL_SIZE; // push out
    }
    
    // Scoring
    if (ballX < 0) {
      aiScore++;
      ballX = 64; ballY = 32;
      ballSpeedX = 2.0;
      delay(500);
    } else if (ballX > 128) {
      playerScore++;
      ballX = 64; ballY = 32;
      ballSpeedX = -2.0;
      delay(500);
    }
    
    // 4. Draw
    display.clearDisplay();
    display.setTextColor(1);
    
    display.setFont(); // standard 5x7 font
    display.setCursor(30, 2);
    display.print(playerScore);
    display.setCursor(90, 2);
    display.print(aiScore);
    
    // Draw net
    for (int i=0; i<64; i+=6) {
      display.drawLine(64, i, 64, i+3, 1);
    }
    
    // Draw paddles
    display.fillRect(10, playerY, PONG_PADDLE_W, PONG_PADDLE_H, 1);
    display.fillRect(116, (int)aiY, PONG_PADDLE_W, PONG_PADDLE_H, 1);
    
    // Draw ball
    display.fillRect((int)ballX, (int)ballY, PONG_BALL_SIZE, PONG_BALL_SIZE, 1);
    
    display.display();
    delay(20);
  }
}
