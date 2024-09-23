#include "../kernel/draw.h"
#include "../uart/uart1.h"
// #include "./background.h"
#include "./balls.h"
#include "./player.h"
#include "shooter.h"
#include "gameWelcome.h"


#include "ballExplode.h"


void playGame() {
  welcomeGame();
  // drawImage(0, 0, myBackground, 700, 800);
  moveShooter();
  // ballExplosion();
}
