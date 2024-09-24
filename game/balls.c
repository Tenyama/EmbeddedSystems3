#include "./balls.h"
#include "../kernel/framebf.h"
#include "../kernel/mbox.h" // For mailbox handling
#include "../kernel/menu.h"
#include "../uart/uart1.h"
#include "./ballExplode.h"
#include "./interrupt.h"

#define SCORE_REGION_WIDTH 228
#define SCOREBOARD_BORDER_COLOR 0xFFEA7F7D // Border color for the scoreboard
#define SCOREBOARD_BACKGROUND_COLOR                                            \
  0xFF999999               // Background color for the scoreboard
#define LEVEL_UP_SCORE 100 // Score required to level up

typedef unsigned char uint8_t;
// Player player;
// initPlayer(&player);
// Directions: left, right, up, down, diagonals
int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

// Forward declaration of generateRandomColor()
unsigned int
generateRandomColor() { // Declare the function prototype before its usage
  unsigned int colors[3] = {
      0xFFFFFF00, // Yellow
      0xFF0000FF, // Blue
      0xFFFF0000  // Red
  };

  // Use the system timer to introduce randomness
  unsigned long timerValue;
  asm volatile("mrs %0, cntpct_el0"
               : "=r"(timerValue)); // Read the current timer value

  // Use the timer value to pick a random index (0, 1, or 2)
  unsigned int randomIndex = timerValue % 3;

  // Return the color at the random index
  return colors[randomIndex];
}

// Function to draw a circle given its center, radius, and color
void drawCircle(int centerX, int centerY, int radius, unsigned int color) {
  for (int y = -radius; y <= radius; y++) {
    for (int x = -radius; x <= radius; x++) {
      if (x * x + y * y <=
          radius * radius) { // Circle equation: (x^2 + y^2 <= r^2)
        drawPixelARGB32(centerX + x, centerY + y,
                        color); // Draw pixel at (centerX + x, centerY + y)
      }
    }
  }
}

// Function to draw a ball from a Ball struct
void drawBall(struct Ball ball) {
  drawCircle(ball.centerX, ball.centerY, ball.radius, ball.color);
}
void eraseBall(struct Ball ball) {
  eraseLCircle(ball.centerX, ball.centerY, ball.radius);
}

struct Ball balls[ROWS][COLS];
struct Ball viewableBalls[ROWS][COLS] = {{{0}}};
int rowsOnScreen = 0;

void initializeBalls() {
  int startX = 256; // Start drawing from the 157th pixel (adjust as necessary)
  int spacing = 59; // Set spacing equal to ball diameter (59 pixels)
  int startY = 0;
  for (int row = 0; row < ROWS; row++) {
    startY = row * 59 + 29;
    // Set vertical position based on the row (each row 59 pixels tall)
    for (int i = 0; i < COLS; i++) {
      balls[row][i].centerX = startX + i * spacing;
      // Set horizontal position with spacing
      balls[row][i].centerY = startY; // Set vertical position based on the row
      balls[row][i].radius = 29;      // Radius for a 59x59 ball
      balls[row][i].color = generateRandomColor();
      // Assign a random color (yellow, blue, red)
    }
  }
}

// Function to draw a specific row of balls
void drawRowOfBalls(int row) {
  for (int i = 0; i < COLS; i++) {
    drawBall(viewableBalls[row][i]);
  }
}

struct Ball resetBall() {
  struct Ball newBall = {0, 0, 0, 0};
  // Set all fields to zero (or other default values)
  return newBall;
}
void shiftBallsUp(struct Ball matrix[ROWS][COLS]) {
  int startY = 0;
  for (int i = ROWS - 1; i > 0; i--) {
    startY = i * 59 + 29;
    for (int j = 0; j < COLS; j++) {
      matrix[i][j] = matrix[i - 1][j];
      matrix[i][j].centerY = startY; // Set vertical position based on the
                                     // row
    }
  }
  for (int j = 0; j < COLS; j++) {
    viewableBalls[0][j] = resetBall(); // Set last row to 0 (or any reset value)
  }
}

void copyBallsToScreen() {
  shiftBallsUp(viewableBalls);
  for (int i = 0; i < COLS; i++) {
    viewableBalls[0][i] = balls[rowsOnScreen][i];
    viewableBalls[0][i].centerY = 29;
  }
  rowsOnScreen++;
}

