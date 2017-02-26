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
		//��ptrŪ��ĩβ
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
	printf("����%u,��ǰ¥��:%d,",QueryElevator(GetCurrentThreadId()) + 1,e->floor);
	printf("��ǰĿ��:");
	fprintf(output,"����%u,��ǰ¥��:%d,", QueryElevator(GetCurrentThreadId()) + 1, e->floor);
	fprintf(output,"��ǰĿ��:");
	if (e->current == NULL) {
		fprintf(output,"��,");
		printf("��,");
	}
	else if (e->current->d == 'I') {
		printf("�ڲ�����%d��,", e->current->f);
		fprintf(output,"�ڲ�����%d��,", e->current->f);
	}
	else {
		printf("�ⲿ����%d��,", e->current->f);
		fprintf(output, "�ⲿ����%d��,", e->current->f);
	}
	e->stay == 1 ? printf("ͣ��,") : printf("����,");
	e->stay == 1 ? fprintf(output,"ͣ��,") : fprintf(output,"����,");
	printf("��״̬:");
	fprintf(output,"��״̬:");
	switch (e->door) {
		case CLOSE: printf("�ر�,");
			fprintf(output,"�ر�,");
			break;
		case OPEN: printf("����,");
			fprintf(output,"����,");
			break;
		case OPENING: printf("������,");
			fprintf(output,"������,");
			break;
		case CLOSEING: printf("�ر���,");
			fprintf(output,"�ر���,");
			break;
	}
	printf("����:");
	fprintf(output,"����:");
	if (e->direction == 'H') {
		fprintf(output,"����");
		printf("����");
	}
	else if (e->direction == 'L') {
		fprintf(output,"����");
		printf("����");
	}
	else {
		printf("����");
		fprintf(output, "����");
	}
	printf(".");
	printf("\n�ڲ�δ��Ӧ����:");
	fprintf(output,".");
	fprintf(output,"\n�ڲ�δ��Ӧ����:");
	LISTNODE *cPtr = e->queue->nextPtr;
	if (cPtr == 0) {
		printf("��.");
		fprintf(output,"��.");
	}
	else {
		while (cPtr != NULL) {
			printf("%d%c ", cPtr->f, cPtr->d);
			fprintf(output,"%d%c ", cPtr->f, cPtr->d);
			cPtr = cPtr->nextPtr;
		}
	}
	printf("�ⲿδ��Ӧ����:");
	fprintf(output,"�ⲿδ��Ӧ����:");
	cPtr = WaitSolve->nextPtr;
	if (cPtr == 0) {
		printf("��.");
		fprintf(output,"��.");
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
	struct elevator *e = Elevator + QueryElevator(GetCurrentThreadId());//��ǰ���ݽṹ
	//��ʼ��
	e->direction = 'S';
	e->stay = 1;
	e->current = NULL;
	e->floor = 1;
	e->queue = NewNode();
	e->door = CLOSE;
	MSG temp;
	while (true) {
		GetMessage(&temp, 0, 0, 0);
		//�ڲ�����
		if (temp.message >= 2001 && temp.message <= 2099 && temp.wParam == 'I') {
			int tempf = temp.message - 2000;//��������
			//�ظ���ɾ��
			if (exsist(e->queue, tempf, 'I'))
				PostThreadMessage(GetCurrentThreadId(), OUT_REINPUT, (WPARAM)exsist(e->queue, tempf, 'I'), GetCurrentThreadId());
			else if (tempf == e->floor && e->stay == 1)//ͬ¥�㴦��
				PostThreadMessage(dWinMain, WM_USER + 'I', tempf, GetCurrentThreadId());
			else {
				LISTNODE *templ = NewNode();//�����½ڵ�
				templ->d = 'I';
				templ->f = tempf;
				add(e->queue, templ);
				//����ǰ����
				PostThreadMessage(dConsole, NEW_INPUT, (WPARAM)(e->current), GetCurrentThreadId());
				if (e->current == NULL) {
					e->direction = tempf > e->floor ? 'H' : 'L';
					if (e->door == CLOSE) {
						e->current = templ;
						PostThreadMessage(dTimer, TIMER, RunSpeed, GetCurrentThreadId());
						e->stay = 0;
					}
				}
				//����ǰΪ�ڲ�Ŀ��
				else if (e->current->d == 'I') {
					if (nearSafe(e->floor, e->current, e->stay, e->direction, tempf, 'I'))
						e->current = templ;
				}
				//����ǰΪ�ⲿĿ��
				else if (e->current->d == 'H' || e->current->d == 'L') {
					if (nearSafe(e->floor, e->current, e->stay, e->direction, tempf, temp.wParam)) {
						e->current->running = 0;
						PostThreadMessage(dConsole, THROW_OUT, (WPARAM)e->current, GetCurrentThreadId());
						e->current = templ;
					}
				}
			}
		}
		//�ⲿ����timer������
		else if (temp.message == OUT_REQUEST) {
			//��������;
			LISTNODE *templ = (LISTNODE*)temp.wParam;
			int tempf = templ->f;
			char tempd = templ->d;
			//ͬ¥�㴦��
			if (e->stay == 1 && e->floor == tempf && (e->direction == tempd || e->direction == 'S')) {
				PostThreadMessage(GetCurrentThreadId(), DOOR_OPEN, 0, GetCurrentThreadId());
				((LISTNODE *)temp.wParam)->available = 0;
				del((LISTNODE **)(&temp.wParam));
			}
			//��ǰ����
			else if (e->current == NULL) {
				e->direction = tempf > e->floor ? 'H' : 'L';
				if (e->door == CLOSE) {
					e->current = templ;
					e->stay = 0;
					PostThreadMessage(dTimer, TIMER, RunSpeed, GetCurrentThreadId());
				}
			}
			//��ǰΪ�ڲ�Ŀ��
			else if (e->current->d == 'I')
				e->current = templ;
			//��ǰΪ�ⲿĿ��
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
			//�����ݵ�ǰ������
			if (e->stay == 0) {
				e->direction == 'H' ? e->floor++ : e->floor--;
				//������ط���
				if (e->floor == e->current->f) {
					e->stay = 1;
					e->current->available = 0;
					PostThreadMessage(GetCurrentThreadId(), DOOR_OPEN, 0, GetCurrentThreadId());
				}
				//���û��
				else {
					PostThreadMessage(dTimer, TIMER, RunSpeed, GetCurrentThreadId());
				}
			}
			//������ͣ��
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
						//��Ѱ��һĿ��
						LISTNODE *templ = findTarget(e, e->direction);
						if (templ != NULL) {
							del(&(e->current));
							e->current = templ;
							templ->running = 1;
						}
						//�������
						else {
							templ = findTarget(e, e->direction == 'H' ? 'L' : 'H');
							//��û��Ŀ��
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
			//������console����ɾ������
			if (temp.lParam == dConsole) {
				//��Ѱ��һĿ��
				LISTNODE *templ = findTarget(e, e->direction);
				if (templ != NULL)
					e->current = templ;
				//�������
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
				//���ݷ�����ɾ���ڲ�����
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