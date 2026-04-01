#pragma once
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

#define FLAPPY_BIRD_SIZE 5
#define FLAPPY_GRAVITY 0.3
#define FLAPPY_JUMP -2.8
#define FLAPPY_PIPE_W 10
#define FLAPPY_GAP 30
#define FLAPPY_SPEED 1.5

struct FlappyPipe {
    float x;
    int gapY;
    bool passed;
};

struct FlappyGame {
    float birdY;
    float birdVelocity;
    FlappyPipe pipes[3];
    int score;
    bool gameOver;

    void init() {
        birdY = 32;
        birdVelocity = 0;
        score = 0;
        gameOver = false;
        randomSeed(millis() + analogRead(0));

        // Initial pipes
        for (int i=0; i<3; i++) {
            pipes[i].x = 128 + i * 60; // Spaced by 60 pixels
            pipes[i].gapY = random(10, 64 - FLAPPY_GAP - 10);
            pipes[i].passed = false;
        }
    }

    void update(bool jumped) {
        if (gameOver) return;

        if (jumped) {
            birdVelocity = FLAPPY_JUMP;
        }

        birdVelocity += FLAPPY_GRAVITY;
        birdY += birdVelocity;

        if (birdY < 0) {
            birdY = 0;
            birdVelocity = 0;
        }

        if (birdY > 64 - FLAPPY_BIRD_SIZE) {
            birdY = 64 - FLAPPY_BIRD_SIZE;
            gameOver = true; // hit floor
            return;
        }

        for (int i=0; i<3; i++) {
            pipes[i].x -= FLAPPY_SPEED;

            // Score point
            if (!pipes[i].passed && pipes[i].x < 20 - FLAPPY_PIPE_W) {
                score++;
                pipes[i].passed = true;
            }

            // Pipe reset
            if (pipes[i].x < -FLAPPY_PIPE_W) {
                // Find rightmost pipe
                float maxX = 0;
                for (int j=0; j<3; j++) {
                    if (pipes[j].x > maxX) maxX = pipes[j].x;
                }
                pipes[i].x = maxX + 60;
                pipes[i].gapY = random(10, 64 - FLAPPY_GAP - 10);
                pipes[i].passed = false;
            }

            // Collision
            // Bird is at X=20
            if (20 + FLAPPY_BIRD_SIZE > pipes[i].x && 20 < pipes[i].x + FLAPPY_PIPE_W) {
                // Inside pipe X range
                if (birdY < pipes[i].gapY || birdY + FLAPPY_BIRD_SIZE > pipes[i].gapY + FLAPPY_GAP) {
                    gameOver = true;
                }
            }
        }
    }

    void draw() {
        display.clearDisplay();
        display.setFont();
        display.setTextColor(1);

        // draw bird
        display.fillRect(20, (int)birdY, FLAPPY_BIRD_SIZE, FLAPPY_BIRD_SIZE, 1);

        // draw pipes
        for (int i=0; i<3; i++) {
            if (pipes[i].x < 128 && pipes[i].x > -FLAPPY_PIPE_W) {
                // Top pipe
                display.fillRect((int)pipes[i].x, 0, FLAPPY_PIPE_W, pipes[i].gapY, 1);
                // Bottom pipe
                int bottomY = pipes[i].gapY + FLAPPY_GAP;
                display.fillRect((int)pipes[i].x, bottomY, FLAPPY_PIPE_W, 64 - bottomY, 1);
            }
        }

        // Score
        display.setTextSize(1);
        display.setCursor(2, 2);
        display.print(score);

        if (gameOver) {
            int boxWidth = 66; 
            int boxHeight = 15;
            int boxX = (128 - boxWidth) / 2;
            int boxY = (64 - boxHeight) / 2;
            
            display.fillRect(boxX, boxY, boxWidth, boxHeight, 0);
            display.drawRect(boxX, boxY, boxWidth, boxHeight, 1);
            
            int textX = (128 - 54) / 2;
            display.setCursor(textX, boxY + 4);
            display.print("GAME OVER");
        }

        display.display();
    }
};

void playFlappy() {
    FlappyGame game;
    game.init();
    
    bool playing = true;
    bool btnUpPrev = HIGH;

    while (playing) {
        bool btnUp = digitalRead(BTN_UP);
        bool btnSelect = digitalRead(BTN_SELECT);

        if (btnSelect == LOW) {
            playing = false;
            delay(200);
            continue;
        }

        if (game.gameOver) {
            if (btnUp == LOW) {
                game.init();
                btnUpPrev = HIGH;
                delay(300);
            }
            game.draw();
            delay(20);
            continue;
        }

        bool jump = false;
        if (btnUp == LOW && btnUpPrev == HIGH) {
            jump = true;
        }

        game.update(jump);
        
        btnUpPrev = btnUp;

        game.draw();
        delay(20); // 50 FPS target
    }
}
