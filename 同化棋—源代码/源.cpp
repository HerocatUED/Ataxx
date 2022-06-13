#include <conio.h>
#include <easyx.h>
#include <graphics.h>
#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <stdio.h>
#include <cstring>
#include <cmath>
#include <fstream>
using namespace std;
int NumBoard[8][8] = { 0 };//数字棋盘，没有棋子为0
int s, r;//数字棋盘上的坐标
int dx[24] = { -1,0,1,1,1,0,-1,-1 ,-2,-1,0,1,2,2,2,2,2,1,0,-1,-2,-2,-2,-2 };
int dy[24] = { -1,-1,-1,0,1,1,1,0,-2,-2,-2,-2,-2,-1,0,1,2,2,2,2,2,1,0,-1 };//遍历可落子棋盘
double weight[8][8] =
{ { 0,0,0,0,0,0,0,0,},
{ 0,1.0,      0.965051, 0.849347, 0.803681, 0.779659, 0.789668, 1.0, },
{ 0,0.965051, 0.87366,  0.835621, 0.743914, 0.715635, 0.719026, 0.789668, },
{ 0,0.849347, 0.835621, 0.788361, 0.701912, 0.677423, 0.715635, 0.779659, },
{ 0,0.803681, 0.743914, 0.701912, 0.70752,  0.701912, 0.743914, 0.803681, },
{ 0,0.779659, 0.715635, 0.677423, 0.701912, 0.788361, 0.835621, 0.849347, },
{ 0,0.789668, 0.719026, 0.715635, 0.743914, 0.835621, 0.87366,  0.965051, },
{ 0,1.0,      0.789668, 0.779659, 0.803681, 0.849347, 0.965051, 1.0, } };
int VS1, VS2, VR1, VR2;//AI最优选择坐标
int myplay = 0;//记录AI扮演的玩家
struct Move { int s1; int s2; int r1; int r2; int board[8][8] = { 0 }; };//记录每一次移动的相关状态
Move makemove[5];//搜索深度为4
int x1 = 0, x2 = 0, p1 = 0, p2 = 0;//从(x1,p1)下到(x2,p2)
int px, py;//鼠标点击位置转化为棋盘位置(贴图位置)
int cnt = 0;//棋盘上的棋子数-4
int player = -1;//黑棋-1，白棋1
IMAGE white; IMAGE black; IMAGE WhiteChosen; IMAGE BlackChosen; IMAGE WhiteTurn; IMAGE BlackTurn; IMAGE background;IMAGE blank; IMAGE pause; IMAGE out;
IMAGE pvp; IMAGE AITurn; IMAGE PlayerTurn; IMAGE save; IMAGE readsave; IMAGE AIFirst; IMAGE PlayerFirst; IMAGE AIFirstChosen; IMAGE PlayerFirstChosen;
IMAGE pvpChosen; IMAGE saveChosen; IMAGE readsaveChosen; IMAGE AIWinning; IMAGE PlayerWinning; IMAGE BlackWinning; IMAGE WhiteWinning; IMAGE AnotherBot; 
void draw();//初始棋盘
void PVP();//双人模式
void PVE(int myplay);//单人模式
void GetPlace(int flag, int mode);//获取鼠标点击位置
void move(int px1, int py1, int px2, int py2, int player);//下棋
void MoveTry(int s1, int r1, int s2, int r2, int player, int depth);//从(s1,r1)落到(s2,r2)处
double MoveValue(int player, int depth, double down, double up);//评估当前状态最佳落点
double value(int player, int depth);//评估价值
bool CanMove(int player, int depth, int mode);//判断能否移动
void FillBoard(int player);//玩家不能移动则AI直接填满棋局
void GameOver(int flag);//游戏结束
void clear();//清盘
void CreatSave();//创建存档
void ReadSave();//读取存档
void function(int px1, int py1);//执行玩家在下棋期间选择的菜单栏功能

