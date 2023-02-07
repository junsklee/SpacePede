/**********************************************************************
  Module: centipede.h
  Author: Junseok Lee

  Purpose: The management of the game engine. Includes a screen
  refresh thread, a keyboard thread, an enemy spawn thread and a
  main thread.

**********************************************************************/

#ifndef CENTIPEDE_H
#define CENTIPEDE_H

#include "console.h"
#include "centipede.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "bulletList.h"
#include "upKeep.h"
#include "threadwrappers.h"

/* Runs the game engine (main thread). Handles
  thread initialization, joins, finalization, etc.
  Blocks when all threads are initialized and run,
  and when signalled, cleans up and ends game */
void centipedeRun();

/* Screen refresh thread run*/
void * screenRunT(void * mutex);

/* Thread run to get keyboard input,
  uses the select command */
void * getKeyT(void * data);

/* Thread to run and handle the 
  enemy spawns. Joins all enemies
  when this thread ends. */
void * spawnRunT(void * data);

/* Joins and frees all bullets,
   and enemy bullets. */
void joinExternalThreads();

/* Cleans up remaining lists */
void cleanUp();

#endif
