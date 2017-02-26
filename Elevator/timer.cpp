#include <stdio.h>
#include "stdafx.h"
#include <time.h>
#include <math.h>

struct tinytimer {
	int time;//��ʱʱ��
	HANDLE *tinyMutex;//��������ַ
	DWORD elevator;//�����߳�ID
};//����tinyTimer�����ýṹ��

struct a {
	clock_t startTime;//��ʼ��ʱʱ��
	HANDLE *tinyMutex;//�������
	int doorTimes;//��¼�����Ŵ���
	int remainTime;//������ʣ��ʱ��
};//ÿ�����ݵ�ʱ�ӽṹ��

struct a* timer;

//����ĳ�������ⲿ�������
int queryWaitSolve(DWORD de, elevator *e, int target, char targetdirection) {
	LISTNODE* currentPtr = WaitSolve->nextPtr;
	int count = 0;
	if (e->floor == target)
		return 0;
	if (e->direction == 'S') {
		return 0;
	}
	else if (e->direction != targetdirection) {
		if (e->direction == 'H') {
			while (currentPtr != NULL) {
				if ((currentPtr->occupied == de && currentPtr->d == e->direction && currentPtr->f > e->floor) || (currentPtr->occupied == de && currentPtr->d != e->direction && currentPtr->f > target))
					count++;
				currentPtr = currentPtr->nextPtr;
			}
			currentPtr = e->queue->nextPtr;
			while (currentPtr != NULL) {
				int smaller = target < e->floor ? target : e->floor;
				if (currentPtr->f > smaller)
					count++;
				currentPtr = currentPtr->nextPtr;
			}
		}
		else {
			while (currentPtr != NULL) {
				if ((currentPtr->occupied == de && currentPtr->d == e->direction && currentPtr->f < e->floor) || (currentPtr->occupied == de && currentPtr->d != e->direction && currentPtr->f < target))
					count++;
				currentPtr = currentPtr->nextPtr;
			}
			currentPtr = e->queue->nextPtr;
			while (currentPtr != NULL) {
				int bigger = target > e->floor ? target : e->floor;
				if (currentPtr->f < bigger)
					count++;
				currentPtr = currentPtr->nextPtr;
			}
		}
	}
	else if (e->direction == targetdirection) {
		if (e->direction == 'H' ? target > e->floor : target < e->floor) {
			while (currentPtr != NULL) {
				if (currentPtr->occupied == de && currentPtr->d == e->direction && (e->direction == 'H' ? (currentPtr->f > e->floor && currentPtr->f < target) : (currentPtr->f < e->floor && currentPtr->f > target)))
					count++;
				currentPtr = currentPtr->nextPtr;
			}
			currentPtr = e->queue->nextPtr;
			while (currentPtr != NULL) {
				if (e->direction == 'H' ? (currentPtr->f > e->floor && currentPtr->f < target) : (currentPtr->f < e->floor && currentPtr->f > target))
					count++;
				currentPtr = currentPtr->nextPtr;
			}
		}
		else {
			int amount = 0;
			while (currentPtr != NULL) {
				if (currentPtr->occupied == de)
					amount++;
				currentPtr = currentPtr->nextPtr;
			}
			currentPtr = WaitSolve->nextPtr;
			while (currentPtr != NULL) {
				if (currentPtr->occupied == de && currentPtr->d == e->direction && (e->direction == 'L' ? (currentPtr->f > e->floor && currentPtr->f < target) : (currentPtr->f < e->floor && currentPtr->f > target)))
					amount--;
				currentPtr = currentPtr->nextPtr;
			}
			currentPtr = e->queue->nextPtr;
			while (currentPtr != NULL) {
				amount++;
				currentPtr = currentPtr->nextPtr;
			}
			currentPtr = e->queue->nextPtr;
			while (currentPtr != NULL) {
				if ((e->direction == 'L' ? (currentPtr->f > e->floor && currentPtr->f < target) : (currentPtr->f < e->floor && currentPtr->f > target)))
					amount--;
				currentPtr = currentPtr->nextPtr;
			}
			count = amount;

		}
	}
	return count;
}
//���ؼ��¥����
int queryFloor(DWORD de, elevator *e, int target, char targetdirection) {
	LISTNODE* currentPtr = WaitSolve->nextPtr;
	int maxFloor = e->current != NULL ? e->current->f : MaxFloor;
	int minFloor = e->current != NULL ? e->current->f : 0;
	while (currentPtr != NULL) {
		if (currentPtr->occupied == de && maxFloor < e->floor)
			maxFloor = e->floor;
		currentPtr = currentPtr->nextPtr;
	}
	currentPtr = e->queue->nextPtr;
	while (currentPtr != NULL) {
		if (maxFloor < e->floor)
			maxFloor = e->floor;
		currentPtr = currentPtr->nextPtr;
	}
	currentPtr = WaitSolve->nextPtr;
	while (currentPtr != NULL) {
		if (currentPtr->occupied == de && maxFloor > e->floor)
			minFloor = e->floor;
		currentPtr = currentPtr->nextPtr;
	}
	currentPtr = e->queue->nextPtr;
	while (currentPtr != NULL) {
		if (maxFloor > e->floor)
			minFloor = e->floor;
		currentPtr = currentPtr->nextPtr;
	}
	int count = 0;
	if (e->floor == target)
		return 0;
	else if (e->direction == targetdirection || e->direction == 'S') {
		if ((e->direction == 'H' ? target > e->floor : target < e->floor) || (e->direction == 'S'))
			return abs(target - e->floor);
		else {
			if (e->direction == 'H')
				return (2 * maxFloor - (e->floor + target));
			else return (e->floor + target - 2 * minFloor);
		}
	}
	else if (e->direction != targetdirection) {
		if (e->direction == 'H')
			return (maxFloor - e->floor + target - minFloor);
		else return (maxFloor - target + e->floor - minFloor);
	}

	return count;
}

