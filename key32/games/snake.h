#pragma once
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

#define SNAKE_W 32
#define SNAKE_H 14
#define SNAKE_BLOCK 4
#define SNAKE_OFFSET_Y 8

struct SnakeGame {
    uint8_t snakeX[400];
    uint8_t snakeY[400];
    int snakeLength;
    int dirX;
    int dirY;
    int lastDirX;
    int lastDirY;
    int foodX;
    int foodY;
    int score;
    bool gameOver;
    unsigned long lastMoveTime;
    int moveDelay;
    bool turnRegistered;

    void spawnFood() {
        bool valid = false;
        while (!valid) {
            foodX = random(SNAKE_W);
            foodY = random(SNAKE_H);
            valid = true;
            for (int i=0; i<snakeLength; i++) {
                if (snakeX[i] == foodX && snakeY[i] == foodY) {
                    valid = false;
                    break;
                }
            }
        }
    }

    void init() {
        snakeLength = 4;
        for (int i=0; i<snakeLength; i++) {
            snakeX[i] = SNAKE_W / 2 - i;
            snakeY[i] = SNAKE_H / 2;
        }
        dirX = 1;
        dirY = 0;
        lastDirX = 1;
        lastDirY = 0;
        turnRegistered = false;
        score = 0;
        moveDelay = 150;
        gameOver = false;
        randomSeed(millis() + analogRead(0));
        spawnFood();
    }

    void update() {
        if (gameOver) return;

        int nextX = snakeX[0] + dirX;
        int nextY = snakeY[0] + dirY;

        // Wall collision
        if (nextX < 0 || nextX >= SNAKE_W || nextY < 0 || nextY >= SNAKE_H) {
            gameOver = true;
            return;
        }

        // Self collision
        for (int i=0; i<snakeLength; i++) {
            if (snakeX[i] == nextX && snakeY[i] == nextY) {
                gameOver = true;
                return;
            }
        }

        int oldTailX = snakeX[snakeLength - 1];
        int oldTailY = snakeY[snakeLength - 1];

        // Move body
        for (int i = snakeLength - 1; i > 0; i--) {
            snakeX[i] = snakeX[i-1];
            snakeY[i] = snakeY[i-1];
        }
        snakeX[0] = nextX;
        snakeY[0] = nextY;

        // Food collision
        if (nextX == foodX && nextY == foodY) {
            if (snakeLength < 400) {
                snakeX[snakeLength] = oldTailX;
                snakeY[snakeLength] = oldTailY;
                snakeLength++;
            }
            score += 10;
            if (moveDelay > 50) {
                moveDelay -= 2; // Speed up
            }
            spawnFood();
        }

        lastDirX = dirX;
        lastDirY = dirY;
        turnRegistered = false;
    }

    void draw() {
        display.clearDisplay();
        display.setFont(); // MUST reset to standard font to avoid alignment issues

        // UI
        display.setTextColor(1);
        display.setTextSize(1);
        display.setCursor(2, 0); 
        display.print("Score: ");
        display.print(score);

        // Frame
        display.drawLine(0, SNAKE_OFFSET_Y - 1, 128, SNAKE_OFFSET_Y - 1, 1);

        // draw food
        display.fillRect(foodX * SNAKE_BLOCK, SNAKE_OFFSET_Y + foodY * SNAKE_BLOCK, SNAKE_BLOCK, SNAKE_BLOCK, 1);

        // draw snake
        for (int i=0; i<snakeLength; i++) {
            display.fillRect(snakeX[i] * SNAKE_BLOCK, SNAKE_OFFSET_Y + snakeY[i] * SNAKE_BLOCK, SNAKE_BLOCK, SNAKE_BLOCK, 1);
        }

        if (gameOver) {
            int boxWidth = 66; 
            int boxHeight = 15;
            int boxX = (128 - boxWidth) / 2;
            int boxY = (64 - boxHeight) / 2;
            
            display.fillRect(boxX, boxY, boxWidth, boxHeight, 0);
            display.drawRect(boxX, boxY, boxWidth, boxHeight, 1);
            
            // 9 chars * 6 pixels = 54 pixels
            int textX = (128 - 54) / 2;
            display.setCursor(textX, boxY + 4);
            display.print("GAME OVER");
        }

        display.display();
    }
};

void playSnake() {
    SnakeGame game;
    game.init();
    game.lastMoveTime = millis();
    
    bool playing = true;
    bool btnLeftPrev = HIGH, btnRightPrev = HIGH, btnUpPrev = HIGH, btnDownPrev = HIGH;

    while (playing) {
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
                game.lastMoveTime = millis();
                btnLeftPrev = HIGH; btnRightPrev = HIGH; btnUpPrev = HIGH; btnDownPrev = HIGH;
                delay(300);
            }
            game.draw();
            delay(20);
            continue;
        }

        if (!game.turnRegistered) {
            if (btnLeft == LOW && btnLeftPrev == HIGH && game.lastDirX != 1) {
                game.dirX = -1; game.dirY = 0;
                game.turnRegistered = true;
            }
            else if (btnRight == LOW && btnRightPrev == HIGH && game.lastDirX != -1) {
                game.dirX = 1; game.dirY = 0;
                game.turnRegistered = true;
            }
            else if (btnUp == LOW && btnUpPrev == HIGH && game.lastDirY != 1) {
                game.dirX = 0; game.dirY = -1;
                game.turnRegistered = true;
            }
            else if (btnDown == LOW && btnDownPrev == HIGH && game.lastDirY != -1) {
                game.dirX = 0; game.dirY = 1;
                game.turnRegistered = true;
            }
        }

        if (millis() - game.lastMoveTime > game.moveDelay) {
            game.update();
            game.lastMoveTime = millis();
        }

        btnLeftPrev = btnLeft;
        btnRightPrev = btnRight;
        btnUpPrev = btnUp;
        btnDownPrev = btnDown;

        game.draw();
        delay(20);
    }
}
