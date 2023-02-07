/**********************************************************************
  Module: llist.h
  Author: Junseok Lee
  Contains linked list for the enemy. Contains the node head.


**********************************************************************/
#ifndef LLIST_H
#define LLIST_H

#include "enemy.h"

/*Represents a node containing an enemy*/
typedef struct Node 
{
    enemy * e;  //single enemy 
    struct Node * next;   //next node in ll
	
} node;

//linked list node head
struct Node *nodeHead;


#endif