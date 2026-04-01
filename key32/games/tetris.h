#pragma once
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

#define TETRIS_W 10
#define TETRIS_H 20
#define TETRIS_BLOCK_SIZE 3

const uint16_t TETROMINOES[7][4] = {
  {0x0F00, 0x2222, 0x0F00, 0x2222}, // I
  {0x8E00, 0x6440, 0x0E20, 0x44C0}, // J
  {0x2E00, 0x4460, 0x0E80, 0xC440}, // L
  {0xCC00, 0xCC00, 0xCC00, 0xCC00}, // O
  {0x6C00, 0x4620, 0x6C00, 0x4620}, // S
  {0x4E00, 0x4640, 0x0E40, 0x4C40}, // T
  {0xC600, 0x2640, 0xC600, 0x2640}  // Z
};

struct TetrisGame {
  bool board[TETRIS_H][TETRIS_W];
  int currentPiece, currentRot, currentX, currentY;
  int nextPiece;
  unsigned long lastDropTime;
  int dropDelay;
  int score;
  int linesCleared;
  bool gameOver;

  void spawnPiece() {
    currentPiece = nextPiece;
    nextPiece = random(7);
    currentRot = 0;
    currentX = TETRIS_W / 2 - 2;
    currentY = -1; 
    if (checkCollision(currentX, currentY, currentRot)) {
      gameOver = true;
    }
  }

  void init() {
    for (int y = 0; y < TETRIS_H; y++)
      for (int x = 0; x < TETRIS_W; x++)
        board[y][x] = false;
        
    score = 0;
    linesCleared = 0;
    dropDelay = 500;
    gameOver = false;
    randomSeed(millis() + analogRead(0));
    nextPiece = random(7);
    spawnPiece();
  }

  bool checkCollision(int x, int y, int rot) {
    uint16_t shape = TETROMINOES[currentPiece][rot];
    for (int r = 0; r < 4; r++) {
      for (int c = 0; c < 4; c++) {
        if (shape & (1 << (15 - (r * 4 + c)))) {
          int boardX = x + c;
          int boardY = y + r;
          if (boardX < 0 || boardX >= TETRIS_W || boardY >= TETRIS_H) return true;
          if (boardY >= 0 && board[boardY][boardX]) return true;
        }
      }
    }
    return false;
  }

  void lockPiece() {
    uint16_t shape = TETROMINOES[currentPiece][currentRot];
    for (int r = 0; r < 4; r++) {
      for (int c = 0; c < 4; c++) {
        if (shape & (1 << (15 - (r * 4 + c)))) {
          int boardY = currentY + r;
          int boardX = currentX + c;
          if (boardY >= 0 && boardY < TETRIS_H && boardX >= 0 && boardX < TETRIS_W) {
            board[boardY][boardX] = true;
          }
        }
      }
    }
  }

  void clearLines() {
    int lines = 0;
    for (int y = TETRIS_H - 1; y >= 0; y--) {
      bool full = true;
      for (int x = 0; x < TETRIS_W; x++) {
        if (!board[y][x]) {
          full = false; break;
        }
      }
      if (full) {
        lines++;
        for (int dropY = y; dropY > 0; dropY--) {
          for (int x = 0; x < TETRIS_W; x++) {
            board[dropY][x] = board[dropY - 1][x];
          }
        }
        for (int x = 0; x < TETRIS_W; x++) board[0][x] = false;
        y++; 
      }
    }
    if (lines > 0) {
      if (lines == 1) score += 100;
      else if (lines == 2) score += 300;
      else if (lines == 3) score += 500;
      else if (lines == 4) score += 800;
      linesCleared += lines;
      dropDelay = max(100, 500 - (linesCleared / 10) * 50);
    }
  }

