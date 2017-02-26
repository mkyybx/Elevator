#include <windows.h>
#include "listnode.h"
#include <stdlib.h>

struct elevator {
	char direction;// �����з���H / L / S����Ŀ�꣩��
	LISTNODE *current; //����ǰĿ��¥��ṹ��
	int stay;//�Ƿ�ͣ��
	int floor; //����ǰ����¥�㣬����Ŀ�ĵ�ʱ���ģ�
	int door;//����״̬��
	LISTNODE* queue; //��������У�
};

int Num = 9;//�����ݸ�����3
int MaxFloor = 20;//�����¥������9
int RunSpeed = 1000;//�������ٶȣ���ÿ�㣩5000
int WaitTime = 500;//���ȴ�ʱ�䣬��ÿ�Σ�3000
int DoorTime = 250;//��������ʱ�䣩2000

struct elevator *Elevator = (elevator*)malloc(sizeof(elevator) * Num);

//��¼�߳̾��
HANDLE hInput;
HANDLE hTimer;
HANDLE hConsole;
HANDLE *hElevator;
DWORD dInput;
DWORD dTimer;
DWORD dConsole;
DWORD *dElevator;

//������
HANDLE Mutex;//���߳����жϽ���

//���ݵ���ID���ҵڼ�������
int QueryElevator(DWORD d) {
	for (int i = 0; i < Num; i++) {
		if (*(dElevator + i) == d)
			return i;
	}
	return -1;
}

//����safe��
BOOL nearSafe(int currentFloor, LISTNODE* currentTarget, int stay, char currentDirection, int newTarget ,char newDirection, int para) {//paraΪ0ֻ�ж�safe
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
