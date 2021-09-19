#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <string.h>

//use itoa()
#include <CONVERT_syscalls.h>

void printNumber(int number, int x, int y) {
    char buffer[14]; 
    itoa(number,(unsigned char*)buffer + 2); // Leave the garbage bytes alone
    PrintXY(x,y,buffer,0,TEXT_COLOR_BLUE);
}


void drawBars() {
    unsigned short *p = GetVRAMAddress();
    int i;

    for (i = 0; i < LCD_WIDTH_PX; i++) *p++ = 0;

    p += LCD_WIDTH_PX * (LCD_HEIGHT_PX - 1);

    for (i = 0; i < LCD_WIDTH_PX; i++) *--p = 0;
    Bdisp_PutDisp_DD();
}

void clearScreen(void) {
    Bdisp_AllClr_VRAM();
    Bdisp_PutDisp_DD();
}
 
void main(void) {
    int key;
     
    Bdisp_AllClr_VRAM();
    /*locate_OS(1,1);
    Print_OS("Press EXE to exit", 0, 0);

    while (1) {
        GetKey(&key);
        PrintXY(2,8,"--Hello World!", TEXT_MODE_NORMAL, TEXT_COLOR_RED);
        int x = 0;
        int y = 70;
        PrintMini(&x, &y, "abcdEFGH1234!@#$.,;'", 0, 0xffffffff, 0,0,COLOR_DARKGREEN, COLOR_GRAY, 1,0);

        if (key == KEY_CTRL_EXE) {
            break;
        }
    }*/
    drawBars();
    unsigned short *pointer = GetVRAMAddress();
    clearScreen();
    printNumber(11,1 , 1);
    while (1) {
        GetKey(&key);
    }
 
    return;
}
