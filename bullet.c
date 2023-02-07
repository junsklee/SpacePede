/**********************************************************************
  Module: bullet.c
  Author: Junseok Lee

  Represents a player bullet

**********************************************************************/
#include "bullet.h"


void newPlayerBullet(bullet *b) 
{
	b->row = b->startRow;
	b->col = b->startCol;
    b->state = TRANSIT;

}

void playerBulletMove(bullet *b)
{
    enemy * e = head; //head enemy

    //collide when bullets hits ceiling
    if(b->row < TOP_WALL){
        wrappedMutexLock(&b->mutex);
        b->state = COLLISION;
        wrappedMutexUnlock(&b->mutex);
    }

    //Traverse through enemy list to check if the bullet hit
    while(e != NULL)
    {
        //when player bullet hits a caterpillar
        if(enemyHit(e,b))  
        {
            enemy* newEnemy;
            node * nodeToAdd = (node*)(malloc(sizeof(node))); 
            if(nodeToAdd == NULL) return; //check for malloc failure

            //set bulllet state to collide
            wrappedMutexLock(&b->mutex);
            b->state = COLLISION;
            wrappedMutexUnlock(&b->mutex);

            //save enemy state set it to split
            wrappedMutexLock(&e->mutex);
            e->prevState = e->state;
            e->state = SPLIT;
            wrappedMutexUnlock(&e->mutex);

            int prevLength = e->length; //enemy previous length
            int prevRow = e->row;   //enemy previous row
            int prevCol = e->col;   //enemy previous column

            //SPLIT ENEMY MOVING LEFT
            if(e->direction == LEFT){
                
                //cut enemy length
                wrappedMutexLock(&e->mutex);                
                e->length = e->length - (b->col - e->col)-1; 
                wrappedMutexUnlock(&e->mutex);

                
                    //move cut enemy back 
                    enemyMove(e,e->row, b->col); 
                    
                    //spawn new enemy in front of the moved old enemy
                    newEnemy = spawnEnemy(prevLength-(e->length+1), prevRow, prevCol,LIST_TYPE_BODY,ENEMY_SPEED_FAST,LEFT);
             
				
            }
            //SPLIT ENEMY MOVING RIGHT
            else if (e->direction == RIGHT){
                
                //cut enemy length
                wrappedMutexLock(&e->mutex);
                e->length = e->length-(e->col-b->col)-1;
                wrappedMutexUnlock(&e->mutex);
                
                enemyMove(e,e->row, b->col); //move cut enemy back

                //spawn new enemy in front of the moved old enemy
                newEnemy = spawnEnemy(prevLength-(e->length+1), prevRow, prevCol,LIST_TYPE_BODY,ENEMY_SPEED_FAST,RIGHT); 

            }            
            nodeToAdd->e = newEnemy;

            //ADD NEW ENEMY TO END OF LIST
            wrappedMutexLock(&caterpillarMutex);                                                                
            node * cur = nodeHead;
            //traverse to end of list                
            while(cur->next != NULL)
            {
                    cur = cur->next;
            }        
            cur->next = nodeToAdd; //enemy is now @ end of list
            wrappedMutexUnlock(&caterpillarMutex); 

            break;
            
        }
        //move to next enemy on llist
        e = e->next;
    
    }
    //keep moving up if bullet hasn't collided
    if(b->state != COLLISION)
    {
        wrappedMutexLock(&b->mutex);
        b->prevRow = b->row;
        b->row--;
        wrappedMutexUnlock(&b->mutex);
    }
}


int enemyHit(enemy * e, bullet * b){
    int bulletCol = b->col;
    int success = false;

    //enemy going left & bullet hit a part of enemy
    bool leftHit = (e->direction == LEFT) && ((bulletCol-(e->length) < (e->col)) && (bulletCol-(e->length) >= (e->col - e->length)) && (b->row == e->row+ENEMY_HEIGHT));

    //enemy going right & bullet hit body)
    bool rightHit = (e->direction == RIGHT) && (((bulletCol + e->length) > (e->col)) && ((bulletCol+e->length) <= (e->col + e->length)) && (b->row == e->row+ENEMY_HEIGHT));

    bool moving = (e->state != DEAD_E && e->state != SPLIT);

    if((leftHit || rightHit) && moving)
    {
        success = true;
    } 
    return success;       
}

bullet* spawnBullet(int startRow, int startCol)
{
    bullet* b = (bullet*)(malloc(sizeof(bullet))); 
	b->startCol = startCol;
	b->startRow = startRow;
	b->running = true;

    //set as head if bullet list is undefined
    if(bulletHead == NULL){
        wrappedMutexLock(&bulletMutex);
        bulletHead = b;
        wrappedMutexUnlock(&bulletMutex);
        
    }
    //add to bullet list
    else{ 
        bullet * node = bulletHead;

        //traverse to end of list
        while(node->next != NULL)
        {
            node = node->next;
        }

        //set as last element of list
        wrappedMutexLock(&bulletMutex);
        node->next = b;
        wrappedMutexUnlock(&bulletMutex);
    }

	//Init mutex
	pthread_mutex_init(&b->mutex, NULL);

    //create & run bullet thread
	wrappedPthreadCreate(&(b->thread), NULL, runBulletT, (void*)b);
	return b;
}

void *runBulletT(void *data) 
{
	bullet* b = (bullet*)data;
	newPlayerBullet(b);
	
	while (b->running && gameRunning) 
	{
		switch(b->state) 
		{
			case TRANSIT:
                playerBulletMove(b);
                bulletRedraw(b, true); //redraw moved bullet, lock = true
                break;

			case COLLISION:
                wrappedMutexLock(&b->mutex);
				b->running = false;
                wrappedMutexUnlock(&b->mutex);
                break;

            case DESTROY:
                b->running = false;                
                break;
                
			default:
				break;
		}
        //sleep for next bullet tick
		sleepTicks(PLAYER_BULLET_SPEED); 
	}
	return NULL;
}

char * BULLET_BODY[BULLET_SIZE] = {"@"};
void bulletRedraw(bullet *b, bool lock) 
{
	if(lock)
	{
        wrappedMutexLock(&b->mutex);
        consoleClearImage(b->prevRow, b->col, BULLET_SIZE, BULLET_SIZE);        
        consoleDrawImage(b->row, b->col, BULLET_BODY, BULLET_SIZE);
        wrappedMutexUnlock(&b->mutex);
	}
}

void killBullets(){
    if(bulletHead != NULL){
        bullet * b = bulletHead;

        //traverse through bullet list
        while (b != NULL){
            wrappedMutexLock(&b->mutex);
            consoleClearImage(b->row,b->col,1,1); 
            wrappedMutexUnlock(&b->mutex);
            b = b->next;
            //free(temp);
        }
    }
}
void freeBullets(bullet * headB){
        bullet * current = headB;
        bullet * temp;

        //return if head 
        if( current == NULL || current->next == NULL) return;
        
        wrappedMutexLock(&bulletMutex);
        while (current != NULL){
            //wrappedPthreadJoin(current->thread,NULL);
            temp = current;
            current = current->next;
            
            free(temp);
            
        }       
        bulletHead = NULL; //set bullet head to null
        wrappedMutexUnlock(&bulletMutex);
       
}
