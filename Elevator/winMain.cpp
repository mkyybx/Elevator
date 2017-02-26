#include <stdio.h>
#include "stdafx.h"
#include "glut.h"
#include <time.h>
#include <math.h>

//#define testgl
#define texture

//#pragma comment( lib, "opengl32.lib" ) 
//#pragma comment( lib, "glu32.lib" ) 
#pragma comment( lib, "glut32.lib")

extern int consolemain();
extern void elevatorMain(void);
extern void timerMain(void);
extern void consoleMain(void);
extern int timePass(clock_t s, clock_t e);
extern void inputMain(void);

struct a {
	clock_t startTime;//��ʼ��ʱʱ��
	HANDLE *tinyMutex;//�������
	int doorTimes;//��¼�����Ŵ���
	int remainTime;//������ʣ��ʱ��
};//ÿ�����ݵ�ʱ�ӽṹ��
extern struct a *timer;

double currentw = 720 * 0.9 / (MaxFloor + 1) / 0.57 * (Num + 1);//��ǰ���ڿ��
double currenth = 720;//��ǰ���ڸ߶�
double wlength = currentw / (Num + 1);//��ǰ���ڸ��ӿ��
double hlength = currenth / (MaxFloor + 1);//��ǰ���ڸ��Ӹ߶�
double ***c;//���������������飬0��1Ϊ�������꣬2��3Ϊ��һ����ť����,4��5Ϊ�ڶ�����ť���꣬6Ϊ�����µİ�ť��ţ�1��2����7��8Ϊ��һ������ť�Ƿ���
double length;//���ݱ߳�
double blength;//��ť�߳�
double dblength;//�����Ű�ť�߳�
int fontChanged = 0;//�����Ƿ�ı�

//��������
GLuint u, up, d, dp, bg, bg1, o, op, close, cp;

DWORD dWinMain = 0;

int power_of_two(int n)
{
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}

GLuint load_texture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint texture_ID = 0;
	GLint last_texture_ID = 0;
	// ���ļ������ʧ�ܣ����� 
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0)
		return 0;
	// ��ȡ�ļ���ͼ��Ŀ�Ⱥ͸߶� 
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, 54, SEEK_SET);
	// ����ÿ��������ռ�ֽ����������ݴ����ݼ����������ֽ��� 
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}
	// �����������ֽ��������ڴ� 
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}
	// ��ȡ�������� 
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}
	// �ھɰ汾�� OpenGL �� 
	// ���ͼ��Ŀ�Ⱥ͸߶Ȳ��ǵ������η�������Ҫ�������� 
	// ���ﲢû�м�� OpenGL �汾�����ڶ԰汾�����ԵĿ��ǣ����ɰ汾���� 
	// ���⣬�����Ǿɰ汾�����°汾�� 
	// ��ͼ��Ŀ�Ⱥ͸߶ȳ�����ǰ OpenGL ʵ����֧�ֵ����ֵʱ��ҲҪ�������� 
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!power_of_two(width)
			|| !power_of_two(height)
			|| width > max
			|| height > max)
		{
			const GLint new_width = 256;
			const GLint new_height = 256; // �涨���ź��µĴ�СΪ�߳��������� 
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;
			// ����ÿ����Ҫ���ֽ��������ֽ��� 
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;
			// �����ڴ� 
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}
			// ������������ 
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);
			// �ͷ�ԭ�����������ݣ��� pixels ָ���µ��������ݣ����������� width �� height 
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}
	// ����һ���µ������� 
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}
	// ���µ������������������������ 
	// �ڰ�ǰ���Ȼ��ԭ���󶨵������ţ��Ա��������лָ� 
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, last_texture_ID);
	// ֮ǰΪ pixels ������ڴ����ʹ�� glTexImage2D �Ժ��ͷ� 
	// ��Ϊ��ʱ���������Ѿ��� OpenGL ���б�����һ�ݣ����ܱ����浽ר�ŵ�ͼ��Ӳ���У� 
	free(pixels);
	return texture_ID;
}

//ѡ������
void selectFont(int size, int charset, const char* face) {
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

//�������ֺ���
// ASCII�ַ��ܹ�ֻ��0��127��һ��128���ַ�
#define MAX_CHAR       128
#define FLOOR 1
#define BUTTON 2
void drawString(int para, const char* str, int fontsize) {
	static int isFirstCall = 1;
	static GLuint floorlists;
	static GLuint buttonlists;
	//ͬ������ı�
	static int buttonchanged = 0;
	static int floorchanged = 0;
	if (isFirstCall) { // ����ǵ�һ�ε��ã�ִ�г�ʼ��
		// ����MAX_CHAR����������ʾ�б���
		floorlists = glGenLists(MAX_CHAR);
		buttonlists = glGenLists(MAX_CHAR);
		isFirstCall = 0;
	}
	// ��ÿ���ַ��Ļ������װ����Ӧ����ʾ�б���
	if (fontChanged) {
		selectFont(fontsize, ANSI_CHARSET, "GungsuhChe");
		//selectFont(fontsize, ANSI_CHARSET, "Consolas");
		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, para == FLOOR ? floorlists : buttonlists);
		para == FLOOR ? floorchanged = 1 : buttonchanged = 1;
		//printf("button=%d,floor=%d\n", buttonchanged, floorchanged);
	}
	if (buttonchanged && floorchanged && fontChanged) {
		buttonchanged = 0;
		floorchanged = 0;
		fontChanged = 0;
	}
	// ����ÿ���ַ���Ӧ����ʾ�б�����ÿ���ַ�
	if (para == FLOOR) {
		for (; *str != '\0'; ++str)
			glCallList(floorlists + *str);
	}
	else if (para == BUTTON) {
		for (; *str != '\0'; ++str)
			glCallList(buttonlists + *str);
	}
}

