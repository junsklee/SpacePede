/**********************************************************************
  Module: centipede.c
  Author: Junseok Lee

  Purpose: Game engine management. 

**********************************************************************/

#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h> 
#include "centipede.h"


/**** Game Board Dimensions */
char *GAME_BOARD[] = {
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

/****GLOBALS*/

//Mutex to control screen locking
pthread_mutex_t screenMutex; 
pthread_mutex_t keyMutex; //keyboard mutex


/*************GLOBALS from other files*************************/
//player *playerPtr; //defined in player.h
//pthread_mutex_t * caterpillarMutex; //defined in enemy.h
//enemy * head; //defined in enemy.h
//enemy * last; //defined in enemy.h
//pthread_cond_t gameCondition; //defined in threadwrappers.h
//pthread_mutex_t * gameMutex; //defined in threadwrappers.h
//char dir; //defined in globals.h
//int gameRunning; //defined in globals.h
/**************************************************************/

/* 1 if initial spawn thread run, 0 otherwise*/
int initialRun = true;

void centipedeRun()
{
	if (consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD))
        {
                /************* MAIN THREAD *************************/
                
                pthread_t screenThread; //Thread to re-draw the screen / perform a refresh using the command in console.c
                pthread_t keyThread; //Thread to get input from the keyboard
                pthread_t spawnThread; //Thread to handle enemy spawns
                
                gameRunning = true; //set game as running
                
                //initialize cateripllar mutex
                wrappedMutexInit(&caterpillarMutex, NULL);

                //initialize keyboard mutex
                wrappedMutexInit(&keyMutex, NULL);

                //create player
                playerPtr = spawnPlayer(PLAYER_START_ROW, PLAYER_START_COL, PLAYER_START_LIVES);

                //create enemy
                enemy * firstEnemy = spawnEnemy(ENEMY_DEFAULT_LENGTH,INITIAL_ENEMY_Y,GAME_COLS,LIST_TYPE_HEAD,ENEMY_SPEED_SLOW,LEFT);
                nodeHead =(node*)(malloc(sizeof(node)));

                if(nodeHead == NULL) return; //check for malloc failure

                nodeHead->e = firstEnemy;

                //create upkeep thread
                pthread_t upkeepThread = createUpkeepT(&screenMutex);
                                
                //create screen thread
                wrappedPthreadCreate(&screenThread, NULL, screenRunT, (void*)&screenMutex); 

                //create keyboard thread
                wrappedPthreadCreate(&keyThread,NULL, getKeyT, (void*)playerPtr); 

                //run spawn thread 
                wrappedPthreadCreate(&spawnThread, NULL, spawnRunT, (void*)&screenMutex);



                /* now started all threads, sleep using condition variable */
                
                //initialize game mutex
                wrappedMutexInit(&gameMutex, NULL);
                
                //block the main thread until signal
                wrappedMutexLock(&gameMutex);               
                wrappedCondWait(&gameCondition, &gameMutex); 
                //when game end is signalled, switch gameRunning to false     
                gameRunning = false;
                wrappedMutexUnlock(&gameMutex);


                
                /* Join all threads */

                //disable console while we wait for threads to join
                disableConsole(true);

                //wait for player thread to end
                wrappedPthreadJoin(playerPtr->thread, NULL); 

                //wait for screen thread to end
                wrappedPthreadJoin(screenThread, NULL);
                 
                //wait for key thread to end
                wrappedPthreadJoin(keyThread,NULL);
                
                //wait for spawn thread to end
                //joins enemy threads here
                wrappedPthreadJoin(spawnThread,NULL);

                //wait for upkeep thread to end
                wrappedPthreadJoin(upkeepThread,NULL);
                
                //join enemy bullet and bullet threads
                joinExternalThreads();                           
                
                //free pointers
                free(playerPtr);

                /*all other memory allocations are freed elsewhere
                  e.g. joinExternalThreads(), spawnRunT, etc. */
                                
        }
        //wait for final key press and finish console
        finalKeypress();
        consoleFinish();        	
}

void * screenRunT(void * data)
{
        pthread_mutex_t * mutex= (pthread_mutex_t *)(data); 

        while(gameRunning)
        {
                wrappedMutexLock(mutex);
                consoleRefresh(); //draw everything to screen.		
                wrappedMutexUnlock(mutex);

                sleepTicks(PLAYER_ANIM_TICKS); //sleep for given time
        }
        return NULL;
}


