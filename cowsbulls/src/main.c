#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/misc.h>
#include <fxcg/rtc.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>

//use itoa()
#include <CONVERT_syscalls.h>


static void getStrn(int x, int y, int n, char*buffer){
    int start = 0; // Used for scrolling left and right
    int cursor = 0; // Cursor position
    buffer[0] = '\0'; // This sets the first character to \0, also represented by "", an empty string
    DisplayMBString((unsigned char*)buffer, start, cursor, x, y); // Last to parameters are X,Y coords (not in pixels)
    int key;
    while(1){
        GetKey(&key); // Blocking is GOOD.  This gets standard keys processed and, possibly, powers down the CPU while waiting
        if(key == KEY_CTRL_EXE){
            // Ok
            break;
        }else if(key == KEY_CTRL_EXIT){
            // Aborted
            break;
        }
        if(key && key < 30000){
            cursor = EditMBStringChar((unsigned char*)buffer, n, cursor, key);
            DisplayMBString((unsigned char*)buffer, start, cursor, x,y);
        }else{
            EditMBStringCtrl((unsigned char*)buffer, n, &start, &cursor, &key, x, y);
        }
    }
}
void printNumber(int number, int x, int y) {
    char buffer[14]; 
    itoa(number,(unsigned char*)buffer + 2); // Leave the garbage bytes alone
    PrintXY(x,y,buffer,0,TEXT_COLOR_BLUE);
}

int randBetween(int start, int end) {
    return (sys_rand()%(end-start)) + start;
}
char randDigit() {
    return (char)randBetween(49,58);
}

const char * printableChar(char character) {
    const char* temp = strncat("  ", &character, 1);
    return temp;
}

char * generateNumber() {
    char * string = malloc(5);
    for (int x = 0; x < 4; x++) {
        *(string + x) = randDigit();
    }
    return string;
}

void checkCowsBulls(char * guess, const char * num, int * cows, int * bulls, int * turns) {
    for (int x; x < 4; x++) {
        if (*(guess+x) == *num || *(guess+x) == *(num+1) || *(guess+x) == *(num+2) || *(guess+x) == *(num+3)) {
            if (*(guess+x) == *(num+x)){
                (*cows)++;
            }
            else {
                (*bulls)++;
            }
        }
    }
    ++(*turns);
}

void playGame() {
    char * number = generateNumber();
    char * guess = malloc(5);
    int cows = 0;
    int bulls = 0;
    int turns = 0;

    for(;;) {
        getStrn(1, 2, 4, guess);
        checkCowsBulls(number, guess, &cows, &bulls, &turns);
        printNumber(cows, 7, 4);
        printNumber(bulls, 8, 5);
        printNumber(turns, 8, 6);
        locate_OS(16, 3);
        Print_OS(guess,0,0);

        if (cows == 4){
            PrintXY(1, 7, "  CONGRATS, YOU WON", 0, 0);
            break;
        }
        cows = 0;
        bulls = 0;
    }
}

void main(void){
    clock_t current_time = clock();
    sys_srand(current_time);

    Bdisp_EnableColor(0);

    int key;
    for(;;) {
        Bdisp_AllClr_VRAM();
        //Print the "explanation"
        PrintXY(1,1, "  COWS AND BULLS", 0, 0);

        PrintXY(1,3, "  PREVIOUS GUESS:", 0, 0);
        PrintXY(1,4, "  COWS:", 0, 0);
        PrintXY(1,5, "  BULLS:", 0, 0);
        PrintXY(1,6, "  TURNS:", 0, 0);
        playGame();
        GetKey(&key);
    }
    return;
}