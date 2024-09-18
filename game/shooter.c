#include "../kernel/framebf.h"
#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "./background.h"
#include "balls.h"


#define MIN_ANGLE 0
#define MAX_ANGLE 180
#define ANGLE_STEP 10
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 800

// Shooter's base point (assumed at the center bottom of the screen)
#define BASE_X ((SCREEN_WIDTH + 228) / 2)
#define BASE_Y SCREEN_HEIGHT

// Length of the shooter (the line)
#define SHOOTER_LENGTH 500

// Initialize the shooter's current angle
int shooter_angle = 90; // Start at 90 degrees (straight up)

// Lookup tables for sine and cosine values (multiplied by 1000 to avoid float)
const int sine_table[19] = {0, 174, 342, 500, 643, 766, 866, 940, 985, 1000,
                            985, 940, 866, 766, 643, 500, 342, 174, 0};
const int cosine_table[19] = {1000, 985, 940, 866, 766, 643, 500,
                              342, 174, 0, -174, -342, -500, -643,
                              -766, -866, -940, -985, -1000};

// Function to get sine value from the lookup table based on the angle
int get_sine(int angle)
{
  return sine_table[angle / 10]; // Divide by 10 to get the index for the table
}

// Function to get cosine value from the lookup table based on the angle
int get_cosine(int angle)
{
  return cosine_table[angle /
                      10]; // Divide by 10 to get the index for the table
}

// Function to calculate the endpoint of the shooter based on the angle
void calculateShooterEndpoint(int base_x, int base_y, int shooter_angle,
                              int *end_x, int *end_y)
{
  int sine_val = get_sine(shooter_angle);
  int cosine_val = get_cosine(shooter_angle);
  int shooter_length_dummy = SHOOTER_LENGTH;

  *end_x = base_x + (cosine_val * SHOOTER_LENGTH) / 1000;

  if (*end_x > 700)
  {
    *end_x = 700;
    float cosine_float = cosine_val / 1000.0;
    shooter_length_dummy = (int)(241 / cosine_float);
  }
  else if (*end_x < 228)
  {
    *end_x = 228;

    // Convert cosine_val to floating-point and adjust shooter length
    float cosine_float =
        cosine_val / 1000.0; // Convert to floating point by dividing by 1000
    if (cosine_float < 0)
    {
      cosine_float = -cosine_float; // Make sure cosine value is positive
    }

    // Adjust shooter length based on cosine value
    shooter_length_dummy =
        (int)(241 / cosine_float); // Use floating point division, then cast
                                   // back to int
  }

  // Adjust the y-coordinate based on the new shooter length
  *end_y = base_y - (sine_val * shooter_length_dummy) / 1000;
}

// Example function to retrieve the color of the background image pixel
unsigned int getBackgroundPixelColor(int x, int y)
{
  // Return the color of the pixel at (x, y) from the background image
  return myBackground[y * 700 + x]; // Assuming background is a 1D array
}

// Function to draw a pixel from the background image at a specific position
void drawBackgroundPixel(int x, int y)
{
  unsigned int color =
      getBackgroundPixelColor(x, y); // Fetch pixel from background
  drawPixelARGB32(x, y, color);      // Draw that pixel at the specified location
}

// Function to erase the shooter by restoring the part of the background it
// covered
void eraseShooter(int base_x, int base_y, int shooter_angle)
{
  // Calculate sine and cosine for the angle
  int sine_val = get_sine(shooter_angle);
  int cosine_val = get_cosine(shooter_angle);

  // Loop through the length of the shooter
  for (int i = 0; i < SHOOTER_LENGTH; i++)
  {
    // Loop through the width of the shooter (10 pixels)
    for (int j = -4; j <= 4; j++)
    {
      // Calculate the x and y positions for the shooter based on angle and
      // thickness
      int x = base_x + (cosine_val * i - sine_val * j) /
                           1000; // X position considering thickness
      int y = base_y - (sine_val * i + cosine_val * j) /
                           1000; // Y position considering thickness

      // Check bounds before restoring the background
      if (x >= 0 && x < 700 && y >= 0 && y < 800)
      {
        drawBackgroundPixel(x, y); // Restore the pixel from the background
      }
    }
  }
}

// Function to draw the shooter at the new position
void drawShooter(int base_x, int base_y, int shooter_angle)
{
  // Calculate sine and cosine for the angle
  int sine_val = get_sine(shooter_angle);
  int cosine_val = get_cosine(shooter_angle);

  // Loop through the length of the shooter
  for (int i = 0; i < SHOOTER_LENGTH; i++)
  {
    // Loop through the width of the shooter (10 pixels)
    for (int j = -4; j <= 4; j++)
    {
      // Calculate the x and y positions for the shooter based on angle and
      // thickness
      int x = base_x + (cosine_val * i - sine_val * j) /
                           1000; // X position considering thickness
      int y = base_y - (sine_val * i + cosine_val * j) /
                           1000; // Y position considering thickness

      // Check bounds before drawing the shooter
      if (x >= 0 && x < 700 && y >= 0 && y < 800)
      {
        drawPixelARGB32(x, y, 0x00C00000); // Draw the shooter pixel (red color)
      }
    }
  }
}

