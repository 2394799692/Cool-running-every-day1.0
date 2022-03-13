#define _CRT_SECURE_NO_WARNINGS
/*
* ������ܿ�����־
* 1.������Ŀ
* 2.�����ز�
* 3.ʵ�ʵĿ������̣����û���������
* 1)������Ϸ����
* 2��ʵ����Ϸ����
* a:���ر�����ͬ���ٶ�
* b��ѭ������������ʵ��
* 3��ʵ����Ϸ����
* a.���ر�����Դ
* b,��Ⱦ(����)
* �������⣺����ͼƬ��png��ʽͼƬ���ֺ�ɫ
* ����취���ٶ����������ͷ�ļ��е�putimagePNG2�������
* 4.ʵ����ҵı���
* 5.ʵ����ҵ���Ծ���ո�
* 6.ʵ�����С�ڹ�
* 7.�����ϰ���ṹ����������
* 8.ʹ���ϰ���ṹ������³�ʼ��
* 9.��װ�����ϰ������ʾ
* 10.ʵ������¶׼���
* 11.ʵ�֡����ӡ��ϰ���
* ���ڡ�easyx��ͼ�ο�
*/
#define WIN_SCORE 20
#define WIDTH 1012
#define HEIGHT 396
#define OBSTACLE_COUNT 10
#include<stdio.h>
#include<graphics.h>//����ͼ�ο�
#include<conio.h>//��������
#include"tools.h"//��ӱ���ͷ�ļ�
#include<vector>//����c++������ʹ�������䳤����
using namespace std;//���������ռ�

