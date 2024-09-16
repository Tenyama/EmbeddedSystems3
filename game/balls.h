#ifndef BALLS_H
#define BALLS_H

typedef unsigned char uint8_t;

// Ball structure to hold ball attributes
struct Ball {
  int centerX;        // X coordinate of the circle center
  int centerY;        // Y coordinate of the circle center
  int radius;         // Radius of the ball, CONSTANT
  unsigned int color; // Color of the ball
};


// Function declarations
void drawCircle(int centerX, int centerY, int radius, unsigned int color);
void drawBall(struct Ball ball);
void initializeBalls();
void drawRowOfBalls(int row);
void drawBallsMatrix();
void copyBallsToScreen();
void drawAllBalls();
int check_explosion(int x, int y);

#endif // BALLS_H

// fromShooter
