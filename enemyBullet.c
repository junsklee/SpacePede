/**********************************************************************
  Module: enemyBullet.c
  Author: Junseok Lee
  Represents an enemy bullet. 

**********************************************************************/
#include "enemyBullet.h"

//Enemy bullet body data
char * ENEMY_BULLET_BODY[1] = {"."};

void newEnemyBullet(enemyBullet * b) 
{
	b->row = b->startRow;
	b->col = b->startCol;
    b->state = TRANSIT_E;
}


void eBulletMove(enemyBullet * b, player * p)
{
    //Bullet out of bound: set to CLEAN
    if(b->row > GAME_ROWS - 2){
        wrappedMutexLock(&b->mutex);
        b->state = CLEAN;
        wrappedMutexUnlock(&b->mutex);
    }
    
    //Enenmy bullet collides with player ship
    if(shipHit(p,b))  
    {
        //set bullet state
        wrappedMutexLock(&b->mutex);
        b->state = PLAYERHIT;
        wrappedMutexUnlock(&b->mutex);

        //kill player
        wrappedMutexLock(&p->mutex);
        p->state = DEAD;
        wrappedMutexUnlock(&p->mutex);
        
            
    }
    //Otherwise move bullet up
    else{        
        wrappedMutexLock(&b->mutex);
        b->prevRow = b->row;
        b->row++;
        wrappedMutexUnlock(&b->mutex);

    }

}

int shipHit(player * p,enemyBullet * b){
    int success = false;

    if( ((p->row == b->row-1) || (p->row == b->row)) && //bullet's row matches player's 
        ((p->col == b->col-1) || (p->col == b->col-2) || (p->col == b->col)))  //bullet's column matches player's 
    {
        success = true;
    } 
    return success;
}


enemyBullet* spawnEnemyBullet(int startRow, int startCol)
{
    enemyBullet * b = (enemyBullet*)(malloc(sizeof(enemyBullet)));
    if(b == NULL) return NULL; //CHECK FOR MALLOC FAILURE

	b->startCol = startCol;
	b->startRow = startRow;
	b->running = true;

    //first bullet: set as head
    if(enemyBulletHead == NULL){
        wrappedMutexLock(&enemyBulletMutex);
        enemyBulletHead = b;
        wrappedMutexUnlock(&enemyBulletMutex);
    }
    //add to the enemy bullet list
    else{
        enemyBullet * node = enemyBulletHead;

        //traverse to end of list
        while(node->next != NULL)
        {
            node = node->next;
        }
        wrappedMutexLock(&enemyBulletMutex);
        node->next = b; //set as last node
        wrappedMutexUnlock(&enemyBulletMutex);

    }

	//Init mutex
	wrappedMutexInit(&b->mutex, NULL);

    //create and run thread
	wrappedPthreadCreate(&(b->thread), NULL, runEBulletT, (void*)b);

	return b;
}



void *runEBulletT(void *data) 
{
	enemyBullet * b = (enemyBullet*)data;
	newEnemyBullet(b); //initialize new bullet
	
	while (b->running && gameRunning) 
	{
		switch(b->state) 
		{
			case TRANSIT_E:
                eBulletMove(b,playerPtr); //move bullet
                break;

			case PLAYERHIT:
				b->running = false; //stop running
                
                break;
            case CLEAN:
                b->running = false; //stop running  
                break;

			default:
				break;
		}		
		enemyBulletRedraw(b, true); //redraw moved bullet, lock = true
		sleepTicks(ENEMY_BULLET_SPEED); //sleep for desired bullet speed

	}
	return NULL;
}


void enemyBulletRedraw(enemyBullet * b, bool lock) 
{
	if(lock)
	{
        //clear and draw image
        wrappedMutexLock(&b->mutex);
        consoleClearImage(b->prevRow, b->col, 1, 1);       
        consoleDrawImage(b->row, b->col, ENEMY_BULLET_BODY, 1);
        wrappedMutexUnlock(&b->mutex);
	}
}

void freeEnemyBullets(enemyBullet * head){
        enemyBullet * temp;

        //free each node
        while (head != NULL){
            temp = head;
            head = head->next;
            free(temp);
        }
        //set head to null
        if(enemyBulletHead) enemyBulletHead = NULL;
}

        