/**********************************************************************
  Module: player.c
  Author: Junseok Lee

  Purpose: Runs the player on a single thread. Manages player moves 
  and draws.

**********************************************************************/

#include "player.h"

//Player's graphic data
char* playerGraphic[PLAYER_ANIM_TILES][PLAYER_HEIGHT] = 
{
    {"/o\\",
     "|||",
     "/^\\"},
    {"/|\\",
     "-o-",
     "/^\\"},
    {"/|\\",
     "|||",
     "/o\\"}
};

/********************support functions***************/

/* reset the player state to start */
void newPlayer(player *p) 
{
	p->row = p->startRow;
	p->col = p->startCol;
	p->prevRow = p->startRow;
	p->prevCol = p->startCol;
	p->animTile = 0;
	p->state = GAME;
}

void _playerRedrawMoved(player *p, int prevRow, int prevCol, bool lock) 
{
	if(lock)
	{
		//clear previous image and draw new image
		wrappedMutexLock(&p->mutex);
		consoleClearImage(prevRow, prevCol, PLAYER_HEIGHT, PLAYER_WIDTH);
		consoleDrawImage(p->row, p->col, playerGraphic[p->animTile], PLAYER_HEIGHT);
		wrappedMutexUnlock(&p->mutex);
	}
}

void playerRedraw(player *p, bool lock) 
{
	_playerRedrawMoved(p, p->prevRow, p->prevCol, lock);
}

/********************THREAD functions***************/

player* spawnPlayer(int startRow, int startCol, int lives)
{
    player* p = (player*)(malloc(sizeof(player)));
	if(p == NULL) return p; //check for malloc failure

	p->lives = lives;
	p->startCol = startCol;
	p->startRow = startRow;
	p->running = true;

	//Init mutex
	pthread_mutex_init(&p->mutex, NULL);

	//create and run player thread
	wrappedPthreadCreate(&(p->thread), NULL, runPlayerT, (void*)p);
	return p;
}

void *runPlayerT(void *data) 
{
	player* p = (player*)data;
	newPlayer(p);
	
	while (p->running && gameRunning) 
	{

		switch(p->state) 
		{
			case DEAD:
				//killPlayer(p); //done in upKeep
				break;
			case GAMEOVER:
			putBanner(LOSE_MSG);
				wrappedMutexLock(&p->mutex);		
				p->running = false;
				wrappedMutexUnlock(&p->mutex);
				
				wrappedMutexLock(&gameMutex);
				wrappedCondSignal(&gameCondition);
				wrappedMutexUnlock(&gameMutex);
				//pthread_exit(&p->col);
				break;
				
				//display game over
			case GAME:
				//move to direction from keyboard thread
				keyMove(p,dir);

				//show win message, send signal
				wrappedMutexLock(&p->mutex);
				p->animTile++;
				p->animTile %= PLAYER_ANIM_TILES;
				wrappedMutexUnlock(&p->mutex);
				
				//redraw moved player, lock = true
				playerRedraw(p, true); 
				
				sleepTicks(PLAYER_ANIM_TICKS);
				
				break;

			case WIN:
				p->running = false;
				//show win message, send signal
				putBanner(WIN_MSG);
				wrappedMutexLock(&gameMutex);
				wrappedCondSignal(&gameCondition);
				wrappedMutexUnlock(&gameMutex);
				return NULL;
			default:
				break;
		}

	}
	//show lose message, send signal
	if(gameRunning)putBanner(LOSE_MSG);
	wrappedMutexLock(&gameMutex);
	wrappedCondSignal(&gameCondition);
	wrappedMutexUnlock(&gameMutex);
	return NULL;
}



void playerMove(player *f, int dRow, int dCol)
{
	//boundaries for movement
	if(dCol <= RIGHT_END && 
		dCol >= LEFT_END &&
		dRow >= BOTTOM_END && 
		dRow <= TOP_END)
	{
	//move to destination
	pthread_mutex_lock(&f->mutex);
	f->prevCol = f->col;
	f->prevRow = f->row;
	f->col = dCol;
	f->row = dRow;
	pthread_mutex_unlock(&f->mutex);
	}
}


void keyMove(player * p, char direction)
{
	char keys[5];
    sprintf(keys, "%d", direction);
    switch(direction){
      case LEFT_KEY:
        playerMove(p,p->row,p->col-1);
		dir=0;
        break;
      case RIGHT_KEY:
        playerMove(p,p->row,p->col+1);
		dir=0;
        break;
      case DOWN_KEY:
        playerMove(p,p->row+1,p->col);
		dir=0;
        break;
      case UP_KEY:
        playerMove(p,p->row-1,p->col);
		dir=0;
        break;
      default:
        break;
    }

}
void killPlayer(player* p){

	//Decrement lives, clear ship
	if(p->lives>0){
		wrappedMutexLock(&p->mutex);
		p->lives--;
		wrappedMutexUnlock(&p->mutex);

		consoleClearImage(p->row, p->col, PLAYER_HEIGHT, PLAYER_WIDTH);

		newPlayer(p); //restart player (sets state back to to GAME)
		
	}
	//out of lives, end game
	else{

		wrappedMutexLock(&p->mutex);
		p->state = GAMEOVER;
		wrappedMutexUnlock(&p->mutex);
		
	}
}

