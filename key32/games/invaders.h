#pragma once
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

#define INV_PLAYER_W 8
#define INV_PLAYER_H 4
#define INV_ALIEN_W 6
#define INV_ALIEN_H 4
#define INV_BULLET_W 2
#define INV_BULLET_H 4
#define INV_ALIEN_COLS 6
#define INV_ALIEN_ROWS 3

void playInvaders() {
  bool playing = true;
  bool gameOver = false;
  bool won = false;
  
  int playerX = 60;
  int score = 0;
  int lives = 3;
  
  bool pBulletActive = false;
  int pBulletX = 0;
  int pBulletY = 0;
  
  bool aBombActive = false;
  int aBombX = 0;
  int aBombY = 0;
  
  bool aliens[INV_ALIEN_ROWS][INV_ALIEN_COLS];
  for(int r = 0; r < INV_ALIEN_ROWS; r++) {
    for(int c = 0; c < INV_ALIEN_COLS; c++) {
      aliens[r][c] = true;
    }
  }
  
  int alienGroupX = 10;
  int alienGroupY = 5;
  int alienDir = 2; // px per move
  unsigned long lastAlienMove = 0;
  
  while(playing) {
    display.clearDisplay();
    display.setTextColor(1);
    display.setFont();
    
    // Inputs (Poll Buttons Fast)
    if (digitalRead(BTN_LEFT) == LOW) {
      playerX -= 3;
    }
    if (digitalRead(BTN_RIGHT) == LOW) {
      playerX += 3;
    }
    if (digitalRead(BTN_UP) == LOW && !pBulletActive && !gameOver && !won) {
      pBulletActive = true;
      pBulletX = playerX + (INV_PLAYER_W / 2) - (INV_BULLET_W / 2);
      pBulletY = 64 - INV_PLAYER_H - INV_BULLET_H;
    }
    if (digitalRead(BTN_SELECT) == LOW) {
      playing = false;
      delay(200);
    }
    
    // Bounds check player
    if (playerX < 0) playerX = 0;
    if (playerX > 128 - INV_PLAYER_W) playerX = 128 - INV_PLAYER_W;
    
    if (!gameOver && !won) {
      // Player Bullet Physics
      if (pBulletActive) {
        pBulletY -= 4;
        if (pBulletY < 0) {
          pBulletActive = false;
        } else {
          // Check collision with aliens array mappings
          for(int r = 0; r < INV_ALIEN_ROWS; r++) {
            for(int c = 0; c < INV_ALIEN_COLS; c++) {
              if (aliens[r][c]) {
                int ax = alienGroupX + (c * 12);
                int ay = alienGroupY + (r * 10);
                if (pBulletX + INV_BULLET_W >= ax && pBulletX <= ax + INV_ALIEN_W &&
                    pBulletY + INV_BULLET_H >= ay && pBulletY <= ay + INV_ALIEN_H) {
                  aliens[r][c] = false;
                  pBulletActive = false;
                  score += 10;
                  break; // Only kill one
                }
              }
            }
          }
        }
      }
      
      // Update Aliens Extents & Speeds
      int livingAliens = 0;
      int minCol = INV_ALIEN_COLS, maxCol = 0;
      int maxRow = 0;
      
      for(int r = 0; r < INV_ALIEN_ROWS; r++) {
        for(int c = 0; c < INV_ALIEN_COLS; c++) {
          if (aliens[r][c]) {
            livingAliens++;
            if (c < minCol) minCol = c;
            if (c > maxCol) maxCol = c;
            if (r > maxRow) maxRow = r;
          }
        }
      }
      
      if (livingAliens == 0) {
        won = true;
      } else {
        // Alien swarm movement, physically speed up the delay ticks as memory decreases!
        int groupSpeedDelay = 10 + (livingAliens * 3); 
        if (millis() - lastAlienMove > groupSpeedDelay) {
          lastAlienMove = millis();
          
          int groupMinX = alienGroupX + (minCol * 12);
          int groupMaxX = alienGroupX + (maxCol * 12) + INV_ALIEN_W;
          
          if (groupMaxX + alienDir >= 128 || groupMinX + alienDir <= 0) {
            alienDir = -alienDir;
            alienGroupY += 4; // Shift formation down closer to the player!
          } else {
            alienGroupX += alienDir;
          }
          
          // Randomly spawn an alien bomb over an active randomly chosen column!
          if (!aBombActive && random(100) < 5) { // 5% chance per swarm tick
            int pcol = random(INV_ALIEN_COLS);
            for(int sr = INV_ALIEN_ROWS-1; sr >= 0; sr--) {
              if (aliens[sr][pcol]) {
                aBombActive = true;
                aBombX = alienGroupX + (pcol * 12) + (INV_ALIEN_W / 2) - (INV_BULLET_W / 2);
                aBombY = alienGroupY + (sr * 10) + INV_ALIEN_H;
                break;
              }
            }
          }
          
          // Check if alien block invades the surface
          if (alienGroupY + (maxRow * 10) + INV_ALIEN_H >= 64 - INV_PLAYER_H) {
            lives = 0;
            gameOver = true;
          }
        }
      }
      
      // Alien Bomb Physics & Tracking
      if (aBombActive) {
        aBombY += 3;
        if (aBombY > 64) {
          aBombActive = false;
        } else {
          // Check collision with player ship specifically mapped bounds
          if (aBombX + INV_BULLET_W >= playerX && aBombX <= playerX + INV_PLAYER_W &&
              aBombY + INV_BULLET_H >= 64 - INV_PLAYER_H && aBombY <= 64) {
            aBombActive = false;
            lives--;
            if (lives <= 0) gameOver = true;
            // Simple visual hit feedback via native inversion
            display.invertDisplay(true);
            delay(50);
            display.invertDisplay(false);
          }
        }
      }
    }
    
    // Render Current Frame Layer
    if (gameOver) {
      display.setCursor(35, 20);
      display.print("GAME OVER");
      display.setCursor(35, 30);
      display.print("Score:");
      display.print(score);
    } else if (won) {
      display.setCursor(40, 20);
      display.print("YOU WON!");
      display.setCursor(35, 30);
      display.print("Score:");
      display.print(score);
    } else {
      // Draw Player Ship Base & Turret Nozzle
      display.fillRect(playerX, 64 - INV_PLAYER_H, INV_PLAYER_W, INV_PLAYER_H, 1);
      display.fillRect(playerX + (INV_PLAYER_W/2) - 1, 64 - INV_PLAYER_H - 2, 2, 2, 1);
      
      // Draw Active Aliens in standard array order
      for(int r = 0; r < INV_ALIEN_ROWS; r++) {
        for(int c = 0; c < INV_ALIEN_COLS; c++) {
          if (aliens[r][c]) {
            int ax = alienGroupX + (c * 12);
            int ay = alienGroupY + (r * 10);
            display.fillRect(ax, ay, INV_ALIEN_W, INV_ALIEN_H, 1);
            // Slice out tiny graphical "eyes" rendering black pixel inversions on top!
            display.drawPixel(ax + 1, ay + 1, 0);
            display.drawPixel(ax + 4, ay + 1, 0);
          }
        }
      }
      
      // Draw Live Projectiles
      if (pBulletActive) display.fillRect(pBulletX, pBulletY, INV_BULLET_W, INV_BULLET_H, 1);
      if (aBombActive) display.fillRect(aBombX, aBombY, INV_BULLET_W, INV_BULLET_H, 1);
      
      // Draw Top HUD Overlay Graphics
      display.setCursor(0, 0);
      display.print(score);
      for(int i = 0; i < lives; i++) {
        display.fillRect(124 - (i * 6), 2, 4, 3, 1); // Life ship indicators top-right
      }
    }
    
    display.display();
    delay(20); // 50FPS lock roughly
  }
}
