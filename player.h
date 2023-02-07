/**********************************************************************
  Module: player.h
  Author: Junseok Lee

  Purpose: Manages the player's ship for invaders

**********************************************************************/

#ifndef PLAYER_H
#define PLAYER_H

#include "console.h"
#include "globals.h"
#include "threadwrappers.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <curses.h>
#include <string.h>


/* Enum of player's state */
typedef enum playerState_enum 
{
	GAME,	//game running
	DEAD,	//player dead
	GAMEOVER,	//game over
	WIN		//player win, game end
} playerState; 

/* Represents a player */
typedef struct player_struct 
{
	int startCol;	//starting column of player
	int startRow;	//starting row of player
	playerState state;	//player's current state: GAME,DEAD,GAMEOVER,WIN
	bool running;	//current player is running
	int lives;		//lives the player has
	int row;		//the player's current row
	int col;		//the player's current column
	int prevRow;	//the player's previous row
	int prevCol;	//the player's previous column
	int animTile;	//the player animation tile
	pthread_t thread;	//the player thread
	pthread_mutex_t mutex;	//the player mutex
} player;

/* The thread runs as the player */
void *runPlayerT(void *data);

/* Spawns the player for the first time, called externally
   returns the newly spawned player*/
player* spawnPlayer(int startRow, int startCol, int lives);

/* Moves player to the destination row & column*/
void playerMove(player *f, int dRow, int dCol); 

/* removes a life, ends the game if all lives gone */
void killPlayer(player* p);

/* specifies left,right,up,down to playerMove*/
void keyMove(player * p, char direction); 

/* Points to the player object*/
player * playerPtr;


#endif