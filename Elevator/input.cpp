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
		printf("\noutput.log��������⣬���򼴽��˳�������output.log�Ƿ����ڱ���������򿪣�\n");
		Sleep(3000);
		exit(0);
	};//�����¼
	char temp[100];//��ʱ�ַ�����
	int tempi;//��ʱ���ִ洢
	int state = Q1;//״̬
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
			printf("���������\n");
		break;
	case 2 :
		if (temp[0] < ('A' + Num) && temp[0] >= 'A') {
			if (temp[1] == '>')
				PostThreadMessage(*(dElevator + temp[0] - 'A'), DOOR_CLOSE, 0, dInput);
			else if (temp[1] == '<')
				PostThreadMessage(*(dElevator + temp[0] - 'A'), DOOR_OPEN, 0, dInput);
			else printf("���������\n");
		}
		else printf("���������\n");
		break;
	case 1:
		if (temp[0] == '?')
			printf("===========================================\n"
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
		else if (temp[0] == '|') {
			fclose(output);
			ReleaseMutex(Mutex);
		}
		else printf("���������.\n");
		break;
	default: printf("���������.\n");
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

	