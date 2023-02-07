/**********************************************************************
  Module: enemy.h
  Author: Junseok Lee

  Purpose: manages the enemy caterpillar thread. Enemy bullets are 
  used (called) by this thread. Handles enemy positioning and enemy
  caterpillar draws. 

**********************************************************************/

#ifndef ENEMY_H
#define ENEMY_H




#include "console.h"
#include "globals.h"
#include "enemyBullet.h"
#include "threadwrappers.h"
#include "enemyBulletList.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <curses.h>
#include <string.h>



/* Enum, Represents an enemy state */
typedef enum enemyState_enum 
{
    START_E, //enemy start
	LEFT_E,	//enemy moving left
    RIGHT_E,  //enemy moving right
	DEAD_E,	//enemy dead
    WRAPAROUND_L,  //enemy wrapping around LHS
    WRAPAROUND_R,  //enemy wrapping around RHS
    SPLIT	//enemy splitting 
} enemyState; 

/* Represents an enemy */
typedef struct enemy_struct 
{
    int length;	//enemy length
	int startCol; //enemy starting column
	int startRow;	//enemy starting row
    int index; //list index, 0 = head, 1 = body
    int speed;	//enemy speed in ticks
    int direction; //enemy movement direction LEFT = -1, RIGHT = 1
	enemyState state;	//state of enemy
    enemyState prevState;	//enemy's previous state

	bool running;	//enemy is in motion when true
	int row;	//enemy's position in row
	int col;	//enemy's position in column
	int tailRow;	//the last row of enemy (enemy tail row)
	int tailCol;	//the last column enemy (enemy tail column)
	int animTile;	//animation tile 
	pthread_t thread;	//thread for each enemy
	pthread_mutex_t mutex;	//mutex for each enemy

    struct enemy_struct * next;	//next enemy in list
} enemy;

struct enemy_struct *head;	//head enemy list


/* The thread runs as the enemy */
void *runEnemyT(void *data);	

/* Spawns an enemy for the first time, called externally,
    returns the newly spawned enemy*/
enemy* spawnEnemy(int length, int startRow, int startCol, int index, int speed, int direction);

/* Moves enemy to the destination row & column*/
void enemyMove(enemy *f, int dRow, int dCol); 

/* Draws the redrawn enemy & animates the enemy using its tiles*/
void _enemyRedrawMoved(enemy *p, int tailRow, int tailCol, bool lock);

/* Run when enemy is split */
void enemySplit(enemy * p);

/* Deletes an enemy */
void deleteEnemy(enemy * p);

/* cleans and frees an enemy */
void cleanEnemy();

/* Deletes the enemy list */
void deleteEnemyList();

/* Checks if all enemies are dead */
bool allEnemiesDead();

/* Mutex for caterpillar list */
pthread_mutex_t caterpillarMutex;


#endif