#include "../kernel/draw.h"
#include "../kernel/framebf.h"
#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "./background.h"
#include "./balls.h"
#include "./displayGameFrame.h"
#include "./interrupt.h"
#include "./player.h"

#define MIN_ANGLE 0
#define MAX_ANGLE 180
#define ANGLE_STEP 2
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 800
#define PI 3.14159265358979323846

// Shooter's base point (assumed at the center bottom of the screen)
#define BASE_X ((SCREEN_WIDTH + 228) / 2)
#define BASE_Y SCREEN_HEIGHT

// Length of the shooter (the line)
#define SHOOTER_LENGTH 700

// Initialize the shooter's current angle
int shooter_angle = 90; // Start at 90 degrees (straight up)

// Helper function to convert degrees to radians
double degreesToRadians(double degrees) { return degrees * PI / 180.0; }

// Helper function to calculate factorial
long factorial(int n) {
  long result = 1;
  for (int i = 2; i <= n; ++i) {
    result *= i;
  }
  return result;
}

// Sine function using Taylor Series expansion
double get_sine(double degrees) {
  double radians = degreesToRadians(degrees);
  double sine = 0.0;
  double term = radians;
  int sign = 1;

  // Using 5 terms of the Taylor Series
  for (int i = 1; i <= 9; i += 2) {
    sine += sign * term;
    term *= (radians * radians) / ((i + 1) * (i + 2));
    sign = -sign; // Alternate sign
  }

  return sine * 1000;
}

// Cosine function using Taylor Series expansion
double get_cosine(double degrees) {
  double radians = degreesToRadians(degrees);
  double cosine = 0.0;
  double term = 1.0;
  int sign = 1;

  // Using 5 terms of the Taylor Series
  for (int i = 0; i <= 8; i += 2) {
    cosine += sign * term;
    term *= (radians * radians) / ((i + 1) * (i + 2));
    sign = -sign; // Alternate sign
  }

  return cosine * 1000;
}

// Function to calculate the endpoint of the shooter based on the angle
void calculateShooterEndpoint(int base_x, int base_y, int shooter_angle,
                              int *end_x, int *end_y) {
  int sine_val = get_sine(shooter_angle);
  int cosine_val = get_cosine(shooter_angle);
  int shooter_length_dummy = SHOOTER_LENGTH;

  *end_x = base_x + (cosine_val * SHOOTER_LENGTH) / 1000;

  if (*end_x > 700) {
    *end_x = 700;
    float cosine_float = cosine_val / 1000.0;
    shooter_length_dummy = (int)(241 / cosine_float);
  } else if (*end_x < 228) {
    *end_x = 228;

    float cosine_float =
        cosine_val / 1000.0; // Convert to floating point by dividing by 1000
    if (cosine_float < 0) {
      cosine_float = -cosine_float; // Make sure cosine value is positive
    }

    shooter_length_dummy = (int)(241 / cosine_float);
  }

  *end_y = base_y - (sine_val * shooter_length_dummy) / 1000;
}

// Function to erase the shooter by restoring the part of the background it
// covered
void eraseShooter(int base_x, int base_y, int shooter_angle) {
  int sine_val = get_sine(shooter_angle);
  int cosine_val = get_cosine(shooter_angle);

  for (int i = 0; i < SHOOTER_LENGTH; i++) {
    for (int j = -4; j <= 4; j++) {
      int x = (base_x * 1000) + (cosine_val * i - sine_val * j);
      int y = (base_y * 1000) - (sine_val * i + cosine_val * j);

      // Manual rounding: adding 500 to shift toward the nearest integer
      int rounded_x = (x + 500) / 1000;
      int rounded_y = (y + 500) / 1000;

      if (rounded_x >= 0 && rounded_x < 700 && rounded_y >= 0 &&
          rounded_y < 800) {
        drawBackgroundPixel(rounded_x, rounded_y);
      }
      if (rounded_x + 1 < 700)
        drawBackgroundPixel(rounded_x + 1, rounded_y);
      if (rounded_y + 1 < 800)
        drawBackgroundPixel(rounded_x, rounded_y + 1);
    }
  }
}

