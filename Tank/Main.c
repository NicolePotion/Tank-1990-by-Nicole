/// \file
/// \brief Welcome to CS100 Tank!
/// It seems to be a really large project...
/// But don't worry, we do not intend to make it difficult.
/// Enjoy this homework and good luck, OwO!
///
/// Getting started:
/// 1. Compile with `gcc Tank/Main.c`.
/// 2. Run the executable.
/// 3. Move with "WASD" and shoot with "K".
/// 4. Exit with "ESC".

//
//
//
//
//
#include "Game.h"

int main(void) {
  // Uses the current time as a seed for the pseudo-random number generator.
  srand(time(NULL));

  // Configure the game.
  config.fps = 30;
  config.mapSize = (Vec){70, 20};
  config.nEnemies = 4;
  config.nSolids = 5;
  config.nWalls = 8;

  // Other parameter.
  config.EnemyPower = 10; // 0-10  - No shooting when 0;Most frequently shooting when 10.
  // You can show and close the blood by press the 'B'.
  config.EnemyBlood = 7;  // 1-9   - please give a number in the range of 0-9.
  config.PlayerBlood = 9; // 1-9   - please give a number in the range of 0-9.

  // Run the game.
  GameLifecycle();

  return 0;
}
