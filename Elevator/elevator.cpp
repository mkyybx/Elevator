#include <stdio.h>
#include "stdafx.h"
#include <math.h>
#include <time.h>

#define debug

extern FILE *output;

LISTNODE *findTarget(elevator* e, char direction) {
	LISTNODE* innerTarget = (e->current == NULL || e->current->d != 'I') ? query(e->queue, e->floor, direction) : query(e->queue, e->current, direction);
	LISTNODE* outTarget = WaitSolve->nextPtr;
	DWORD id = GetCurrentThreadId();
	if (direction == 'H') {
		while (outTarget != NULL) {
			if (outTarget->f >= e->floor) {
				if (outTarget->occupied == GetCurrentThreadId()) {
					if (direction == outTarget->d && outTarget->available == 1) {
						if (outTarget->f == e->floor) {
							outTarget->available = 0;
							LISTNODE* tempTarget = outTarget->nextPtr;
							del(&outTarget);
							outTarget = tempTarget;
							continue;
						}
						else break;
					}
				}
			}
			outTarget = outTarget->nextPtr;
		}
	}
	else {
		//把ptr弄到末尾
		outTarget = WaitSolve;
		while (outTarget->nextPtr != NULL)
			outTarget = outTarget->nextPtr;
		while (outTarget ->f != 0) {
			if (outTarget->f <= e->floor && outTarget->f != 0) {
				if (outTarget->occupied == GetCurrentThreadId()) {
					if (direction == outTarget->d && outTarget->available == 1) {
						if (outTarget->f == e->floor) {
							outTarget->available = 0;
							LISTNODE* tempTarget = outTarget->previousPtr;
							del(&outTarget);
							outTarget = tempTarget;
							continue;
						}
						else break;
					}
				}
			}
			outTarget = outTarget->previousPtr;
		}
	}
	if (outTarget == NULL || outTarget->f == 0 || innerTarget == NULL)
		return (outTarget == NULL || outTarget->f == 0) ? innerTarget : outTarget;
	else {
		if (direction == 'H')
			return innerTarget->f < outTarget->f ? innerTarget : outTarget;
		else return innerTarget->f > outTarget->f ? innerTarget : outTarget;

	}
}

void printStatus(struct elevator *e, UINT message) {
	
	fprintf(output, "%dms: message=%u", clock() * 1000 / CLOCKS_PER_SEC, message);
	printf("电梯%u,当前楼层:%d,",QueryElevator(GetCurrentThreadId()) + 1,e->floor);
	printf("当前目标:");
	fprintf(output,"电梯%u,当前楼层:%d,", QueryElevator(GetCurrentThreadId()) + 1, e->floor);
	fprintf(output,"当前目标:");
	if (e->current == NULL) {
		fprintf(output,"无,");
		printf("无,");
	}
	else if (e->current->d == 'I') {
		printf("内部请求%d层,", e->current->f);
		fprintf(output,"内部请求%d层,", e->current->f);
	}
	else {
		printf("外部请求%d层,", e->current->f);
		fprintf(output, "外部请求%d层,", e->current->f);
	}
	e->stay == 1 ? printf("停靠,") : printf("运行,");
	e->stay == 1 ? fprintf(output,"停靠,") : fprintf(output,"运行,");
	printf("门状态:");
	fprintf(output,"门状态:");
	switch (e->door) {
		case CLOSE: printf("关闭,");
			fprintf(output,"关闭,");
			break;
		case OPEN: printf("开启,");
			fprintf(output,"开启,");
			break;
		case OPENING: printf("开启中,");
			fprintf(output,"开启中,");
			break;
		case CLOSEING: printf("关闭中,");
			fprintf(output,"关闭中,");
			break;
	}
	printf("方向:");
	fprintf(output,"方向:");
	if (e->direction == 'H') {
		fprintf(output,"上行");
		printf("上行");
	}
	else if (e->direction == 'L') {
		fprintf(output,"下行");
		printf("下行");
	}
	else {
		printf("空闲");
		fprintf(output, "空闲");
	}
	printf(".");
	printf("\n内部未响应请求:");
	fprintf(output,".");
	fprintf(output,"\n内部未响应请求:");
	LISTNODE *cPtr = e->queue->nextPtr;
	if (cPtr == 0) {
		printf("无.");
		fprintf(output,"无.");
	}
	else {
		while (cPtr != NULL) {
			printf("%d%c ", cPtr->f, cPtr->d);
			fprintf(output,"%d%c ", cPtr->f, cPtr->d);
			cPtr = cPtr->nextPtr;
		}
	}
	printf("外部未响应请求:");
	fprintf(output,"外部未响应请求:");
	cPtr = WaitSolve->nextPtr;
	if (cPtr == 0) {
		printf("无.");
		fprintf(output,"无.");
	}
	else {
		while (cPtr != NULL) {
#ifndef debug
			printf("%d%c ", cPtr->f, cPtr->d);
			fprintf(output,"%d%c ", cPtr->f, cPtr->d);
#endif
#ifdef debug
			printf("%d%c(%d) ", cPtr->f, cPtr->d,QueryElevator(cPtr->occupied) + 1);
			fprintf(output, "%d%c(%d) ", cPtr->f, cPtr->d, QueryElevator(cPtr->occupied) + 1);
#endif
			cPtr = cPtr->nextPtr;
		}
	}
	printf("\n\n");
	fprintf(output, "\n\n");
	
}


