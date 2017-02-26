//
//  main.c
//  add
//
//  Created by liu on 15/5/2.
//  Copyright (c) 2015年 liu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

extern DWORD dWinMain;

struct listNode
{
    int f;
    char d;
	int occupied;
	int running;
	int available;
    struct listNode*nextPtr,*previousPtr;
};

typedef struct listNode LISTNODE;
typedef LISTNODE* LISTNODEPTR;
void add(LISTNODEPTR headPtr,LISTNODEPTR aPtr);
void del(LISTNODEPTR* aPtr);
LISTNODEPTR query(LISTNODEPTR headPtr,LISTNODEPTR aPtr,char direction);
LISTNODEPTR next(LISTNODEPTR headPtr,LISTNODEPTR aPtr);
void freelist(LISTNODEPTR headPtr);

//added by mky
LISTNODEPTR exsist(LISTNODE *headPtr, int f, char d) {
	LISTNODE* currentPtr = headPtr->nextPtr;
	while (currentPtr != NULL) {
		if (currentPtr->d == d && currentPtr->f == f)
			return currentPtr;
		currentPtr = currentPtr->nextPtr;
	}
	return NULL;
}
//end

///////////////////////////////////////////////////////////////////////////////////////////////////
void add(LISTNODEPTR headPtr,LISTNODEPTR aPtr)
{
    LISTNODEPTR currentPtr=headPtr;
    while(currentPtr!=NULL)
    {
        
        if(currentPtr->previousPtr==headPtr&&aPtr->f < currentPtr->f)
        {
            aPtr->nextPtr=currentPtr;
            headPtr->nextPtr=aPtr;
            currentPtr->previousPtr=aPtr;
            aPtr->previousPtr=headPtr;
            break;
        }
        if(currentPtr->nextPtr==NULL&&aPtr->f >= currentPtr->f)
        {
            aPtr->previousPtr=currentPtr;
            currentPtr->nextPtr=aPtr;
            break;
        }
        if(currentPtr->f <= aPtr->f&&(currentPtr->nextPtr)->f > aPtr->f)
        {
            aPtr->nextPtr=currentPtr->nextPtr;
            (currentPtr->nextPtr)->previousPtr=aPtr;
            currentPtr->nextPtr=aPtr;
            aPtr->previousPtr=currentPtr;
            break;
        }
        currentPtr=currentPtr->nextPtr;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void del(LISTNODEPTR* aPtr)
{
	if (*aPtr != NULL) {
		if (dWinMain != 0)
			PostThreadMessage(dWinMain, WM_USER + (*aPtr)->d, (*aPtr)->f, GetCurrentThreadId());
		((*aPtr)->previousPtr)->nextPtr = (*aPtr)->nextPtr;
		if ((*aPtr)->nextPtr != NULL)
			((*aPtr)->nextPtr)->previousPtr = (*aPtr)->previousPtr;
		free(*aPtr);
		*aPtr = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
LISTNODEPTR query(LISTNODEPTR headPtr,LISTNODEPTR aPtr,char direction)
{
    if(direction=='H')
    {
        if(aPtr->nextPtr==NULL) return NULL;
        else if(aPtr->nextPtr->available == 1) return aPtr->nextPtr;
    }
    else
    {
        if(aPtr->previousPtr==headPtr) return NULL;
		else  if (aPtr->previousPtr->available == 1) return aPtr->previousPtr;
    }
}
//modified by mky
LISTNODEPTR query(LISTNODEPTR headPtr, int f, char d, int para)//para为1则删除节点
{
	LISTNODEPTR currentPtr = NULL;
	if (headPtr->nextPtr == NULL)
		return NULL;


	if (d == 'H')
	{
		currentPtr = headPtr;
		while (currentPtr != NULL)
		{
			if (currentPtr->f == f && para == 1)
				del(&currentPtr);
			else {
				if (currentPtr->nextPtr != NULL && currentPtr->nextPtr->f > f && currentPtr->f <= f)
					return currentPtr->nextPtr;
				currentPtr = currentPtr->nextPtr;
			}
		}
		return NULL;
	}

	if (d == 'L')
	{
		currentPtr = headPtr->nextPtr;
		while (currentPtr != NULL)
		{
			if (currentPtr->f == f && para == 1)
				del(&currentPtr);
			else {
				if (currentPtr->previousPtr != headPtr && currentPtr->previousPtr->f < f && currentPtr->f >= f)
					return currentPtr->previousPtr;
				if (currentPtr->nextPtr == NULL && currentPtr->f < f)
					return currentPtr;
				currentPtr = currentPtr->nextPtr;
			}
		}
		return NULL;
	}
	return NULL;
}
	LISTNODEPTR query(LISTNODEPTR headPtr, int f, char d) {
		return query(headPtr, f, d, 1);
	}


/////////////////////////////////////////////////////////////////////////////////////////////////
LISTNODEPTR next(LISTNODEPTR headPtr,LISTNODEPTR aPtr)
{
//有问题的代码
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void freelist(LISTNODEPTR headPtr)
{
    LISTNODEPTR temPtr=NULL;
    while (headPtr!=NULL)
    {
        temPtr=headPtr;
        headPtr=headPtr->nextPtr;
        free(temPtr);
    }
    free(headPtr);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//新建并初始化节点
LISTNODE *NewNode()
{
    LISTNODEPTR headPtr;
    headPtr=(LISTNODEPTR)malloc(sizeof(LISTNODE));
    headPtr->d = 0;
    headPtr->f = 0;
	headPtr->occupied = 0;
	headPtr->running = 0;
	headPtr->available = 1;
    headPtr->nextPtr = NULL;
    headPtr->previousPtr=NULL;
    return headPtr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void move(LISTNODEPTR headPtr, LISTNODEPTR aPtr)
{
	(aPtr->previousPtr)->nextPtr = aPtr->nextPtr;
	aPtr->nextPtr == NULL ? 0 : aPtr->nextPtr->previousPtr = aPtr->previousPtr;
	add(headPtr, aPtr);
	//free(aPtr);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
LISTNODEPTR search(LISTNODEPTR headPtr, int f)
{
	LISTNODEPTR currentPtr;
	currentPtr = headPtr;
	while (currentPtr != NULL)
	{
		if (currentPtr->f == f)
			return currentPtr;
		else
			currentPtr = currentPtr->nextPtr;
	}
	return currentPtr;
}