//��Ϸ��ʼ��
IMAGE imgBgs[3];//ȫ�ֱ�����������ر���
int bgX[3];//����ͼƬ��x����
int bgSpeed[3] = { 1,2,4 };//�����ƶ����ٶ�
IMAGE imgHeros[12];//����
int heroX;//��ҵ�x����
int heroY;
int heroIndex;//��ұ��ܵ�ͼƬ֡���
bool heroJump;//�����Ծ״̬
int jumpHeightMax;//������Ծ���߶�
int heroJumoOff;//����ƫ����
int update;//��ʾ�Ƿ���Ҫ����ˢ��
//IMAGE imgTortoise;//С�ڹ�
//int torToiseX;//С�ڹ��ˮƽ����
//int torToiseY;
//bool torToiseXExist;//��ǰ�����Ƿ���С�ڹ�
int heroBlood;//���Ѫ��
int score;//����
typedef enum {
	TORTOISE,
	LION,
	HOOK1,
	HOOK2,
	HOOK3,
	HOOK4,
	OBSTACLE_TYPE_COUNT
}obstacle_type;
//typedef enum {
//	TORTOISE,//�ڹ� 0
//	LION��//ʨ�� 1
//}obstacle_type;
//vector<vector<int>> data;//�൱��int data[3][5]
vector<vector<IMAGE>>obstacleImgs;//�൱��IMAGE obstacleImgs[][]
typedef struct obstacle {
	int type;//�ϰ��������
	int imgIndex;//��ǰ��ʾ��ͼƬ�����
	int x, y;//�ϰ��������
	int speed;//�ٶ�
	int power;//ɱ����
	bool exist;
	bool hited;//��ʾ�Ƿ��Ѿ�������ײ
	bool passed;//��ʾ�Ƿ��Ѿ���ͨ��
}obstacle_t;
obstacle_t obstacles[OBSTACLE_COUNT] ;
int lastObsIndex;
IMAGE imgHeroDown[2];
bool heroDown;//��ʾ����Ƿ����¶�״̬
IMAGE imgSZ[10];
void init() {
	//������Ϸ����
	initgraph(WIDTH, HEIGHT,EW_SHOWCONSOLE);//ҳ���С
	char name[64];
	//������Ϸ������Դ
	for (int i = 0; i < 3; i++) {
		sprintf(name, "res/bg%03d.png", i + 1);//��3λ����ǰ�油��0
		loadimage(&imgBgs[i], name);
		bgX[i] = 0;
		heroJump = false;
	}
	//�������Hero���ܵ�ͼƬ֡�ز�
	for (int i = 0; i < 12; i++) {
		sprintf(name, "res/hero%d.png", i + 1);
		loadimage(&imgHeros[i], name);
	}
	//������ҵĳ�ʼλ��
	heroX = WIDTH * 0.5 - imgHeros[0].getwidth() * 0.5;
	heroY = 345 - imgHeros[0].getheight();
	heroIndex = 0;
	heroJump = false;
	jumpHeightMax = 345 - imgHeros[0].getheight() - 120;
	heroJumoOff = -4;
	update = true;
	//����С�ڹ�
	/*loadimage(&imgTortoise, "res/t1.png");
	torToiseXExist = false;//С�ڹ��ʼ��
	torToiseY = 345 - imgTortoise.getheight()+5;*/
	//�ýṹ�����С�ڹ�
	IMAGE imgTort;
	loadimage(&imgTort, "res/t1.png");
	vector<IMAGE>imgTortArray;
	imgTortArray.push_back(imgTort);
	obstacleImgs.push_back(imgTortArray);
	//����ʨ��
	IMAGE imgLion;
	vector<IMAGE> imgLionArray;
	for (int i = 0; i < 6; i++) {
		sprintf(name, "res/p%d.png", i + 1);
		loadimage(&imgLion, name);
		imgLionArray.push_back(imgLion);
	}
	obstacleImgs.push_back(imgLionArray);
	//��ʼ���ϰ����
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		obstacles[i].exist = false;
	}
	//�����¶��ز�
	loadimage(&imgHeroDown[0], "res/d1.png");
	loadimage(&imgHeroDown[1], "res/d2.png");
	heroDown = false;
	IMAGE imgH;
	
	for (int i = 0; i < 4; i++) {
		vector<IMAGE>imgHookArray;
		sprintf(name, "res/h%d.png", i + 1);
		loadimage(&imgH, name,60,260,true);
		imgHookArray.push_back(imgH);
		obstacleImgs.push_back(imgHookArray);
	}
	heroBlood = 100;//��ʼ��Ѫ��Ϊ��Ѫ
	//Ԥ������Ч
	preLoadSound("res/hit.mp3");
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	lastObsIndex = -1;
	score = 0;
	//��������ͼƬ
	for (int i = 0; i < 10; i++) {
		sprintf(name, "res/sz/%d.png", i);
		loadimage(&imgSZ[i], name);
	}
}
void createObstacle() {
	int i;
	for ( i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist == false) {
			break;
		}
	}
	if (i >= OBSTACLE_COUNT) {
		return;
	}
	obstacles[i].exist = true;
	obstacles[i].hited = false;
	obstacles[i].imgIndex = 0;
	/*obstacles[i].type =(obstacle_type) (rand() % OBSTACLE_TYPE_COUNT);*/
	obstacles[i].type = (obstacle_type)(rand() % 3);
	if (lastObsIndex>=0&&
		obstacles[lastObsIndex].type >= HOOK1 && 
		obstacles[lastObsIndex].type <= HOOK4 && 
		obstacles[i].type == LION && 
		obstacles[lastObsIndex].x > (WIDTH - 500)) {
	obstacles[i].type = TORTOISE;
	}
	lastObsIndex = i;
	if (obstacles[i].type == HOOK1) {
		obstacles[i].type += rand() % 4; //0-3
	}
	obstacles[i].x = WIDTH;
	obstacles[i].y = 345 + 5 - obstacleImgs[obstacles[i].type][0].getheight();
	if (obstacles[i].type == TORTOISE) {
		obstacles[i].speed = 0;
		obstacles[i].power = 5;
	}
	else if (obstacles[i].type == LION) {
		obstacles[i].speed = 4;
		obstacles[i].power = 20;
	}
	else if (obstacles[i].type>= HOOK1&& obstacles[i].type <= HOOK4) {
		obstacles[i].speed = 0;
		obstacles[i].power = 20;
		obstacles[i].y = 0;
	}
	obstacles[i].passed = false;
}
void checkHit() {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist&&obstacles[i].hited==false) {
			int alx, aly, a2x, a2y;
			int off = 30;
			if (!heroDown) {//���¶�״̬
				alx = heroX + off;
				aly = heroY + off;
				a2x = heroX + imgHeros[heroIndex].getwidth() - off;
				a2y = heroY + imgHeros[heroIndex].getheight();
			}
			else {
				alx = heroX + off;
				aly = 345-imgHeroDown[heroIndex].getheight();
				a2x = heroX + imgHeroDown[heroIndex].getwidth()-off;
				a2y = 345;
			}
			IMAGE img = obstacleImgs[obstacles[i].type][obstacles[i].imgIndex];
			int blx = obstacles[i].x + off;
			int bly = obstacles[i].y + off;
			int b2x = obstacles[i].x + img.getwidth() - off;
			int b2y = obstacles[i].y + img.getheight() - 10;
			if (rectIntersect(alx, aly, a2x, a2y, blx, bly, b2x, b2y)) {
				heroBlood -= obstacles[i].power;
				printf("Ѫ��ʣ�� %d\n", heroBlood);
				playSound("res/hit.mp3");
				obstacles[i].hited = true;
			}
		}
	}
}
void fly() {
	for (int i = 0; i < 3; i++) {
		bgX[i] -=bgSpeed[i];
		if (bgX[i] < -WIDTH) {
			bgX[i] = 0;
		}
	}
	//ʵ����Ծ
	if (heroJump) {
		if (heroY < jumpHeightMax) {//��ֹ����
			heroJumoOff = 4;
		}
		heroY += heroJumoOff;//����
		if (heroY > 345 - imgHeros[0].getheight()) {//��ֹ�ݵ�
			heroJump = false;
			heroJumoOff = -4;
		}
	}
	else if (heroDown) {
		static int count = 0;
		int delays[2] = { 8,30 };
		count++;
		if (count >= delays[heroIndex]) {
			count = 0;
			heroIndex++;
			if (heroIndex >= 2) {
				heroIndex = 0;
				heroDown = false;
			}
		}
	}else {//����Ծʱ�Ȳ���
		heroIndex = (heroIndex + 1) % 12;
	}
	//�����ϰ���
	static int frameCount = 0;//��̬����������Ч
	static int enemyFre = 50;
	frameCount++;
	if (frameCount > enemyFre) {
		frameCount = 0;
		enemyFre =50 + rand() % 50;//50-99�����
		createObstacle();
	}
	//if (torToiseXExist) {
	//	//����С�ڹ���ƶ�
	//	torToiseX -= bgSpeed[2];
	//	if (torToiseX < -imgTortoise.getwidth()) {
	//		torToiseXExist = false;
	//	}
	//}
	//���������ϰ��������
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist) {
			obstacles[i].x -= obstacles[i].speed + bgSpeed[2];
			if (obstacles[i].x < -obstacleImgs[obstacles[i].type][0].getwidth()*2) {//obstacleImgs[obstacles[i].type][0].getwidth() * 2
				obstacles[i].exist = false;
			}
			int len = obstacleImgs[obstacles[i].type].size();
			obstacles[i].imgIndex = (obstacles[i].imgIndex + 1) % len;
		}
	}
	//��Һ��ϰ������ײ���
	checkHit();
	
}
//��Ⱦ����Ϸ������
void updateBg() {
	putimagePNG2(bgX[0],0,&imgBgs[0]);
	putimagePNG2(bgX[1], 119, &imgBgs[1]);
	putimagePNG2(bgX[2], 330, &imgBgs[2]);
}
//�����û���������
void jump() {
	heroJump = true;
	update = true;
}
void down() {
	update = true;
	heroDown = true;
	heroIndex = 0;
}
void keyEvent() {
	char ch;
	if (_kbhit()) {//����а������£�����true
		ch = getch();//����Ҫ���س����ɶ�ȡ
		if (ch == ' ') {
			jump();
		}
		else if (ch == '	') {
			down();
		}
	}
	
}
void updateEnemy() {
	//��ȾС�ڹ�
	/*if (torToiseXExist) {
		putimagePNG2(torToiseX, torToiseY,WIDTH,&imgTortoise);
	}*/
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist) {
			putimagePNG2(obstacles[i].x, obstacles[i].y, WIDTH, &obstacleImgs[obstacles[i].type][obstacles[i].imgIndex]);

		}
	}
	
}
void updateHero() {
	if (!heroDown) {
		putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);
	}
	else {
		int y = 345 - imgHeroDown[heroIndex].getheight();
		putimagePNG2(heroX, y, &imgHeroDown[heroIndex]);
	}
}
void updateBloodBar() {//���õ������ӿ�
	drawBloodBar(10, 10, 200, 10, 2, BLUE, DARKGRAY, RED, heroBlood / 100.0);
}
void checkOver() {
	if (heroBlood <= 0) {
		loadimage(0, "res/over.png");
		FlushBatchDraw();//ˢ�»���
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");
		//��֮ͣ�󣬸�����߿�ʼ��һ��
		heroBlood = 100;
		score = 0;
		mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	}
}
void checkScore() {
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		if (obstacles[i].exist &&
			obstacles[i].passed == false && 
			obstacles[i].hited==false &&
			obstacles[i].x + obstacleImgs[obstacles[i].type][0].getwidth() < heroX) {
			score++;
			obstacles[i].passed = true;
			printf("score:%d\n", score);
		}
	}
}
void updateScore() {
	char str[8];
	sprintf(str, "%d", score);
	int x = 20;
	int y = 25;
	for (int i = 0; str[i]; i++) {
		int sz = str[i] - '0';
		putimagePNG(x, y, &imgSZ[sz]);
		x += imgSZ[sz].getwidth() + 5;
	}
}
void checkWin() {
	if (score >= WIN_SCORE) {
		FlushBatchDraw();
		mciSendString("play res/win.mp3", 0, 0, 0);
		Sleep(2000);
		loadimage(0, "res/win.png");
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");
		heroBlood = 100;
		score = 0;
		mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	}
}
int main(void) { 
	init();
	loadimage(0, "res/over.png");//��ʼ����
	system("pause");
	int timer = 0;
	while (1) {
		keyEvent();
		timer+=getDelay();
		if (timer > 30) {
			timer = 0;
			update = true;
		}
		if (update) {//ˢ��ҳ��
			update = false;
			BeginBatchDraw();//ȥ����˸
			updateBg();
			//putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);
			updateHero();
			updateEnemy();
			updateBloodBar();
			updateScore();
			checkWin();
			EndBatchDraw();//ȥ����˸
			checkOver();
			checkScore();
			fly();
		}
		

		//Sleep(30);//����
	}
	system("pause");
	return 0;
}