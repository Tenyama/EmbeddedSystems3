#include "../kernel/draw.h"
#include "../uart/uart1.h"
#include "./background.h"
#include "./balls.h"
#include "./displayGameFrame.h"
#include "./player.h"
#include "./scoreboard.h"
void playGame() {
  drawImage(0, 0, myBackground, 700, 800);
  initializeBalls();
  Player player;
  initPlayer(&player);
  // drawBallsMatrix();
  copyBallsToScreen();
  while (1) {
    if (uart_getc() == 'c') {
      increaseScore(&player, 40);
    }
  }
}