void drawBackground() {
	wlength = currentw / (Num + 1);
	hlength = currenth / (MaxFloor + 1);
	//��ʼ��
	for (int i = 1; i < MaxFloor + 2; i++) {
		for (int j = 1; j < Num + 2; j++) {
			c[i][j][0] = -currentw / 2 + wlength*(j-1);
			c[i][j][1] = -currenth / 2 + hlength*(i-1);
		}
	}
	glColor3ub(0, 0, 0);
#ifdef testgl
	for (int i = 0; i < Num; i++) {
		glBegin(GL_LINES); {
			glVertex2d(-currentw / 2 + wlength*(i + 1), currenth / 2);
			glVertex2d(-currentw / 2 + wlength*(i + 1), -currenth / 2);
		}
		glEnd();
	}
#endif
#ifdef testgl
	for (int i = 0; i < MaxFloor; i++) {
		glBegin(GL_LINES); {
			glVertex2d(currentw / 2, -currenth / 2 + hlength*(i + 1));
			glVertex2d(-currentw / 2, -currenth / 2 + hlength*(i + 1));
		}
		glEnd();
	}
#endif
	//���Ƶ��ݾ�
#ifdef texture
	glBindTexture(GL_TEXTURE_2D, bg);
#endif
	for (int i = 1; i <= Num; i++) {
		glBegin(GL_POLYGON); {
			glTexCoord2d(0, 0);
			glVertex2d(-currentw / 2 + wlength * i - wlength * 0.05, currenth / 2 - 0.95*hlength);
			glTexCoord2d(1, 0);
			glVertex2d(-currentw / 2 + wlength * i + wlength * 0.05, currenth / 2 - 0.95*hlength);
			glTexCoord2d(1, 1);
			glVertex2d(-currentw / 2 + wlength * i + wlength * 0.05, -currenth / 2);
			glTexCoord2d(0, 1);
			glVertex2d(-currentw / 2 + wlength * i - wlength * 0.05, -currenth / 2);
		}
		glEnd();
	}
	for (int i = 1; i <= Num; i++) {
		glBegin(GL_POLYGON); {
			glTexCoord2d(0, 0);
			glVertex2d(-currentw / 2 + wlength * i + wlength * 0.05 + length, currenth / 2 - 0.95*hlength);
			glTexCoord2d(1, 0);
			glVertex2d(-currentw / 2 + wlength * i + wlength * 0.15 + length, currenth / 2 - 0.95*hlength);
			glTexCoord2d(1, 1);
			glVertex2d(-currentw / 2 + wlength * i + wlength * 0.15 + length, -currenth / 2);
			glTexCoord2d(0, 1);
			glVertex2d(-currentw / 2 + wlength * i + wlength * 0.05 + length, -currenth / 2);
		}
		glEnd();
	}
	//�����컨��
#ifdef texture
	glBindTexture(GL_TEXTURE_2D, bg1);
#endif
	for (int i = 1; i <= MaxFloor; i++) {
		glBegin(GL_POLYGON); {
			glTexCoord2d(0, 0);
			glVertex2d(-currentw / 2 + wlength, -currenth / 2 + hlength*i - 0.05*hlength);
			glTexCoord2d(1, 0);
			glVertex2d(currentw / 2, -currenth / 2 + hlength*i - 0.05*hlength);
			glTexCoord2d(1, 1);
			glVertex2d(currentw / 2, -currenth / 2 + hlength*i + hlength*0.05);
			glTexCoord2d(0, 1);
			glVertex2d(-currentw / 2 + wlength, -currenth / 2 + hlength*i + 0.05*hlength);
		}
		glEnd();
		//printf("%.2lf ", (-currenth / 2 + hlength*i + 0.05*hlength + currenth / 2 - hlength*i + 0.05*hlength)/hlength);
	}
#ifdef texture
	glBindTexture(GL_TEXTURE_2D, 0);
#endif
}

void reshape(int w, int h) {
	//  
	GLfloat aspectRatio;
	//  ��ֹ��0����
	if (0 == h)
		h = 1;
	//  �����ӿ�Ϊ���ڵĴ�С
	glViewport(0, 0, w, h);
	//  ѡ��ͶӰ���󣬲���������ϵͳ
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//  ���㴰�ڵ��ݺ�ȣ����رȣ�
	aspectRatio = (GLfloat)w / (GLfloat)h;
	//  ����ü����򣨸��ݴ��ڵ��ݺ�ȣ���ʹ����ͶӰ��
	if (w <= h)// �� < ��
		glOrtho(-1.0, 1.0, -1 / aspectRatio, 1 / aspectRatio, 1.0, -1.0);
	else // �� > ��
		glOrtho(-1.0 * aspectRatio, 1.0 *aspectRatio, -1.0, 1.0, 1.0, -1.0);
	//  ѡ��ģ����ͼ���󣬲���������ϵͳ
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	int smaller = h > w ? w : h;
	glScaled(1.0 / smaller * 2, 1.0 / smaller * 2, 0);
	currentw = w;
	currenth = h;
	fontChanged = 1;
	//printf("1\n");
}


