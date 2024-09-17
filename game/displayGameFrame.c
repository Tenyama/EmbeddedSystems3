#include "../kernel/framebf.h"
#include "../kernel/gpio.h"
#include "./gameIntro.h"
#include "./gameLevel.h"

// Display game intro
void displayGameIntro(int x, int y) {
  for (int h = 0; h < gameIntroHeight; h++) {
    for (int w = 0; w < gameIntroWidth; w++) {
      // Get the pixel color
      uint32_t pixelColor = gameIntro[h * gameIntroWidth + w];

      // Check if it's not black (assuming green is 0x00FF00FF in ARGB32)
      if (pixelColor != 0x00000000) {
        // Draw the pixel if it's not black
        drawPixelARGB32(x + w, y + h, pixelColor);
      }
    }
  }
}

// Display game level
void gameLevelDisplay(int x, int y) {
  drawImage(x, y, gameLevelArray[0], gameLevelWidth, gameLevelHeight);
}

void displayShooter(int x, int y) {}
