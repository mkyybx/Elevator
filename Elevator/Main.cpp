

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
	//读取配置文件
	start:
	FILE *config = fopen("config.ini", "r");
	if (config == NULL) {
		printf("config.ini文件不存在，正在重新建立……\n");
		if ((config = fopen("config.ini", "w")) == NULL) {
			printf("\nconfig.ini写入有问题，程序即将退出，请检查config是否正在被其他程序打开！\n");
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
		printf("config.ini数据无效，有效范围为（单位ms）：\n1=<Num=<9，99=>MaxFloor>=2，RunSpeed>=1000，WaitTime>=1000，1000>=DoorTime>=500\n正在重建config.ini……\n");
		fclose(config);
		if ((config = fopen("config.ini", "w")) == NULL) {
			printf("\nconfig.ini写入有问题，程序即将退出，请检查config.ini是否正在被其他程序打开！\n");
			Sleep(3000);
			exit(0);
		}
		fprintf(config, "Num=3\nMaxFloor=9\nRunSpeed=1800\nWaitTime=3000\nDoorTime=1000\n");
		fclose(config);
		goto start;
	}
	fclose(config);
	//建立input线程
	hInput = CreateThread(0, 0, (routine)inputMain, 0, 0, &dInput);
	Sleep(100);
	//初始化电梯数组
	hElevator = (HANDLE*)malloc(sizeof(HANDLE)* Num);
	dElevator = (DWORD*)malloc(sizeof(DWORD)* Num);
	for (int i = 0; i < Num; i++) {
		*(hElevator + i) = CreateThread(0, 0, (routine)elevatorMain, 0, 0, dElevator + i);
	}
	//建立其他线程
	hConsole = CreateThread(0, 0, (routine)consoleMain, 0, 0, &dConsole);
	hTimer = CreateThread(0, 0, (routine)timerMain, 0, 0, &dTimer);
		//用户提示语
		printf("初始化完毕.\n");
		printf("\n===========================================\n"
			"以回车代表一个指令的结束，不接受空格分隔符|\n"
			"若须修改配置请修改config.ini              |\n"
			"默认有3部电梯，9层楼                      |\n"
			"===========================================\n"
			"\n"
			"**********************************************************************\n"
			"①楼层编号为两位，若十位以下请加0.                                   |\n"
			"②电梯编号为ABCDEFGHI.                                               |\n"
			"③键入“U”+楼层编号为某层楼上行请求；“L”+楼层编号为某层楼下行请求.|\n"
			"④键入电梯编号+楼层代表内部楼层请求.                                 |\n"
			"⑤键入电梯编号+“>”代表关门请求；电梯编号+“<”为开门请求.          |\n"
			"⑥键入“?”以获取帮助.                                               |\n"
			"⑦若再次键入相同的请求，则视为取消请求.                              |\n"
			"⑧输入“|”退出程序.                                                 |\n"
			"**********************************************************************\n");
		//等待互斥对象释放
		WaitForSingleObject(Mutex, INFINITE);
		//退出提示语
		printf("谢谢使用，再见！");
#ifndef debug
		Sleep(2000);
#endif
	return 0;
}
#endif
#ifndef test

#endif