void myDisplay(void) {
	//printf("excute!");
	glClear(GL_COLOR_BUFFER_BIT);
#ifdef testgl
	glColor3ub(0, 0, 255);
	glBegin(GL_LINES);
	glVertex2i(0, 0);
	glVertex2i(100, 100);
	glEnd();
	glBegin(GL_LINES);
	glVertex2i(0, -currenth);
	glVertex2i(0, currenth);
	glEnd();
	glBegin(GL_LINES);
	glVertex2i(-currentw, 0);
	glVertex2i(currentw, 0);
	glEnd();
	for (int i = 1; i <= Num + 1; i++) {
		glBegin(GL_LINES);
		glVertex2d(-currentw / 2 + wlength * i + wlength * 1 / 3, currenth / 2);
		glVertex2d(-currentw / 2 + wlength * i + wlength * 1 / 3, -currenth / 2);
		glEnd();
		glBegin(GL_LINES);
		glVertex2d(-currentw / 2 + wlength * i + wlength * 1 / 2, currenth / 2);
		glVertex2d(-currentw / 2 + wlength * i + wlength * 1 / 2, -currenth / 2);
		glEnd();
	}
#endif
	//glutSwapBuffers();
}

void idleAnimation() {
	static char tempc[3];
	myDisplay();
	//glClear(GL_COLOR_BUFFER_BIT);
	//������
	//�߳�
	length = wlength / 3 * 2 - 0.10 * wlength > hlength * 0.9 ? hlength * 0.9 : wlength / 3 * 2 - 0.10 * wlength;
	//��������
	static double *ex = (double*)malloc(sizeof(double) * Num);
	static double *ey = (double*)malloc(sizeof(double) * Num);
	for (int i = 0; i < Num; i++) {
		if ((Elevator + i)->stay == 1) {
			*(ex + i) = c[(Elevator + i)->floor][i + 2][0];
			*(ey + i) = c[(Elevator + i)->floor][i + 2][1];
		}
		else {
			*(ex + i) = c[(Elevator + i)->floor][i + 2][0];
			//printf("%d %d %f\n", (timer + i)->startTime, clock(), ((double)timePass((timer + i)->startTime, clock()) / RunSpeed * hlength) * ((Elevator + i)->direction == 'L' ? 1 : 1));
			*(ey + i) = c[(Elevator + i)->floor][i + 2][1] + ((double)timePass((timer + i)->startTime, clock()) / RunSpeed * hlength) * ((Elevator + i)->direction == 'L' ? -1 : 1);
		}
		glColor3ub(64, 128, 255);
		glBegin(GL_POLYGON); {
			glVertex2d(*(ex + i) + 0.05 * wlength, *(ey + i) + 0.05 * hlength);
			glVertex2d(*(ex + i) + 0.05 * wlength + length, *(ey + i) + 0.05 * hlength);
			glVertex2d(*(ex + i) + 0.05 * wlength + length, *(ey + i) + length + 0.05 * hlength);
			glVertex2d(*(ex + i) + 0.05 * wlength, *(ey + i) + length + 0.05 * hlength);
		}
		glEnd();
		//����
		glColor3ub(37, 52, 103);
		if ((Elevator + i)->door == CLOSE) {
			glBegin(GL_POLYGON); {
				glVertex2d(*(ex + i) + 0.05 * wlength, *(ey + i) + 0.05 * hlength);
				glVertex2d(*(ex + i) + 0.05 * wlength + length / 2 - 1, *(ey + i) + 0.05 * hlength);
				glVertex2d(*(ex + i) + 0.05 * wlength + length / 2 - 1, *(ey + i) + length + 0.05 * hlength);
				glVertex2d(*(ex + i) + 0.05 * wlength, *(ey + i) + length + 0.05 * hlength);
			}
			glEnd();
			glBegin(GL_POLYGON); {
				glVertex2d(*(ex + i) + 0.05 * wlength + length / 2 + 1, *(ey + i) + 0.05 * hlength);
				glVertex2d(*(ex + i) + 0.05 * wlength + length, *(ey + i) + 0.05 * hlength);
				glVertex2d(*(ex + i) + 0.05 * wlength + length, *(ey + i) + length + 0.05 * hlength);
				glVertex2d(*(ex + i) + 0.05 * wlength + length / 2 + 1, *(ey + i) + length + 0.05 * hlength);
			}
			glEnd();
		}
		else if ((Elevator + i)->door == OPEN)
			;
		else {
			static double doorlength;//�ų���
			static double baselength;//remaintime��ʾ����
			if ((Elevator + i)->door == OPENING || (Elevator + i)->door == CLOSEING) {
				Sleep(1);//�˲�����ֹ�ж�Ϊopening��˲����ݽ���closing
				switch ((Elevator + i)->door) {
				case OPENING:
					baselength = (double)(timer + i)->remainTime / DoorTime * (length / 2 - 1);
					doorlength = baselength - (double)(clock() - (timer + i)->startTime) / (timer + i)->remainTime * baselength;
					//printf("remaintime=%d,doorlength=%f,baselength=%f,length=%f\n", (timer + i)->remainTime, doorlength, baselength, length / 2 - 1);
					break;
				case CLOSEING:
					baselength = (1 - (double)(timer + i)->remainTime / DoorTime) * (length / 2 - 1);
					doorlength = baselength + (double)(clock() - (timer + i)->startTime) / (timer + i)->remainTime * (length / 2 - 1 - baselength);
					//printf("doorlength=%f,baselength=%f,length=%f\n", doorlength, baselength, length / 2 - 1);
					break;

				}

				glBegin(GL_POLYGON); {
					glVertex2d(*(ex + i) + 0.05 * wlength, *(ey + i) + 0.05 * hlength);
					glVertex2d(*(ex + i) + 0.05 * wlength + doorlength, *(ey + i) + 0.05 * hlength);
					glVertex2d(*(ex + i) + 0.05 * wlength + doorlength, *(ey + i) + length + 0.05 * hlength);
					glVertex2d(*(ex + i) + 0.05 * wlength, *(ey + i) + length + 0.05 * hlength);
				}
				glEnd();
				glBegin(GL_POLYGON); {
					glVertex2d(*(ex + i) + 0.05 * wlength + length - doorlength, *(ey + i) + 0.05 * hlength);
					glVertex2d(*(ex + i) + 0.05 * wlength + length, *(ey + i) + 0.05 * hlength);
					glVertex2d(*(ex + i) + 0.05 * wlength + length, *(ey + i) + length + 0.05 * hlength);
					glVertex2d(*(ex + i) + 0.05 * wlength + length - doorlength, *(ey + i) + length + 0.05 * hlength);
				}
				glEnd();
			}
		}
	}
	//���ڲ�����ť
	//һ��㻭
	for (int i = 0; i < MaxFloor; i++) {
		sprintf(tempc, "%d", i + 1);
		//Ѱ����������
		static double *bx = (double*)malloc(sizeof(double)*Num);
		static double *by = (double*)malloc(sizeof(double)*Num);
		double xlength = -(length + wlength * 0.15 - wlength * 0.95) * 2 / 3;
		double ylength = hlength * 0.9 * 2 / 3;
		double smaller = xlength > ylength ? ylength : xlength;
		blength = smaller;
		for (int j = 0; j < Num; j++) {
			bx[j] = (c[i + 1][j + 2][0] + wlength * 0.95 + c[i + 2][j + 2][0] + length + wlength * 0.15) / 2 - smaller / 2;
			by[j] = c[i + 1][j + 2][1] + hlength * 0.50 - smaller / 2;
			c[i + 1][j + 2][2] = bx[j];
			c[i + 1][j + 2][3] = by[j];
		}
		for (int j = 0; j < Num; j++) {
			if (abs(c[i + 1][j + 2][7] - 1) <= 1e-6)
				glColor3ub(111, 142, 191);
			else
				glColor3ub(96, 3, 8);
			if (abs(c[i + 1][j + 2][6] - 1) >= 1e-6) {//����ť���ڱ��������С��
				glBegin(GL_POLYGON); {
					glVertex2d(bx[j], by[j]);
					glVertex2d(bx[j] + smaller, by[j]);
					glVertex2d(bx[j] + smaller, by[j] + smaller);
					glVertex2d(bx[j], by[j] + smaller);
				}
				glEnd();
				glColor3ub(255, 255, 255);
				glRasterPos2d(bx[j] + 0.05 * blength, by[j] + 0.05 * blength);
				drawString(BUTTON, tempc, (int)blength);
			}
			else {
				glBegin(GL_POLYGON); {
					glVertex2d(c[i + 1][j + 2][2] + blength * 0.05, c[i + 1][j + 2][3] + blength * 0.05);
					glVertex2d(c[i + 1][j + 2][2] + blength * 0.95, c[i + 1][j + 2][3] + blength * 0.05);
					glVertex2d(c[i + 1][j + 2][2] + blength * 0.95, c[i + 1][j + 2][3] + blength * 0.95);
					glVertex2d(c[i + 1][j + 2][2] + blength * 0.05, c[i + 1][j + 2][3] + blength * 0.95);
				}
				glEnd();
				glColor3ub(255, 255, 255);
				glRasterPos2d(c[i + 1][j + 2][2] + blength * 1.95 * 0.05, c[i + 1][j + 2][3] + blength * 1.95 * 0.05);
				drawString(BUTTON, tempc, (int)blength * 0.95);
			}
		}
	}
	//���ⲿ����ť
	static double interval;
	//��������
	if (0.95 * wlength - 2 * blength > hlength - 2 * blength) {
		interval = (0.95 * wlength - 2 * blength) / 3;
		for (int i = 1; i <= MaxFloor; i++) {
			c[i][1][2] = -currentw / 2 + interval;
			c[i][1][4] = -currentw / 2 + 2 * interval + blength;
			c[i][1][3] = -currenth / 2 + hlength * (i - 1) + (0.9 * hlength - blength) / 2;
			c[i][1][5] = -currenth / 2 + hlength * (i - 1) + (0.9 * hlength - blength) / 2;
		}
	}
	else {
		interval = (hlength - 2 * blength) / 3;
		for (int i = 1; i <= MaxFloor; i++) {
			c[i][1][2] = -currentw / 2 + (0.95 * wlength - blength) / 2;
			c[i][1][4] = -currentw / 2 + (0.95 * wlength - blength) / 2;
			c[i][1][3] = -currenth / 2 + hlength * (i - 1) + interval;
			c[i][1][5] = -currenth / 2 + hlength * (i - 1) + interval * 2+ blength;
		}
	}
	//���ư�ť�����У�
	for (int i = 2; i <= MaxFloor; i++) {
		if (abs(c[i][1][7] - 1) <= 1e-6)
#ifdef texture
			glBindTexture(GL_TEXTURE_2D, dp);
#endif
#ifndef texture
		glColor3ub(111, 142, 191);
#endif
		else
#ifdef texture
			glBindTexture(GL_TEXTURE_2D, d);
#endif
#ifndef texture
		glColor3ub(95, 9, 19);
#endif
		if (abs(c[i][1][6] - 1) >= 1e-6) {//����ť���ڱ��������С��
			glBegin(GL_POLYGON); {
				glTexCoord2d(0, 0);
				glVertex2d(c[i][1][2], c[i][1][3]);
				glTexCoord2d(1, 0);
				glVertex2d(c[i][1][2] + blength, c[i][1][3]);
				glTexCoord2d(1, 1);
				glVertex2d(c[i][1][2] + blength, c[i][1][3] + blength);
				glTexCoord2d(0, 1);
				glVertex2d(c[i][1][2], c[i][1][3] + blength);
			}
			glEnd();
#ifndef texture
			glColor3ub(255, 255, 255);
			glRasterPos2d(c[i][1][2] + 0.05 * blength, c[i][1][3] + 0.05 * blength);
			sprintf(tempc, "D");
			drawString(BUTTON, tempc, blength);
#endif
		}
		else {
			glBegin(GL_POLYGON); {
				glTexCoord2d(0, 0);
				glVertex2d(c[i][1][2] + blength * 0.05, c[i][1][3] + blength * 0.05);
				glTexCoord2d(1, 0);
				glVertex2d(c[i][1][2] + blength * 0.95, c[i][1][3] + blength * 0.05);
				glTexCoord2d(1, 1);
				glVertex2d(c[i][1][2] + blength * 0.95, c[i][1][3] + blength * 0.95);
				glTexCoord2d(0, 1);
				glVertex2d(c[i][1][2] + blength * 0.05, c[i][1][3] + blength * 0.95);
			}
			glEnd();
#ifndef texture
			glColor3ub(255, 255, 255);
			glRasterPos2d(c[i][1][2] + blength * 1.95 * 0.05, c[i][1][3] + blength * 1.95 * 0.05);
			sprintf(tempc, "D");
			drawString(BUTTON, tempc, blength * 0.95);
#endif
		}
	}
#ifdef texture
	glBindTexture(GL_TEXTURE_2D, 0);
#endif
	for (int i = 1; i <= MaxFloor - 1; i++) {
		//���Ƶڶ��������У�
		if (abs(c[i][1][8] - 1) <= 1e-6)
#ifdef texture
			glBindTexture(GL_TEXTURE_2D, up);
#endif
#ifndef texture
			glColor3ub(111, 142, 191);
#endif
		else
#ifdef texture
			glBindTexture(GL_TEXTURE_2D, u);
#endif
#ifndef texture
			glColor3ub(95, 9, 19);
#endif
		if (abs(c[i][1][6] - 2) >= 1e-6) {//����ť���ڱ��������С��
			glBegin(GL_QUADS); {
				glTexCoord2d(0, 0);
				glVertex2d(c[i][1][4], c[i][1][5]);
				glTexCoord2d(1, 0);
				glVertex2d(c[i][1][4] + blength, c[i][1][5]);
				glTexCoord2d(1, 1);
				glVertex2d(c[i][1][4] + blength, c[i][1][5] + blength);
				glTexCoord2d(0, 1);
				glVertex2d(c[i][1][4], c[i][1][5] + blength);
			}
			glEnd();
#ifndef texture
			glColor3ub(255, 255, 255);
			glRasterPos2d(c[i][1][4] + 0.05 * blength, c[i][1][5] + 0.05 * blength);
			sprintf(tempc, "U");
			drawString(BUTTON, tempc, blength);
#endif
		}
		else {
			glBegin(GL_QUADS); {
				glTexCoord2d(0, 0);
				glVertex2d(c[i][1][4] + blength * 0.05, c[i][1][5] + blength * 0.05);
				glTexCoord2d(1, 0);
				glVertex2d(c[i][1][4] + blength * 0.95, c[i][1][5] + blength * 0.05);
				glTexCoord2d(1, 1);
				glVertex2d(c[i][1][4] + blength * 0.95, c[i][1][5] + blength * 0.95);
				glTexCoord2d(0, 1);
				glVertex2d(c[i][1][4] + blength * 0.05, c[i][1][5] + blength * 0.95);
			}
			glEnd();
#ifndef texture
			glColor3ub(255, 255, 255);
			glRasterPos2d(c[i][1][4] + blength * 1.95 * 0.05, c[i][1][5] + blength * 1.95 * 0.05);
			sprintf(tempc, "U");
			drawString(BUTTON, tempc, blength * 0.95);
#endif
		}
	}
#ifdef texture
	glBindTexture(GL_TEXTURE_2D, 0);
#endif
	//���ƿ�����
	//����߳�
	if (0.95 * hlength < wlength / 2) {
		dblength = 0.95 * hlength * 0.4;
		interval = (wlength / 2 - dblength * 2) / 3;
		//������������
		for (int i = 2; i <= Num + 1; i++) {
			c[MaxFloor + 1][i][2] = -currentw / 2 + wlength * (i - 1) + 0.5 * wlength + interval;
			c[MaxFloor + 1][i][4] = -currentw / 2 + wlength * (i - 1) + 0.5 * wlength + interval * 2 + dblength;
			c[MaxFloor + 1][i][3] = currenth / 2 - (0.95 * hlength - dblength) / 2 - dblength;
			c[MaxFloor + 1][i][5] = currenth / 2 - (0.95 * hlength - dblength) / 2 - dblength;
		}
	}
	else {
		dblength = wlength / 2 * 0.4;
		interval = (0.95 * hlength - dblength * 2) / 3;
		//������������
		for (int i = 2; i <= Num + 1; i++) {
			c[MaxFloor + 1][i][2] = -currentw / 2 + wlength * (i - 1) + 0.5 * wlength + (0.5 * wlength - dblength) / 2;
			c[MaxFloor + 1][i][4] = -currentw / 2 + wlength * (i - 1) + 0.5 * wlength + (0.5 * wlength - dblength) / 2;
			c[MaxFloor + 1][i][3] = currenth / 2 - interval - dblength;
			c[MaxFloor + 1][i][5] = currenth / 2 - interval * 2 - dblength * 2;
		}
	}
	//���Ƶ�һ����ť
	for (int i = 2; i <= Num + 1; i++) {
		if (abs(c[MaxFloor + 1][i][7] - 1) <= 1e-6)
#ifdef texture
			glBindTexture(GL_TEXTURE_2D, close);
#endif
#ifndef texture
		glColor3ub(111, 142, 191);
#endif
		else
#ifdef texture
			glBindTexture(GL_TEXTURE_2D, cp);
#endif
#ifndef texture
		glColor3ub(95, 9, 19);
#endif
		if (abs(c[MaxFloor + 1][i][6] - 1) >= 1e-6) {//����ť���ڱ��������С��
			glBegin(GL_POLYGON); {
				glTexCoord2d(0, 0);
				glVertex2d(c[MaxFloor + 1][i][2], c[MaxFloor + 1][i][3]);
				glTexCoord2d(1, 0);
				glVertex2d(c[MaxFloor + 1][i][2] + dblength, c[MaxFloor + 1][i][3]);
				glTexCoord2d(1, 1);
				glVertex2d(c[MaxFloor + 1][i][2] + dblength, c[MaxFloor + 1][i][3] + dblength);
				glTexCoord2d(0, 1);
				glVertex2d(c[MaxFloor + 1][i][2], c[MaxFloor + 1][i][3] + dblength);
			}
			glEnd();
#ifndef texture
			glColor3ub(255, 255, 255);
			glRasterPos2d(c[MaxFloor + 1][i][2] + 0.05 * dblength, c[MaxFloor + 1][i][3] + 0.05 * dblength);
			sprintf(tempc, "C");
			drawString(BUTTON, tempc, dblength);
#endif
		}
		else {
			glBegin(GL_POLYGON); {
				glTexCoord2d(0, 0);
				glVertex2d(c[MaxFloor + 1][i][2] + dblength * 0.05, c[MaxFloor + 1][i][3] + dblength * 0.05);
				glTexCoord2d(1, 0);
				glVertex2d(c[MaxFloor + 1][i][2] + dblength * 0.95, c[MaxFloor + 1][i][3] + dblength * 0.05);
				glTexCoord2d(1, 1);
				glVertex2d(c[MaxFloor + 1][i][2] + dblength * 0.95, c[MaxFloor + 1][i][3] + dblength * 0.95);
				glTexCoord2d(0, 1);
				glVertex2d(c[MaxFloor + 1][i][2] + dblength * 0.05, c[MaxFloor + 1][i][3] + dblength * 0.95);
			}
			glEnd();
#ifndef texture
			glColor3ub(255, 255, 255);
			glRasterPos2d(c[MaxFloor + 1][i][2] + dblength * 1.95 * 0.05, c[MaxFloor + 1][i][3] + dblength * 1.95 * 0.05);
			sprintf(tempc, "C");
			drawString(BUTTON, tempc, dblength * 0.95);
#endif
		}
	}
	for (int i = 2; i <= Num + 1; i++) {
		//���Ƶڶ���
		if (abs(c[MaxFloor + 1][i][8] - 1) <= 1e-6)
#ifdef texture
			glBindTexture(GL_TEXTURE_2D, op);
#endif
#ifndef texture
		glColor3ub(111, 142, 191);
#endif
		else
#ifdef texture
			glBindTexture(GL_TEXTURE_2D, o);
#endif
#ifndef texture
		glColor3ub(95, 9, 19);
#endif
		if (abs(c[MaxFloor + 1][i][6] - 2) >= 1e-6) {//����ť���ڱ��������С��
			glBegin(GL_POLYGON); {
				glTexCoord2d(0, 0);
				glVertex2d(c[MaxFloor + 1][i][4], c[MaxFloor + 1][i][5]);
				glTexCoord2d(1, 0);
				glVertex2d(c[MaxFloor + 1][i][4] + dblength, c[MaxFloor + 1][i][5]);
				glTexCoord2d(1, 1);
				glVertex2d(c[MaxFloor + 1][i][4] + dblength, c[MaxFloor + 1][i][5] + dblength);
				glTexCoord2d(0, 1);
				glVertex2d(c[MaxFloor + 1][i][4], c[MaxFloor + 1][i][5] + dblength);
			}
			glEnd();
#ifndef texture
			glColor3ub(255, 255, 255);
			glRasterPos2d(c[MaxFloor + 1][i][4] + 0.05 * dblength, c[MaxFloor + 1][i][5] + 0.05 * dblength);
			sprintf(tempc, "O");
			drawString(BUTTON, tempc, dblength);
#endif
		}
		else {
			glBegin(GL_POLYGON); {
				glTexCoord2d(0, 0);
				glVertex2d(c[MaxFloor + 1][i][4] + dblength * 0.05, c[MaxFloor + 1][i][5] + dblength * 0.05);
				glTexCoord2d(1, 0);
				glVertex2d(c[MaxFloor + 1][i][4] + dblength * 0.95, c[MaxFloor + 1][i][5] + dblength * 0.05);
				glTexCoord2d(1, 1);
				glVertex2d(c[MaxFloor + 1][i][4] + dblength * 0.95, c[MaxFloor + 1][i][5] + dblength * 0.95);
				glTexCoord2d(0, 1);
				glVertex2d(c[MaxFloor + 1][i][4] + dblength * 0.05, c[MaxFloor + 1][i][5] + dblength * 0.95);
			}
			glEnd();
#ifndef texture
			glColor3ub(255, 255, 255);
			glRasterPos2d(c[MaxFloor + 1][i][4] + dblength * 1.95 * 0.05, c[MaxFloor + 1][i][5] + dblength * 1.95 * 0.05);
			sprintf(tempc, "O");
			drawString(BUTTON, tempc, dblength * 0.95);
#endif

		}
	}
#ifdef texture
	glBindTexture(GL_TEXTURE_2D, 0);
#endif
	//����¥��ָʾ
	//����߳�
	static double charlength;
	static int hoffset;
	static int loffset;
	if (0.95 * hlength < wlength / 2) {
		charlength = 0.95 * hlength * 0.8 * 2 / 3;
		interval = (wlength / 2 * 2 / 3 - charlength) / 2;
		glColor3ub(188, 106, 81);
		for (int i = 2; i <= Num + 1; i++) {
			//glRasterPos2d(1, 1);
			glRasterPos2d(c[MaxFloor + 1][i][0] + interval, c[MaxFloor + 1][i][1] + 0.15 * hlength);
#ifdef testgl
			glBegin(GL_LINES); {
				glVertex2d(c[MaxFloor + 1][i][0] + interval, c[MaxFloor + 1][i][1] + 0.15 * hlength);
				glVertex2d(c[MaxFloor + 1][i][0] + interval + charlength, c[MaxFloor + 1][i][1] + 0.15 * hlength + charlength);
			}
			glEnd();
#endif
			sprintf(tempc, "%d", (Elevator + i - 2)->floor);
			drawString(FLOOR, tempc, (int)charlength);
			switch ((Elevator + i - 2)->direction) {
			case 'S' :
				break;
			default :
				glRasterPos2d(c[MaxFloor + 1][i][0] + interval * 2 + charlength, c[MaxFloor + 1][i][1] + 0.15 * hlength);
				sprintf(tempc, "%c", (Elevator + i - 2)->direction == 'H' ? 'U' : 'D');
				drawString(FLOOR, tempc, (int)charlength);
				break;
			}
		}

	}
	else {
		charlength = wlength / 2 * 2 / 3 * 0.8;
		interval = (0.95 * hlength * 2 / 3 - charlength) / 2;
		glColor3ub(188, 106, 81);
		for (int i = 2; i <= Num + 1; i++) {
			//glRasterPos2d(1, 1);
			glRasterPos2d(c[MaxFloor + 1][i][0] + wlength / 2 * 2 / 3 * 0.1, c[MaxFloor + 1][i][1] + interval);
			sprintf(tempc, "%d", (Elevator + i - 2)->floor);
			drawString(FLOOR, tempc, (int)charlength);
			switch ((Elevator + i - 2)->direction) {
			case 'S':
				break;
			default:
				glRasterPos2d(c[MaxFloor + 1][i][0] + wlength / 2 * 2 / 3 * 0.1, c[MaxFloor + 1][i][1] + interval * 2 + charlength);
				sprintf(tempc, "%c", (Elevator + i - 2)->direction == 'H' ? 'U' : 'D');
				drawString(FLOOR, tempc, (int)charlength);
				break;
			}

		}
	}
	//����
	drawBackground();
	glutSwapBuffers();

}

