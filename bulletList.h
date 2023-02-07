/**********************************************************************
  Module: bulletList.h
  Author: Junseok Lee
  Contains linked list for enemy bullets, and the enemy bullet head

**********************************************************************/
#ifndef BULLET_LIST_H
#define BULLET_LIST_H

#include "bullet.h"

//represents bullet list
typedef struct bullet_node 
{
    bullet * b; //a single bullet
    struct bullet_node * next; //next bullet
	
} bulletList;

//enemy bullet head
struct bullet_node *bNodeHead;




#endif