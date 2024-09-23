#include "../uart/uart0.h"
#include "../uart/uart0.h"
#include "./gameIntro.h"
#include "./background.h"
#include "./shooter.h"



#define UART_STATUS_REGISTER (*(volatile unsigned char *)0x40013800)
#define UART_RECEIVE_FLAG (1 << 5)

int uart_input_ready() {
  // Check if bit 5 (receive flag) is set in the UART status register
  return (UART_STATUS_REGISTER & UART_RECEIVE_FLAG) != 0;
}


int gameRunning = 1;
int gamePaused = 0;

void displayGameIntro(int x, int y)
{
  for (int h = 0; h < 800; h++)
  {
    for (int w = 0; w < 700; w++)
    {
      // Get the pixel color
      uint32_t pixelColor = gameIntro[h * 700 + w];

      // Check if it's not black (assuming green is 0x00FF00FF in ARGB32)
      if (pixelColor != 0x00000000)
      {
        // Draw the pixel if it's not black
        drawPixelARGB32(x + w, y + h, pixelColor);
      }
    }
  }
}

void welcomeGame() {
  displayGameIntro(0, 0);  // Display game intro screen
  uart_puts("\nPress space to confirm: ");
  char input = uart_getc();  // Wait for user to press space to start the game
   
  if (input == ' ') {
    drawImage(0, 0, myBackground, 700, 800);  // Start the game
    moveShooter();
    gamePaused = 0;  // Ensure the game is not paused initially
    gameRunning = 1; // Start the game loop
  }
}