void mousefunc(int button, int state, int x, int y) {
	//����ϵ�任
	x = x - currentw / 2;
	y = currenth / 2 - y;
	static int i = 0;
	static int j = 0;
	static int flag = 0;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//����ť
		flag = 0;
		for (i = 1; i <= MaxFloor + 1; i++) {
			for (j = 1; j <= Num + 1; j++) {
				if (i == MaxFloor + 1) {
					if (j == 1)
						continue;
					else {
						if (x - c[i][j][2] < blength && x - c[i][j][2] > 0 && y - c[i][j][3] < blength && y - c[i][j][3] > 0) {
							c[i][j][6] = 1.0;
							c[i][j][7] = 1.0;
							PostThreadMessage(*(dElevator + j - 2), DOOR_CLOSE, 0, dInput);
							flag = 2;
							break;
						}
						else if (x - c[i][j][4] < blength && x - c[i][j][4] > 0 && y - c[i][j][5] < blength && y - c[i][j][5] > 0) {
							c[i][j][6] = 2.0;
							c[i][j][8] = 1.0;
							PostThreadMessage(*(dElevator + j - 2), DOOR_OPEN, 0, dInput);
							flag = 2;
							break;
						}
					}
				}
				else if (j == 1) {
					if (x - c[i][j][2] < blength && x - c[i][j][2] > 0 && y - c[i][j][3] < blength && y - c[i][j][3] > 0 && i != 1) {
						c[i][j][6] = 1.0;
						c[i][j][7] = 1.0;
						PostThreadMessage(dConsole, 2000 + i, 'L', dInput);
						flag = 1;
						break;
					}
					else if (x - c[i][j][4] < blength && x - c[i][j][4] > 0 && y - c[i][j][5] < blength && y - c[i][j][5] > 0 && i != MaxFloor) {
						c[i][j][6] = 2.0;
						c[i][j][8] = 1.0;
						PostThreadMessage(dConsole, 2000 + i, 'H', dInput);
						flag = 1;
						break;
					}
				}
				else if (x - c[i][j][2] < blength && x - c[i][j][2] > 0 && y - c[i][j][3] < blength && y - c[i][j][3] > 0) {
					c[i][j][6] = 1.0;
					c[i][j][7] = 1.0;
					PostThreadMessage(*(dElevator + j - 2), 2000 + i, 'I', dInput);
					flag = 1;
					break;
				}
			}
			if (flag == 1 || flag == 2)
				break;
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (i == MaxFloor + 1 && flag == 2) {
			c[i][j][6] = 0;
			c[i][j][7] = 0;
			c[i][j][8] = 0;
		}
		else if (i >= 1 && i <= MaxFloor && j >= 1 && j <= Num + 1)
			c[i][j][6] = 0;
	}

}

