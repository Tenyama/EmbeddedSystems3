#include "../kernel/draw.h"
#include "../uart/uart1.h"
#include "./background.h"
#include "./balls.h"
#include "./displayGameFrame.h"
#include "./player.h"
#include "./shooter.h"

#include "ballExplode.h"

void playGame()
{
  drawImage(0, 0, myBackground, 700, 800);
  unsigned int msVal = 5000;
  static unsigned long expiredTime = 0; // declare static to keep value
  register unsigned long f, t;
  asm volatile("mrs %0, cntfrq_el0" : "=r"(f));
  asm volatile("mrs %0, cntpct_el0" : "=r"(t));
  expiredTime = t + f * msVal / 1000;

  // initializeBalls();
  // copyBallsToScreen();
  // drawBallsMatrix();
  // struct Ball shooterBall = {BASE_X, BASE_Y, 29, generateRandomColor()};
  // Player player;
  // initPlayer(&player);
  // moveShooterAndShootBall();

  moveShooter();

  // ballExplosion();
}
