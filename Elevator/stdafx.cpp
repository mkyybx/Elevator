#include <windows.h>
#include "listnode.h"
#include <stdlib.h>

struct elevator {
	char direction;// （运行方向H / L / S（无目标））
	LISTNODE *current; //（当前目的楼层结构）
	int stay;//是否停靠
	int floor; //（当前所在楼层，到达目的地时更改）
	int door;//（门状态）
	LISTNODE* queue; //（任务队列）
};

int Num = 9;//（电梯个数）3
int MaxFloor = 20;//（最大楼层数）9
int RunSpeed = 1000;//（运行速度，秒每层）5000
int WaitTime = 500;//（等待时间，秒每次）3000
int DoorTime = 250;//（开关门时间）2000

struct elevator *Elevator = (elevator*)malloc(sizeof(elevator) * Num);

//记录线程句柄
HANDLE hInput;
HANDLE hTimer;
HANDLE hConsole;
HANDLE *hElevator;
DWORD dInput;
DWORD dTimer;
DWORD dConsole;
DWORD *dElevator;

//互斥句柄
HANDLE Mutex;//主线程用判断结束

//依据电梯ID查找第几部电梯
int QueryElevator(DWORD d) {
	for (int i = 0; i < Num; i++) {
		if (*(dElevator + i) == d)
			return i;
	}
	return -1;
}

//近且safe？
BOOL nearSafe(int currentFloor, LISTNODE* currentTarget, int stay, char currentDirection, int newTarget ,char newDirection, int para) {//para为0只判断safe
	if (para == 0) {
		if (currentDirection == newDirection && currentDirection == 'L' ? currentFloor - newTarget == 1 : currentFloor - newTarget == 1)
			return false;
		return true;
	}
	else {
		if (currentDirection == 'S')
			return true;
		else if (currentDirection == newDirection || newDirection == 'I') {
			if (stay == 1 ? 1 : abs(currentFloor - newTarget) > 1) {
				if (currentDirection == 'H' ? (newTarget <= currentTarget->f && currentFloor < newTarget) : (newTarget >= currentTarget->f && currentFloor > newTarget))
					return true;
			}
		}
		return false;
	}
}
BOOL nearSafe(int currentFloor, LISTNODE* currentTarget, int stay, char currentDirection, int newTarget, char newDirection) {
	return nearSafe(currentFloor, currentTarget, stay, currentDirection, newTarget, newDirection, 1);
}
