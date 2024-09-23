#include "./balls.h"
#include "../kernel/framebf.h"
#include "../uart/uart1.h"
typedef unsigned char uint8_t;


// Directions: left, right, up, down, diagonals
int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

// Forward declaration of generateRandomColor()
unsigned int
generateRandomColor()
{ // Declare the function prototype before its usage
  unsigned int colors[3] = {
      0xFFFFFF00, // Yellow
      0xFF0000FF, // Blue
      0xFFFF0000  // Red
  };

  // Use the system timer to introduce randomness
  unsigned long timerValue;
  asm volatile("mrs %0, cntpct_el0" : "=r"(timerValue)); // Read the current timer value

  // Use the timer value to pick a random index (0, 1, or 2)
  unsigned int randomIndex = timerValue % 3;

  // Return the color at the random index
  return colors[randomIndex];
}

// Function to draw a circle given its center, radius, and color
void drawCircle(int centerX, int centerY, int radius, unsigned int color)
{
  for (int y = -radius; y <= radius; y++)
  {
    for (int x = -radius; x <= radius; x++)
    {
      if (x * x + y * y <=
          radius * radius)
      { // Circle equation: (x^2 + y^2 <= r^2)
        drawPixelARGB32(centerX + x, centerY + y,
                        color); // Draw pixel at (centerX + x, centerY + y)
      }
    }
  }
}

// Function to draw a ball from a Ball struct
void drawBall(struct Ball ball)
{
  drawCircle(ball.centerX, ball.centerY, ball.radius, ball.color);
}
void eraseBall(struct Ball ball)
{
  eraseLCircle(ball.centerX, ball.centerY, ball.radius);
}

struct Ball balls[ROWS][COLS];
struct Ball viewableBalls[ROWS][COLS] = {{{0}}};
int rowsOnScreen = 0;

void initializeBalls()
{
  int startX = 256; // Start drawing from the 157th pixel (adjust as necessary)
  int spacing = 59; // Set spacing equal to ball diameter (59 pixels)
  int startY = 0;
  for (int row = 0; row < ROWS; row++)
  {
    startY = row * 59 + 29;
    // Set vertical position based on the row (each row 59 pixels tall)
    for (int i = 0; i < COLS; i++)
    {
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
void drawRowOfBalls(int row)
{
  for (int i = 0; i < COLS; i++)
  {
    drawBall(viewableBalls[row][i]);
  }
}

struct Ball resetBall()
{
  struct Ball newBall = {0, 0, 0, 0};
  // Set all fields to zero (or other default values)
  return newBall;
}
void shiftBallsUp(struct Ball matrix[ROWS][COLS])
{
  int startY = 0;
  for (int i = ROWS - 1; i > 0; i--)
  {
    startY = i * 59 + 29;
    for (int j = 0; j < COLS; j++)
    {
      matrix[i][j] = matrix[i - 1][j];
      matrix[i][j].centerY = startY; // Set vertical position based on the
                                     // row
    }
  }
  for (int j = 0; j < COLS; j++)
  {
    viewableBalls[0][j] = resetBall(); // Set last row to 0 (or any reset value)
  }
}

void copyBallsToScreen()
{
  shiftBallsUp(viewableBalls);
  for (int i = 0; i < COLS; i++)
  {
    viewableBalls[0][i] = balls[rowsOnScreen][i];
    viewableBalls[0][i].centerY = 29;
  }
  rowsOnScreen++;
}

void drawBallsMatrix()
{
  for (int i = 0; i < rowsOnScreen; i++)
  {
    for (int j = 0; j < COLS; j++)
    {
      drawBall(viewableBalls[i][j]);
    }
  }
}

int is_valid(int x, int y, int color)
{
  return x >= 0 && x < ROWS && y >= 0 && y < COLS && balls[x][y].color == color;
}

int depthFirstSearch(int x, int y, int color)
{
  if (!is_valid(x, y, color))
    return 0;
  balls[x][y].color = 0;

  int count = 1;
  for (int i = 0; i < 4; i++)
  {
    int new_x = x + dx[i];
    int new_y = y + dy[i];
    count += depthFirstSearch(new_x, new_y, color);
  }

  return count;
}

// Function to check if the placed ball can explode
int check_explosion(int x, int y)
{
  unsigned int color = balls[x][y].color;
  int connected_count = depthFirstSearch(x, y, color);

  return (connected_count >= 3);
}
