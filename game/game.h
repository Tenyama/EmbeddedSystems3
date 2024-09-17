#ifndef GAME_H
#define GAME_H

#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "framebf.h"
#include "background.h"

// Constants
#define MIN_ANGLE 0
#define MAX_ANGLE 180
#define ANGLE_STEP 10
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 800
#define BASE_X (SCREEN_WIDTH / 2 + 100)
#define BASE_Y SCREEN_HEIGHT
#define SHOOTER_LENGTH 500

// Shooter and ball structs
struct Ball {
    int centerX;
    int centerY;
    int radius;
    unsigned int color;
};

// Function declarations
int get_sine(int angle); 
int get_cosine(int angle);

void calculateShooterEndpoint(int base_x, int base_y, int shooter_angle, int *end_x, int *end_y);

void drawShooter(int base_x, int base_y, int shooter_angle);
void eraseShooter(int base_x, int base_y, int shooter_angle);

void move_left();
void move_right();

void drawBall(struct Ball ball);
void eraseBall(int centerX, int centerY);

void moveBallAlongShooterLine(struct Ball *ball, int shooter_angle, float speed);

void moveShooterAndShootBall();

#endif // GAME_H