// Function to draw the shooter at the new position
void drawShooter(int base_x, int base_y, int shooter_angle) {
  int sine_val = get_sine(shooter_angle);
  int cosine_val = get_cosine(shooter_angle);

  for (int i = 0; i < SHOOTER_LENGTH; i++) {
    for (int j = -2; j <= 2; j++) {
      // Calculating x and y based on shooter angle and length
      int x = (base_x * 1000) + (cosine_val * i - sine_val * j);
      int y = (base_y * 1000) - (sine_val * i + cosine_val * j);

      // Manual rounding: adding 500 to shift toward the nearest integer
      int rounded_x = (x + 500) / 1000;
      int rounded_y = (y + 500) / 1000;

      // Checking if the rounded coordinates are within screen boundaries
      if (rounded_x >= 0 && rounded_x < 700 && rounded_y >= 0 &&
          rounded_y < 800) {
        drawPixelARGB32(rounded_x, rounded_y,
                        0x00C00000); // Draw the shooter pixel (red color)
      }

      // Optional: Filling nearby pixels to cover gaps
      if (rounded_x + 1 < 700)
        drawPixelARGB32(rounded_x + 1, rounded_y, 0x00C00000);
      if (rounded_y + 1 < 800)
        drawPixelARGB32(rounded_x, rounded_y + 1, 0x00C00000);
    }
  }
}

int end_x = BASE_X, end_y = BASE_Y;
int reflected_angle;

// Global variables to track if the shooter bounced and the bounce point
int has_bounced = 0;
int bounce_x = 0, bounce_y = 0;

// Function to bounce the shooter off the borders
int bounceShooter() {
  eraseShooter(end_x, end_y, reflected_angle);
  calculateShooterEndpoint(BASE_X, BASE_Y, shooter_angle, &end_x, &end_y);
  uart_dec(end_x);
  uart_puts("  ");
  uart_dec(end_y);

  // Check for border collisions
  if (end_x <= 228 || end_x >= 700) {
    // Shooter hits left or right border, bounce it by mirroring the angle
    reflected_angle = 180 - shooter_angle;
    uart_puts("Shooter bounced off the border!\n");

    // Store bounce point and set has_bounced to 1
    has_bounced = 1;
    bounce_x = end_x;
    bounce_y = end_y;

    drawShooter(end_x, end_y, reflected_angle);
    drawShooter(BASE_X, BASE_Y, shooter_angle);
    eraseShooter(end_x, end_y, shooter_angle);
    return 1;
  }
  return 0;
}
// Function to move the shooter to the left
void move_left() {
  if (shooter_angle + ANGLE_STEP <= MAX_ANGLE) {
    // Erase the previous shooter
    eraseShooter(BASE_X, BASE_Y, shooter_angle);

    // Move the shooter by increasing the angle
    shooter_angle += ANGLE_STEP;
    uart_puts("Shooter moved left\n");

    // Bounce if hitting the borders
    // bounceShooter();
    if (!bounceShooter()) {
      // Draw the updated shooter
      drawShooter(BASE_X, BASE_Y, shooter_angle);
    }
  } else {
    shooter_angle = MAX_ANGLE; // Prevent going above maximum
  }
}

// Function to move the shooter to the right
void move_right() {
  if (shooter_angle - ANGLE_STEP >= MIN_ANGLE) {
    // Erase the previous shooter
    eraseShooter(BASE_X, BASE_Y, shooter_angle);

    // Move the shooter by decreasing the angle
    shooter_angle -= ANGLE_STEP;
    uart_puts("Shooter moved right\n");

    // Bounce if hitting the borders
    // bounceShooter();
    if (!bounceShooter()) {
      // Draw the updated shooter
      drawShooter(BASE_X, BASE_Y, shooter_angle);
    }
  } else {
    shooter_angle = MIN_ANGLE; // Prevent going below minimum
  }
}

