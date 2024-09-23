#include "../kernel/framebf.h"
#include "../kernel/gpio.h"
#include "../uart/uart1.h"
#include "./background.h"
#include "./ballExplosion.h"
#include "./shooter.h"
#include "interrupt.h"

#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 800

#define BASE_X ((SCREEN_WIDTH + 228) / 2)
#define BASE_Y SCREEN_HEIGHT

void drawExplode(int x, int y) {
  // Adjust the explosion position so its center aligns with the ball center
  int adjusted_x = x - 200 / 2;
  int adjusted_y = y - 200 / 2;
  for (int h = 0; h < 200; h++) {
    for (int w = 0; w < 200; w++) {
      // Get the pixel color
      uint32_t pixelColor = ballExpode[h * 200 + w];

      // Check if it's not black (assuming green is 0x00FF00FF in ARGB32)
      if (pixelColor != 0x00000000) {
        // Draw the pixel if it's not black
        drawPixelARGB32(adjusted_x + w, adjusted_y + h, pixelColor);
      }
    }
  }
}

// Function to clear the explosion by redrawing the background
void clearExplosion(int x, int y) {
  int adjusted_x = x - 200 / 2;
  int adjusted_y = y - 200 / 2;

  // Redraw the background over the explosion area
  for (int h = 0; h < 200; h++) {
    for (int w = 0; w < 200; w++) {
      // Get the background color
      unsigned int color =
          getBackgroundPixelColor(adjusted_x + w, adjusted_y + h);
      // Redraw the background pixel
      drawPixelARGB32(adjusted_x + w, adjusted_y + h, color);
    }
  }
}
