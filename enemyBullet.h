/**********************************************************************
  Module: enemyBullet.h
  Author: Junseok Lee

  Purpose: Manages the enemy bullet on its own thread. Also detects
  Collision with the player. Returns a pointer to an enemy bullet.


**********************************************************************/

#ifndef ENEMYBULLET_H
#define ENEMYBULLET_H


#include "console.h"
#include "globals.h"
#include "player.h"
#include "threadwrappers.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <string.h>

/* Enum, Represents an enemy bullet state */
typedef enum enemyBulletState_enum 
{
    TRANSIT_E,  //Enemy bullet in transit
    PLAYERHIT,  //Enemy bullet hits player
    CLEAN   //Enemy bullet finishes job
} enemyBulletState; 

/* Represents an enemy bullet */
typedef struct enemy_bullet_struct 
{
	
	int startCol;   //bullet starting column at enemy position
	int startRow;   //bullet starting column at enemy position
    enemyBulletState state; //state of bullet stored in enum

	bool running;   //bullet is in motion when true
	int row;        //bullet's position (row)
	int col;        //bullet's position (columns)
	int prevRow;    //bullet's previous Row

	pthread_t thread;   //thread for each enemy bullet
	pthread_mutex_t mutex;  //mutex for each enemy bullet

    struct enemy_bullet_struct * next;  //next bullet linked list
} enemyBullet;

/* Creates new enemy bullet. Changes state to TRANSIT_E,
   rows to startRow, col to startCol */
void newEnemyBullet(enemyBullet *b);

/* Moves enemy bullet one tick. Checks if player has been hit
   (using the shipHit method), and checks if bullet is out 
   of boundaries.  */
void eBulletMove(enemyBullet * b, player * p);

/* Spawns enemy bullet in given row and column, 
   returns spawned enemy bullet pointer. Initializes enemy 
   bullet mutex and connects list*/
enemyBullet* spawnEnemyBullet(int startRow, int startCol);

/* Runs Enemy bullet thread. Manages bullet state and 
   bullet movement. */
void *runEBulletT(void *data); 

/* Redraws the moved bullet. */
void enemyBulletRedraw(enemyBullet *b, bool lock); 

/* Checks if player ship is hit at enemy bullet's
   current position. */
int shipHit(player * p, enemyBullet * b);

/* Redraws the moved bullet. */
void freeEnemyBullets(enemyBullet * head);


struct enemy_bullet_struct *enemyBulletHead; //Head of enemy bullet list 
pthread_mutex_t enemyBulletMutex; //Enemy bullet mutex

#endif