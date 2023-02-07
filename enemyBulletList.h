/**********************************************************************
  Module: enemyBulletList.h
  Author: Junseok Lee
  Contains linked list for enemy bullets, and the enemy bullet head

**********************************************************************/
#ifndef ENEMY_BULLET_LIST_H
#define ENEMY_BULLET_LIST_H

#include "enemyBullet.h"

//represents enmey bullet list
typedef struct enemy_bullet_node 
{
    enemyBullet * eB; //a single enemy bullet
    struct enemy_bullet_node * next; //next enemy bullet
	
} enemyBulletList;

//enemy bullet head
struct enemy_bullet_node *eBnodeHead;




#endif