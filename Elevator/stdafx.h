// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#include <windows.h>
#include <stdlib.h>
#include "listnode.h"

struct elevator {
	char direction;// （运行方向H / L / S（无目标））
	LISTNODE *current; //（当前目的楼层结构）
	int stay;//是否停靠
	int floor; //（当前所在楼层，到达目的地时更改）
	int door;//（门状态）
	LISTNODE* queue; //（任务队列）
};

extern struct elevator* Elevator;

#define routine LPTHREAD_START_ROUTINE

#define DOOR_CLOSE 2300//（电梯关门事件）
#define DOOR_OPEN 2301//（电梯开门事件）
#define TIMER 2302//（计时事件）
#define DOOR_TIMER 2303//（开关门事件）
#define TIMEUP 2304//（计时器到时事件）
#define RESET_TIMER 2305//（计时器重置事件）
#define NEW_INPUT 2306//（电梯内部出现新请求）
#define STATUS_CHANGE 2307//（电梯调头）
#define THROW_OUT 2309//（电梯抛出外部请求事件）
#define OUT_REQUEST 2310//（console通知电梯外部请求事件）
#define OUT_REINPUT 3000//（删除消息基数）


//门状态
#define CLOSE 1
#define OPEN 2
#define OPENING 3
#define CLOSEING 4

extern int Num;
extern int MaxFloor;
extern int RunSpeed;
extern int WaitTime;
extern int DoorTime;

extern HANDLE hInput;
extern HANDLE hTimer;
extern HANDLE hConsole;
extern HANDLE *hElevator;
extern DWORD dInput;
extern DWORD dTimer;
extern DWORD dConsole;
extern DWORD dWinMain;
extern DWORD *dElevator;

extern LISTNODEPTR WaitSolve;//console链表，用于print

extern HANDLE Mutex;

extern int QueryElevator(DWORD d);
BOOL nearSafe(int currentFloor, LISTNODE* currentTarget, int stay, char currentDirection, int newTarget, char newDirection, int para);
BOOL nearSafe(int currentFloor, LISTNODE* currentTarget, int stay, char currentDirection, int newTarget, char newDirection);





// TODO:  在此处引用程序需要的其他头文件