void checkConfig() {
	//����Ƿ����Ҫ��
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
		fprintf(config, "Num=3\nMaxFloor=9\nRunSpeed=3000\nWaitTime=3000\nDoorTime=1000\n");
		fclose(config);
		goto start;
	}
	if ((fscanf(config, "Num=%d", &Num) == 1 && fscanf(config, "\nMaxFloor=%d", &MaxFloor) == 1 && fscanf(config, "\nRunSpeed=%d", &RunSpeed) == 1 && fscanf(config, "\nWaitTime=%d", &WaitTime) == 1) && fscanf(config, "\nDoorTime=%d", &DoorTime) == 1 && Num <= 9 && Num >= 1 && MaxFloor >= 2 && MaxFloor <= 20 && RunSpeed >= 1000 && WaitTime >= 1000 && DoorTime >= 100 && DoorTime <= 2500)
		;
	else {
		printf("config.ini������Ч����Ч��ΧΪ����λms����\n1=<Num=<9��20=>MaxFloor>=2��RunSpeed>=1000��WaitTime>=1000��2500>=DoorTime>=100\n�����ؽ�config.ini����\n");
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
}

int glmain() {
	//glut��ʼ��
	int argc = 1;
	char *cd = (char*)malloc(sizeof(char));
	char **arg = &cd;
	glutInit(&argc, arg);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	//��ʼ������
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(currentw, currenth);
	glutCreateWindow("Elevator pwered by ���&��&��");
	//������ֱͬ��
	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapIntervalEXT(1);//�򿪴�ֱ�ֲ�������֡��
	//����������������
	c = (double ***)malloc(sizeof(double**)* (MaxFloor + 2));
	for (int i = 1; i < MaxFloor + 2; i++)
		*(c + i) = (double **)malloc(sizeof(double*)* (Num + 2));
	for (int i = 1; i < MaxFloor + 2; i++) {
		for (int j = 1; j < Num + 2; j++) {
			*(*(c + i) + j) = (double*)malloc(sizeof(double)* 9);
		}
	}
	//���ûص�����
	glClearColor(0.996, 0.992, 0.901, 1);
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(reshape);
	glutMouseFunc(mousefunc);
	glutIdleFunc(idleAnimation);
#ifdef texture
	//��������֧��(�����⣬ʱ�����ޣ�ʵ����������)
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	//��������
	u = load_texture("up.bmp");
	up = load_texture("upp.bmp");
	d = load_texture("down.bmp");
	dp = load_texture("downp.bmp");
	bg = load_texture("bg1.bmp");
	bg1 = load_texture("bg2.bmp");
	o = load_texture("o.bmp");
	op = load_texture("op.bmp");
	close = load_texture("c.bmp");
	cp = load_texture("cp.bmp");

#endif
	//��ѭ��
	glutMainLoop();
	return 0;
}
#ifndef test
int main(int argc, char *argv[]) {
	//ѡ�����з�ʽ
	printf("��ѡ�����з�ʽ��\n"
		"1�������з�ʽ. 2��GUI����.\n");
	int choice;
b:
	scanf("%d", &choice);
	flushall();
	switch (choice) {
	case 1:
		system("cls");
		consolemain();
		break;
	case 2:
		system("cls");
		break;
	default:
		printf("�������������롭��\n");
		goto b;
	}
	if (choice == 1)
		return 0;
	//��������ļ�
	checkConfig();
	dWinMain = GetCurrentThreadId();
	double currentw = 720 * 0.9 / (MaxFloor + 1) / 0.57 * (Num + 1);
	double currenth = 720;
	//�����߳�
	//��ʼ����������
	hElevator = (HANDLE*)malloc(sizeof(HANDLE)* Num);
	dElevator = (DWORD*)malloc(sizeof(DWORD)* Num);
	for (int i = 0; i < Num; i++) {
		*(hElevator + i) = CreateThread(0, 0, (routine)elevatorMain, 0, 0, dElevator + i);
	}
	//���������߳�
	hConsole = CreateThread(0, 0, (routine)consoleMain, 0, 0, &dConsole);
	hTimer = CreateThread(0, 0, (routine)timerMain, 0, 0, &dTimer);
	//����input�߳�
	hInput = CreateThread(0, 0, (routine)inputMain, 0, 0, &dInput);
	//����gl
	Sleep(300);
	CreateThread(0,0,(routine)glmain,0,0,0);
	MSG a;
	while (true) {
		GetMessage(&a, 0, WM_USER, 0);
		if (a.message == WM_USER + 'I')
			c[a.wParam][QueryElevator(a.lParam) + 2][7] = 0;
		else if (a.message == WM_USER + 'H')
			c[a.wParam][1][8] = 0;
		else if (a.message == WM_USER + 'L')
			c[a.wParam][1][7] = 0;
	}
	return 0;
}
#endif

#ifdef test
#include <windows.h>
int f1() {
	MSG a;
	GetMessage(&a, 0, 0, 0);
	return 0;
}
int main() {
	//checkConfig();
	hElevator = (HANDLE*)malloc(sizeof(HANDLE)* Num);
	dElevator = (DWORD*)malloc(sizeof(DWORD)* Num);
	for (int i = 0; i < Num; i++) {
		*(hElevator + i) = CreateThread(0, 0, (routine)elevatorMain, 0, 0, dElevator + i);
	}
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)inputMain, 0, 0, &dInput);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)consoleMain, 0, 0, &dConsole);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)timerMain, 0, 0, &dTimer);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)glmain, 0, 0, 0);
	MSG a;
	while (1)
		GetMessage(&a, 0, 0, 0);
	return 0;
}
#endif