void drawBallsMatrix() {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      drawBall(viewableBalls[i][j]);
    }
  }
}

int getMaxRow(int currentX) {
  // Determine the column based on the ball's X position
  int column = (currentX - 228) / 59;
  if (column > 7) {
    column = 7;
  } else if (column < 0) {
    column = 0;
  }
  for (int row = ROWS - 1; row > 0; row--) {
    if (viewableBalls[row][column].centerX != 0)
      return row + 1;
  }
  return 1;
}
int getMaxRowGame() {
  for (int row = ROWS - 1; row > 0; row--) {
    for (int col = 0; col < COLS; col++) {
      if (viewableBalls[row][col].centerX != 0)
        return row + 1;
    }
  }
  return 1;
}

int checkEmptySpot(int x, int y) {

  // Determine the column based on the ball's X position
  int column = (x - 228) / 59;
  if (column > 7) {
    column = 7;
  } else if (column < 0) {
    column = 0;
  }

  // Calculate the row based on the end_y position
  int row = y / 59;
  if (row < 0) {
    row = 0;
  } else if (row >= ROWS) {
    row = ROWS - 1;
  }

  return viewableBalls[row][column].centerX == 0;
}

// Declare a visited array globally or pass it to the function
int visited[ROWS][COLS];

// Initialize the visited array before starting the depth-first search

void resetVisited() {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      visited[i][j] = 0;
    }
  }
}

int is_valid(int x, int y, int color) {
  // Ensure the position is within bounds and the ball hasn't been visited yet
  // Also check that the color matches the original color
  return x >= 0 && x < ROWS && y >= 0 && y < COLS &&
         viewableBalls[x][y].color == color && !visited[x][y];
}
int depthFirstSearch(int x, int y, int color) {
  // If the ball is not valid (out of bounds or already visited), return
  if (!is_valid(x, y, color)) {

    return 0;
  }
  // Mark this ball as visited in the auxiliary array
  visited[x][y] = 1;
  int count = 1;
  for (int i = 0; i < 4; i++) {
    int new_x = x + dx[i];
    int new_y = y + dy[i];
    count += depthFirstSearch(new_x, new_y, color);
  }
  return count;
}
// Function to check if the placed ball can explode
int check_explosion(int x, int y) {
  unsigned int color = viewableBalls[x][y].color;
  resetVisited(); // Clear the visited array before each check
  int connected_count = depthFirstSearch(x, y, color);
  if (connected_count >= 3) {
    return connected_count;
  }
  return 0;
}

// Function to clear all connected balls of the same color
void clearConnectedBalls(int row, int col, int color) {
  resetVisited();
  if (!is_valid(row, col, color)) {
    return;
  }
  eraseBall(viewableBalls[row][col]);
  // Clear the current ball (set its color to 0)
  drawExplode(viewableBalls[row][col].centerX, viewableBalls[row][col].centerY);
  wait_msec(50);
  clearExplosion(viewableBalls[row][col].centerX,
                 viewableBalls[row][col].centerY);
  viewableBalls[row][col] = resetBall();
  // Recursively clear adjacent balls of the same color
  for (int i = 0; i < 4; i++) {
    int new_row = row + dx[i];
    int new_col = col + dy[i];
    clearConnectedBalls(new_row, new_col, color);
  }
}

// Function to handle the explosion logic
void handleExplosion(int row, int col) {
  unsigned int color =
      viewableBalls[row][col].color; // Store the color before clearing
  int ballsExploded = check_explosion(row, col);
  // Check if the placed ball can cause an explosion
  if (ballsExploded >= 3) {
    // Animate explosion at the coordinates of the center ball
    drawExplode(viewableBalls[row][col].centerX,
                viewableBalls[row][col].centerY);
    wait_msec(100);
    clearExplosion(viewableBalls[row][col].centerX,
                   viewableBalls[row][col].centerY);

    increaseScore(&player, ballsExploded * 10);
    updatePlayerScoreDisplay(&player);
    // Clear the balls involved in the explosion by setting their color to 0
    clearConnectedBalls(row, col, color); // Pass the stored color
    // Redraw the screen to reflect the cleared balls
    drawBallsMatrix();
    updatePlayerScoreDisplay(&player);
    // displayScoreRegion();
  }
}

