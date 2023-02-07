/**********************************************************************
  Module: enemy.c
  Author: Junseok Lee
  Represents an enemy caterpillar. Runs on a separate thread.
  
**********************************************************************/

#include "enemy.h"

//Enemy body diagram
char* ENEMY_BODY[ENEMY_BODY_ANIM_TILES][ENEMY_HEIGHT] = 
{
  
  {"|",
   ";"},
  {"|",
   ","},
  {"^",
   ";"},
   {"@",
  "="}
};

/********************support functions***************/

/* reset the enemy state to start */
void newEnemy(enemy *p) 
{
	p->row = p->startRow;
	p->col = p->startCol;
    p->tailRow = p->startRow + p->length;
    p->tailCol = p->startRow - ENEMY_HEIGHT;
	p->animTile = 3;
    if(p->direction == LEFT)
	    p->state = LEFT_E;
    else   
        p->state = RIGHT_E;
}

void _enemyRedrawMoved(enemy *p, int tailRow, int tailCol, bool lock) 
{

	if(!lock)return;
	
    int i;
    //ENEMY MOVING LEFT
    if(p->direction == LEFT && p->state == LEFT_E)
    {
        wrappedMutexLock(&p->mutex);

        //draw head
        consoleDrawImage(p->row, p->col, ENEMY_BODY[ENEMY_ANIM_TILES], ENEMY_HEIGHT); 
        //draw enemy body        
        for(i=0; i< (p->length) ;i++){
            char** tile = ENEMY_BODY[(p->animTile+i)%ENEMY_ANIM_TILES];
            consoleDrawImage(p->row, p->col+i+1, tile, ENEMY_HEIGHT); //draw body (col+i+1 b/c head is drawn)
            
        }
        //clear enemy trail
        consoleClearImage(p->row, p->col + (p->length), ENEMY_HEIGHT, 1); 

        wrappedMutexUnlock(&p->mutex);

    }
    //ENEMY WRAPPING AROUND LEFT SIDE
    else if (p->direction == RIGHT && p->state == WRAPAROUND_L){
        wrappedMutexLock(&p->mutex);
        
        //ANIMATE REST OF PREVIOUS LINE
        for(i=0; i< (p->length-1) ;i++){
            char** tile = ENEMY_BODY[(p->animTile+i)%ENEMY_ANIM_TILES];
            consoleDrawImage(p->row-2, p->col+i, tile, ENEMY_HEIGHT); //draw body     
        }
        //clear enemy trail of previous line
        consoleClearImage(p->row-2, p->col + (p->length), ENEMY_HEIGHT, 1); //put this outside for loop
        

        //ANIMATE NEW LINE
        consoleDrawImage(p->row, -1*p->col, ENEMY_BODY[ENEMY_ANIM_TILES], ENEMY_HEIGHT); //draw head
        for(i=0; i< (p->length );i++){
            char** tile = ENEMY_BODY[(p->animTile+i)%ENEMY_ANIM_TILES];
            consoleDrawImage(p->row, -1*p->col-i-1, tile, ENEMY_HEIGHT); //draw body
            
        }
        wrappedMutexUnlock(&p->mutex);
    }
    //ENEMY WRAPPING AROUND RIGHT SIDE
    else if (p->direction == LEFT && p->state == WRAPAROUND_R){
        wrappedMutexLock(&p->mutex);
        
        //ANIMATE REST OF PREVIOUS LINE
        for(i=0; i< (p->length) ;i++){
            
            char** tile = ENEMY_BODY[(p->animTile+i)%ENEMY_ANIM_TILES];
            consoleDrawImage(p->row-2, p->col-i-1, tile, ENEMY_HEIGHT); //draw body
         
        }
        //clear enemy trail of previous line
        consoleClearImage(p->row-2, p->col - p->length, ENEMY_HEIGHT, 1);//put this outside for loop
        

        //ANIMATE NEW LINE
        consoleDrawImage(p->row, p->col-(p->col - GAME_COLS)*2, ENEMY_BODY[ENEMY_ANIM_TILES], ENEMY_HEIGHT); //draw head  
        //draw enemy body             
        for(i=0; i< (p->length) ;i++){
            char** tile = ENEMY_BODY[(p->animTile+i)%ENEMY_ANIM_TILES];
            consoleDrawImage(p->row, p->col-(p->col - GAME_COLS)*2 + 1, tile, ENEMY_HEIGHT); //draw body
            
        }
        wrappedMutexUnlock(&p->mutex);
    }
    //ENEMY MOVING RIGHT
    else if(p->direction == RIGHT && p->state == RIGHT_E ){ 
        wrappedMutexLock(&p->mutex);
        //draw head
        consoleDrawImage(p->row, p->col, ENEMY_BODY[ENEMY_ANIM_TILES], ENEMY_HEIGHT); 
        //draw enemy body
        for(i=0; i< (p->length );i++){
            char** tile = ENEMY_BODY[(p->animTile+i)%ENEMY_ANIM_TILES];
            consoleDrawImage(p->row, p->col-i-1, tile, ENEMY_HEIGHT); //draw body
            
        }
        //clear enemy trail of previous line
        consoleClearImage(p->row, p->col - p->length*2 + 1, ENEMY_HEIGHT, p->length); 
        wrappedMutexUnlock(&p->mutex);
                
    }   

		
}