int end_x, end_y;
int reflected_angle;
// Function to bounce the shooter off the borders
int bounceShooter()
{
  eraseShooter(end_x, end_y, reflected_angle);
  calculateShooterEndpoint(BASE_X, BASE_Y, shooter_angle, &end_x, &end_y);
  uart_dec(end_x);
  uart_puts("  ");
  uart_dec(end_y);

  // Check for border collisions
  if (end_x <= 228 || end_x >= 700)
  {
    // Shooter hits left or right border, bounce it by mirroring the angle
    reflected_angle = 180 - shooter_angle;
    uart_puts("Shooter bounced off the border!\n");
    drawShooter(end_x, end_y, reflected_angle);
    drawShooter(BASE_X, BASE_Y, shooter_angle);
    eraseShooter(end_x, end_y, shooter_angle);
    return 1;
  }
  return 0;
}

// Function to move the shooter to the left
void move_left()
{

  if (shooter_angle + ANGLE_STEP <= MAX_ANGLE)
  {
    // Erase the previous shooter
    eraseShooter(BASE_X, BASE_Y, shooter_angle);

    // Move the shooter by increasing the angle
    shooter_angle += ANGLE_STEP;
    uart_puts("Shooter moved left\n");

    // Bounce if hitting the borders
    // bounceShooter();
    if (!bounceShooter())
    {
      // Draw the updated shooter
      drawShooter(BASE_X, BASE_Y, shooter_angle);
    }
  }
  else
  {
    shooter_angle = MAX_ANGLE; // Prevent going above maximum
  }
}

// Function to move the shooter to the right
void move_right()
{
  if (shooter_angle - ANGLE_STEP >= MIN_ANGLE)
  {
    // Erase the previous shooter
    eraseShooter(BASE_X, BASE_Y, shooter_angle);

    // Move the shooter by decreasing the angle
    shooter_angle -= ANGLE_STEP;
    uart_puts("Shooter moved right\n");

    // Bounce if hitting the borders
    // bounceShooter();
    if (!bounceShooter())
    {
      // Draw the updated shooter
      drawShooter(BASE_X, BASE_Y, shooter_angle);
    }
  }
  else
  {
    shooter_angle = MIN_ANGLE; // Prevent going below minimum
  }
}

void moveShooter()
{
  uart_puts("\nPress A to move shooter to left: ");
  uart_puts("\nPress D to move shooter to right: ");
  drawShooter(BASE_X, BASE_Y, shooter_angle); // Draw initial shooter

  while (1)
  {
    char input = uart_getc();
    if (input == 'a')
    {
      move_left();
    }
    else if (input == 'd')
    {
      move_right();
    }
  }
}
void set_wait_timer(int set, unsigned int msVal)
{
  static unsigned long expiredTime = 0; // declare static to keep value
  register unsigned long r, f, t;

  if (set)
  { /* SET TIMER */
    // Get the current counter frequency (Hz)
    asm volatile("mrs %0, cntfrq_el0" : "=r"(f));

    // Read the current counter
    asm volatile("mrs %0, cntpct_el0" : "=r"(t));

    // Calculate expired time:
    expiredTime = t + f * msVal / 1000;
  }
  else
  { /* WAIT FOR TIMER TO EXPIRE */
    do
    {
      asm volatile("mrs %0, cntpct_el0" : "=r"(r));
    } while (r < expiredTime);
  }
}

// Function to move the ball along the shooter's line
void moveBallAlongShooterLine(struct Ball *ball, int shooter_angle, float speed)
{
  float currentX = ball->centerX;
  float currentY = ball->centerY;

  int sine_val = get_sine(shooter_angle);
  int cosine_val = get_cosine(shooter_angle);

  float deltaX = (cosine_val * speed) / 1000.0;
  float deltaY = -(sine_val * speed) / 1000.0;

  while (currentX >= 0 && currentX < SCREEN_WIDTH && currentY >= 0 && currentY < SCREEN_HEIGHT)
  {
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
void moveShooterAndShootBall()
{
  uart_puts("\nPress A to move shooter left, D to move right, Space to shoot: ");
  drawShooter(BASE_X, BASE_Y, shooter_angle); // Initial shooter position

  struct Ball myBall;
  myBall.centerX = 236;
  myBall.centerY = 771;
  myBall.radius = 29;
  myBall.color = 0xFF0000FF; // Blue ball

  while (1)
  {
    char input = uart_getc();
    if (input == 'a')
    {
      move_left();
    }
    else if (input == 'd')
    {
      move_right();
    }
    else if (input == ' ')
    {
      // Shoot the ball when space is pressed
      moveBallAlongShooterLine(&myBall, shooter_angle, 0.5);
    }
  }
}
