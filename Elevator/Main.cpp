

#include <stdio.h>
#include <time.h>
#include "stdafx.h"

extern void inputMain(void);
extern void elevatorMain(void);
extern void timerMain(void);
extern void consoleMain(void);

#define debug

//#pragma comment( lib, "opengl32.lib" ) 
//#pragma comment( lib, "glu32.lib" ) 
#pragma comment( lib, "glut32.lib")

#ifndef test
int consolemain() {
	//��ȡ�����ļ�
	start:
	FILE *config = fopen("config.ini", "r");
	if (config == NULL) {
		printf("config.ini�ļ������ڣ��������½�������\n");
		if ((config = fopen("config.ini", "w")) == NULL) {
			printf("\nconfig.iniд�������⣬���򼴽��˳�������config�Ƿ����ڱ���������򿪣�\n");
			Sleep(3000);
			exit(0);
		}
		fprintf(config,"Num=3\nMaxFloor=9\nRunSpeed=3000\nWaitTime=3000\nDoorTime=1000\n");
		fclose(config);
		goto start;
	}
#ifdef debug
	if ((fscanf(config, "Num=%d", &Num) == 1 && fscanf(config, "\nMaxFloor=%d", &MaxFloor) == 1 && fscanf(config, "\nRunSpeed=%d", &RunSpeed) == 1 && fscanf(config, "\nWaitTime=%d", &WaitTime) == 1) && fscanf(config, "\nDoorTime=%d", &DoorTime) == 1 && Num <= 9 && Num >= 1 && MaxFloor >= 2 && MaxFloor <= 99)
#endif
#ifndef debug
	if ((fscanf(config, "Num=%d", &Num) == 1 && fscanf(config, "\nMaxFloor=%d", &MaxFloor) == 1 && fscanf(config, "\nRunSpeed=%d", &RunSpeed) == 1 && fscanf(config, "\nWaitTime=%d", &WaitTime) == 1) && fscanf(config, "\nDoorTime=%d", &DoorTime) == 1 && Num <= 9 && Num >= 1 && MaxFloor >= 2 && MaxFloor <= 99 && RunSpeed >= 1000 && WaitTime >= 1000 && DoorTime >= 100 && DoorTime <= 1000)
#endif
		;
	else {
		printf("config.ini������Ч����Ч��ΧΪ����λms����\n1=<Num=<9��99=>MaxFloor>=2��RunSpeed>=1000��WaitTime>=1000��1000>=DoorTime>=500\n�����ؽ�config.ini����\n");
		fclose(config);
		if ((config = fopen("config.ini", "w")) == NULL) {
			printf("\nconfig.iniд�������⣬���򼴽��˳�������config.ini�Ƿ����ڱ���������򿪣�\n");
			Sleep(3000);
			exit(0);
		}
		fprintf(config, "Num=3\nMaxFloor=9\nRunSpeed=1800\nWaitTime=3000\nDoorTime=1000\n");
		fclose(config);
		goto start;
	}
	fclose(config);
	//����input�߳�
	hInput = CreateThread(0, 0, (routine)inputMain, 0, 0, &dInput);
	Sleep(100);
	//��ʼ����������
	hElevator = (HANDLE*)malloc(sizeof(HANDLE)* Num);
	dElevator = (DWORD*)malloc(sizeof(DWORD)* Num);
	for (int i = 0; i < Num; i++) {
		*(hElevator + i) = CreateThread(0, 0, (routine)elevatorMain, 0, 0, dElevator + i);
	}
	//���������߳�
	hConsole = CreateThread(0, 0, (routine)consoleMain, 0, 0, &dConsole);
	hTimer = CreateThread(0, 0, (routine)timerMain, 0, 0, &dTimer);
		//�û���ʾ��
		printf("��ʼ�����.\n");
		printf("\n===========================================\n"
			"�Իس�����һ��ָ��Ľ����������ܿո�ָ���|\n"
			"�����޸��������޸�config.ini              |\n"
			"Ĭ����3�����ݣ�9��¥                      |\n"
			"===========================================\n"
			"\n"
			"**********************************************************************\n"
			"��¥����Ϊ��λ����ʮλ�������0.                                   |\n"
			"�ڵ��ݱ��ΪABCDEFGHI.                                               |\n"
			"�ۼ��롰U��+¥����Ϊĳ��¥�������󣻡�L��+¥����Ϊĳ��¥��������.|\n"
			"�ܼ�����ݱ��+¥������ڲ�¥������.                                 |\n"
			"�ݼ�����ݱ��+��>������������󣻵��ݱ��+��<��Ϊ��������.          |\n"
			"�޼��롰?���Ի�ȡ����.                                               |\n"
			"�����ٴμ�����ͬ����������Ϊȡ������.                              |\n"
			"�����롰|���˳�����.                                                 |\n"
			"**********************************************************************\n");
		//�ȴ���������ͷ�
		WaitForSingleObject(Mutex, INFINITE);
		//�˳���ʾ��
		printf("ллʹ�ã��ټ���");
#ifndef debug
		Sleep(2000);
#endif
	return 0;
}
#endif
#ifndef test

#endif
