#ifndef VGA_FLAPPY_H
#define VGA_FLAPPY_H

// VGA Controller Register Offsets (from base address)
#define BIRD_Y_OFFSET     0x00
#define PILLAR_X_OFFSET   0x04

// Game constants
#define SCREEN_HEIGHT     480
#define SCREEN_WIDTH      640
#define GRAVITY           1
#define JUMP_STRENGTH     10
#define PILLAR_SPEED      2
#define FRAME_DELAY       30000   // microseconds

#endif