int timePass(clock_t s, clock_t e) {
	return (int)(double)(e - s) * 1000 / CLOCKS_PER_SEC;
}

DWORD WINAPI tinyTimer(void *i) {
	tinytimer *i1 = (struct tinytimer*)i;
	if (WaitForSingleObject(*(i1->tinyMutex), i1->time) == WAIT_TIMEOUT)
		PostThreadMessage(i1->elevator, TIMEUP, 0, dTimer);
	else
		ReleaseMutex(*(i1->tinyMutex));
	free(i1);
	return 0;
}


#define debug

void timerMain() {
#ifdef test
	clock_t start = clock();
	while (1) {
		printf("%d",timePass(start,clock()));
		Sleep(1000);
	}
#endif
#ifndef test
	struct tinytimer* tempt;//��ʱ����
	timer = (struct a*)malloc(sizeof(struct a) * Num);//���ݶ�Ӧ�Ӽ�ʱ�ṹ
	for (int i = 0; i < Num; i++) {
		(timer + i)->tinyMutex = (HANDLE *)malloc(sizeof HANDLE);
		*((timer + i)->tinyMutex) = CreateMutex(0, true, 0);
	}
	MSG temp;
	int tempi;//���ڴ�ŵ������
	HANDLE temph;//��ʱ���
	while (true) {
		GetMessage(&temp, 0, 0, 0);
		tempi = QueryElevator(temp.lParam);
		switch (temp.message) {
		case STATUS_CHANGE :
		case THROW_OUT:
		case OUT_REQUEST:
			//��������
			if (true) {//Ӧ��case��ǩ���ܳ�ʼ�������Ʊ���������
				LISTNODE * templ = (LISTNODE *)(temp.wParam);
				char d = templ->d;//����
				int f = templ->f; //¥��
				int* time = (int *)malloc(sizeof(int)* Num);//ʱ��
				int no = 0;//���
				//������ѡ
				for (int i = 0; i < Num; i++) {
					if (!nearSafe((Elevator + i)->floor, (Elevator + i)->current, (Elevator + i)->stay, (Elevator + i)->direction, f, d, 0))
						*(time + i) = -1;
					else {
						int temp;
						temp = queryFloor(*(dElevator + i),Elevator + i, f, d) * RunSpeed + (queryWaitSolve(*(dElevator + i), Elevator + i, f, d)) * (WaitTime + DoorTime * 2);
						if (!((Elevator + i)->direction == 'S')) {
							if ((Elevator + i)->stay == 1) {
								if ((Elevator + i)->door == OPEN)
									temp += WaitTime - timePass((timer + i)->startTime, clock()) + DoorTime;
								else if ((Elevator + i)->door == OPENING)
									temp += WaitTime + (timer + i)->remainTime + DoorTime;
								else if ((Elevator + i)->door == CLOSEING)
									temp += (timer + i)->remainTime;
							}
							else
								temp -= timePass((timer + i)->startTime, clock());
						}
						*(time + i) = temp;
					}
				}
				int tempi = *time;
				for (int i = 0; i < Num; i++) {
					if (tempi == -1) {
						tempi = *(time + i);
						no = i;
					}
					else if (*(time + i) < tempi && *(time + i) != -1) {
						tempi = *(time + i);
						no = i;
					}
				}
				//�������
				if (tempi != -1) {
					templ->occupied = *(dElevator + no);
					if (nearSafe((Elevator + no)->floor, (Elevator + no)->current, (Elevator + no)->stay, (Elevator + no)->direction, f, d)) {
						PostThreadMessage(*(dElevator + no), OUT_REQUEST, temp.wParam, dTimer);
						templ->running = 1;
					}
				}
				free(time);
			}
			break;
		case TIMER:
			if (true) {
				(timer + tempi)->startTime = clock();
				tempt = (tinytimer *)malloc(sizeof tinytimer);
				tempt->time = temp.wParam;
				tempt->elevator = temp.lParam;
				tempt->tinyMutex = (timer + tempi)->tinyMutex;
				temph = CreateThread(0, 0, (routine)tinyTimer, tempt, 0, 0);
				CloseHandle(temph);
			}
			break;
		case RESET_TIMER:
			ReleaseMutex(*((timer + tempi)->tinyMutex));
			WaitForSingleObject(*((timer + tempi)->tinyMutex), INFINITE);
			(timer + tempi)->startTime = 0;
			(timer + tempi)->doorTimes = 0;
			(timer + tempi)->remainTime = DoorTime;
			break;
		case DOOR_TIMER:
			if ((timer + tempi)->doorTimes == 0)
				PostThreadMessage(dTimer, TIMER, DoorTime, temp.lParam);
			else {
				ReleaseMutex(*((timer + tempi)->tinyMutex));
				WaitForSingleObject(*((timer + tempi)->tinyMutex), INFINITE);
				int timePassi = timePass((timer + tempi)->startTime, clock());
				(timer + tempi)->remainTime -= timePassi;
				(timer + tempi)->remainTime = DoorTime - (timer + tempi)->remainTime;
				//printf("renmain = %d ", (timer + tempi)->remainTime);
				//printf("timepass=%d\n", timePassi);
				tempt = (tinytimer *)malloc(sizeof tinytimer);
				tempt->time = (timer + tempi)->remainTime;
				tempt->elevator = temp.lParam;
				tempt->tinyMutex = (timer + tempi)->tinyMutex;
				temph = CreateThread(0, 0, (routine)tinyTimer, tempt, 0, 0);
				(timer + tempi)->startTime = clock();
				CloseHandle(temph);
			}
			(timer + tempi)->doorTimes++;
			break;
		}
	}


#endif
}