int main()
{
	draw();//初始棋盘
	while (1)//默认进行无限局游戏
	{//选模式
		player = -1;
		int px0, py0, flag = 0;
		bool ChoosePVP, ChooseSave, ChooseReadSave, ChooseAIFirst, ChoosePlayerFirst, ChooseAnotherBot;
		do
		{
			GetPlace(1, 0);
			px0 = px; py0 = py;
			ChooseAIFirst = px0 >= 520 && px0 <= 720 && py0 >= 150 && py0 <= 230;
			ChoosePlayerFirst = px0 >= 520 && px0 <= 720 && py0 >= 250 && py0 <= 330;
			ChoosePVP = px0 >= 520 && px0 <= 720 && py0 >= 350 && py0 <= 430;
			ChooseReadSave= px0 >= 760 && px0 <= 960 && py0 >= 250 && py0 <= 330;
		} while (!ChoosePVP && !ChoosePlayerFirst && !ChooseAIFirst && !ChooseReadSave);
		//进行游戏
		if (ChoosePVP)//选择双人模式
		{
			flag = 0;
			putimage(520, 350, &pvpChosen);
			PVP();
		}
		else if (ChooseAIFirst || ChoosePlayerFirst)//选择单人模式
		{
			if (ChooseAIFirst)
			{
				flag = 1;
				putimage(520, 150, &AIFirstChosen);
				myplay = -1;
			}
			else
			{
				flag = 2;
				putimage(520, 250, &PlayerFirstChosen);
				myplay = 1;
			}
			PVE(myplay);
		}
		else if (ChooseReadSave)//读取存档
		{
			putimage(760, 250, &readsaveChosen);
			Sleep(50);
			putimage(760, 250, &readsave);
			ReadSave();
			if (myplay == 0)//存档为PVP模式
			{
				flag = 0;
				putimage(520, 350, &pvpChosen);
				PVP();
			}
			else//存档为PVE模式
			{
				if (myplay == -1)
				{
					flag = 1;
					putimage(520, 150, &AIFirstChosen);
				}
				else
				{
					flag = 2;
					putimage(520, 250, &PlayerFirstChosen);
				}
				PVE(myplay);
			}
		}
		GameOver(flag);//游戏结束
		do
		{
			GetPlace(1, 0);
			px0 = px; py0 = py;
			ChooseAnotherBot = px0 >= 365 && px0 <= 545 && py0 >= 500 && py0 <= 580;
		} while (!ChooseAnotherBot);
		clear();
	}
	return 0;
}


