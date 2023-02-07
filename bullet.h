/**********************************************************************
  Module: bullet.h
  Author: Junseok Lee

  Purpose: manages the player bullet thread. Moves the bullets and the
  linked list that controls the player bullet.


**********************************************************************/

#ifndef BULLET_H
#define BULLET_H

#include "console.h"
#include "globals.h"
#include "enemy.h"
#include "llist.h"
#include "threadwrappers.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <curses.h>
#include <string.h>

/* Enum, Represents bullet state */
typedef enum bulletState_enum 
{
    TRANSIT,    //bullet is in transit
    COLLISION,  //bullet colldies with player
    DESTROY     //bullet done, to be cleaned
} bulletState; 

/* Represents player bullet */
typedef struct bullet_struct 
{
	int startCol;   //bullet starting column at player position
	int startRow;   //bullet starting row at player position       
    bulletState state;  //current state of bullet

	bool running;   //bullet in motion when true
	int row;        //bullet's current position (row)
	int col;        //bullet's current position (column)
	int prevRow;    //bullet's row in the previous tick
	int prevCol;    //bullet's column in the previous tick

	pthread_t thread;   //thread for each bullet
	pthread_mutex_t mutex;  //mutex for each bullet
    struct bullet_struct * next;  //next bullet linked list
} bullet;

/* Creates new player bullet. Changes state to TRANSIT,
   rows to startRow, col to startCol */
void newPlayerBullet(bullet *b);

/* Moves bullet one tick. Checks if enemy has been hit
   (using the enemyHit(enemy * e, bullet * b) method), 
   and checks if bullet is out of boundaries.  */
void playerBulletMove(bullet *b);

/* Spawns player bullet in given row and column, 
   returns spawned bullet pointer. Initializes enemy bullet 
   mutex and connects list*/
bullet* spawnBullet(int startRow, int startCol); // player bulllet = 0, enemy bullet = 1

/* Runs bullet thread. Manages bullet state and 
   bullet movement. */
void *runBulletT(void *data); 

/* Redraws the moved bullet. */
void bulletRedraw(bullet *b, bool lock); 

/* Checks if enemy caterpillar is hit at 
   player bullet's current position. */
int enemyHit(enemy * e, bullet * b);

/* Clears bullet from the screen */
void killBullets();

/* Frees bullets from allocation */
void freeBullets(bullet * head);

struct bullet_struct *bulletHead; //Head of player bullet list 
pthread_mutex_t bulletMutex;    //Player bullet mutex

#endif