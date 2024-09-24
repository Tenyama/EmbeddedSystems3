#include "../kernel/framebf.h"
#include "../uart/uart0.h"
#include "./background.h"
#include "./gameIntro.h"
#include "./shooter.h"

void displayGameIntro(int x, int y) {
  for (int h = 0; h < 800; h++) {
    for (int w = 0; w < 700; w++) {
      // Get the pixel color
      uint32_t pixelColor = gameIntro[h * 700 + w];

      // Check if it's not black (assuming green is 0x00FF00FF in ARGB32)
      if (pixelColor != 0x00000000) {
        // Draw the pixel if it's not black
        drawPixelARGB32(x + w, y + h, pixelColor);
      }
    }
  }
}

void welcomeGame() {
  displayGameIntro(0, 0);
  uart_puts("Press space to confirm\n");
  char input = uart_getc();
  if (input == ' ') {
    drawImage(0, 0, myBackground, 700, 800);
    moveShooter();
  }
}
