#pragma once

/// \file
/// \brief This file contains the game lifecycle and logics.
/// There are 5 important functions:
/// `GameInit`, `GameInput`, `GameUpdate`, `GameTerminate`, and
/// the most important: `GameLifecycle`.
/// Please read the corresponding comments to understand their relationships.

//
//
//
//
//
#include "Config.h"
#include "Renderer.h"

#include <time.h>

typedef struct {
  char keyHit; // The keyboard key hit by the player at this frame.
  int  TankDirection_move;
  int  TankDirection_look;
  int  shotCoolDown;
} Game;

// The game singleton.
static Game game;

// The keyboard key "ESC".
static const char keyESC = '\033';

//
//
//
/// \brief Configure the scene (See `Scene.h`) with `config` (See `Config.h`), and
/// perform initializations including:
/// 1. Terminal setup.
/// 2. Memory allocations.
/// 3. Map and object generations.
/// 4. Rendering of the initialized scene.
///
/// \note This function should be called at the very beginning of `GameLifecycle`.
void GameInit(void) {
  // Setup terminal.
  TermSetupGameEnvironment();
  TermClearScreen();

  // Configure scene.
  map.size = config.mapSize;
  int nEnemies = config.nEnemies;
  int nSolids = config.nSolids;
  int nWalls = config.nWalls;
  game.shotCoolDown = 0;




  // Initialize scene.
  RegInit(regTank);
  RegInit(regBullet);

  map.flags = (Flag *)malloc(sizeof(Flag) * map.size.x * map.size.y);
  

  {
    Tank *tank = RegNew(regTank);
    tank->pos = (Vec){2, 1};
    tank->dir = eDirOP;
    tank->color = TK_GREEN;
    tank->isPlayer = true;
  }

  for (int y = 0; y < map.size.y; ++y)
    for (int x = 0; x < map.size.x; ++x) {
      Vec pos = {x, y};

      Flag flag = eFlagNone;
      if (x == 0 || y == 0 || x == map.size.x - 1 || y == map.size.y - 1)
        flag = eFlagSolid;

      map.flags[Idx(pos)] = flag;
    }
    
  for (int i = 0; i < nWalls; i++){

    int x = (rand() % (map.size.x - 4)) + 2;
    int y = (rand() % (map.size.y - 4)) + 2;
    
    for(int xi = x; xi < x + 3 ; xi++){
      for(int yi = y; yi < y + 3; yi++){
        Vec pos = {xi,yi};
        if (map.flags[Idx(pos)] == eFlagNone)
          map.flags[Idx(pos)] = eFlagWall;

      }


    }
    


  }


  // Initialize renderer.
  renderer.csPrev = (char *)malloc(sizeof(char) * map.size.x * map.size.y);
  renderer.colorsPrev = (Color *)malloc(sizeof(Color) * map.size.x * map.size.y);
  renderer.cs = (char *)malloc(sizeof(char) * map.size.x * map.size.y);
  renderer.colors = (Color *)malloc(sizeof(Color) * map.size.x * map.size.y);

  for (int i = 0; i < map.size.x * map.size.y; ++i) {
    renderer.csPrev[i] = renderer.cs[i] = ' ';
    renderer.colorsPrev[i] = renderer.colors[i] = TK_NORMAL;
  }

  // Render scene.
  for (int y = 0; y < map.size.y; ++y)
    for (int x = 0; x < map.size.x; ++x) {
      Vec pos = {x, y};
      RdrPutChar(pos, map.flags[Idx(pos)], TK_AUTO_COLOR);
    }
  RdrRender(game.TankDirection_look);
  RdrFlush();
}

