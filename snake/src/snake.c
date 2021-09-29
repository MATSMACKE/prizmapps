#include "snake.h"

// ==============================
//THE GAME AREA:
// DISPLAY RES = 384/216
// BLOCKS OF 8x8
// 48 x 24 BLOCKS
// ==============================

int main() {
    //===========================
    // INIT
    //===========================

    EnableStatusArea(3);

    Bdisp_EnableColor(1);
    int key = 0;

    clock_t current_time = clock();
    sys_srand(current_time);

    int scores[5] = {0, 0, 0, 0, 0};


    //===========================
    // MAIN LOOP
    //===========================
    for (;;) {
        int menuOption = mainMenu();

        if (menuOption == MAIN_MENU_PLAY) {
            playGame();
        }
    }
}

block makeBlock(short x, short y) {
    block newBlock;
    newBlock.x = x;
    newBlock.y = y;
    return newBlock;
}

block makeApple() {
    block newApple;
    newApple.x = randBetween(0, GAME_AREA_WIDTH);
    newApple.y = randBetween(0, GAME_AREA_HEIGHT);
    return newApple;
}

int clamp(int x, int y) {
    if (x < 0) return 0;
    if (x > y) return y;
    return x;
}

void fillArea(int x, int y, int w, int h, color_t color){
    w = (x + w > LCD_WIDTH_PX) ? LCD_WIDTH_PX - x : ((x < 0) ? x + w : w);
    h = (y + h > LCD_HEIGHT_PX) ? LCD_HEIGHT_PX - y : ((y < 0) ? y + h : h);
    if (w <= 0 || h <= 0) return;
    x = clamp(x, LCD_WIDTH_PX);
    y = clamp(y, LCD_HEIGHT_PX);
    unsigned short * s = (unsigned short*)GetVRAMAddress();
    s += (y*384) + x;
    while(h--){
        int w2 = w;
        while(w2--)
            *s++ = color;
        s += 384 - w;
    }
}

void fillScreen(color_t color) {
    fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, color);
}

static int keyPressed(int basic_keycode){
    const unsigned short* keyboard_register = (unsigned short*)0xA44B0000;
    int row, col, word, bit;
    row = basic_keycode % 10;
    col = basic_keycode / 10-1;
    word = row >> 1;
    bit = col + ((row & 1) << 3);
    return (0 != (keyboard_register[word] & 1<<bit));
}

int randBetween(int start, int end) {
    return (sys_rand()%(end-start)) + start;
}

int randPosNeg(){
    return (sys_rand() % 2 == 0) ? -1 : 1;
}

void printNumber(int number, int x, int y) {
    char buffer[14]; 
    itoa(number,(unsigned char*)buffer + 2); // Leave the garbage bytes alone
    PrintXY(x, y, buffer, 1, TEXT_COLOR_BLUE);
}

int mainMenu() {
    int cursor = 0;
    int key;
    DisplayStatusArea();
    for (;;) {
        fillScreen(COLOR_WHITE);
        PrintXY(1, 1, "  SNAKE", cursor == 0, 0);
        PrintXY(1, 2, "  1: Play", cursor == MAIN_MENU_PLAY, 0);
        PrintXY(1, 3, "  2: Scores", cursor == MAIN_MENU_SCORES, 0);
        GetKey(&key);

        if (key == KEY_CTRL_EXE) {
            return cursor;
        }

        else if (key == KEY_CTRL_DOWN) {
            cursor += 1;
            if (cursor > 2) cursor = 0;
        }

        else if (key == KEY_CTRL_UP) {
            cursor -= 1;
            if (cursor < 0) cursor = 2;
        }

        else if (key == KEY_CTRL_EXIT) {
            cursor = 0;
        }
    }
}

int playGame() {
    int key;

    block snake[1152];
    snake[0] = makeBlock(1, 0);
    snake[1] = makeBlock(1, 0);
    snake[2] = makeBlock(0, 0);

    int snakeLength = 3;
    short snakeDirection = RIGHT;

    block apple = makeApple();

    for (;;) {
        int initTicks = RTC_GetTicks();
        while (RTC_GetTicks() < initTicks + FRAMELENGTH) {
            if (keyPressed(KEY_PRGM_MENU)){
            GetKey(&key);
            }
            else if (keyPressed(KEY_PRGM_EXIT)) {
                return 0;
            }

            if (keyPressed(KEY_PRGM_UP)){
                snakeDirection = UP;
            }
            else if (keyPressed(KEY_PRGM_RIGHT)){
                snakeDirection = RIGHT;
            }
            else if (keyPressed(KEY_PRGM_DOWN)){
                snakeDirection = DOWN;
            }
            else if (keyPressed(KEY_PRGM_LEFT)){
                snakeDirection = LEFT;
            }
        };

        if (keyPressed(KEY_PRGM_UP)){
                snakeDirection = UP;
        }
        else if (keyPressed(KEY_PRGM_RIGHT)){
            snakeDirection = RIGHT;
        }
        else if (keyPressed(KEY_PRGM_DOWN)){
            snakeDirection = DOWN;
        }
        else if (keyPressed(KEY_PRGM_LEFT)){
            snakeDirection = LEFT;
        }

        fillScreen(COLOR_BLACK);

        fillArea(0, 24, LCD_WIDTH_PX, 1, COLOR_WHITE);

        if (snakeDirection == UP) {
            snake[0].y -= 1;
        }
        else if (snakeDirection == RIGHT) {
            snake[0].x += 1;
        }
        else if (snakeDirection == DOWN) {
            snake[0].y += 1;
        }
        else {
            snake[0].x -= 1;
        }

        if (snake[0].x < 0 || snake[0].x >= GAME_AREA_WIDTH || snake[0].y < 0 || snake[0].y >= GAME_AREA_HEIGHT) {
            gameover(snakeLength);
            return snakeLength;
        }

        for (int i = 1; i < snakeLength; i++) {
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                gameover(snakeLength);
                return snakeLength;
            }
        }

        if (snake[0].x == apple.x && snake[0].y == apple.y) {
            snake[snakeLength] = snake[snakeLength - 1];
            snakeLength++;
            int generating = TRUE;
            while (generating) {
                apple = makeApple();
                for (int i = 0; i < snakeLength; i++) {
                    if (apple.x == snake[i].x && apple.y == snake[i].y) {
                        break;
                    }
                }
                generating = FALSE;
            }
        }

        for (int i = snakeLength; i > 0; i--) {
            snake[i] = snake[i - 1];
            fillArea((snake[i].x * 8) + 2, (snake[i].y * 8) + 24 + 2, 6, 6, COLOR_WHITE);
        }

        fillArea((apple.x * 8) + 2, (apple.y * 8) + 24 + 2, 6, 6, COLOR_RED);

        if (keyPressed(KEY_PRGM_MENU)){
            GetKey(&key);
        }
        if (keyPressed(KEY_PRGM_EXIT)) {
            return 0;
        }

        printNumber(snakeLength, 1, 0);

        Bdisp_PutDisp_DD();
    }
}

void gameover(int score) {
    PrintXY(1, 1, "  GAME OVER", 0, COLOR_BLACK);
    int initTicks = RTC_GetTicks();
    while (RTC_GetTicks() < initTicks + 128) {
        if (keyPressed(KEY_PRGM_MENU)){
        GetKey(&score);
        }
        else if (keyPressed(KEY_PRGM_EXIT)) {
            return;
        }
    };
    GetKey(&score);
}