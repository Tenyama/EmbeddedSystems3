#ifndef BALLS_H
#define BALLS_H

typedef unsigned char uint8_t;

extern int rowsOnScreen;
// Ball structure to hold ball attributes
struct Ball {
  int centerX;        // X coordinate of the circle center
  int centerY;        // Y coordinate of the circle center
  int radius;         // Radius of the ball, CONSTANT
  unsigned int color; // Color of the ball
};

#define ROWS 16
#define COLS 8

// Array to hold multiple balls
extern struct Ball balls[ROWS][COLS];

// Function declarations
unsigned int generateRandomColor();
void drawCircle(int centerX, int centerY, int radius, unsigned int color);
void drawBall(struct Ball ball);
void eraseBall(struct Ball ball);
void initializeBalls();
void drawRowOfBalls(int row);
void drawBallsMatrix();
void copyBallsToScreen();
void drawAllBalls();
void handleExplosion(int row, int col);
int check_explosion(int x, int y);
void registerBall(int end_x, struct Ball ball);
int getMaxRow(int currentX);
int getMaxRowGame();
int checkEmptySpot(int x, int y);
void resetViewableBalls();

// Define the Player structure
typedef struct {
  int score; // The player's current score
  int level; // The player's current level
} Player;

extern Player player;

// Function declarations
void initPlayer(Player *player);                // Initialize a player
void increaseScore(Player *player, int amount); // Increase the player's score
void decreaseScore(Player *player, int amount); // Decrease the player's score
void updatePlayerScoreDisplay(
    Player *player); // Update the player's score display
void displayScoreRegion();
int ballTime(); // Display the scoreboard region
#endif          // BALLS_H

// fromShooter
