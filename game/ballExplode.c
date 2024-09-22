#include "./ballExplosion.h"
#include "../kernel/framebf.h"
#include "../kernel/gpio.h"
#include "./background.h"
#include "./shooter.h"
#include "interrupt.h"

#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 800

#define BASE_X ((SCREEN_WIDTH + 228) / 2)
#define BASE_Y SCREEN_HEIGHT


void drawExplode(int x, int y){
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

// Function to clear the ball by drawing the background over it
void deleteBallExplode(int x, int y, int radius) {
    int left = x - radius;
    int top = y - radius;
    int diameter = radius * 2;

    // Redraw the background over the ball's area
    for (int h = 0; h < diameter; h++) {
        for (int w = 0; w < diameter; w++) {
            // Only clear the part of the circle area
            int dist_squared = (w - radius) * (w - radius) + (h - radius) * (h - radius);
            if (dist_squared <= radius * radius) {
                // Get the color from the background image
                unsigned int color = getBackgroundPixelColor(left + w, top + h);
                // Draw that pixel from the background
                drawPixelARGB32(left + w, top + h, color);
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
            unsigned int color = getBackgroundPixelColor(adjusted_x + w, adjusted_y + h);
            // Redraw the background pixel
            drawPixelARGB32(adjusted_x + w, adjusted_y + h, color);
        }
    }
}

void ballExplosion(){
    drawLCircle(300, 400, 29, 0xAABBCCDD, 1);
    uart_puts("\nPress space to shoot: ");
    while (1) {
    char input = uart_getc();
    if(input == ' '){
        uart_puts("\nExplosion Triggered!\n");

        deleteBallExplode(300, 400, 29);
        drawExplode(300, 400);  // Call explosion drawing function
        wait_msec(100);
        clearExplosion(300, 400);
        break;  // Exit loop after explosion (for testing purposes)
    }
}
}