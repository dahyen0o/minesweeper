#include "minesweeper.h"

#define WHAT 0
#define EMPTY -1
#define MINE -2
#define FLAG -3

#define CIN GetStdHandle(STD_INPUT_HANDLE) // 표준 입력 디바이스
#define COUT GetStdHandle(STD_OUTPUT_HANDLE) // 표준 출력 디바이스

int main() {
	/* initialize */
	int menu;

	/* main menu */
	while (1) {
		PrintMain();
		menu = _getch();
		switch (menu) {
		case '1':
			GameStart();
			break;
		case '2':
			HowToGame();
			break;
		case '3':
			ShowRank();
			break;
		case '4':
			End();
			return 0;
		default:
			printf("\n\n\t    Wrong menu number.");
			Sleep(500); // wait
			break;
		}
	}

	return 0;
}

void gotoxy(int x, int y){
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void PrintMain() {
	system("cls"); // clear console

	// 테두리: ㅂ + 한자	
	gotoxy(0, 3);
	printf("\t┌────────────────────────────────────────┐\n"); 
	printf("\t│            MINE SWEEPER GAME           │\n");
	printf("\t└────────────────────────────────────────┘\n\n");
	printf("\t    1. GAME START\n\n");
	printf("\t    2. HOW TO GAME\n\n");
	printf("\t    3. SHOW LANK\n\n");
	printf("\t    4. END\n\n\n\n");

	printf("\t    ENTER NUM OF MENU ");
}

void GameStart() {
	PrepareGame();

	time_t starttime, currtime;
	time(&starttime);
	int menu, succflag = 0;
	LPPOINT lpPoint; DWORD mode;

	do {

		system("cls");
		gotoxy(5, 2); time(&currtime);
		printf("TIME  %d sec  |  ", TIMER - (int)(currtime - starttime));
		printf("Get %d mine", minecnt);

		gotoxy(5, 4);
		PrintArr();

		// set to get mouse coordinate
		GetConsoleMode(CIN, &mode);
		SetConsoleMode(CIN, mode | ENABLE_MOUSE_INPUT);
		
		ReadConsoleInput(CIN, &rc, 1, &dw);
		int y = rc.Event.MouseEvent.dwMousePosition.X;
		int x = rc.Event.MouseEvent.dwMousePosition.Y;
		x -= 4; y -= 5; y /= 2; // x, y corrdinate -> index of arr

		if (rc.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
			if (CheckRange(x, y) && winarr[x][y] == WHAT && !PressGround(x, y)) break; // press mine -> game over
		}
		else if (rc.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
			if(!SetMineFlag(x, y) && winarr[x][y] == WHAT) break;
		}

		if (minecnt >= minen) {
			GameSuccess((int)(currtime - starttime)); succflag = 1; break;
		}
		Sleep(30);
	}while ((int)(currtime - starttime) < TIMER);

	if(!succflag) GameOver();
}

void GameSuccess(int time) {
	system("cls");

	gotoxy(5, 4);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			switch (arr[i][j]) {
			case WHAT: printf("□"); break;
			case MINE: printf("■"); break;
			case EMPTY: printf("▩"); break;
			case FLAG: printf("♨"); break;
			default: printf("%2d", arr[i][j]);
			}
		}
		gotoxy(5, 5 + i);
	}

	printf("\n\n\tYOU FOUND ALL MINES!\n");
	printf("\tSCORE IS %d\n", time + minen * 5);
	Sleep(1000);
	printf("\n\tTYPE NAME(max 10 letters) ON RANK\n\t");
	
	char name[100];
	while (1) {
		scanf("%s", name);
		if (strlen(name) > 0 && strlen(name) < 11) break;
	}

	InsertRank(name, time + minen * 5);
	printf("\n\tInserted on rank successfully");
}

