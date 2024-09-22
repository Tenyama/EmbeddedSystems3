#include "../kernel/draw.h"
#include "../kernel/framebf.h"
#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "./background.h"
#include "./balls.h"
#include "./displayGameFrame.h"
#include "./player.h"

#define MIN_ANGLE 0
#define MAX_ANGLE 180
#define ANGLE_STEP 5
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 800

// Shooter's base point (assumed at the center bottom of the screen)
#define BASE_X ((SCREEN_WIDTH + 228) / 2)
#define BASE_Y SCREEN_HEIGHT

// Length of the shooter (the line)
#define SHOOTER_LENGTH 700

// Initialize the shooter's current angle
int shooter_angle = 90; // Start at 90 degrees (straight up)

const int sine_table[37] = {
    0,    87,   173,  258,  342,  422,  500,  573,  642,  707, 
    766,  819,  866,  906,  939,  965,  984,  996,  1000, 
    996,  984,  965,  939,  906,  866,  819,  766,  707,  642,
    573,  500,  422,  342,  258,  173,  87,   0
};

const int cosine_table[37] = {
    1000, 996,  984,  965,  939,  906,  866,  819,  766,  707,
    642,  573,  500,  422,  342,  258,  173,  87,   0, 
    -87,  -173, -258, -342, -422, -500, -573, -642, -707, -766,
    -819, -866, -906, -939, -965, -984, -996, -1000
};

// Function to get sine value from the lookup table based on the angle
int get_sine(int angle) {
  return sine_table[angle / 5]; // Divide by 10 to get the index for the table
}

// Function to get cosine value from the lookup table based on the angle
int get_cosine(int angle) {
  return cosine_table[angle /
                      5]; // Divide by 10 to get the index for the table
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

// Example function to retrieve the color of the background image pixel
unsigned int getBackgroundPixelColor(int x, int y) {
  // Return the color of the pixel at (x, y) from the background image
  return myBackground[y * 700 + x]; // Assuming background is a 1D array
}

// Function to draw a pixel from the background image at a specific position
void drawBackgroundPixel(int x, int y) {
  unsigned int color =
      getBackgroundPixelColor(x, y); // Fetch pixel from background
  drawPixelARGB32(x, y, color); // Draw that pixel at the specified location
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
    for (int j = -4; j <= 4; j++) {
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

int end_x, end_y;
int reflected_angle;

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
    drawShooter(end_x, end_y, reflected_angle);
    drawShooter(BASE_X, BASE_Y, shooter_angle);
    eraseShooter(end_x, end_y, shooter_angle);
    return 1;
  }
  return 0;
}

// int secondEndX;
// int secondEndY;
// int secondReflectedAngle;
// // Function to bounce the shooter off the borders
// int secondBounceShooter() {
//   eraseShooter(end_x, end_y, reflected_angle);
//   calculateShooterEndpoint(end_x, end_y, reflected_angle, &secondEndX, &secondEndY);
//   uart_dec(secondEndX);
//   uart_puts("  ");
//   uart_dec(secondEndY);

//   // Check for border collisions
//   if (secondEndX <= 228 || secondEndX >= 700) {
//     // Shooter hits left or right border, bounce it by mirroring the angle
//     secondReflectedAngle = 180 - reflected_angle;
//     uart_puts("Shooter second bounced off the border!\n");
//     drawShooter(end_x, end_y, reflected_angle);
//     drawShooter(secondEndX, secondEndY, secondReflectedAngle);
//     // eraseShooter(end_x, end_y, shooter_angle);
//     return 1;
//   }
//   return 0;
// }


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
    } /*else {
      // Handle the second bounce
      if (!secondBounceShooter()) {
        // No second bounce, draw the updated shooter after first bounce
        drawShooter(end_x, end_y, reflected_angle);
      }
    }*/
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
    }/*else {
      // Handle the second bounce
      if (!secondBounceShooter()) {
        // No second bounce, draw the updated shooter after first bounce
        drawShooter(end_x, end_y, reflected_angle);
      }
    }*/

  } else {
    shooter_angle = MIN_ANGLE; // Prevent going below minimum
  }
}

void moveShooter() {
  initializeBalls();
  Player player;
  initPlayer(&player);
  copyBallsToScreen();
  uart_puts("\nPress A to move shooter to left: ");
  uart_puts("\nPress D to move shooter to right: ");
  drawShooter(BASE_X, BASE_Y, shooter_angle); // Draw initial shooter
  while (1) {
    char input = uart_getc();
    if (input == 'a') {
      move_left();
      copyBallsToScreen();
    } else if (input == 'd') {
      move_right();
      copyBallsToScreen();
    } else if (input == 'q') {
      uart_puts("\n");
      break;
    }
    updatePlayerScoreDisplay(&player);
  }
}
