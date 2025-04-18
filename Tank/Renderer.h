#pragma once

/// \file
/// \brief This file introduces a renderer which
/// helps us visualize objects in the scene.

//
//
//
//
//
# include "Scene.h"
# include <string.h>

/// \brief Let the renderer automatically deduce the color.
#define TK_AUTO_COLOR ((const char *)(NULL))

/// \brief The error message for undefined behaviors.
#define TK_INVALID_COLOR "Invalid Color"

/// \example Consequently call these functions to render a frame.
/// ```c
/// RdrClear();
/// // Update objects in the scene here.
/// RdrRender();
/// RdrFlush();
/// ```
///
/// \details The renderer is implemented by
/// imitating the double buffering technique.
/// See https://en.wikipedia.org/wiki/Multiple_buffering for more details.
typedef struct {
  char *csPrev;      // Characters of the previous frame.
  Color *colorsPrev; // Character colors of the previous frame.
  char *cs;          // Characters of the current frame.
  Color *colors;     // Character colors of the current frame.
  bool Bloodprint;
} Renderer;

// The renderer singleton.
static Renderer renderer;

//
//
//
/// \brief Render character `c` at position `pos` with color `color`.
///
/// \example ```c
/// // Explicitly specify the color.
/// RdrPutChar(pos, 'o', TK_RED);
/// // Let the renderer automatically deduce the color.
/// RdrPutChar(pos, eFlagWall, TK_AUTO_COLOR);
/// ```
void RdrPutChar(Vec pos, char c, Color color) {
  renderer.cs[Idx(pos)] = c;

  if (color == TK_AUTO_COLOR) {
    Flag flag = (Flag)c;
    color = flag == eFlagNone    ? TK_NORMAL
            : flag == eFlagSolid ? TK_BLUE
            : flag == eFlagWall  ? TK_WHITE

            : flag == eFlagTank ? TK_INVALID_COLOR
                                : TK_INVALID_COLOR;

  }
  renderer.colors[Idx(pos)] = color;
}

void RdrPutString(int line, char* c, Color color) {
  int len = strlen(c);
  int startposx = (map.size.x - len)/2;
  Vec pos;
  for(int i = startposx; i < startposx + len; i++){
    pos = (Vec){i,line};
    renderer.cs[Idx(pos)] = *c;
    renderer.colors[Idx(pos)] = color;

    c++;
  }
  
}

//
//
//
/// \brief Clear all the objects in the scene from the frame.
void RdrClear(void) {
  
  // Clear tanks.
  for (RegIterator it = RegBegin(regTank); it != RegEnd(regTank); it = RegNext(it)) {
    Tank *tank = RegEntry(regTank, it);
    Vec pos = tank->pos;

    for (int y = -1; y <= 1; ++y){
      for (int x = -1; x <= 1; ++x){
        RdrPutChar(Add(pos, (Vec){x, y}), map.flags[Idx(pos)], TK_AUTO_COLOR);
      }
    }

    for (int x = -1; x <= 1; ++x){
      RdrPutChar(Add(pos, (Vec){x, 2}), map.flags[Idx(Add(pos, (Vec){x, 2}))], TK_AUTO_COLOR);
    }
  }

  // Clear bullets.
  for (RegIterator it = RegBegin(regBullet); it != RegEnd(regBullet); it = RegNext(it)) {
    Bullet *bullet = RegEntry(regBullet, it);
    Vec pos = bullet->pos;
    RdrPutChar(pos, map.flags[Idx(pos)], TK_AUTO_COLOR);
  }
}

/// \brief Render all the objects in the scene to the frame.
void RdrRender() {
  // Render tanks.
  
  for (RegIterator it = RegBegin(regTank); it != RegEnd(regTank); it = RegNext(it)) {
    Tank *tank = RegEntry(regTank, it);
    Vec pos = tank->pos;
    Color color = tank->color;

    // Render the tank with the current direction.
    for (int y = -1; y <= 1; ++y){
      for (int x = -1; x <= 1; ++x){
        RdrPutChar(Add(pos, (Vec){x, y}), Tanklook[tank->dir][1-y][x+1], color);
      }
    }



    // Render the blood at the top of the tank.
    if (renderer.Bloodprint == true){
      char blood_ = '0'+ tank->blood;
      RdrPutChar(Add(pos, (Vec){-1, 2}), '[', color);
      RdrPutChar(Add(pos, (Vec){0, 2}), blood_, color);
      RdrPutChar(Add(pos, (Vec){1, 2}), ']', color);
    }
    
    
  }

  // Render bullets.
  for (RegIterator it = RegBegin(regBullet); it != RegEnd(regBullet); it = RegNext(it)) {
    Bullet *bullet = RegEntry(regBullet, it);
    Vec pos = bullet->pos;
    Color color = bullet->color;

    RdrPutChar(pos, 'o', color);
  }
}

/// \brief Flush the previously rendered frame to screen to
/// make it truly visible.
void RdrFlush(void) {
  char *csPrev = renderer.csPrev;
  Color *colorsPrev = renderer.colorsPrev;
  const char *cs = renderer.cs;
  const Color *colors = renderer.colors;

  for (int y = 0; y < map.size.y; ++y)
    for (int x = 0; x < map.size.x; ++x) {
      Vec pos = {x, y};
      int i = Idx(pos);

      if (cs[i] != csPrev[i] || colors[i] != colorsPrev[i]) {
        MoveCursor(pos);
        printf(TK_TEXT("%c", TK_RUNTIME_COLOR), colors[i], cs[i]);

        csPrev[i] = cs[i];
        colorsPrev[i] = colors[i];
      }
    }
}