void GameOver() {
	system("cls");

	gotoxy(5, 4);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			switch (arr[i][j]) {
			case WHAT: printf("□"); break;
			case MINE: printf("■"); break;
			case EMPTY: printf("▩"); break;
			case FLAG: printf("♨"); break;
			default: printf("%2d", arr[i][j]);
			}
		}
		gotoxy(5, 5 + i);
	}

	free(mine);
	for (int i = 0; i < height; i++) free(arr[i]);
	free(arr);

	printf("\n\n");
	printf("\tGAME OVER\n");

	printf("\n\n\tTYPE '1' to go back to MENU\n");
	while (1) {
		int key = _getch();
		if (key == '1') break;
	}
}

int PressGround(int i, int j) {
	switch (arr[i][j]) {
	case MINE: // game over
		return 0;
	case EMPTY: // show ground using BFS
		Bfs(i, j);
		break;
	default: winarr[i][j] = arr[i][j];
	}
	return 1;
}

void Bfs(int i, int j) {
	int visited[30][30] = { 0, };
	queue = NULL; 
	
	InsertQnode(i, j); visited[i][j] = 1;

	while (queue) {
		QNODE *curr = DeleteQnode();
		int curri = curr->i, currj = curr->j;
		winarr[curri][currj] = arr[curri][currj];
		free(curr);

		if (arr[curri][currj] > 0) continue; // if num ground, stop search
		for (int k = 0; k < 8;k++) {
			int newi = curri + offset[k].i, newj = currj + offset[k].j;

			if (CheckRange(newi, newj) && !visited[newi][newj] && arr[newi][newj] != MINE) {
				// press only empty ground
				InsertQnode(newi, newj);
				visited[newi][newj] = 1;
			}
		}
	}
}

void InsertQnode(int i, int j) {
	QNODE *newnode = (QNODE *)malloc(sizeof(QNODE));
	newnode->i = i; newnode->j = j; newnode->link = NULL;
	QNODE *curr = queue;

	if (!queue) queue = newnode;
	else {
		while (curr->link) curr = curr->link;
		curr->link = newnode;
	}
}

QNODE* DeleteQnode() {
	if (!queue) return NULL;

	QNODE *del = queue;
	queue = queue->link;
	return del;
}

int SetMineFlag(int i, int j) {
	if (arr[i][j] == MINE) winarr[i][j] = FLAG;
	else return 0;
	minecnt++; return 1;
}

void HowToGame() {
	system("cls");

	gotoxy(0, 3);
	printf("\t┌────────────────────────────────────────┐\n");
	printf("\t│               BEFORE GAME              │\n");
	printf("\t│  - Set width, height, num of mines     │\n");
	printf("\t│                                        │\n");
	printf("\t│               DURING GAME              │\n");
	printf("\t│  -  □: not pressed yet (don't know),  │\n");
	printf("\t│     ■: mine, ▩: empty (pressed)      │\n");
	printf("\t│     ♨: flag                           │\n");
	printf("\t│  - Set all flags on mine during time   │\n");
	printf("\t│  - If press mine, game will be over    │\n");
	printf("\t│                                        │\n");
	printf("\t│               AFTER GAME               │\n");
	printf("\t│  - Input your name on rank             │\n");
	printf("\t└────────────────────────────────────────┘\n\n");

	printf("\n\tPress '1' to go back");
	while (1) {
		int key = _getch();
		if (key == '1') break;
		else continue;
	}
}

void ShowRank() {
	system("cls");

	printf("\n\n");
	printf("\t┌────────────────────────────────────────┐\n");
	printf("\t│                 R A N K                │\n");
	printf("\t│          (press '1' to return)         │\n");
	printf("\t└────────────────────────────────────────┘\n\n");

	RNODE *curr = rank;
	for (int i = 0; curr; i++, curr = curr->link) {
		printf("\n\t%2d   %11s (%d)\n", i + 1, curr->name, curr->score);
	}

	while (1) {
		int key = _getch();
		if (key == '1') break;
	}
}