void * getKeyT(void * data)
{
  player * p = (player*)(data);
  int fd=0; //file descriptor
  int selStatus; //status of select 
  fd_set rdfds; //read file descriptor
  struct timeval timeout; //time out value struct
  
  while(gameRunning)
  {
    //stdin check for input
    FD_ZERO(&rdfds); 
    FD_SET(fd, &rdfds); 

    //wait for specified TIMEOUT_SEC
    timeout.tv_sec = TIMEOUT_SEC; 
    timeout.tv_usec = TIMEOUT_USEC;  
    
    selStatus = select(WRITE_FD, &rdfds, NULL, NULL, &timeout);
    if(selStatus != FALSE)
    {
      //get character input 
      char c = getchar();

      //directional specifications
      if(c == UP_KEY  ||
        c == DOWN_KEY ||
        c == LEFT_KEY ||
        c == RIGHT_KEY) 
        {
            wrappedMutexLock(&keyMutex);
            dir = c;
            wrappedMutexUnlock(&keyMutex);
        }  
        else if(c == SPACE_KEY)
        {
             //Fire player bullet
             bullet * bToAdd = spawnBullet(p->row, p->col+1);    
             bulletList* bL = (bulletList*)(malloc(sizeof(bulletList)));

            if(bL == NULL) return NULL; //check for malloc failure

            bL->b = bToAdd;
            
            if(bNodeHead == NULL)bNodeHead = bL;
            else{
            wrappedMutexLock(&bulletMutex);
            bulletList * cur = bNodeHead;                                
                //traverse to end of list                
                while(cur->next != NULL)
                {
                    cur = cur->next;
                }        
                cur->next = bL;
            }
            wrappedMutexUnlock(&bulletMutex);           
 
        }  
        else if(c == QUIT_KEY)
        {
                
                putBanner(QUIT_MSG); //post quit banner

                /*SIGNAL (pthread_cond_signal) THE START OF
                  CLEANUP THREAD (JOINS ALL THREADS, ETC)*/
                wrappedMutexLock(&gameMutex);                                 
                wrappedCondSignal(&gameCondition); //signal main thread
                wrappedMutexUnlock(&gameMutex);

                return NULL;

        }      
    }    
    else //no data for 3 seconds
    {
      if(!gameRunning)
      {
        //stop thread
        break;
      }
    }
  }
  return NULL;
}

void * spawnRunT(void * data)
{
        pthread_mutex_t * scrMutex= (pthread_mutex_t *)(data); 
        int i=0; //amount of ticks requred to match enemy tick rate 
        enemy * e;

        //Spawn thread when game is running 
        while(gameRunning)
        {       //Sleep one tick to check if game is running every tick
                sleepTicks(ENEMY_SINGLE_TICK); 

                //Run if slept for exactly 'ENEMY_SPAWN_RATE' ticks
                if(i>ENEMY_SPAWN_RATE){
                        
                        //INITIAL RUN
                        if(initialRun){
                                node * nodeToAdd = (node*)(malloc(sizeof(node)));
                                wrappedMutexLock(scrMutex);

                                //spawns enemy
                                e= spawnEnemy(ENEMY_DEFAULT_LENGTH,INITIAL_ENEMY_Y,GAME_COLS,LIST_TYPE_BODY,ENEMY_SPEED_SLOW,LEFT); //spawn enemy

                                initialRun=0;
                                wrappedMutexUnlock(scrMutex); 
                                
                                nodeToAdd->e = e;
                                
                                //new node is next to head
                                wrappedMutexLock(&caterpillarMutex);
                                nodeHead->next = nodeToAdd;
                                wrappedMutexUnlock(&caterpillarMutex);
                                i=0;
                        }
                        //EVERY OTHER RUNS
                        else{
                                node * nodeToAdd = (node*)(malloc(sizeof(node)));
                                wrappedMutexLock(scrMutex);
                                e = spawnEnemy(ENEMY_DEFAULT_LENGTH,INITIAL_ENEMY_Y,GAME_COLS,LIST_TYPE_BODY,ENEMY_SPEED_SLOW,LEFT); //spawn enemy
                                wrappedMutexUnlock(scrMutex);
                                nodeToAdd->e = e;

                                node * cur = nodeHead;
                                
                                wrappedMutexLock(&caterpillarMutex);
                                //traverse to end of list                
                                while(cur->next != NULL)
                                {
                                        cur = cur->next;
                                }        
                                cur->next = nodeToAdd; //enemy is now @ end of list
                                wrappedMutexUnlock(&caterpillarMutex);
                                i=0;
                        }

                }
                i++;

        }
        //join and free all enemy threads
        node * next = NULL;
        node * current = nodeHead;
        while (current != NULL) 
        {
                if(current->e != NULL){
                        wrappedPthreadJoin(current->e->thread, NULL);
                        free(current->e);
                }
                next = current->next;                                
                free(current);  //free malloc
                current = next;
        }
        return NULL;
}

void joinExternalThreads(){
        
        //join and free all bullet threads
        bulletList * nextBL = NULL;
        bulletList * currentBL = bNodeHead;
        while (currentBL != NULL) 
        {
                if(currentBL->b != NULL){
                wrappedPthreadJoin(currentBL->b->thread, NULL);
                free(currentBL->b);
                }
                nextBL = currentBL->next;                
                
                free(currentBL);
                currentBL = nextBL;
        }
        
        //join and free all enemy bullet threads
        enemyBulletList * nextEBL = NULL;
        enemyBulletList * currentEBL = eBnodeHead;
        while (currentEBL != NULL) 
        {
                if(currentEBL->eB != NULL){
                wrappedPthreadJoin(currentEBL->eB->thread, NULL);
                free(currentEBL->eB);
                }
                nextEBL = currentEBL->next;                
                
                free(currentEBL);
                currentEBL = nextEBL;
        }        


}
void cleanUp(){

        deleteEnemyList();
        if(bulletHead != NULL) freeBullets(bulletHead);
        freeEnemyBullets(enemyBulletHead);

}