void draw()////初始棋盘
{
	initgraph(1000, 600); //初始化图形系统
	cleardevice(); //清屏幕
	//下面行读取图像文件
	loadimage(&black, _T("black.jpg"), 50, 50, TRUE);
	loadimage(&BlackChosen, _T("BlackChosen.jpg"), 50, 50, TRUE);
	loadimage(&BlackTurn, _T("BlackTurn.jpg"), 180, 80, TRUE);
	loadimage(&BlackWinning, _T("BlackWinning.jpg"), 180, 80, TRUE);
	loadimage(&white, _T("white.jpg"), 50, 50, TRUE);
	loadimage(&WhiteChosen, _T("WhiteChosen.jpg"), 50, 50, TRUE);
	loadimage(&WhiteWinning, _T("WhiteWinning.jpg"), 50, 50, TRUE);
	loadimage(&WhiteTurn, _T("WhiteTurn.jpg"), 180, 80, TRUE);
	loadimage(&background, _T("background.png"), 1000, 600, TRUE);
	loadimage(&pvp, _T("pvp.jpg"), 200, 80, TRUE);
	loadimage(&pvpChosen, _T("pvpChosen.jpg"), 200, 80, TRUE);
	loadimage(&AITurn, _T("AITurn.jpg"), 180, 80, TRUE);
	loadimage(&AIWinning, _T("AIWinning.jpg"), 180, 80, TRUE);
	loadimage(&PlayerTurn, _T("PlayerTurn.jpg"), 180, 80, TRUE);
	loadimage(&PlayerWinning, _T("PlayerWinning.jpg"), 180, 80, TRUE);
	loadimage(&AIFirst, _T("AIFirst.jpg"), 200, 80, TRUE);
	loadimage(&AIFirstChosen, _T("AIFirstChosen.jpg"), 200, 80, TRUE);
	loadimage(&PlayerFirst, _T("PlayerFirst.jpg"), 200, 80, TRUE);
	loadimage(&PlayerFirstChosen, _T("PlayerFirstChosen.jpg"), 200, 80, TRUE);
	loadimage(&save, _T("save.jpg"), 200, 80, TRUE);
	loadimage(&readsave, _T("readsave.jpg"), 200, 80, TRUE);
	loadimage(&saveChosen, _T("saveChosen.jpg"), 200, 80, TRUE);
	loadimage(&readsaveChosen, _T("readsaveChosen.jpg"), 200, 80, TRUE);
	loadimage(&out, _T("out.jpg"), 200, 80, TRUE);
	loadimage(&blank, _T("blank.jpg"), 50, 50, TRUE);
	loadimage(&AnotherBot, _T("AnotherBot.jpg"), 180, 80, TRUE);
	loadimage(&pause, _T("pause.jpg"), 180, 80, TRUE);
	//背景
	putimage(0, 0, &background);
	//棋盘
	fillrectangle(60, 60, 480, 480);
	setcolor(BLACK);
	line(60, 60, 480, 60);//上边界
	line(60, 120, 480, 120);
	line(60, 180, 480, 180);
	line(60, 240, 480, 240);
	line(60, 300, 480, 300);
	line(60, 360, 480, 360);
	line(60, 420, 480, 420);
	line(60, 480, 480, 480);//下边界
	line(60, 60, 60, 480);//左边界
	line(120, 60, 120, 480);
	line(180, 60, 180, 480);
	line(240, 60, 240, 480);
	line(300, 60, 300, 480);
	line(360, 60, 360, 480);
	line(420, 60, 420, 480);
	line(480, 60, 480, 480);//右边界
	//初始棋盘
	putimage(65, 65, &black);
	putimage(425, 425, &black);
	putimage(65, 425, &white);
	putimage(425, 65, &white);
	NumBoard[1][1] = -1;
	NumBoard[1][7] = 1;
	NumBoard[7][1] = 1;
	NumBoard[7][7] = -1;
	//游戏菜单
	putimage(520, 150, &AIFirst);
	putimage(520, 250, &PlayerFirst);
	putimage(520, 350, &pvp);
	putimage(760, 150, &save);
	putimage(760, 250, &readsave);
	putimage(760, 350, &out);
	return;
}

void PVP()//双人模式
{
	int px1, px2, py1, py2;//记录点击坐标
	putimage(305, 500, &pause);//暂停
	//下棋
	while (cnt < 45)
	{
		if (player == -1)
			putimage(75, 500, &BlackTurn);
		else
			putimage(75, 500, &WhiteTurn);
		if (CanMove(player, 0, 1))
		{//选中要移动的棋子
			do
			{
				GetPlace(1, 1);
				function(px, py);
				px1 = px; py1 = py;
			} while (px1 < 65 || px1 > 485 || py1 < 65 || py1 > 485 || NumBoard[(py1 - 5) / 60][(px1 - 5) / 60] != player);//越界无效且选中区域必须有棋子
			//选中落点处的棋子
			if (player == -1)
				putimage(px1, py1, &BlackChosen);
			else
				putimage(px1, py1, &WhiteChosen);
			do
			{
				GetPlace(1, 1);
				px2 = px, py2 = py;
			} while (px2 < 65 || px2 > 485 || py2 < 65 || py2 > 485 || (px1 == px2 && py1 == py2) || abs(px1 - px2) > 120 || abs(py1 - py2) > 120 || NumBoard[(py2 - 5) / 60][(px2 - 5) / 60] != 0);//越界无效且棋子落点要合法
			move(px1, py1, px2, py2, player);
		}
		//换人下棋
		player = -player;
	}
	return;
}

