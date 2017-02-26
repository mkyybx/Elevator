// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#include <windows.h>
#include <stdlib.h>
#include "listnode.h"

struct elevator {
	char direction;// �����з���H / L / S����Ŀ�꣩��
	LISTNODE *current; //����ǰĿ��¥��ṹ��
	int stay;//�Ƿ�ͣ��
	int floor; //����ǰ����¥�㣬����Ŀ�ĵ�ʱ���ģ�
	int door;//����״̬��
	LISTNODE* queue; //��������У�
};

extern struct elevator* Elevator;

#define routine LPTHREAD_START_ROUTINE

#define DOOR_CLOSE 2300//�����ݹ����¼���
#define DOOR_OPEN 2301//�����ݿ����¼���
#define TIMER 2302//����ʱ�¼���
#define DOOR_TIMER 2303//���������¼���
#define TIMEUP 2304//����ʱ����ʱ�¼���
#define RESET_TIMER 2305//����ʱ�������¼���
#define NEW_INPUT 2306//�������ڲ�����������
#define STATUS_CHANGE 2307//�����ݵ�ͷ��
#define THROW_OUT 2309//�������׳��ⲿ�����¼���
#define OUT_REQUEST 2310//��console֪ͨ�����ⲿ�����¼���
#define OUT_REINPUT 3000//��ɾ����Ϣ������


//��״̬
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

extern LISTNODEPTR WaitSolve;//console��������print

extern HANDLE Mutex;

extern int QueryElevator(DWORD d);
BOOL nearSafe(int currentFloor, LISTNODE* currentTarget, int stay, char currentDirection, int newTarget, char newDirection, int para);
BOOL nearSafe(int currentFloor, LISTNODE* currentTarget, int stay, char currentDirection, int newTarget, char newDirection);





// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
