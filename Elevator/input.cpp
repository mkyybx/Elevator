#include <stdio.h>
#include "stdafx.h"
#include <string.h>

#define debug

#define Q1 0
#define Q2 1
#define Q3 2
#define Q4 3
#define Q5 4
#define Q6 5
#define Q7 6
#define error 7
#define finished 8

FILE *output;

#ifndef test
void inputMain() {
	Mutex = CreateMutex(0, 1, 0);
	if ((output = fopen("output.log", "w")) == NULL) {
		printf("\noutput.log输出有问题，程序即将退出，请检查output.log是否正在被其他程序打开！\n");
		Sleep(3000);
		exit(0);
	};//输出记录
	char temp[100];//临时字符数组
	int tempi;//临时数字存储
	int state = Q1;//状态
start:
	gets(temp);
	fputc(' ', output);
	fputs(temp, output);
	fputc(' ', output);
	int length = strlen(temp);
	char a = temp[0];
	switch (length) {
	case 3:
		state = Q2;
		while (state != error && state != finished) {
			switch (state) {
			case Q2:
				if (temp[0] == 'U' || temp[0] == 'L')
					state = Q3;
				else if (temp[0] >= 'A' && temp[0] <= 'A' + Num - 1)
					state = Q3;
				else
					state = error;
				break;
			case Q3:
				if (temp[1] >= '0' && temp[1] <= '9')
					state = Q4;
				else state = error;
				break;
			case Q4:
				if (temp[2] >= '0' && temp[2] <= '9')
					state = Q5;
				else state = error;
				break;
			case Q5:
				tempi = (temp[1] - '0') * 10 + (temp[2] - '0');
				if (tempi > MaxFloor)
					state = error;
				else {
					state = finished;
					if (temp[0] == 'U') {
						if (tempi != MaxFloor)
							PostThreadMessage(dConsole, 2000 + tempi, 'H', dInput);
						else state = error;
					}
					else if (temp[0] == 'L') {
						if (tempi != 1)
							PostThreadMessage(dConsole, 2000 + tempi, 'L', dInput);
						else state = error;
					}
					else if (temp[0] >= 'A' && temp[0] <= 'A' + Num - 1)
						PostThreadMessage (*(dElevator + temp[0] - 'A'),2000+tempi,'I',dInput);
					else state = error;
				}	
				break;
			}
		}
		if (state == error)
			printf("错误的请求\n");
		break;
	case 2 :
		if (temp[0] < ('A' + Num) && temp[0] >= 'A') {
			if (temp[1] == '>')
				PostThreadMessage(*(dElevator + temp[0] - 'A'), DOOR_CLOSE, 0, dInput);
			else if (temp[1] == '<')
				PostThreadMessage(*(dElevator + temp[0] - 'A'), DOOR_OPEN, 0, dInput);
			else printf("错误的请求\n");
		}
		else printf("错误的请求\n");
		break;
	case 1:
		if (temp[0] == '?')
			printf("===========================================\n"
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
		else if (temp[0] == '|') {
			fclose(output);
			ReleaseMutex(Mutex);
		}
		else printf("错误的请求.\n");
		break;
	default: printf("错误的请求.\n");
	}
	goto start;
}
#endif
#ifdef test
void inputMain() {
	while (1) {
		Mutex = CreateMutex(0, 1, 0);

	}
}
#endif

	