void PVE(int myplay)//单人模式
{
	int px1, px2, py1, py2;//记录点击坐标
	putimage(305, 500, &pause);//暂停
	//下棋
	while (cnt < 45)
	{
		if (player == myplay && cnt < 45)//AI下棋
		{
			putimage(75, 500, &AITurn);
			MoveValue(myplay, 0, -1000, 1000);
			int px1 = VS1 * 60 + 5, py1 = VR1 * 60 + 5, px2 = VS2 * 60 + 5, py2 = VR2 * 60 + 5;
			if (myplay == -1)
			{
				putimage(px1, py1, &BlackChosen);
				Sleep(300);//便于玩家观察AI棋步
			}
			else
			{
				putimage(px1, py1, &WhiteChosen);
				Sleep(300);//便于玩家观察AI棋步
			}
			move(px1, py1, px2, py2, myplay);
		}
		else//玩家下棋
		{
			if (CanMove(player, 0, 1) && cnt < 45)
			{
				putimage(75, 500, &PlayerTurn);
				//选中要移动的棋子
				do
				{
					GetPlace(1, 1);
					function(px, py);
					px1 = px; py1 = py;
				} while (px1 < 65 || px1 > 485 || py1 < 65 || py1 > 485 || NumBoard[(py1 - 5) / 60][(px1 - 5) / 60] != player);//越界无效且选中区域必须有棋子
				//选中落点处的棋子
				if (player == -1)
					putimage(px1, py1, &BlackChosen);
				else
					putimage(px1, py1, &WhiteChosen);
				do
				{
					GetPlace(1, 1);
					px2 = px, py2 = py;
				} while (px2 < 65 || px2 > 485 || py2 < 65 || py2 > 485 || (px1 == px2 && py1 == py2) || abs(px1 - px2) > 120 || abs(py1 - py2) > 120 || NumBoard[(py2 - 5) / 60][(px2 - 5) / 60] != 0);//越界无效且棋子落点要合法
				move(px1, py1, px2, py2, player);
				Sleep(60);
			}
			else FillBoard(myplay);
		}
		player = -player;
	}
	return;
}


void GetPlace(int flag, int mode)//获取鼠标点击位置
{
	flushmessage(EM_MOUSE);//清空鼠标消息缓存区
	ExMessage msg;
	while (flag)
	{
		msg = getmessage(EM_MOUSE);//获取鼠标点击位置
		if (msg.message == WM_LBUTTONUP)
		{
			if (mode == 1)
			{
				px = msg.x - msg.x % 60 + 5;
				py = msg.y - msg.y % 60 + 5;
			}
			else if (mode == 0)
			{
				px = msg.x;
				py = msg.y;
			}
			flag = 0;
		}
		flushmessage(EM_MOUSE);//清空鼠标消息缓存区
	}
	return;
}

void move(int px1, int py1, int px2, int py2, int player)//下棋
{
	cnt++;//棋子数加一
	int s1 = (px1 - 5) / 60;
	int r1 = (py1 - 5) / 60;
	int s2 = (px2 - 5) / 60;
	int r2 = (py2 - 5) / 60;
	NumBoard[r2][s2] = player;
	if (player == -1)
	{
		putimage(px1, py1, &black);
		putimage(px2, py2, &black);
	}
	else
	{
		putimage(px1, py1, &white);
		putimage(px2, py2, &white);
	}
	//同化
	for (int i = 0; i < 8; i++)
	{
		if (s2 + dx[i] > 7 || s2 + dx[i] < 1 || r2 + dy[i]>7 || r2 + dy[i] < 0)//越界
			continue;
		if (NumBoard[r2 + dy[i]][s2 + dx[i]] == -player)
		{
			NumBoard[r2 + dy[i]][s2 + dx[i]] = player;
			if (player == -1)
				putimage(px2 + dx[i] * 60, py2 + dy[i] * 60, &black);
			else
				putimage(px2 + dx[i] * 60, py2 + dy[i] * 60, &white);
		}
	}
	//判断是否需要把原来位置上的棋子消除
	if (abs(r1 - r2) == 2 || abs(s1 - s2) == 2)
	{
		NumBoard[r1][s1] = 0;
		putimage(px1, py1, &blank);
		cnt--;//棋子数减一
	}
	return;
}

