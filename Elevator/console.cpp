#include <stdio.h>
#include <stdlib.h>
#include "stdafx.h"
#include <math.h>

#define test

LISTNODEPTR WaitSolve;

void consoleMain() {
#ifdef test
	MSG outTemp;

	//WaitSolve 为console的链表的表头， TempSolve为input输入的外部请求节点，PostSolve为将要抛出处理的节点
	WaitSolve = NewNode();
	LISTNODEPTR TempSolve, PostSolve;
	PostSolve = WaitSolve->nextPtr;   //将抛出处理节点设置为头结点

	while (TRUE) {
		GetMessage(&outTemp, 0, 0, 0);
		//一个电梯改变，则全部等于0 的都算一遍；
		if (outTemp.message == STATUS_CHANGE) { //电梯状态有改变
			//TempSolve = (LISTNODEPTR)outTemp.wParam;//改变状态的电梯抛回的外部请求，将抛回的外部请求的指针给TempSolve
			//	TempSolve->occupied = 0;//将此节点设置为未占用
			if (WaitSolve->nextPtr == NULL)
				printf("已无外部请求可处理\n");
			else {
				PostSolve = WaitSolve->nextPtr;
				while (PostSolve != NULL) {
					if ((PostSolve->occupied == 0 || PostSolve->occupied == outTemp.lParam)&& PostSolve->available == 1 && PostSolve->running == 0)
						PostThreadMessage(dTimer, STATUS_CHANGE, (WPARAM)PostSolve, dConsole);
					PostSolve = PostSolve->nextPtr;
				}
			}
		}
		//modified by mky
		//电梯新增内部请求
		else if (outTemp.message == NEW_INPUT) {
			//解析请求
			PostSolve = WaitSolve->nextPtr;
			DWORD de = outTemp.lParam;
			while (PostSolve != NULL) {
				if (PostSolve->occupied == de && PostSolve != (LISTNODE*)outTemp.wParam && PostSolve->available == 1) {
					PostThreadMessage(dTimer, THROW_OUT, (WPARAM)PostSolve, dConsole);
				}
				PostSolve = PostSolve->nextPtr;
			}
		}
		else if ((outTemp.message >= 2001 && outTemp.message <= 2099) && (outTemp.wParam == 'H' || outTemp.wParam == 'L')) {   //将input输入的上行外部请求加入链表
			LISTNODE* temp = exsist(WaitSolve, outTemp.message - 2000, outTemp.wParam);
			if (temp) {//是否存在相同的请求
				if (temp->available == 0)
					PostThreadMessage(temp->occupied, DOOR_OPEN, 0, dConsole);
				else 
					PostThreadMessage(dConsole, OUT_REINPUT + outTemp.message - 2000, outTemp.wParam, dConsole);
			}
			else {
				TempSolve = NewNode();
				TempSolve->f = outTemp.message - 2000;
				TempSolve->d = outTemp.wParam;
				add(WaitSolve, TempSolve);
				PostThreadMessage(dTimer, OUT_REQUEST, (WPARAM)TempSolve, dConsole);
			}
		}
		else if (outTemp.message >= OUT_REINPUT) {//input输入了重复的外部请求
			PostSolve = exsist(WaitSolve, outTemp.message - OUT_REINPUT, outTemp.wParam);
			if (PostSolve->running != 0) {
				if (abs((Elevator + (QueryElevator(PostSolve->occupied)))->floor - PostSolve->f) > 1) {
					PostSolve->available = 0;
					PostThreadMessage(PostSolve->occupied, OUT_REINPUT, (WPARAM)PostSolve, dConsole);
				}
			}
			else {
				PostSolve->available = 0;//将重复的外部请求删除
				del(&PostSolve);
			}
		}

	}

#endif
#ifndef test
	MSG a;
	while (1) {
		GetMessage(&a, 0, 0, 0);
		printf("consoleHANDLE=%d\n", GetCurrentThreadId());
		printf("a.message=%u,a.lpram=%u\n", a.message, a.lParam);
	}
#endif

}