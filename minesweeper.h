#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define TIMER 300 // 5 min

#include <stdio.h>
#include <time.h> 
#include <Windows.h> 
#include <stdlib.h>
#include <conio.h> 

typedef struct RNODE {
	struct RNODE *link;
	char name[11];
	int score;
} RNODE; // rank node

typedef struct QNODE {
	struct QNODE *link;
	int i, j;
} QNODE;

typedef struct OFFSET {
	int i, j;
} OFFSET; // offset

int *mine; int minen, minecnt; // mine[minen], minecnt: user mine count
RNODE *rank = NULL; QNODE *queue;
char **arr; int width, height; // arr[arrx][arry]
char **winarr; // user sees this arr
OFFSET offset[8] = { {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1} };

INPUT_RECORD rc;
DWORD        dw;

void gotoxy(int x, int y); // go to window ÁÂÇ¥(x, y)
void PrintMain(); // print main

void GameStart();
void HowToGame();
void ShowRank();
void End();

void InsertRank(char [], int);
void DeleteRank();
void PrepareGame();
int CheckRange(int, int);
void PrintArr();
int PressGround(int, int);
int SetMineFlag(int, int);
void GameOver();
void GameSuccess(int);
void Bfs(int, int);
void InsertQnode(int, int);
QNODE* DeleteQnode();