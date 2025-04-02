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
  int  TankDirection;
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

  // Initialize scene.
  RegInit(regTank);
  RegInit(regBullet);

  map.flags = (Flag *)malloc(sizeof(Flag) * map.size.x * map.size.y);
  



  for (int y = 0; y < map.size.y; ++y)
    for (int x = 0; x < map.size.x; ++x) {
      Vec pos = {x, y};

      Flag flag = eFlagNone;
      if (x == 0 || y == 0 || x == map.size.x - 1 || y == map.size.y - 1)
        flag = eFlagSolid;

      map.flags[Idx(pos)] = flag;
    }
  // Vec pos={1,0};
  // printf("%c",map.flags[Idx(pos)]);

  {
    Tank *tank = RegNew(regTank);
    tank->pos = (Vec){2, 2};
    tank->dir = eDirOP;
    tank->color = TK_GREEN;
    tank->isPlayer = true;
    tank->bullet_cool = 0;
    tank->move_cool = 0;
    tank->isOperate = false;
  }
  for (int i = 0; i < nEnemies; ++i) {
    Tank *tank = RegNew(regTank);
    // 随机生成敌方坦克位置，这里简单示例，可根据实际需求调整
    tank->pos = RandPos(); 
    tank->dir = (Dir)(rand() % 4); // 随机方向
    tank->color = TK_RED; // 假设敌方坦克颜色为红色
    tank->isPlayer = false;
    tank->isPlayer = false;
    tank->bullet_cool = 0;
    tank->move_cool = 0;
    tank->isOperate = true;
    
  }
  // 添加不可摧毁的实体   
  for (int i = 0; i < nSolids; i++){
    int x = (rand() % (map.size.x - 4)) + 1;
    int y = (rand() % (map.size.y - 4)) + 1;
    bool correct = true;
    
    for(int xi = x; xi < x + 3 ; xi++){
      for(int yi = y; yi < y + 3; yi++){
        Vec pos = {xi,yi};
        if (map.flags[Idx(pos)] != eFlagNone)
          correct = false;
      }
    }
    if (correct == true){
      for(int xi = x; xi < x + 3 ; xi++){
        for(int yi = y; yi < y + 3; yi++){
          Vec pos = {xi,yi};
          if (map.flags[Idx(pos)] == eFlagNone)
            map.flags[Idx(pos)] = eFlagSolid;
        }
      }
    }
    else{
      i--;
    }
  }
  // 添加墙   
  for (int i = 0; i < nWalls; i++){
    int x = (rand() % (map.size.x - 4)) + 1;
    int y = (rand() % (map.size.y - 4)) + 1;
    bool correct = true;
    
    for(int xi = x; xi < x + 3 ; xi++){
      for(int yi = y; yi < y + 3; yi++){
        Vec pos = {xi,yi};
        if (map.flags[Idx(pos)] != eFlagNone)
          correct = false;
      }
    }
    if (correct == true){
      for(int xi = x; xi < x + 3 ; xi++){
        for(int yi = y; yi < y + 3; yi++){
          Vec pos = {xi,yi};
          if (map.flags[Idx(pos)] == eFlagNone)
            map.flags[Idx(pos)] = eFlagWall;
        }
      }
    }
    else{
      i--;
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
  RdrRender();
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
    tank->isOperate = true;
} else if (game.keyHit == 'd') {
    tank->dir = eDirPO;
    tank->isOperate = true;
} else if (game.keyHit =='s') {
    tank->dir = eDirON;
    tank->isOperate = true;
} else if (game.keyHit == 'a') {
    tank->dir = eDirNO;
    tank->isOperate = true;
} else if (game.keyHit == 'k' && tank->bullet_cool <= 0) {
  // 这里假设后续会根据坦克朝向生成炮弹
  // 先设置冷却时间为15帧
  tank->bullet_cool = 15; 
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
  // Clear all the tanks and bullets.
  RdrClear();

  // Tranverse all the tanks,update there direction and position.
  for (RegIterator it = RegBegin(regTank); it != RegEnd(regTank); it = RegNext(it)) {
    Tank *tank = RegEntry(regTank, it);
    if (!tank->isPlayer){
      int change = rand() % 4;
      if (change == 0 && tank->move_cool <= 0){
        tank->dir = (Dir)(rand() % 4);
      }
    }

    Vec newPos,newPosmiddle,newPosleft,newPosright;
    switch (tank->dir) {
        case eDirOP: // 上
            newPos = Add(tank->pos, (Vec){0, 1});
            newPosmiddle = Add(tank->pos, (Vec){0, 2});
            newPosleft = Add(tank->pos, (Vec){-1, 2});
            newPosright = Add(tank->pos, (Vec){1, 2});
            break;
        case eDirPO: // 右
            newPos = Add(tank->pos, (Vec){1, 0});
            newPosmiddle = Add(tank->pos, (Vec){2, 0});
            newPosleft = Add(tank->pos, (Vec){2, -1});
            newPosright = Add(tank->pos, (Vec){2, 1});
            break;
        case eDirON: // 下
            newPos = Add(tank->pos, (Vec){0, -1});
            newPosmiddle = Add(tank->pos, (Vec){0, -2});
            newPosleft = Add(tank->pos, (Vec){-1, -2});
            newPosright = Add(tank->pos, (Vec){1, -2});
            break;
        case eDirNO: // 左
            newPos = Add(tank->pos, (Vec){-1, 0});
            newPosmiddle = Add(tank->pos, (Vec){-2, 0});
            newPosleft = Add(tank->pos, (Vec){-2, -1});
            newPosright = Add(tank->pos, (Vec){-2, 1});
            break;
        default:
            break;
    }

    // If nothing hold the tank back,then it go forward in its direction.
    if (map.flags[Idx(newPosmiddle)] == eFlagNone && map.flags[Idx(newPosleft)] == eFlagNone 
    && map.flags[Idx(newPosright)] == eFlagNone && tank->isOperate && tank->isPlayer) {
      tank->pos = newPos; 
      tank->isOperate = false;
    }
    if (map.flags[Idx(newPosmiddle)] == eFlagNone && map.flags[Idx(newPosleft)] == eFlagNone 
    && map.flags[Idx(newPosright)] == eFlagNone && tank->move_cool <= 0 && !tank->isPlayer) {
      tank->pos = newPos; 
      tank->move_cool = 20;
    }
    
    // In every circle, the cooling time of tank moving decrease with the sentence below.
    if (tank->move_cool > 0) {
      --tank->move_cool;
    }

    // If the player tank press K,the bullet inited.
    if (tank->bullet_cool == 15 && tank->isPlayer) {
      Bullet *bullet = RegNew(regBullet);
      bullet->pos = tank->pos;
      bullet->dir = tank->dir;
      bullet->color = tank->color;
      bullet->isPlayer = tank->isPlayer;
      bullet->hit = false;
    }

    // The AI tank will shoot with a 0.1 possibility, and the init of the bullet write below.
    int shoot = config.EnemyPower == 0 ? 1 : rand() % ( 100 / config.EnemyPower);
    if (!tank->isPlayer && shoot == 0 && tank->bullet_cool <= 0) {
      Bullet *bullet = RegNew(regBullet);
      bullet->pos = tank->pos;
      bullet->dir = tank->dir;
      bullet->color = tank->color;
      bullet->isPlayer = !tank->isPlayer;
      bullet->hit = false;
      tank->bullet_cool = 15; 
    }

    // In every circle, the cooling time of bullets decrease with the sentence below.
    if (tank->bullet_cool > 0) {
      --tank->bullet_cool;
    }
  }
    
  // Tranverse every bullets, and update their positions.
  for (RegIterator it = RegBegin(regBullet); it != RegEnd(regBullet); it = RegNext(it)) {
    Bullet *bullet = RegEntry(regBullet, it);
    Vec newPos;
    switch (bullet->dir) {
        case eDirOP: // 上
            newPos = Add(bullet->pos, (Vec){0, 1});
            break;
        case eDirPO: // 右
            newPos = Add(bullet->pos, (Vec){1, 0});
            break;
        case eDirON: // 下
            newPos = Add(bullet->pos, (Vec){0, -1});
            break;
        case eDirNO: // 左
            newPos = Add(bullet->pos, (Vec){-1, 0});
            break;
        default:
            break;
    }

    // If the bullet has already hit something, then delete it.
    if (bullet->hit == true) {
      RegDelete(bullet); 
    }

    // Otherwise,if the newPos is eFlagNone,update the bullet to the new position.
    else if (map.flags[Idx(newPos)] == eFlagNone) {
      bullet->pos = newPos; 
    }

    // Otherwise,if the newPos is eFlagWall,a process of hit will happen,
    // including changing the wall to None,update the bullet to new position,and set the bullet->hit to true.
    else if (map.flags[Idx(newPos)] == eFlagWall) {
      map.flags[Idx(newPos)] = eFlagNone; 
      bullet->pos = newPos;
      bullet->hit = true;
    } 

    // Otherwise,delete the bullet for it may encounter something can't pass through.
    else {
      RegDelete(bullet);
    }
    
  }
  
  RdrRender();
  RdrFlush();

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
