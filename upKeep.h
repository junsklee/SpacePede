/**********************************************************************
  Module: upKeep.h
  Author: Junseok Lee

  Purpose: Updates player lives to screen, checks
  if there are no enemies left, and does regular cleanup.

**********************************************************************/

#ifndef UPKEEP_H
#define UPKEEP_H


#include "console.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "globals.h"
#include "threadwrappers.h"


/* Creates/initializes upkeep thread, returns 
   created thread */
pthread_t createUpkeepT(pthread_mutex_t * mutex);

/* Runs upkeep thread, check if enemies dead */
void *runUpkeepT(void *data) ;

/* Prints the player's current life on the screen*/
void printLives(pthread_mutex_t * mutex);

#endif
