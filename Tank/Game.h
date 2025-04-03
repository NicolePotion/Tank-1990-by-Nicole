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
  bool alive;
  // int  TankDirection;
} Game;

// The game singleton.
static Game game;


// The keyboard key "ESC".
static const char keyESC = '\033';


// Write some function!!----To help make the things easy.--x
// Input the pos, and the function will tell you whether a tank is standing at the position,if it does,tank->id will be returned.
int Tankcheck(Vec pos){
  for (RegIterator it = RegBegin(regTank); it != RegEnd(regTank); it = RegNext(it)) {
    Tank *tank = RegEntry(regTank, it);
    if(-1 <= Sub(pos,tank->pos).x && Sub(pos,tank->pos).x <= 1 
    && -1 <= Sub(pos,tank->pos).y && Sub(pos,tank->pos).y <= 1 ){
      return tank->id;
    }
  }
  return -1; // -1 will be returned if there is no tank.
}

// Deleting the specific id tank.
void RegDelete_tankid(int id){
  for (RegIterator it = RegBegin(regTank); it != RegEnd(regTank); it = RegNext(it)) {
    Tank *tank = RegEntry(regTank, it);
    if(tank->id == id){
      RegDelete(tank);
    }
  }
}


// Create 3*3 something.
void Creating3by3(int num, char object){
  for (int i = 0; i < num; i++){
    int x = (rand() % (map.size.x - 4)) + 1;
    int y = (rand() % (map.size.y - 4)) + 1;
    bool correct = true;
    
    for(int xi = x; xi < x + 3 ; xi++){
      for(int yi = y; yi < y + 3; yi++){
        Vec pos = {xi,yi};
        if (map.flags[Idx(pos)] != eFlagNone || Tankcheck(pos) != -1)
          correct = false;
      }
    }

    if (correct == true){
      for(int xi = x; xi < x + 3 ; xi++){
        for(int yi = y; yi < y + 3; yi++){
          Vec pos = {xi,yi};
          map.flags[Idx(pos)] = object;
        }
      }
    }
    else{
      i--;
    }
  }
}

