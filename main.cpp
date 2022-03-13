#define _CRT_SECURE_NO_WARNINGS
/*
* 天天酷跑开发日志
* 1.创建项目
* 2.导入素材
* 3.实际的开发流程：从用户界面入手
* 1)创建游戏窗口
* 2）实现游戏背景
* a:三重背景不同的速度
* b：循环滚动背景的实现
* 3）实现游戏背景
* a.加载背景资源
* b,渲染(坐标)
* 遇到问题：背景图片的png格式图片出现黑色
* 解决办法：百度引入第三方头文件中的putimagePNG2函数解决
* 4.实现玩家的奔跑
* 5.实现玩家的跳跃（空格）
* 6.实现随机小乌龟
* 7.创建障碍物结构体数据类型
* 8.使用障碍物结构体后重新初始化
* 9.封装后多个障碍物的显示
* 10.实现玩家下蹲技能
* 11.实现“柱子”障碍物
* 基于“easyx”图形库
*/
#define WIN_SCORE 20
#define WIDTH 1012
#define HEIGHT 396
#define OBSTACLE_COUNT 10
#include<stdio.h>
#include<graphics.h>//引入图形库
#include<conio.h>//按键输入
#include"tools.h"//添加本地头文件
#include<vector>//引入c++库用于使用容器变长数组
using namespace std;//声明命名空间