//
//
//
/// \brief Read input from the player.
///
/// \note This function should be called in the loop of `GameLifecycle` before `GameUpdate`.
void GameInput(void) {
  game.keyHit = kbhit_t() ? (char)getch_t() : '\0';
  RegIterator it = RegBegin(regTank);
  Tank *tank = RegEntry(regTank, it);
  if (game.keyHit == 'w') {
    tank->dir = eDirOP;
    game.TankDirection_move = 0; // 上
    game.TankDirection_look = 0;
} else if (game.keyHit == 'd') {
    tank->dir = eDirPO;
    game.TankDirection_move = 1; // 右
    game.TankDirection_look = 1;
} else if (game.keyHit =='s') {
    tank->dir = eDirON;
    game.TankDirection_move = 2; // 下
    game.TankDirection_look = 2;
} else if (game.keyHit == 'a') {
    tank->dir = eDirNO;
    game.TankDirection_move = 3; // 左
    game.TankDirection_look = 3;
} else if (game.keyHit == 'k' && game.shotCoolDown <= 0) {
  // 这里假设后续会根据坦克朝向生成炮弹
  // 先设置冷却时间为15帧
  game.shotCoolDown = 15; 
}
}

//
//
//
/// \brief Perform all tasks required for a frame update, including:
/// 1. Game logics of `Tank`s, `Bullet`s, etc.
/// 2. Rerendering all objects in the scene.
///
/// \note This function should be called in the loop of `GameLifecycle` after `GameInput`.
void GameUpdate(void) {
  RdrClear();

  // TODO: You may need to delete or add codes here.
  for (RegIterator it = RegBegin(regTank); it != RegEnd(regTank); it = RegNext(it)) {
    Tank *tank = RegEntry(regTank, it);
    // if (tank->pos.y < map.size.y - 3)
    //   ++tank->pos.y;
    switch (game.TankDirection_move) {
      case 0: // 上
          Vec pos1 = {tank->pos.x - 1,tank->pos.y + 2};
          Vec pos2 = {tank->pos.x,tank->pos.y + 2};
          Vec pos3 = {tank->pos.x + 1,tank->pos.y + 2};
          if (map.flags[Idx(pos1)] == eFlagNone 
          && map.flags[Idx(pos2)] == eFlagNone
          && map.flags[Idx(pos3)] == eFlagNone) {
              ++tank->pos.y;
          }break;
      case 1: // 右
          Vec pos4 = {tank->pos.x + 2,tank->pos.y - 1};
          Vec pos5 = {tank->pos.x + 2,tank->pos.y};
          Vec pos6 = {tank->pos.x + 2,tank->pos.y + 1};
          if (map.flags[Idx(pos4)] == eFlagNone 
          && map.flags[Idx(pos5)] == eFlagNone
          && map.flags[Idx(pos6)] == eFlagNone) {
              ++tank->pos.x;
          }break;
          // if (tank->pos.x < map.size.x - 3) {
          //     ++tank->pos.x;
          // }break;
      case 2: // 下
          Vec pos7 = {tank->pos.x - 1,tank->pos.y - 2};
          Vec pos8 = {tank->pos.x,tank->pos.y - 2};
          Vec pos9 = {tank->pos.x + 1,tank->pos.y - 2};
          if (map.flags[Idx(pos7)] == eFlagNone 
          && map.flags[Idx(pos8)] == eFlagNone
          && map.flags[Idx(pos9)] == eFlagNone) {
              --tank->pos.y;
          }break;
          // if (tank->pos.y > 2) {
          //     --tank->pos.y;
          //     // printf("%d",tank->pos.y);
          // }break;
      case 3: // 左
          Vec pos10 = {tank->pos.x - 2,tank->pos.y - 1};
          Vec pos11 = {tank->pos.x - 2,tank->pos.y};
          Vec pos12 = {tank->pos.x - 2,tank->pos.y + 1};
          if (map.flags[Idx(pos10)] == eFlagNone 
          && map.flags[Idx(pos11)] == eFlagNone
          && map.flags[Idx(pos12)] == eFlagNone) {
              --tank->pos.x;
          }break;
          // if (tank->pos.x > 2) {
          //     --tank->pos.x;
          // } break;
      default:
          break;
    }

    


    if (game.shotCoolDown == 15) {
      Bullet *bullet = RegNew(regBullet);
      bullet->pos = tank->pos;
      bullet->dir = tank->dir;
      bullet->color = tank->color;
      bullet->isPlayer = tank->isPlayer;
    }
    if (game.shotCoolDown > 0) {
      --game.shotCoolDown;
    }
    for (RegIterator it = RegBegin(regBullet); it != RegEnd(regBullet); it = RegNext(it)) {
      Bullet *bullet = RegEntry(regBullet, it);
      Vec newPos;
      switch (bullet->dir) {
          case eDirOP: // 上
              newPos = Add(bullet->pos, (Vec){0, 1});
              if (map.flags[Idx(newPos)] == eFlagNone) {
                  // printf("%d",newPos.y);
                  bullet->pos = newPos;
              }
              else if (map.flags[Idx(newPos)] == eFlagWall) {
                map.flags[Idx(newPos)] = eFlagNone;
                // printf("%d",newPos.y);
                RegDelete(bullet);
              } 
              else {
                // printf("%d",newPos.y);
                RegDelete(bullet); // 遇到非空标志位删除炮弹
              }
              break;
          case eDirPO: // 右
              newPos = Add(bullet->pos, (Vec){1, 0});
              if (map.flags[Idx(newPos)] == eFlagNone) {
                bullet->pos = newPos;
              } 
              // if (map.flags[Idx(newPos)] == eFlagWall) {
              //   RegDelete(bullet);
              // } 
              else {
                RegDelete(bullet);
              }
              break;
          case eDirON: // 下
              newPos = Add(bullet->pos, (Vec){0, -1});
              if (map.flags[Idx(newPos)] == eFlagNone) {
                bullet->pos = newPos;
              } 
              // if (map.flags[Idx(newPos)] == eFlagWall) {
              //   RegDelete(bullet);
              // } 
              else {
                RegDelete(bullet);
              }
              break;
          case eDirNO: // 左
              newPos = Add(bullet->pos, (Vec){-1, 0});
              if (map.flags[Idx(newPos)] == eFlagNone) {
                bullet->pos = newPos;
              } 
              // if (map.flags[Idx(newPos)] == eFlagWall) {
              //   RegDelete(bullet);
              // } 
              else {
                RegDelete(bullet);
              }
              break;
          default:
              break;
      }
    }
  }
  
  

  RdrRender(game.TankDirection_look);
  RdrFlush();
  game.TankDirection_move = -1;
}

//
//
//
/// \brief Terminate the game and free all the resources.
///
/// \note This function should be called at the very end of `GameLifecycle`.
void GameTerminate(void) {
  while (RegSize(regTank) > 0)
    RegDelete(RegEntry(regTank, RegBegin(regTank)));

  while (RegSize(regBullet) > 0)
    RegDelete(RegEntry(regBullet, RegBegin(regBullet)));

  free(map.flags);

  free(renderer.csPrev);
  free(renderer.colorsPrev);
  free(renderer.cs);
  free(renderer.colors);

  TermClearScreen();
}

//
//
//
/// \brief Lifecycle of the game, defined by calling the 4 important functions:
/// `GameInit`, `GameInput`, `GameUpdate`, and `GameTerminate`.
///
/// \note This function should be called by `main`.
void GameLifecycle(void) {
  GameInit();

  double frameTime = (double)1000 / (double)config.fps;
  clock_t frameBegin = clock();

  while (true) {
    GameInput();
    if (game.keyHit == keyESC)
      break;

    GameUpdate();

    while (((double)(clock() - frameBegin) / CLOCKS_PER_SEC) * 1000.0 < frameTime - 0.5)
      Daze();
    frameBegin = clock();
  }

  GameTerminate();
}