void enemyRedraw(enemy *p, bool lock) 
{
	_enemyRedrawMoved(p, p->tailRow, p->tailCol, lock);
}


/********************THREAD functions***************/

enemy* spawnEnemy(int len, int startRow, int startCol, int index, int speed, int direction)
{
    if (len<5) return NULL;
    enemy* p = (enemy*)(malloc(sizeof(enemy)));
    if(p == NULL) return p; //check for malloc failure

    /* Initialize enemy values*/
    p->length = len;
	p->startCol = startCol;
	p->startRow = startRow;
    p->index = index;
	p->running = true;
    p->speed = speed;
    p->direction = direction;

    //head enemy
    if(index == LIST_TYPE_HEAD){
        wrappedMutexLock(&caterpillarMutex);
        head = p;
        wrappedMutexUnlock(&caterpillarMutex);
    }

    //remaining enemies
    else if (index == LIST_TYPE_BODY){                
        enemy * node = head;

        //if first body
        if(head->next == NULL){
            wrappedMutexLock(&caterpillarMutex);
            head->next = p;
            wrappedMutexUnlock(&caterpillarMutex);
        }
        else{
            wrappedMutexLock(&caterpillarMutex);
            //traverse to end of list                
            while(node->next != NULL)
            {
                node = node->next;
            }        
            node->next = p; //enemy is now @ end of list
            wrappedMutexUnlock(&caterpillarMutex);
        }
    }

	//Init mutex
	wrappedMutexInit(&p->mutex, NULL);

    //create and run thread
	wrappedPthreadCreate(&(p->thread), NULL, runEnemyT, (void*)p);

	return p;
}