  void draw() {
    display.clearDisplay();
    display.setFont(); // use standard font
    display.setTextColor(1);
    
    int offsetX = 40;
    int offsetY = 2; // centers the 60px playfield on 64px display
    
    // Draw boundary
    display.drawRect(offsetX - 1, offsetY - 1, TETRIS_W * TETRIS_BLOCK_SIZE + 2, TETRIS_H * TETRIS_BLOCK_SIZE + 2, 1);
    
    // Display stored blocks
    for (int y = 0; y < TETRIS_H; y++) {
      for (int x = 0; x < TETRIS_W; x++) {
        if (board[y][x]) {
          display.fillRect(offsetX + x * TETRIS_BLOCK_SIZE, offsetY + y * TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, 1);
        }
      }
    }
    
    // Display current piece
    if (!gameOver) {
      uint16_t shape = TETROMINOES[currentPiece][currentRot];
      for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
          if (shape & (1 << (15 - (r * 4 + c)))) {
            int boardX = currentX + c;
            int boardY = currentY + r;
            if (boardY >= 0) {
              display.fillRect(offsetX + boardX * TETRIS_BLOCK_SIZE, offsetY + boardY * TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, 1);
            }
          }
        }
      }
    }
    
    // Display score UI
    display.setCursor(77, 5);
    display.print("SCR:");
    display.setCursor(77, 15);
    display.print(score);
    
    // Display next piece UI
    display.setCursor(77, 30);
    display.print("NXT:");
    
    uint16_t nextShape = TETROMINOES[nextPiece][0];
    for (int r = 0; r < 4; r++) {
      for (int c = 0; c < 4; c++) {
        if (nextShape & (1 << (15 - (r * 4 + c)))) {
          display.fillRect(77 + c * TETRIS_BLOCK_SIZE, 42 + r * TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, 1);
        }
      }
    }
    
    // Check game over
    if (gameOver) {
      display.fillRect(24, 25, 62, 12, 0); // Black box behind text
      display.drawRect(24, 25, 62, 12, 1); // White border
      display.setCursor(28, 27);
      display.print("GAME OVER"); // 9 chars * 6 pixels = 54 pixels
    }
    
    display.display();
  }
};

void playTetris() {
  TetrisGame game;
  game.init();
  game.lastDropTime = millis();
  
  bool playing = true;
  bool btnLeftPrev = HIGH, btnRightPrev = HIGH, btnUpPrev = HIGH, btnDownPrev = HIGH;
  
  while(playing) {
    bool btnLeft = digitalRead(BTN_LEFT);
    bool btnRight = digitalRead(BTN_RIGHT);
    bool btnUp = digitalRead(BTN_UP);
    bool btnDown = digitalRead(BTN_DOWN);
    bool btnSelect = digitalRead(BTN_SELECT);
    
    if (btnSelect == LOW) {
      playing = false;
      delay(200);
      continue;
    }
    
    if (game.gameOver) {
      if (btnUp == LOW || btnDown == LOW || btnLeft == LOW || btnRight == LOW) {
        game.init();
        game.lastDropTime = millis();
        btnLeftPrev = HIGH; btnRightPrev = HIGH; btnUpPrev = HIGH; btnDownPrev = HIGH;
        delay(300);
      }
      game.draw();
      delay(20);
      continue;
    }
    
    if (btnLeft == LOW && btnLeftPrev == HIGH) {
      if (!game.checkCollision(game.currentX - 1, game.currentY, game.currentRot)) game.currentX--;
    }
    if (btnRight == LOW && btnRightPrev == HIGH) {
      if (!game.checkCollision(game.currentX + 1, game.currentY, game.currentRot)) game.currentX++;
    }
    if (btnUp == LOW && btnUpPrev == HIGH) {
      int nextRot = (game.currentRot + 1) % 4;
      if (!game.checkCollision(game.currentX, game.currentY, nextRot)) {
        game.currentRot = nextRot;
      }
    }
    if (btnDown == LOW) {
      if (millis() - game.lastDropTime > 50) { 
        if (!game.checkCollision(game.currentX, game.currentY + 1, game.currentRot)) {
          game.currentY++;
          game.lastDropTime = millis();
        } else {
          game.lockPiece();
          game.clearLines();
          game.spawnPiece();
        }
      }
    }
    
    // handle auto drop
    if (millis() - game.lastDropTime > game.dropDelay && btnDown == HIGH) {
      if (!game.checkCollision(game.currentX, game.currentY + 1, game.currentRot)) {
        game.currentY++;
      } else {
        game.lockPiece();
        game.clearLines();
        game.spawnPiece();
      }
      game.lastDropTime = millis();
    }
    
    btnLeftPrev = btnLeft;
    btnRightPrev = btnRight;
    btnUpPrev = btnUp;
    btnDownPrev = btnDown;
    
    game.draw();
    delay(20);
  }
}
