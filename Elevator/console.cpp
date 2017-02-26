#include <stdio.h>
#include <stdlib.h>
#include "stdafx.h"
#include <math.h>

#define test

LISTNODEPTR WaitSolve;

void consoleMain() {
#ifdef test
	MSG outTemp;

	//WaitSolve Ϊconsole������ı�ͷ�� TempSolveΪinput������ⲿ����ڵ㣬PostSolveΪ��Ҫ�׳�����Ľڵ�
	WaitSolve = NewNode();
	LISTNODEPTR TempSolve, PostSolve;
	PostSolve = WaitSolve->nextPtr;   //���׳�����ڵ�����Ϊͷ���

	while (TRUE) {
		GetMessage(&outTemp, 0, 0, 0);
		//һ�����ݸı䣬��ȫ������0 �Ķ���һ�飻
		if (outTemp.message == STATUS_CHANGE) { //����״̬�иı�
			//TempSolve = (LISTNODEPTR)outTemp.wParam;//�ı�״̬�ĵ����׻ص��ⲿ���󣬽��׻ص��ⲿ�����ָ���TempSolve
			//	TempSolve->occupied = 0;//���˽ڵ�����Ϊδռ��
			if (WaitSolve->nextPtr == NULL)
				printf("�����ⲿ����ɴ���\n");
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
		//���������ڲ�����
		else if (outTemp.message == NEW_INPUT) {
			//��������
			PostSolve = WaitSolve->nextPtr;
			DWORD de = outTemp.lParam;
			while (PostSolve != NULL) {
				if (PostSolve->occupied == de && PostSolve != (LISTNODE*)outTemp.wParam && PostSolve->available == 1) {
					PostThreadMessage(dTimer, THROW_OUT, (WPARAM)PostSolve, dConsole);
				}
				PostSolve = PostSolve->nextPtr;
			}
		}
		else if ((outTemp.message >= 2001 && outTemp.message <= 2099) && (outTemp.wParam == 'H' || outTemp.wParam == 'L')) {   //��input����������ⲿ�����������
			LISTNODE* temp = exsist(WaitSolve, outTemp.message - 2000, outTemp.wParam);
			if (temp) {//�Ƿ������ͬ������
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
		else if (outTemp.message >= OUT_REINPUT) {//input�������ظ����ⲿ����
			PostSolve = exsist(WaitSolve, outTemp.message - OUT_REINPUT, outTemp.wParam);
			if (PostSolve->running != 0) {
				if (abs((Elevator + (QueryElevator(PostSolve->occupied)))->floor - PostSolve->f) > 1) {
					PostSolve->available = 0;
					PostThreadMessage(PostSolve->occupied, OUT_REINPUT, (WPARAM)PostSolve, dConsole);
				}
			}
			else {
				PostSolve->available = 0;//���ظ����ⲿ����ɾ��
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