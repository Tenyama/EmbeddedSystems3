#ifndef SHOOTER_H
#define SHOOTER_H

#include "./balls.h"
typedef unsigned char uint8_t;

// Screen dimensions
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 800

// Shooter's base point (assumed at the center bottom of the screen)
#define BASE_X ((SCREEN_WIDTH + 228) / 2)
#define BASE_Y SCREEN_HEIGHT

// Shooter movement angle step
#define ANGLE_STEP 5

// Shooter structure definition
typedef struct {
  int angle; // Shooter's angle
} Shooter;

// Enumeration for shooter movement direction
typedef enum {
  LEFT, // Moving left
  RIGHT // Moving right
} Direction;

// Ball movement functions
// void moveBallAlongShooterLine(struct Ball *ball, int shooter_angle, float
// speed); // Move ball along the shooter line
void eraseBall(struct Ball ball); // Erase the ball from the screen

// Shooter functions
void move_left();  // Move shooter to the left
void move_right(); // Move shooter to the right
// void moveShooterAndShootBall();            // Main function to control the
// shooter and ball
void drawShooter(
    int base_x, int base_y,
    int shooter_angle); // Draw the shooter based on its position and angle
void eraseShooter(
    int base_x, int base_y,
    int shooter_angle); // Erase shooter by restoring the background it covers
void calculateShooterEndpoint(
    int base_x, int base_y, int shooter_angle, int *end_x,
    int *end_y);           // Calculate the endpoint of the shooter line
int bounceShooter();       // Handle shooter bouncing off screen borders
int get_sine(int angle);   // Get sine value for the given angle
int get_cosine(int angle); // Get cosine value for the given angle
void set_wait_timer(
    int set,
    unsigned int msVal); // Wait function to simulate ball movement delay
void moveShooter();

#endif // SHOOTER_H
