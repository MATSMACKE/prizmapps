#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/misc.h>
#include <fxcg/rtc.h>

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <CONVERT_syscalls.h>

#define LEFT 38
#define RIGHT 27
#define UP 28
#define DOWN 37

#define MENU 48

// STATS:
// Field size: 21*8

int PRGM_GetKey(void) {
    unsigned char buffer[12];
    PRGM_GetKey_OS( buffer );
    return ( buffer[1] & 0x0F ) * 10 + ( ( buffer[2] & 0xF0 ) >> 4 );
}

void printNumber(int number, int x, int y) {
    char buffer[14]; 
    itoa(number,(unsigned char*)buffer + 2);
    PrintXY(x,y,buffer,0,TEXT_COLOR_BLUE);
}

void clearScreen(void) {
    /*Bdisp_AllClr_VRAM();
    Bdisp_PutDisp_DD();*/

    PrintXY(1,1, "--                     ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    PrintXY(1,2, "--                     ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    PrintXY(1,3, "--                     ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    PrintXY(1,4, "--                     ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    PrintXY(1,5, "--                     ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    PrintXY(1,6, "--                     ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    PrintXY(1,7, "--                     ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
    PrintXY(1,8, "--                     ", TEXT_MODE_NORMAL, TEXT_COLOR_BLACK);
}

void drawHead(int x, int y, int direction) {
    if (direction == LEFT) {
        PrintXY(x,y,"--<", TEXT_MODE_NORMAL, TEXT_COLOR_GREEN);
    }
    if (direction == RIGHT) {
        PrintXY(x,y,"-->", TEXT_MODE_NORMAL, TEXT_COLOR_GREEN);
    }
    if (direction == UP) {
        PrintXY(x,y,"--^", TEXT_MODE_NORMAL, TEXT_COLOR_GREEN);
    }
    if (direction == DOWN) {
        PrintXY(x,y,"--v", TEXT_MODE_NORMAL, TEXT_COLOR_GREEN);
    }
}

void movePiece(int *x, int*y, int direction) {
    if (direction == LEFT) {
        *x = *x - 1;
    }
    if (direction == RIGHT) {
        *x = *x + 1;
    }
    if (direction == UP) {
        *y = *y - 1;
    }
    if (direction == DOWN) {
        *y = *y + 1;
    }
}


int checkCollision(int x, int y, int snake[168][2], int snake_length) {
    if (x > 32 || y > 18 || x < 1 || y < 1) {
        return 1;
    }
    else {
        for (int i = 0; i < snake_length - 1; i++) {
            if (x == snake[i][0] && y == snake[i][1]){
                return 1;
            }
        }
        return 0;
    }
}


// ------------------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
int key;

int playing;
int score;

int input;
int direction;

//Array represents X, Y
int apple[2];
int snake[164][2];
int snake_length = 3;

// Every array represents a spot on the field, every array within the array uses X, Y, Direction, Distance along the snake
int turns[168][4];
int number_of_turns = 0;

// Array represents X, Y, Direction
int head[3];

int timer_slot;

void frame() {
    //INPUT
    input = PRGM_GetKey();
    if (input == LEFT || input == RIGHT || input == UP || input == DOWN) {
        direction = input;
    }
    else if(input == MENU) {
        playing = 0;
    }


    //printNumber(direction, 1, 1);


    //LOGIC
    movePiece(&head[0], &head[1], direction);
    if (checkCollision(head[0], head[1], snake, snake_length)) {
        playing = 0;
    }
    if (head[0] == apple[0] && head[1] == apple[1]) {
        score++;
        apple[0] = (sys_rand()%21) + 1;
        apple[1] = (sys_rand()%7) + 1;
    }

    //printNumber(head[0], 1, 2);
    //printNumber(head[1], 1, 3);


    
    //GRAPHICS
    clearScreen();
    drawHead(head[0], head[1], head[2]);
    PrintXY(apple[0], apple[1], "--O", 0, TEXT_COLOR_RED);
    
}

void main(void) {
    clock_t current_time = clock();
    sys_srand(current_time);
    

    

    PrintXY(9,4,"--SNAKE", 0, TEXT_COLOR_BLACK);
    PrintXY(4,5,"--PRESS ANY KEY", 0, TEXT_COLOR_BLACK);
    PrintXY(9,4,"--     ", 0, TEXT_COLOR_BLACK);
    GetKey(&key);

    playing = 1;
    head[0] = 3;
    head[1] = 4;
    head[2] = RIGHT;

    snake[0][0] = 2;
    snake[0][1] = 4;
    snake[1][0] = 1;
    snake[1][1] = 4;

    apple[0] = (sys_rand()%21) + 1;
    apple[1] = (sys_rand()%7) + 1;
    clearScreen();
    timer_slot = Timer_Install(0, frame, 500);
    Timer_Start(timer_slot);
    
    while (1) {
        GetKey(&key);
        if(input == MENU) {
            playing = 0;
        }
    }

    return;
}
 