//游戏初始化
IMAGE imgBgs[3];//全局变量，存放三重背景
int bgX[3];//背景图片的x坐标
int bgSpeed[3] = { 1,2,4 };//背景移动的速度
IMAGE imgHeros[12];//奔跑
int heroX;//玩家的x坐标
int heroY;
int heroIndex;//玩家奔跑的图片帧序号
bool heroJump;//玩家跳跃状态
int jumpHeightMax;//设置跳跃最大高度
int heroJumoOff;//设置偏移量
int update;//表示是否需要马上刷新
//IMAGE imgTortoise;//小乌龟
//int torToiseX;//小乌龟的水平坐标
//int torToiseY;
//bool torToiseXExist;//当前窗口是否有小乌龟
int heroBlood;//玩家血量
int score;//分数
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
//	TORTOISE,//乌龟 0
//	LION，//狮子 1
//}obstacle_type;
//vector<vector<int>> data;//相当于int data[3][5]
vector<vector<IMAGE>>obstacleImgs;//相当于IMAGE obstacleImgs[][]
typedef struct obstacle {
	int type;//障碍物的类型
	int imgIndex;//当前显示的图片的序号
	int x, y;//障碍物的坐标
	int speed;//速度
	int power;//杀伤力
	bool exist;
	bool hited;//表示是否已经发生碰撞
	bool passed;//表示是否已经被通过
}obstacle_t;
obstacle_t obstacles[OBSTACLE_COUNT] ;
int lastObsIndex;
IMAGE imgHeroDown[2];
bool heroDown;//表示玩家是否处于下蹲状态
IMAGE imgSZ[10];
void init() {
	//创建游戏窗口
	initgraph(WIDTH, HEIGHT,EW_SHOWCONSOLE);//页面大小
	char name[64];
	//加载游戏背景资源
	for (int i = 0; i < 3; i++) {
		sprintf(name, "res/bg%03d.png", i + 1);//宽3位不够前面补充0
		loadimage(&imgBgs[i], name);
		bgX[i] = 0;
		heroJump = false;
	}
	//加载玩家Hero奔跑的图片帧素材
	for (int i = 0; i < 12; i++) {
		sprintf(name, "res/hero%d.png", i + 1);
		loadimage(&imgHeros[i], name);
	}
	//设置玩家的初始位置
	heroX = WIDTH * 0.5 - imgHeros[0].getwidth() * 0.5;
	heroY = 345 - imgHeros[0].getheight();
	heroIndex = 0;
	heroJump = false;
	jumpHeightMax = 345 - imgHeros[0].getheight() - 120;
	heroJumoOff = -4;
	update = true;
	//加载小乌龟
	/*loadimage(&imgTortoise, "res/t1.png");
	torToiseXExist = false;//小乌龟初始化
	torToiseY = 345 - imgTortoise.getheight()+5;*/
	//用结构体加载小乌龟
	IMAGE imgTort;
	loadimage(&imgTort, "res/t1.png");
	vector<IMAGE>imgTortArray;
	imgTortArray.push_back(imgTort);
	obstacleImgs.push_back(imgTortArray);
	//加载狮子
	IMAGE imgLion;
	vector<IMAGE> imgLionArray;
	for (int i = 0; i < 6; i++) {
		sprintf(name, "res/p%d.png", i + 1);
		loadimage(&imgLion, name);
		imgLionArray.push_back(imgLion);
	}
	obstacleImgs.push_back(imgLionArray);
	//初始化障碍物池
	for (int i = 0; i < OBSTACLE_COUNT; i++) {
		obstacles[i].exist = false;
	}
	//加载下蹲素材
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
	heroBlood = 100;//初始化血量为满血
	//预加载音效
	preLoadSound("res/hit.mp3");
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	lastObsIndex = -1;
	score = 0;
	//加载数字图片
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
			if (!heroDown) {//非下蹲状态
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
				printf("血量剩余 %d\n", heroBlood);
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
	//实现跳跃
	if (heroJump) {
		if (heroY < jumpHeightMax) {//防止飞天
			heroJumoOff = 4;
		}
		heroY += heroJumoOff;//上升
		if (heroY > 345 - imgHeros[0].getheight()) {//防止遁地
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
	}else {//不跳跃时腿不动
		heroIndex = (heroIndex + 1) % 12;
	}
	//创建障碍物
	static int frameCount = 0;//静态变量永久有效
	static int enemyFre = 50;
	frameCount++;
	if (frameCount > enemyFre) {
		frameCount = 0;
		enemyFre =50 + rand() % 50;//50-99随机数
		createObstacle();
	}
	//if (torToiseXExist) {
	//	//设置小乌龟的移动
	//	torToiseX -= bgSpeed[2];
	//	if (torToiseX < -imgTortoise.getwidth()) {
	//		torToiseXExist = false;
	//	}
	//}
	//更新所有障碍物的坐标
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
	//玩家和障碍物的碰撞检测
	checkHit();
	
}
//渲染“游戏背景”
void updateBg() {
	putimagePNG2(bgX[0],0,&imgBgs[0]);
	putimagePNG2(bgX[1], 119, &imgBgs[1]);
	putimagePNG2(bgX[2], 330, &imgBgs[2]);
}
//处理用户按键输入
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
	if (_kbhit()) {//如果有按键按下，返回true
		ch = getch();//不需要按回车即可读取
		if (ch == ' ') {
			jump();
		}
		else if (ch == '	') {
			down();
		}
	}
	
}
void updateEnemy() {
	//渲染小乌龟
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
void updateBloodBar() {//调用第三方接口
	drawBloodBar(10, 10, 200, 10, 2, BLUE, DARKGRAY, RED, heroBlood / 100.0);
}
void checkOver() {
	if (heroBlood <= 0) {
		loadimage(0, "res/over.png");
		FlushBatchDraw();//刷新缓存
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");
		//暂停之后，复活或者开始下一局
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
	loadimage(0, "res/over.png");//初始画面
	system("pause");
	int timer = 0;
	while (1) {
		keyEvent();
		timer+=getDelay();
		if (timer > 30) {
			timer = 0;
			update = true;
		}
		if (update) {//刷新页面
			update = false;
			BeginBatchDraw();//去除闪烁
			updateBg();
			//putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);
			updateHero();
			updateEnemy();
			updateBloodBar();
			updateScore();
			checkWin();
			EndBatchDraw();//去除闪烁
			checkOver();
			checkScore();
			fly();
		}
		

		//Sleep(30);//休眠
	}
	system("pause");
	return 0;
}