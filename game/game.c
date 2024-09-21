#include "../kernel/draw.h"
#include "../uart/uart1.h"
#include "./background.h"
#include "./balls.h"
#include "./displayGameFrame.h"
#include "./player.h"
#include "./scoreboard.h"
#include "shooter.h"
void playGame() {
  drawImage(0, 0, myBackground, 700, 800);
  moveShooter();
}