void GameEnd(void){
  char* Dead1 = "You are shooted!";
  char* Dead2 = "Game over";
  char* Dead3 = "Press anything to exit";
  int len = strlen(Dead3);
  int startposy = (map.size.y-3)/2;
  int startposx = (map.size.x - len)/2;
  Vec pos;

  for (int y = startposy - 1; y < startposy + 4; ++y)
    for (int x = startposx - 2; x < startposx + len + 2; ++x) {
      Vec pos = {x, y};
      if (Tankcheck(pos) >= 0){
        RegDelete_tankid(Tankcheck(pos));
      }
      Flag flag;
      if (x == startposx - 2 || y == startposy - 1 || x == startposx + len + 1 || y == startposy + 3){
        flag = eFlagSolid;
        map.flags[Idx(pos)] = flag;
        // RdrPutChar(pos, map.flags[Idx(pos)], TK_AUTO_COLOR);
      }
      else{
        flag = eFlagNone;
        map.flags[Idx(pos)] = flag;
        RdrPutChar(pos, map.flags[Idx(pos)], TK_AUTO_COLOR);
      }
    }

  RdrPutString(startposy+2,Dead1, TK_NORMAL);
  RdrPutString(startposy+1, Dead2, TK_NORMAL);
  RdrPutString(startposy, Dead3, TK_RED);
  game.alive = false;

}
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
  game.alive = true;
  
  // Step1: Create the margin.(Alrady written)
  for (int y = 0; y < map.size.y; ++y)
    for (int x = 0; x < map.size.x; ++x) {
      Vec pos = {x, y};

      Flag flag = eFlagNone;
      if (x == 0 || y == 0 || x == map.size.x - 1 || y == map.size.y - 1)
        flag = eFlagSolid;

      map.flags[Idx(pos)] = flag;
    }

  // Step2: Create player's tank.(Totally already written)
  {
    Tank *tank = RegNew(regTank);
    tank->pos = (Vec){2, 2};
    tank->dir = eDirOP;
    tank->color = TK_GREEN;
    // tank->isPlayer = true;
    tank->bullet_cool = 0;
    tank->move_cool = 0;
    tank->isOperate = false;
    tank->id = 0;
  }

  // Step3: Create AI tank. (Do similar things as player's tank.)
  for (int i = 0; i < nEnemies; ++i) {
    Vec pos = RandPos();
    bool correct = true;
    for(int xi = pos.x; xi < pos.x + 3 ; xi++){
      for(int yi = pos.y; yi < pos.y + 3; yi++){
        Vec pos = {xi,yi};
        if (map.flags[Idx(pos)] != eFlagNone || Tankcheck(pos) != -1)
          correct = false;
      }
    }
    if (correct == true){
      Tank *tank = RegNew(regTank);
      tank->pos = pos; 
      tank->dir = (Dir)(Rand(4));
      tank->color = TK_RED; 
      // tank->isPlayer = false;
      tank->bullet_cool = 0;
      tank->move_cool = 0;
      tank->isOperate = true;
      tank->id = i+1;
    }
    else{
      i--;
    }

  }

 // Step4: Create %%%*3lines , a kind of magic solid.
  Creating3by3(nSolids,eFlagSolid);
  
 // Step5: Create ###*3lines , a kind of beautiful wall.  
  Creating3by3(nWalls,eFlagWall);
  
  
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
  if (game.alive == true){
    if (game.keyHit == 'w') {
      tank->dir = eDirOP;
      tank->isOperate = true;
    } 
    else if (game.keyHit == 'd') {
      tank->dir = eDirPO;
      tank->isOperate = true;
    } 
    else if (game.keyHit =='s') {
      tank->dir = eDirON;
      tank->isOperate = true;
    } 
    else if (game.keyHit == 'a') {
      tank->dir = eDirNO;
      tank->isOperate = true;
    } 
    else if (game.keyHit == 'k' && tank->bullet_cool <= 0) {
    tank->bullet_cool = 15; 
    }
  }
  else{
    if (game.keyHit) {
      exit(0);
    }
  }
}


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
    if (tank->id > 0){
      int change = rand() % 4;
      if (change == 0 && tank->move_cool <= 0){
        tank->dir = (Dir)(Rand(4));
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
    // Player's tank:
    if (map.flags[Idx(newPosmiddle)] == eFlagNone && map.flags[Idx(newPosleft)] == eFlagNone 
    && map.flags[Idx(newPosright)] == eFlagNone && tank->isOperate && tank->id == 0 
    && Tankcheck(newPosmiddle) == -1 && Tankcheck(newPosleft) == -1 && Tankcheck(newPosright) == -1) {
      tank->pos = newPos; 
      tank->isOperate = false;
    }

    // AI tank:
    if (map.flags[Idx(newPosmiddle)] == eFlagNone && map.flags[Idx(newPosleft)] == eFlagNone 
    && map.flags[Idx(newPosright)] == eFlagNone && tank->move_cool <= 0 && tank->id > 0
    && Tankcheck(newPosmiddle) == -1 && Tankcheck(newPosleft) == -1 && Tankcheck(newPosright) == -1) {
      tank->pos = newPos; 
      tank->move_cool = 20;
    }
    
    // In every circle, the cooling time of tank moving decrease with the sentence below.
    if (tank->move_cool > 0) {
      --tank->move_cool;
    }

    // If the player tank press K,the bullet inited.
    if (tank->bullet_cool == 15 && tank->id == 0) {
      Bullet *bullet = RegNew(regBullet);
      bullet->pos = tank->pos;
      bullet->dir = tank->dir;
      bullet->color = tank->color;
      // bullet->isPlayer = tank->isPlayer;
      bullet->hit = false;
      bullet->id = tank->id;
    }

    // The AI tank will shoot with a 0.1 possibility, and the init of the bullet write below.
    int shoot = config.EnemyPower == 0 ? 1 : Rand( 100 / config.EnemyPower);
    if (tank->id > 0 && shoot == 0 && tank->bullet_cool <= 0) {
      Bullet *bullet = RegNew(regBullet);
      bullet->pos = tank->pos;
      bullet->dir = tank->dir;
      bullet->color = tank->color;
      // bullet->isPlayer = !tank->isPlayer;
      bullet->hit = false;
      tank->bullet_cool = 15; 
      bullet->id = tank->id;
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
    
    else if (bullet->id == 0 && Tankcheck(newPos) > 0){
      RegDelete_tankid(Tankcheck(newPos));
      bullet->hit = true;
    }

    else if (bullet->id > 0 && Tankcheck(newPos) == 0){
      GameEnd();
      bullet->hit = true;
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