double MoveValue(int player, int depth, double down, double up)//评估当前状态最佳落点
{
	double max = -1000;
	if (depth == 5)//到达最大搜索深度+1
		return value(player, depth - 1);
	if (depth == 0)//初始化暂存棋盘
	{
		for (int m = 1; m < 8; m++)
			for (int n = 1; n < 8; n++)
				makemove[0].board[m][n] = NumBoard[m][n];
		MoveValue(player, depth + 1, down, up);
		return 0;
	}
	//存棋局
	for (int m = 1; m < 8; m++)
		for (int n = 1; n < 8; n++)
			makemove[depth].board[m][n] = makemove[depth - 1].board[m][n];
	if (!CanMove(player, depth,0))//如果不能下棋
	{
		if (!CanMove(-player, depth,0))
			return value(player, depth);
		return -MoveValue(-player, depth + 1, -up, -down);
	}
	//找位置落子
	for (int i = 1; i < 8; i++)
		for (int j = 1; j < 8; j++)
			if (makemove[depth].board[i][j] == player)
			{
				for (int k = 0; k < 24; k++)
					if (j + dx[k] >= 1 && j + dx[k] <= 7 && i + dy[k] >= 1 && i + dy[k] <= 7)
						if (makemove[depth].board[i + dy[k]][j + dx[k]] == 0)
						{
							MoveTry(j, i, j + dx[k], i + dy[k], player, depth);
							double v = -MoveValue(-player, depth + 1, -up, -down);
							if (v > down)
							{
								if (v >= up)
									return v;
								down = v;
							}
							if (v > max)
							{
								max = v;
								if (depth == 1)
								{//记录移动状态
									VS1 = j;
									VR1 = i;
									VS2 = j + dx[k];
									VR2 = i + dy[k];
								}
							}
							//复原
							for (int m = 1; m < 8; m++)
								for (int n = 1; n < 8; n++)
									makemove[depth].board[m][n] = makemove[depth - 1].board[m][n];
						}
			}
	return max;
}

double value(int player, int depth)//评估价值
{
	double sum = 0.0;
	for (int i = 1; i < 8; i++)
		for (int j = 1; j < 8; j++)
			sum += makemove[depth].board[i][j] * weight[i][j];
	return player * sum;
}

void MoveTry(int s1, int r1, int s2, int r2, int player, int depth)//从(s1,r1)落到(s2,r2)处
{
	makemove[depth].board[r2][s2] = player;
	//判断落点情况
	if (abs(r1 - r2) == 2 || abs(s1 - s2) == 2)
		makemove[depth].board[r1][s1] = 0;
	//同化
	for (int i = 0; i < 8; i++)
		if (s2 + dx[i] >= 1 && s2 + dx[i] <= 7 && r2 + dy[i] >= 1 && r2 + dy[i] <= 7)
			if (makemove[depth].board[r2 + dy[i]][s2 + dx[i]] == -player)
				makemove[depth].board[r2 + dy[i]][s2 + dx[i]] = player;
	return;
}

bool CanMove(int player, int depth, int mode)//判断能否移动
{
	if (mode == 0)//PVE模式
		for (int i = 1; i < 8; i++)
			for (int j = 1; j < 8; j++)
			{
				if (makemove[depth].board[i][j] == player)
				{
					for (int k = 0; k < 24; k++)
						if (j + dx[k] >= 1 && j + dx[k] <= 7 && i + dy[k] >= 1 && i + dy[k] <= 7)
							if (makemove[depth].board[i + dy[k]][j + dx[k]] == 0)
								return true;
				}
			}
	else//PVP模式
	{
		for (int i = 1; i < 8; i++)
			for (int j = 1; j < 8; j++)
				if (NumBoard[i][j] == player)
				{
					for (int k = 0; k < 24; k++)
						if (j + dx[k] >= 1 && j + dx[k] <= 7 && i + dy[k] >= 1 && i + dy[k] <= 7)
							if (NumBoard[i + dy[k]][j + dx[k]] == 0)
								return true;
				}
	}
	return false;
}

void FillBoard(int myplay)//玩家不能移动则AI直接填满棋局
{
	for (int i = 1; i <= 7; i++)
		for (int j = 1; j <= 7; j++)
			if (NumBoard[i][j] == 0)
			{
				if (myplay == -1)//AI黑棋
				{
					putimage(j * 60 + 5, i * 60 + 5, &black);
					Sleep(80);
				}
				else//AI白棋
				{
					putimage(j * 60 + 5, i * 60 + 5, &white);
					Sleep(80);
				}
			}
	cnt = 45;
	return;
}