void elevatorMain() {
#ifndef test
	struct elevator *e = Elevator + QueryElevator(GetCurrentThreadId());//当前电梯结构
	//初始化
	e->direction = 'S';
	e->stay = 1;
	e->current = NULL;
	e->floor = 1;
	e->queue = NewNode();
	e->door = CLOSE;
	MSG temp;
	while (true) {
		GetMessage(&temp, 0, 0, 0);
		//内部请求
		if (temp.message >= 2001 && temp.message <= 2099 && temp.wParam == 'I') {
			int tempf = temp.message - 2000;//解析请求
			//重复则删除
			if (exsist(e->queue, tempf, 'I'))
				PostThreadMessage(GetCurrentThreadId(), OUT_REINPUT, (WPARAM)exsist(e->queue, tempf, 'I'), GetCurrentThreadId());
			else if (tempf == e->floor && e->stay == 1)//同楼层处理
				PostThreadMessage(dWinMain, WM_USER + 'I', tempf, GetCurrentThreadId());
			else {
				LISTNODE *templ = NewNode();//建立新节点
				templ->d = 'I';
				templ->f = tempf;
				add(e->queue, templ);
				//若当前空闲
				PostThreadMessage(dConsole, NEW_INPUT, (WPARAM)(e->current), GetCurrentThreadId());
				if (e->current == NULL) {
					e->direction = tempf > e->floor ? 'H' : 'L';
					if (e->door == CLOSE) {
						e->current = templ;
						PostThreadMessage(dTimer, TIMER, RunSpeed, GetCurrentThreadId());
						e->stay = 0;
					}
				}
				//若当前为内部目标
				else if (e->current->d == 'I') {
					if (nearSafe(e->floor, e->current, e->stay, e->direction, tempf, 'I'))
						e->current = templ;
				}
				//若当前为外部目标
				else if (e->current->d == 'H' || e->current->d == 'L') {
					if (nearSafe(e->floor, e->current, e->stay, e->direction, tempf, temp.wParam)) {
						e->current->running = 0;
						PostThreadMessage(dConsole, THROW_OUT, (WPARAM)e->current, GetCurrentThreadId());
						e->current = templ;
					}
				}
			}
		}
		//外部请求（timer发来）
		else if (temp.message == OUT_REQUEST) {
			//解析请求;
			LISTNODE *templ = (LISTNODE*)temp.wParam;
			int tempf = templ->f;
			char tempd = templ->d;
			//同楼层处理
			if (e->stay == 1 && e->floor == tempf && (e->direction == tempd || e->direction == 'S')) {
				PostThreadMessage(GetCurrentThreadId(), DOOR_OPEN, 0, GetCurrentThreadId());
				((LISTNODE *)temp.wParam)->available = 0;
				del((LISTNODE **)(&temp.wParam));
			}
			//当前空闲
			else if (e->current == NULL) {
				e->direction = tempf > e->floor ? 'H' : 'L';
				if (e->door == CLOSE) {
					e->current = templ;
					e->stay = 0;
					PostThreadMessage(dTimer, TIMER, RunSpeed, GetCurrentThreadId());
				}
			}
			//当前为内部目标
			else if (e->current->d == 'I')
				e->current = templ;
			//当前为外部目标
			else if (e->current->d == 'H' || e->current->d == 'L') {
				e->current->running = 0;
				PostThreadMessage(dTimer, THROW_OUT, (WPARAM)e->current, GetCurrentThreadId());
				e->current = templ;
			}
		}
		else if (temp.message == DOOR_OPEN) {
			if (e->stay == 1) {
				if (e->door == CLOSE) {
					PostThreadMessage(dTimer, RESET_TIMER, 0, GetCurrentThreadId());
					PostThreadMessage(dTimer, DOOR_TIMER, 0, GetCurrentThreadId());
					e->door = OPENING;
				}
				else if (e->door == OPEN) {
					PostThreadMessage(dTimer, RESET_TIMER, 0, GetCurrentThreadId());
					PostThreadMessage(dTimer, TIMER, WaitTime, GetCurrentThreadId());
				}
				else if (e->door == CLOSEING) {
					PostThreadMessage(dTimer, DOOR_TIMER, 0, GetCurrentThreadId());
					e->door = OPENING;
				}
			}
		}
		else if (temp.message == DOOR_CLOSE) {
			if (e->stay == 1) {
				if (e->door == OPENING) {
					PostThreadMessage(dTimer, DOOR_TIMER, 0, GetCurrentThreadId());
					e->door = CLOSEING;
				}
				else if (e->door == OPEN) {
					PostThreadMessage(dTimer, RESET_TIMER, 0, GetCurrentThreadId());
					PostThreadMessage(dTimer, DOOR_TIMER, 0, GetCurrentThreadId());
					e->door = CLOSEING;
				}
			}
		}
		else if (temp.message == TIMEUP) {
			//若电梯当前运行中
			if (e->stay == 0) {
				e->direction == 'H' ? e->floor++ : e->floor--;
				//如果到地方了
				if (e->floor == e->current->f) {
					e->stay = 1;
					e->current->available = 0;
					PostThreadMessage(GetCurrentThreadId(), DOOR_OPEN, 0, GetCurrentThreadId());
				}
				//如果没到
				else {
					PostThreadMessage(dTimer, TIMER, RunSpeed, GetCurrentThreadId());
				}
			}
			//若电梯停靠
			else {
				switch (e->door) {
				case OPEN:
					e->door = CLOSEING;
					PostThreadMessage(dTimer, RESET_TIMER, 0, GetCurrentThreadId());
					PostThreadMessage(dTimer, DOOR_TIMER, 0, GetCurrentThreadId());
					break;
				case CLOSEING:
					if (true) {
						e->door = CLOSE;
						//搜寻下一目标
						LISTNODE *templ = findTarget(e, e->direction);
						if (templ != NULL) {
							del(&(e->current));
							e->current = templ;
							templ->running = 1;
						}
						//反向查找
						else {
							templ = findTarget(e, e->direction == 'H' ? 'L' : 'H');
							//若没有目标
							if (templ == NULL) {
								del(&(e->current));
								e->current = NULL;
								e->direction = 'S';
							}
							else {
								del(&(e->current));
								e->current = templ;
								e->direction = e->direction == 'H' ? 'L' : 'H';
								templ->running = 1;
							}
							PostThreadMessage(dConsole, STATUS_CHANGE, 0, GetCurrentThreadId());
						}
						if (e->direction != 'S') {
							e->stay = 0;
							PostThreadMessage(dTimer, TIMER, RunSpeed, GetCurrentThreadId());
						}
					}
					break;
				case OPENING:
					e->door = OPEN;
					PostThreadMessage(dTimer, RESET_TIMER, 0, GetCurrentThreadId());
					PostThreadMessage(dTimer, TIMER, WaitTime, GetCurrentThreadId());
					break;
				}
			}
		}
		else if (temp.message == OUT_REINPUT) {
			//若果是console发来删除外请
			if (temp.lParam == dConsole) {
				//搜寻下一目标
				LISTNODE *templ = findTarget(e, e->direction);
				if (templ != NULL)
					e->current = templ;
				//反向查找
				else {
					if (e->stay == 0) {
						LISTNODE *tempa = NewNode();
						tempa->d = 'I';
						tempa->f = e->floor + (e->direction == 'H' ? 2 : -2);
						add(e->queue, tempa);
						e->current = tempa;
					}
					else
						PostThreadMessage(GetCurrentThreadId(), (2000 + e->floor), 'I', GetCurrentThreadId());
				}
				del((LISTNODE**)(&temp.wParam));
			}
			if (temp.lParam == GetCurrentThreadId()) {
				//电梯发来的删除内部请求
				if (e->current == (LISTNODE*)temp.wParam && abs(e->current->f - e->floor) > 1){
					LISTNODE *templ = findTarget(e, e->direction);
					if (templ != NULL)
						e->current = templ;
					else {
						if (e->stay == 0) {
							LISTNODE *tempa = NewNode();
							tempa->d = 'I';
							tempa->f = e->floor + (e->direction == 'H' ? 2 : -2);
							add(e->queue, tempa);
							e->current = tempa;
						}
						else
							PostThreadMessage(GetCurrentThreadId(), (2000 + e->floor), 'I', GetCurrentThreadId());
						PostThreadMessage(dConsole, STATUS_CHANGE, 0, GetCurrentThreadId());
					}
					((LISTNODE*)temp.wParam)->available = 0;
					del((LISTNODE**)(&temp.wParam));
				}
				else if (e->current != (LISTNODE*)temp.wParam) {
					((LISTNODE*)temp.wParam)->available = 0;
					del((LISTNODE**)(&temp.wParam));
				}
			}
		}
		printStatus(e,temp.message);
	}
#endif
#ifdef test
	LISTNODE *a = (LISTNODE *)malloc(sizeof(LISTNODE));
	a->d = 'A';
	a->f = 1;
	a->nextPtr = 0;
	LISTNODE *b = (LISTNODE *)malloc(sizeof(LISTNODE));
	b->d = 'B';
	b->f = 2;
	b->nextPtr = 0;
	//printf("%d %d", a, (WPARAM)a);
	//add(a, b);
	/*
	MSG a;
	while (1) {
		GetMessage(&a, 0, 0, 0);
		printf("elevator=%d\n", GetCurrentThreadId());
		printf("a.message=%u,a.lpram=%u\n", a.message, a.lParam);
	}
	*/
#endif

}