void End() {
	system("cls");

	gotoxy(0, 3);
	printf("\t┌────────────────────────────────────────┐\n");
	printf("\t│           EXISTING GAME. . .           │\n");
	printf("\t└────────────────────────────────────────┘\n\n");
	
	Sleep(1000);
	system("cls");

	DeleteRank();

	gotoxy(0, 3);
	printf("\t┌────────────────────────────────────────┐\n");
	printf("\t│          DONE, GOOD BYE! ;-)           │\n");
	printf("\t└────────────────────────────────────────┘\n\n");
}

void InsertRank(char name[], int score) {
	RNODE *newnode = (RNODE *)malloc(sizeof(RNODE));
	newnode->link = NULL; strcpy(newnode->name, name); newnode->score = score;

	// insert by score
	RNODE *curr = rank;
	if (!rank) {
		rank = newnode; return;
	}
	else if (!(rank->link) || rank->score < score) {
		if (rank->score > score) rank->link = newnode;
		else {
			newnode->link = rank;
			rank = newnode;
		}
		return;
	}

	while (curr->link && curr->link->score > score) {
		curr = curr->link;
	}
	newnode->link = curr->link;
	curr->link = newnode;
}

void DeleteRank() {
	RNODE *tmp = rank;
	while (tmp) {
		RNODE *del = tmp;
		tmp = tmp->link;
		free(del);
	}
}

void PrepareGame() {
	/* Select height, width, mine num */
	
	while (1) {
		system("cls");
		gotoxy(0, 3);
		printf("\t┌────────────────────────────────────────┐\n");
		printf("\t│            SELECT DIFFICULTY           │\n");
		printf("\t└────────────────────────────────────────┘\n\n");

		printf("\t\t height of ground(1 ~ 30): ");
		scanf("%d", &height);
		if (height <= 0 || height > 30) continue;
		//printf("height %d\n", height);

		printf("\n\t\t width of ground(1 ~ 30): ");
		scanf("%d", &width);
		if (width <= 0 || width > 30) continue;

		printf("\n\t\t Amount of mine(1 ~ height * width): ");
		scanf("%d", &minen);
		if (minen > 0 && minen < height * width) break;
	}
	
	int i, j, k, minex, miney;

	arr = (char **)malloc(sizeof(char *) * (height + 1));
	winarr = (char **)malloc(sizeof(char *) * (height + 1));
	for (i = 0; i < height; i++) {
		arr[i] = (char *)malloc(sizeof(char) * (width + 1));
		winarr[i] = (char *)malloc(sizeof(char) * (width + 1));
	}

	mine = (int *)malloc(sizeof(int) * (minen + 1));
	srand(time(NULL));
	for (i = 0; i < minen; i++) { // set mine on arr
		mine[i] = rand() % (width * height);
		//printf("mine %d\n", mine[i]);
		minex = mine[i] / width;
		miney = mine[i] % width;
		arr[minex][miney] = MINE;
	}

	for (i = 0; i < height; i++) { // set other on arr
		for (j = 0; j < width; j++) {
			if (arr[i][j] == MINE) continue;
			int minecount = 0, tmpi, tmpj;
			
			for (k = 0; k < 8; k++) {
				tmpi = i + offset[k].i;
				tmpj = j + offset[k].j;

				if (CheckRange(tmpi, tmpj) && arr[tmpi][tmpj] == MINE)
					minecount++;
			}

			if (minecount) arr[i][j] = minecount;
			else arr[i][j] = EMPTY;
		}
	}

	for (i = 0; i < height; i++) { // set winarr
		for (j = 0; j < width; j++) {
			winarr[i][j] = WHAT;
		}
	}

	minecnt = 0;
}

int CheckRange(int i, int j) {
	/* Check range of arr index */
	// valid range: 1, invalid: 0

	if (i < 0 || i >= height) return 0;
	else if (j < 0 || j >= width) return 0;
	else return 1;
}

void PrintArr() {
	/* Print winarr with index */
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			switch (winarr[i][j]) {
			case WHAT: printf("□"); break;
			case MINE: printf("■"); break;
			case EMPTY: printf("▩"); break;
			case FLAG: printf("♨"); break;
			default: printf("%2d", winarr[i][j]);
			}
		}
		gotoxy(5, 5 + i);
	}
}