void GameOver(int flag)//游戏结束
{
	int NumBlack = 0, NumWhite = 0;
	for (int i = 1; i < 8; i++)
		for (int j = 1; j < 8; j++)
		{
			if (NumBoard[i][j] == -1)
				NumBlack++;
			if (NumBoard[i][j] == 1)
				NumWhite++;
		}
	if (flag == 0)//PVP模式
	{
		if (NumBlack > NumWhite)
			putimage(75, 500, &BlackWinning);
		else
			putimage(75, 500, &WhiteWinning);
	}
	else if (flag == 1)//AI先手
	{
		if (NumBlack > NumWhite)
			putimage(75, 500, &AIWinning);
		else
			putimage(75, 500, &PlayerWinning);

	}
	else if (flag == 2)//AI后手
	{
		if (NumBlack > NumWhite)
			putimage(75, 500, &PlayerWinning);
		else
			putimage(75, 500, &AIWinning);
	}
	putimage(305, 500, &AnotherBot);
	return;
}

void clear()//清盘
{
	cnt = 0;
	player = -1;
	initgraph(1000, 600); //初始化图形系统
	cleardevice(); //清屏幕
	//背景
	putimage(0, 0, &background);
	//棋盘
	fillrectangle(60, 60, 480, 480);
	setcolor(BLACK);
	line(60, 60, 480, 60);//上边界
	line(60, 120, 480, 120);
	line(60, 180, 480, 180);
	line(60, 240, 480, 240);
	line(60, 300, 480, 300);
	line(60, 360, 480, 360);
	line(60, 420, 480, 420);
	line(60, 480, 480, 480);//下边界
	line(60, 60, 60, 480);//左边界
	line(120, 60, 120, 480);
	line(180, 60, 180, 480);
	line(240, 60, 240, 480);
	line(300, 60, 300, 480);
	line(360, 60, 360, 480);
	line(420, 60, 420, 480);
	line(480, 60, 480, 480);//右边界
	//初始化棋盘
	memset(NumBoard, 0, sizeof(NumBoard));
	putimage(65, 65, &black);
	putimage(425, 425, &black);
	putimage(65, 425, &white);
	putimage(425, 65, &white);
	NumBoard[1][1] = -1;
	NumBoard[1][7] = 1;
	NumBoard[7][1] = 1;
	NumBoard[7][7] = -1;
	//游戏菜单
	putimage(520, 150, &AIFirst);
	putimage(520, 250, &PlayerFirst);
	putimage(520, 350, &pvp);
	putimage(760, 150, &save);
	putimage(760, 250, &readsave);
	putimage(760, 350, &out);
	return;
}

void CreatSave()//创建存档
{
	ofstream fout("save");
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			fout << NumBoard[i][j] << " ";
	fout << cnt << " " << player << " " << myplay;//记录游戏状态
	fout.close();
	return;
}

void ReadSave()//读取存档
{
	ifstream fin("save");
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			fin >> NumBoard[i][j];
			if (NumBoard[i][j] == -1)
				putimage(j * 60 + 5, i * 60 + 5, &black);
			else if (NumBoard[i][j] == 1)
				putimage(j * 60 + 5, i * 60 + 5, &white);
		}
	fin >> cnt >> player >> myplay;//读取游戏状态
	fin.close();
	return;
}

void function(int px1, int py1)//执行玩家在下棋期间选择的菜单栏功能
{
	bool ChoosePause, ChooseSave, ChooseOut;
	ChoosePause = px1 >= 305 && px1 <= 485 && py1 >= 485 && py1 <= 545;
	ChooseSave = px1 >= 725 && px1 <= 965 && py1 >= 125 && py1 <= 185;
	ChooseOut= px1 >= 725 && px1 <= 965 && py1 >= 305 && py1 <= 365;
	if (ChoosePause)//暂停
		MessageBox(NULL, _T("点击继续"), _T("暂停"), MB_SYSTEMMODAL);
	else if (ChooseSave)//创建存档
	{
		CreatSave();
		putimage(760, 150, &saveChosen);
		Sleep(50);
		putimage(760, 150, &save);
		MessageBox(NULL, _T("存档成功"), _T(" "), MB_SYSTEMMODAL);
	}
	else if (ChooseOut)//退出游戏
	{
		exit(0);
	}
	return;
}