void *runEnemyT(void *data) 
{
	enemy* p = (enemy*)data;
	newEnemy(p); //initialize new enemy
	
	while (p->running && gameRunning) 
	{
		//kill enemies < 5
        if(p->length<5){
            wrappedMutexLock(&p->mutex);
            p->state = DEAD_E;
            wrappedMutexUnlock(&p->mutex);
            break;
        }
		switch(p->state) 
		{
            case LEFT_E:
				enemyMove(p,p->row, p->col-1); //move left
                break;
            case RIGHT_E:
                enemyMove(p,p->row, p->col+1); //Move right
                break;   
            case WRAPAROUND_L:
                enemyMove(p,p->row, p->col-1); //wraparound LHS
                break;
            case WRAPAROUND_R:
                enemyMove(p,p->row, p->col+1); //wraparound RHS
                break;
			case DEAD_E:
                wrappedMutexLock(&p->mutex);
				p->running = false;   
                wrappedMutexUnlock(&p->mutex);
                deleteEnemy(p);
                return NULL;
                break;
            case SPLIT:
                enemySplit(p);
                break;

			default:
				break;
		}

        //head reach left end
        if(p->col == LEFT_END-1) 
        {
            //enemy is in the last row 
            if(p->row >= ENEMY_LAST_ROW){ 
                wrappedMutexLock(&p->mutex);
                p->state = WRAPAROUND_L;
                p->direction = RIGHT;
                wrappedMutexUnlock(&p->mutex);
                //dont move further down
                enemyMove(p, p->row, p->col);
                wrappedMutexUnlock(&p->mutex);
            }
            //otherwise enemy now moving left
            else{    
                wrappedMutexLock(&p->mutex);
                p->state = WRAPAROUND_L;
                p->direction = RIGHT;
                wrappedMutexUnlock(&p->mutex);
                enemyMove(p, p->row+ENEMY_HEIGHT, p->col);
            }
            
        }
        //tail reach end of left end
        if(p->col < (0 - p->length)){
            wrappedMutexLock(&p->mutex);
            p->state = RIGHT_E;
            p->col += p->length*2;
            wrappedMutexUnlock(&p->mutex);
            enemyMove(p, p->row, p->col+ENEMY_HEIGHT);
            
        }
        //head reach right end
        if(p->col == GAME_COLS+1 && p->direction == RIGHT) 
        {
            //enemy is in the last row 
            if(p->row >= ENEMY_LAST_ROW){ 
                wrappedMutexLock(&p->mutex);
                p->state = WRAPAROUND_R;
                p->direction = LEFT;
                wrappedMutexUnlock(&p->mutex);
                //dont move further down
                enemyMove(p, p->row, p->col);
                wrappedMutexUnlock(&p->mutex);
            }
            else{
                wrappedMutexLock(&p->mutex);
                p->direction = LEFT;
                p->state = WRAPAROUND_R;            
                wrappedMutexUnlock(&p->mutex);
                enemyMove(p, p->row+ENEMY_HEIGHT, p->col);
            }
            
            
        }
        //tail reach end of right end:
        if(p->col > (GAME_COLS + p->length)){
            wrappedMutexLock(&p->mutex);
            p->state = LEFT_E;
            p->col -= p->length*2;
            wrappedMutexUnlock(&p->mutex);
            enemyMove(p, p->row, p->col-2);
        }

        //increment animation tile
        wrappedMutexLock(&p->mutex);
        p->animTile++;
		p->animTile %= ENEMY_ANIM_TILES;        
        wrappedMutexUnlock(&p->mutex);

        //redraw moved enemy, lock = true	
		enemyRedraw(p, true); 
		
        //fire bullet at random
        if( p->col+1 < GAME_COLS && rand()%p->length == 0){
            enemyBullet * eBtoAdd =spawnEnemyBullet(p->row + 1, p->col); 
            enemyBulletList* eBL = (enemyBulletList*)(malloc(sizeof(enemyBulletList)));

            if(eBL == NULL) return NULL; //check for malloc failure

            eBL->eB = eBtoAdd;
            
            if(eBnodeHead == NULL)eBnodeHead = eBL;
            else{
            wrappedMutexLock(&enemyBulletMutex);
            enemyBulletList * cur = eBnodeHead;                                
                //traverse to end of list                
                while(cur->next != NULL)
                {
                    cur = cur->next;
                }        
                cur->next = eBL;
            }
            wrappedMutexUnlock(&enemyBulletMutex);
        }

        //sleep according to enemy speed
        sleepTicks(p->speed);

	}
	return NULL;
}
void enemyMove(enemy *f, int dRow, int dCol)
{

	wrappedMutexLock(&f->mutex);
    //move enemy to specified locations
	f->tailCol = dCol + f->length;
	f->tailRow = f->row;
	f->col = dCol;
	f->row = dRow;
	wrappedMutexUnlock(&f->mutex);
	
}

void enemySplit(enemy * p)
{
    wrappedMutexLock(&p->mutex);
    //switch back to stored previous state
    p->state = p->prevState;
    wrappedMutexUnlock(&p->mutex);
}


void deleteEnemy(enemy * p)
{
    enemy *temp = p;
    //LAST ENEMY IN LL
    if(p->next == NULL){
        free(p);
    }
    else if(p == head){
        head = p-> next;
        free(p);
    }
    else{
        p->next = temp->next;
    }

}

void cleanEnemy()
{
    enemy *current = head;
    enemy * prev;

    if(current == NULL){
        return;
    }
    //if head is dead
    else if (current != NULL && current->state == DEAD_E ){
        
        if(current->next) return;

        wrappedMutexLock(&caterpillarMutex);
        if(current->next !=NULL)
        head = current->next;
        free(current);
        wrappedMutexUnlock(&caterpillarMutex);
        

        return; //dont kill head,  we lose pointer
    }
    
    //find not running node
    while(current->state != DEAD_E && current != NULL && current->next != NULL){

        prev = current;
        current = current->next;
       
    }
    if(current == NULL || prev == NULL) return;
    
    if(prev->next != NULL || current->next != NULL){
    
        //remove current node    
        wrappedMutexLock(&caterpillarMutex);
        prev->next = current->next;
        wrappedMutexUnlock(&caterpillarMutex);

        free(current);
        
    }
    
}

void deleteEnemyList(){
    enemy *current = head;
    enemy * next = NULL;

    if(current == NULL){
        return;
    }
    
    wrappedMutexLock(&caterpillarMutex);
    while (current != NULL) 
    {
        next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
    wrappedMutexUnlock(&caterpillarMutex);
}

bool allEnemiesDead(){
    enemy *current = head;

    wrappedMutexLock(&caterpillarMutex);
    //only move to next enemy node if current enemy is dead
    while (current != NULL && (current->state == DEAD_E || current->running==false)) 
    {
        current = current->next;  
    }
    wrappedMutexUnlock(&caterpillarMutex);
    if(current == NULL ) return true;
    else return false;
}

void joinEnemyBullets(enemyBullet * eB){

        wrappedPthreadJoin(eB->thread,NULL);
        free(eB);
}


