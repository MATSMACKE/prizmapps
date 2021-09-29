#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/misc.h>
#include <fxcg/rtc.h>

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <CONVERT_syscalls.h>

#define TRUE 1
#define FALSE 0

#define MAIN_MENU_PLAY 1
#define MAIN_MENU_SCORES 2

#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

#define GAME_AREA_WIDTH 48
#define GAME_AREA_HEIGHT 24

#define FRAMELENGTH 32


typedef struct {
    short x;
    short y;
} block;

block makeBlock(short x, short y);

block makeApple();

int clamp(int x, int y);

void fillArea(int x, int y, int w, int h, color_t color);

void fillScreen(color_t color);

static int keyPressed(int basic_keycode);

int randBetween(int start, int end);

int randPosNeg();

void printNumber(int number, int x, int y);

int mainMenu();

int playGame();

void gameover(int score);