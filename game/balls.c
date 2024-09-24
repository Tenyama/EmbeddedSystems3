#include "./balls.h"
#include "../kernel/framebf.h"
#include "../uart/uart1.h"
#include "./ballExplode.h"
#include "./interrupt.h"

typedef unsigned char uint8_t;

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
    uart_puts("BOOM");
    return 1;
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

  // Check if the placed ball can cause an explosion
  if (check_explosion(row, col) == 1) {
    // Animate explosion at the coordinates of the center ball
    drawExplode(viewableBalls[row][col].centerX,
                viewableBalls[row][col].centerY);
    wait_msec(100);
    clearExplosion(viewableBalls[row][col].centerX,
                   viewableBalls[row][col].centerY);
    // Clear the balls involved in the explosion by setting their color to 0
    clearConnectedBalls(row, col, color); // Pass the stored color
    // Redraw the screen to reflect the cleared balls
    drawBallsMatrix();
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
  int row = (ball.centerY - 29) / 59;
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
