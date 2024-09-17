#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "framebf.h"
#include "background.h"
#include "balls.h"

#define MIN_ANGLE 0
#define MAX_ANGLE 180
#define ANGLE_STEP 10
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 800

// Shooter's base point (center bottom of the screen)
#define BASE_X (SCREEN_WIDTH / 2 + 100)
#define BASE_Y SCREEN_HEIGHT

// Shooter's length (line length)
#define SHOOTER_LENGTH 500

// Shooter's initial angle (straight up)
int shooter_angle = 90; 

// Lookup tables for sine and cosine values
const int sine_table[19] = {0, 174, 342, 500, 643, 766, 866, 940, 985, 1000, 985, 940, 866, 766, 643, 500, 342, 174, 0};
const int cosine_table[19] = {1000, 985, 940, 866, 766, 643, 500, 342, 174, 0, -174, -342, -500, -643, -766, -866, -940, -985, -1000};

// Function to get sine from lookup table
int get_sine(int angle) {
    return sine_table[angle / 10];
}

// Function to get cosine from lookup table
int get_cosine(int angle) {
    return cosine_table[angle / 10];
}

// Function to calculate the shooter endpoint based on the angle
void calculateShooterEndpoint(int base_x, int base_y, int shooter_angle, int *end_x, int *end_y) {
    int sine_val = get_sine(shooter_angle);
    int cosine_val = get_cosine(shooter_angle);
    *end_x = base_x + (cosine_val * SHOOTER_LENGTH) / 1000;
    *end_y = base_y - (sine_val * SHOOTER_LENGTH) / 1000;
}

// Function to erase the shooter
void eraseShooter(int base_x, int base_y, int shooter_angle) {
    int sine_val = get_sine(shooter_angle);
    int cosine_val = get_cosine(shooter_angle);

    for (int i = 0; i < SHOOTER_LENGTH; i++) {
        for (int j = -4; j <= 4; j++) {
            int x = base_x + (cosine_val * i - sine_val * j) / 1000;
            int y = base_y - (sine_val * i + cosine_val * j) / 1000;

            if (x >= 0 && x < 700 && y >= 0 && y < 800) {
                drawBackgroundPixel(x, y); // Restore background
            }
        }
    }
}

// Function to draw the shooter
void drawShooter(int base_x, int base_y, int shooter_angle) {
    int sine_val = get_sine(shooter_angle);
    int cosine_val = get_cosine(shooter_angle);

    for (int i = 0; i < SHOOTER_LENGTH; i++) {
        for (int j = -4; j <= 4; j++) {
            int x = base_x + (cosine_val * i - sine_val * j) / 1000;
            int y = base_y - (sine_val * i + cosine_val * j) / 1000;

            if (x >= 0 && x < 700 && y >= 0 && y < 800) {
                drawPixelARGB32(x, y, 0x00C00000); // Draw shooter in red
            }
        }
    }
}

// Function to move the shooter to the left
void move_left() {
    if (shooter_angle + ANGLE_STEP <= MAX_ANGLE) {
        eraseShooter(BASE_X, BASE_Y, shooter_angle);
        shooter_angle += ANGLE_STEP;
        drawShooter(BASE_X, BASE_Y, shooter_angle);
    }
}

// Function to move the shooter to the right
void move_right() {
    if (shooter_angle - ANGLE_STEP >= MIN_ANGLE) {
        eraseShooter(BASE_X, BASE_Y, shooter_angle);
        shooter_angle -= ANGLE_STEP;
        drawShooter(BASE_X, BASE_Y, shooter_angle);
    }
}

// Function to move the ball along the shooter's line
void moveBallAlongShooterLine(struct Ball *ball, int shooter_angle, float speed) {
    float currentX = ball->centerX;
    float currentY = ball->centerY;

    int sine_val = get_sine(shooter_angle);
    int cosine_val = get_cosine(shooter_angle);

    float deltaX = (cosine_val * speed) / 1000.0;
    float deltaY = -(sine_val * speed) / 1000.0;

    while (currentX >= 0 && currentX < SCREEN_WIDTH && currentY >= 0 && currentY < SCREEN_HEIGHT) {
        eraseBall((int)currentX, (int)currentY);
        currentX += deltaX;
        currentY += deltaY;
        ball->centerX = (int)currentX;
        ball->centerY = (int)currentY;
        drawBall(*ball);
        set_wait_timer(1, 50);
        set_wait_timer(0, 0);
    }
}

// Main function to control the shooter and ball movement
void moveShooterAndShootBall() {
    uart_puts("\nPress A to move shooter left, D to move right, Space to shoot: ");
    drawShooter(BASE_X, BASE_Y, shooter_angle); // Initial shooter position

    struct Ball myBall;
    myBall.centerX = 236;
    myBall.centerY = 771;
    myBall.radius = 29;
    myBall.color = 0xFF0000FF; // Blue ball

    while (1) {
        char input = uart_getc();
        if (input == 'a') {
            move_left();
        } else if (input == 'd') {
            move_right();
        } else if (input == ' ') {
            // Shoot the ball when space is pressed
            moveBallAlongShooterLine(&myBall, shooter_angle, 0.5);
        }
    }
}