void moveBallAlongShooterLine(struct Ball *ball, int shooter_angle,
                              float speed) {
  float currentX = ball->centerX;
  float currentY = ball->centerY;

  // Get sine and cosine values based on the shooter's angle
  int sine_val = get_sine(shooter_angle);
  int cosine_val = get_cosine(shooter_angle);

  // Calculate movement deltas (how much the ball moves per step)
  float deltaX = (cosine_val * speed) / 1000.0;
  float deltaY = -(sine_val * speed) / 1000.0;

  // Keep moving the ball until it hits the screen boundary or bounce point
  while (currentX >= 0 && currentX < SCREEN_WIDTH &&
         currentY > (rowsOnScreen) * 59 + 29 && currentY < SCREEN_HEIGHT) {

    // Erase the ball from its current position
    eraseBall(*ball);

    // Move the ball's position
    currentX += deltaX;
    currentY += deltaY;

    // Check if the ball hits the left border (reverse direction)
    if (currentX <= 256) {
      currentX = 256;   // Keep the ball in bounds
      deltaX = -deltaX; // Reflect the ball by reversing its direction
    }

    // Check if the ball hits the right border (reverse direction)
    if (currentX >= 700) {
      currentX = 699;   // Keep the ball in bounds
      deltaX = -deltaX; // Reflect the ball by reversing its direction
      uart_puts("Reflected at right border\n");
      uart_puts("New deltaX: ");
      uart_dec(deltaX);
      uart_puts("\n");
    }

    // Update ball's center coordinates after applying the reflection logic
    ball->centerX = (int)currentX;
    ball->centerY = (int)currentY;
    // Draw the ball at the new position
    drawBall(*ball);

    // Simulate a short delay (to control ball speed)
    wait_msec(50);
  }
  eraseBall(*ball);
  registerBall(ball->centerX, *ball);
  uart_puts("Ball reached the boundary and stopped.\n");
}

void moveShooter() {
  unsigned int msVal = 5000;
  static unsigned long expiredTime = 0; // declare static to keep value
  register unsigned long f, t;
  asm volatile("mrs %0, cntfrq_el0" : "=r"(f));
  asm volatile("mrs %0, cntpct_el0" : "=r"(t));
  expiredTime = t + f * msVal / 1000;

  initializeBalls();
  copyBallsToScreen();
  drawBallsMatrix();
  copyBallsToScreen();
  drawBallsMatrix();
  copyBallsToScreen();
  drawBallsMatrix();

  struct Ball shooterBall = {BASE_X, 771, 29, generateRandomColor()};
  int ballReady = 1;

  Player player;
  initPlayer(&player);

  uart_puts("\nPress A to move shooter to left: ");
  uart_puts("\nPress D to move shooter to right: ");
  drawShooter(BASE_X, BASE_Y, shooter_angle); // Draw initial shooter
  drawBall(shooterBall);

  while (1) {
    asm volatile("mrs %0, cntpct_el0" : "=r"(t));
    if (t < expiredTime) {
      if (!ballReady) {
        shooterBall.color = generateRandomColor();
        drawBall(shooterBall);
        ballReady = 1;
      }
      char input = uart_getc_game();
      if (input == 'a') {
        move_left();
        drawBall(shooterBall);
        drawBallsMatrix();
      } else if (input == 'd') {
        move_right();
        drawBall(shooterBall);
        drawBallsMatrix();
      } else if (input == ' ') {
        // shootBall(shooterBall, BASE_X, 771, shooter_angle);
        moveBallAlongShooterLine(&shooterBall, shooter_angle, 40);
        // After shooting the ball, reset it for the next shot
        shooterBall.centerX = BASE_X;
        shooterBall.centerY = 771;
        shooterBall.color = generateRandomColor(); // Generate a new random
                                                   // color for the next ball

        // drawBall(shooterBall); // Draw the new ball ready for the next shot
        ballReady = 0;
        drawBallsMatrix();
      } else if (input == 'q') {
        uart_puts("\n");
        break;
      }
      updatePlayerScoreDisplay(&player);
    } else {
      // copyBallsToScreen();
      // drawBallsMatrix();
      expiredTime = t + f * msVal / 1000;
    }
  }
}
