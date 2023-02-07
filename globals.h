/**********************************************************************
  Module: globals.h
  Author: Junseok Lee

  Purpose: Defines and stores globally accessed values and information.

**********************************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H

/**** GAME BOUNDARIES */
#define GAME_ROWS 24
#define GAME_COLS 80
#define TOP_WALL 3
#define FIRST_ROW 0
#define ENEMY_LAST_ROW 14

/**** ENEMY DIMENSIONS */
#define ENEMY_START 75
#define INITIAL_ENEMY_Y 2

#define ENEMY_HEIGHT 2
#define ENEMY_BODY_ANIM_TILES 4
#define ENEMY_DEFAULT_LENGTH 25

#define ENEMY_ANIM_TILES 3
#define ENEMY_ANIM_TICKS 50

#define ENEMY_SPEED_SLOW 20 
#define ENEMY_SPEED_FAST 10

#define LIST_TYPE_HEAD 0
#define LIST_TYPE_BODY 2

#define ENEMY_SPAWN_RATE 1000 //enemy spawn speed in 20ms ticks
#define ENEMY_SINGLE_TICK 1

/**** PLAYER DIMENSIONS */
#define PLAYER_START_ROW 20
#define PLAYER_START_COL 42
#define PLAYER_START_LIVES 3
#define PRINT_LIVES_SIZE 2

#define BOTTOM_END 17
#define LEFT_END 0
#define RIGHT_END 77
#define TOP_END 21

#define PLAYER_ANIM_TILES 3
#define PLAYER_ANIM_TICKS 4 
#define PLAYER_WIDTH 3 
#define PLAYER_HEIGHT 3 

/**** BULLET CONFIG */
#define BULLET_SIZE 1 
#define PLAYER_BULLET_SPEED 4
#define ENEMY_BULLET_SPEED 3

/**** DIRECTION KEYS */
#define LEFT_KEY 'a'
#define RIGHT_KEY 'd'
#define DOWN_KEY 's'
#define UP_KEY 'w'
#define SPACE_KEY ' '
#define QUIT_KEY 'q'

/**** SELECT TIMEOUT VALUES */
#define TIMEOUT_SEC 3
#define TIMEOUT_USEC 0
#define WRITE_FD 1

/**** MESSAGES */
#define QUIT_MSG "quitter"
#define WIN_MSG "YOU DID IT!"
#define LOSE_MSG "GAME OVER..."

char dir; //keyboard input
int gameRunning; //1 if running, 0 if not running

#endif