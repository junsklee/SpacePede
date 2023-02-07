/**********************************************************************
  Module: upKeep.c
  Author: Junseok Lee

  Purpose: Handles and manages the upkeep thread. 

**********************************************************************/

#include "upKeep.h"


/* Board diagram for refresh */
char *BOARD[] = {
"                   Score:          Lives:",
"=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-centipiede!=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"",
"",
"",
"",
"",
"",
"", 
"" };

/********************THREAD functions***************/

pthread_t createUpkeepT(pthread_mutex_t * mutex)
{
    pthread_t upkeepThread;

    //create and run upkeep thread
	wrappedPthreadCreate(&(upkeepThread), NULL, runUpkeepT, (void*)mutex);
    
    return upkeepThread;
}

void *runUpkeepT(void *data) 
{
	player* p = playerPtr;
    pthread_mutex_t * mutex= (pthread_mutex_t *)(data); 
    char lives[PRINT_LIVES_SIZE]; //array containing lives
    
    ///print player's current lives to array
    sprintf(lives,"%d",playerPtr->lives);

    //Post initial player lives data
    putString(lives,FIRST_ROW, (GAME_COLS/PRINT_LIVES_SIZE + PRINT_LIVES_SIZE) , PRINT_LIVES_SIZE-1);

    //RUN THREAD WHILE GAME IS RUNNING & PLAYER IS ALIVE
    while (p->running && p->lives >= 0 && gameRunning) 
	{
		switch(p->state) 
		{            
			case DEAD:
                //WHEN PLAYER DIES	
                killPlayer(p); //KILL (DECREMENT LIFE) PLAYER
                printLives(mutex); //PRINT CURRENT LIFE
				break;

			case GAMEOVER:				
				break;
	
			case GAME:
                //Detect if all enemies dead 
                if(allEnemiesDead()){
                    wrappedMutexLock(&playerPtr->mutex);
                    playerPtr->state = WIN; //change player state
                    wrappedMutexUnlock(&playerPtr->mutex);
                }                
				break;

			default:
				break;
		}
        
	}
	return NULL;
}


/********************support functions***************/

void printLives(pthread_mutex_t * mutex){
    
    char lives[2];

    //redraw board
    consoleInit(GAME_ROWS, GAME_COLS, BOARD);

    //move lives data
    wrappedMutexLock(&playerPtr->mutex); 
    sprintf(lives,"%d",playerPtr->lives);
    wrappedMutexUnlock(&playerPtr->mutex);

    //print lives data
    putString(lives,0, GAME_COLS/2+2 ,1);
 
    //destroy bullets when player dies
    killBullets();

}