void registerBall(int end_x, struct Ball ball) {
  // Determine the column based on the ball's X position
  int column = (end_x - 228) / 59;
  if (column > 7) {
    column = 7;
  } else if (column < 0) {
    column = 0;
  }

  // Calculate the row based on the end_y position
  int row = (ball.centerY) / 59;
  if (row < 0) {
    row = 0;
  } else if (row >= ROWS) {
    row = ROWS - 1;
  }
  // Check if the calculated position is already occupied
  if (viewableBalls[row][column].centerX != 0) {
    // If occupied, move up one row
    row++;
    if (row < 0) {
      uart_puts("Error: No available row to register the ball!\n");
      return;
    }
  }
  // Set ball's position based on the row and column
  ball.centerX = 256 + column * 59;
  // Calculate the correct X position based on column
  ball.centerY = row * 59 + 29;

  // Store the ball in the matrix
  viewableBalls[row][column] = ball;

  handleExplosion(row, column);
  drawBallsMatrix();
}

// init players and functions to track score and levels

// Function to initialize a player with a starting score and level
void initPlayer(Player *player) {
  player->score = 0; // Start the player with a score of 0
  player->level = 1; // Start the player at level 1
}

// Function to update the player's score and level display
void updatePlayerScoreDisplay(Player *player) {
  // Initialize the frame buffer (if not already done)

  int height = mBuf[6]; // Get the actual height of the screen

  // Clear the entire scoreboard region on the left side by redrawing the
  // background
  drawRectARGB32(0, 0, SCORE_REGION_WIDTH - 1, height - 1,
                 SCOREBOARD_BACKGROUND_COLOR, 1);
  drawRectARGB32(0, 0, SCORE_REGION_WIDTH - 1, height - 1,
                 SCOREBOARD_BORDER_COLOR, 0);

  // Coordinates for text
  int x = 20;
  int y = 50;
  unsigned int text_color = 0xFFFFFFFF; // White color for the text
  int scale = 2;                        // Text size

  // Redraw "PLAYER SCORE:" and "PLAYER LEVEL:" to refresh the background and
  // text
  draw_string(x, y, "PLAYER SCORE:", text_color, scale);

  // Convert the player's score to a string
  char score_str[12];
  int_to_str(player->score, score_str); // Manually convert score to string
  draw_string(x, y + 40, score_str, text_color, scale);

  // Display the player's level below the score
  draw_string(x, y + 80, "PLAYER LEVEL:", text_color, scale);
  char level_str[12];
  int_to_str(player->level, level_str); // Manually convert level to string
  draw_string(x, y + 120, level_str, text_color, scale);
}

// Function to check if the player has leveled up
void checkLevelUp(Player *player) {

  if (player->score >= 0 && player->level == 0) {
    player->level = 1;
  } else if (player->score >= 150 && player->level == 1) {
    player->level = 2; // Increase player level to 2
    uart_puts("Congratulations! You've leveled up to Level 2!\n");
  } else if (player->score >= 300 && player->level == 2) {
    player->level = 3; // Increase player level to 3
    uart_puts("Congratulations! You've leveled up to Level 3!\n");
  }
}

// Function to increase the player's score by a certain amount
void increaseScore(Player *player, int amount) {
  player->score += amount;
  uart_puts("Score updated: ");
  uart_dec(player->score);
  uart_puts("\n");
  // Check for level up
  checkLevelUp(player);

  // Update the display after score changes
  updatePlayerScoreDisplay(player);
}

// Function to display the scoreboard region on the left-hand side
void displayScoreRegion() {
  // Initialize the frame buffer

  int height = mBuf[6]; // Get the actual height of the screen

  // Draw the background and border for the scoreboard region
  drawRectARGB32(0, 0, SCORE_REGION_WIDTH - 1, height - 1,
                 SCOREBOARD_BACKGROUND_COLOR, 1);
  drawRectARGB32(0, 0, SCORE_REGION_WIDTH - 1, height - 1,
                 SCOREBOARD_BORDER_COLOR, 0);

  // Display the "PLAYER SCORE:" and "PLAYER LEVEL:" labels
  draw_string(20, 50, "PLAYER SCORE:", 0xFFFFFFFF, 2);
  draw_string(20, 130, "PLAYER LEVEL:", 0xFFFFFFFF, 2);
}

int ballTime() {
  switch (player.level) {
  case 1:
    return 7000;
  case 2:
    return 5000;
  case 3:
    return 2000;
  }
  return 5000;
}
