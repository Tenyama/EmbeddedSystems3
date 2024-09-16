#include "./balls.h"
#include "../kernel/framebf.h"
#include "../uart/uart1.h"
#include "background.h"
typedef unsigned char uint8_t;
#define ROWS 8
#define COLS 8

// Directions: left, right, up, down, diagonals
int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

unsigned int generateRandomColorWithTimer() {
    unsigned int colors[3] = {
        0xFFFFFF00, // Yellow
        0xFF0000FF, // Blue
        0xFFFF0000  // Red
    };
    
    // Use the system counter as a random seed
    unsigned long timerValue;
    asm volatile ("mrs %0, cntpct_el0" : "=r"(timerValue));
    
    // Use the timer value to pick a random index
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

// Array to hold multiple balls
struct Ball balls[ROWS][COLS];

struct Ball viewableBalls[ROWS][COLS] = {{{0}}}; //= balls[1][COLS] interrupt ->
                                                 // viewableBalls[1][COLS] =
int rowsOnScreen = 0;

// Function to initialize balls with random colors and positions, given the row
// number
void initializeBalls() {
  int startX = 257; // Start drawing from the 157th pixel (adjust as necessary)
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
      balls[row][i].color = generateRandomColorWithTimer();
      // Assign a random color (yellow, blue, red)
    }
  }
}

// Function to draw a specific row of balls
void drawRowOfBalls(int row) {
  for (int i = 0; i < COLS; i++) {
    drawBall(viewableBalls[row][i]);
    // Draw each ball at the position specified
    // in initializeBalls()
  }
}

void copyBallsToScreen() {
  for (int i = 0; i < COLS; i++) {
    viewableBalls[rowsOnScreen][i] = balls[rowsOnScreen][i];
    uart_hex(i);
    uart_puts("  ");
  }

  drawRowOfBalls(rowsOnScreen);
}

void drawBallsMatrix() {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      drawBall(balls[i][j]);
    }
  }
}

int is_valid(int x, int y, int color) {
  return x >= 0 && x < ROWS && y >= 0 && y < COLS && balls[x][y].color == color;
}

int depthFirstSearch(int x, int y, int color) {
  if (!is_valid(x, y, color))
    return 0;
  balls[x][y].color = 0;

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
  unsigned int color = balls[x][y].color;
  int connected_count = depthFirstSearch(x, y, color);

  return (connected_count >= 3);
}

//////////////////////////////////////////////////////////////////////////// erase ball and redraw the backgorund /////////////////////////////////////////////////
// Example function to retrieve the color of the background image pixel
unsigned int getBackgroundPixelColor(int x, int y) {
  // Return the color of the pixel at (x, y) from the background image
  // This assumes you have the background image stored in an accessible format.
  return myBackground[y * 700 + x]; // Assuming background is a 1D array
}

// Function to draw a pixel from the background image at a specific position
void drawBackgroundPixel(int x, int y) {
  // Assuming 'background' is an array or buffer that holds the image data
  unsigned int color = getBackgroundPixelColor(x, y); // Fetch pixel from background
  drawPixelARGB32(x, y, color); // Draw that pixel at the specified location
}

// Function to erase the ball by redrawing the part of the background it covered
void eraseBall(int centerX, int centerY) {
  // Loop through the pixels in the ball's area
  for (int y = -29; y <= 29; y++) {
    for (int x = -29; x <= 29; x++) {
      // Check if the pixel is within the circle's bounds
      if (x * x + y * y <= 29 * 29) {
        // Restore the background image pixel at the location where the ball was
        drawBackgroundPixel(centerX + x, centerY + y);
      }
    }
  }
}

// Function to delete a ball at a specific position (x, y)
void deleteBall(int x, int y) {
  if (x >= 0 && x < ROWS && y >= 0 && y < COLS) {
    // Erase the ball's pixels by redrawing the part of the background it covered
    eraseBall(balls[x][y].centerX, balls[x][y].centerY);

    // Optionally, mark the ball as logically deleted by setting radius to 0
    balls[x][y].radius = 0;
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Function to simulate the ball's motion along a line
void shootBallAlongLine(float m, float b, int startX, int startY, float speed) {
    float currentX = startX;
    float currentY = startY;

    int ballRadius = 29;
    unsigned int ballColor = 0xFF0000FF; // Example ball color, blue

    // Draw the initial ball position
    drawCircle((int)currentX, (int)currentY, ballRadius, ballColor);

    // Loop to move the ball along the line
    while (currentY > 0 && currentX > 0 && currentX < 600) { // Stay within screen bounds
        // Erase the previous ball position
        eraseBall((int)currentX, (int)currentY);

        // Update ball's position along the line using y = mx + b
        currentX += speed;   // Move horizontally by 'speed'
        currentY = m * currentX + b;  // Update y based on the line equation
        
        // Draw the ball at the new position
        drawCircle((int)currentX, (int)currentY, ballRadius, ballColor);

        // Simulate a delay between frames (a simple loop delay to simulate time passage)
        for (volatile int i = 0; i < 100000; i++);
    }
}

// Test function to shoot a ball from (236, 771) along a specific line
void testShootBall() {
    float m = -1.2;  // Example slope
    float b = 1054;  // Example y-intercept

    int startX = 236; // Starting x position
    int startY = 771; // Starting y position

    float speed = 0.5; // Speed of ball movement

    shootBallAlongLine(m, b, startX, startY, speed);
}
