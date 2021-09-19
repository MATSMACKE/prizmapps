#include <fxcg/display.h>
#include <fxcg/keyboard.h>

#include <stdlib.h>
#include <time.h>

//use itoa()
#include <CONVERT_syscalls.h>

#define NUM_PARTICLES 32
#define JUMP_ALLOWANCE 600
#define ASPECT_RATIO 0.5625

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

static int keyPressed(int basic_keycode){
    const unsigned short* keyboard_register = (unsigned short*)0xA44B0000;
    int row, col, word, bit;
    row = basic_keycode % 10;
    col = basic_keycode / 10-1;
    word = row >> 1;
    bit = col + ((row & 1) << 3);
    return (0 != (keyboard_register[word] & 1<<bit));
}

float xfloor(float x) {
    return ((int)x) - (x < 0);
}

// Adapted from http://www.splinter.com.au/converting-hsv-to-rgb-colour-using-c/
void hueToRGB(float h, unsigned short * r, unsigned short * g, unsigned short * b) {
    float S = 1;
    float V = 1;

    float H = h;
    while (H < 0) { H += 360; };
    while (H >= 360) { H -= 360; };
    float R, G, B;
    if (V <= 0) { 
        R = G = B = 0;
    }
    else if (S <= 0) {
        R = G = B = V;
    }
    else {
      float hf = H / 60.0;
      int i = (int)xfloor(hf);
      float f = hf - i;
      float pv = V * (1 - S);
      float qv = V * (1 - S * f);
      float tv = V * (1 - S * (1 - f));
      switch (i) {
          // Red is the dominant color
          case 0:
              R = V;
              G = tv;
              B = pv;
              break;
          // Green is the dominant color
          case 1:
              R = qv;
              G = V;
              B = pv;
              break;
          case 2:
              R = pv;
              G = V;
              B = tv;
              break;
          // Blue is the dominant color
          case 3:
              R = pv;
              G = qv;
              B = V;
              break;
          case 4:
              R = tv;
              G = pv;
              B = V;
              break;
          // Red is the dominant color
          case 5:
              R = V;
              G = pv;
              B = qv;
              break;
      }
    }
    *r = clamp((unsigned short)(R * 31.0), 31);
    *g = clamp((unsigned short)(G * 63.0), 63);
    *b = clamp((unsigned short)(B * 31.0), 31);
}

unsigned short getColor(int x){
    if (x >= 360) {
        x -= 360;
    }
    unsigned short red, green, blue;
    hueToRGB((float)x, &red, &green, &blue);
    return (red * 2048) + (green * 32) + blue;
}


int randBetween(int start, int end) {
    return (sys_rand()%(end-start)) + start;
}

int randPosNeg(){
    return (sys_rand() % 2 == 0) ? -1 : 1;
}

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    float maxheight;
} particle;

int particleSize = 4;
int speed = 10;

particle generateParticleMode1() {
    particle generated;
    generated.x = LCD_WIDTH_PX/2;
    generated.y = LCD_HEIGHT_PX/2;
    generated.vx = speed * (((float)randPosNeg()) * ((float)randBetween(10, 100)))/200.;
    generated.vy = speed * (((float)randPosNeg()) * ((float)randBetween(8, 60)))/200.;
    return generated;
}

particle generateParticleMode2() {
    particle generated;
    generated.x = -1 * particleSize;
    generated.y = (float)randBetween(0, LCD_HEIGHT_PX);
    generated.vx = speed * (float)randBetween(20, 60)/200.;
    generated.vy = 0.;
    generated.maxheight = generated.y;
    return generated;
}

particle generateParticleMode3() {
    particle generated;
    generated.x = -1 * particleSize;
    generated.y = (float)randBetween(0, LCD_HEIGHT_PX);
    generated.vx = speed * (float)randBetween(20, 60)/200.;
    return generated;
}

particle generateParticleMode4() {
    particle generated;
    generated.vx = speed * ((float)randBetween(-50, 50))/800.;
    generated.vy = speed * ((float)randBetween(-50, 50))/800.;
    generated.x = LCD_WIDTH_PX/2;
    generated.y = LCD_HEIGHT_PX/2;
    return generated;
}

void printNumber(int number, int x, int y) {
    char buffer[14]; 
    itoa(number,(unsigned char*)buffer + 2); // Leave the garbage bytes alone
    PrintXY(x,y,buffer,0,TEXT_COLOR_BLUE);
}

void showModes(int selected) {
    PrintXY(1, 1, "  P", selected == 0, TEXT_COLOR_RED);
    PrintXY(2, 1, "  a", selected == 0, TEXT_COLOR_YELLOW);
    PrintXY(3, 1, "  r", selected == 0, TEXT_COLOR_GREEN);
    PrintXY(4, 1, "  t", selected == 0, TEXT_COLOR_CYAN);
    PrintXY(5, 1, "  i", selected == 0, TEXT_COLOR_BLUE);
    PrintXY(6, 1, "  c", selected == 0, TEXT_COLOR_PURPLE);
    PrintXY(7, 1, "  l", selected == 0, TEXT_COLOR_RED);
    PrintXY(8, 1, "  e", selected == 0, TEXT_COLOR_YELLOW);
    PrintXY(9, 1, "  s", selected == 0, TEXT_COLOR_GREEN);

    PrintXY(1, 2, "  1: DVD", selected == 1, TEXT_COLOR_BLUE);
    PrintXY(1, 3, "  2: Bouncing Balls", selected == 2, TEXT_COLOR_BLACK);
    PrintXY(1, 4, "  3: Parallax", selected == 3, TEXT_COLOR_RED);
    PrintXY(1, 5, "  4: Warpspeed", selected == 4, TEXT_COLOR_PURPLE);
    PrintXY(1, 6, "  5: Settings", selected == 5, TEXT_COLOR_BLACK);
}

void main(void) {
    Bdisp_EnableColor(1);
    int key = 0;
    int rainbow = 1;

    clock_t current_time = clock();
    sys_srand(current_time);

    particle particles[NUM_PARTICLES];

    fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0xFFFF);

    int selectedMode = 0;
    int modeSelected = 0;

    for (;;) {
        if (key == KEY_CHAR_1 || (selectedMode == 1 && modeSelected == 1)) {
            fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0x0000);
            
            for (int i = 0; i < NUM_PARTICLES; i++) {
                particles[i] = generateParticleMode1();
            }

            for (;;) {
                fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0x0000);
                for (int i = 0; i < NUM_PARTICLES; i++) {
                    particles[i].x += particles[i].vx;
                    particles[i].y += particles[i].vy;
                    if (particles[i].x < 0){
                        particles[i].x *= -1;
                        particles[i].vx *= -1;
                    }
                    if (particles[i].x + particleSize -1 > LCD_WIDTH_PX){
                        particles[i].x -= 2*(particles[i].x + particleSize -1 - LCD_WIDTH_PX);
                        particles[i].vx *= -1;
                    }
                    if (particles[i].y < 0){
                        particles[i].y *= -1;
                        particles[i].vy *= -1;
                    }
                    if (particles[i].y + particleSize -1 > LCD_HEIGHT_PX){
                        particles[i].y -= 2*(particles[i].y + particleSize -1 - LCD_HEIGHT_PX);
                        particles[i].vy *= -1;
                    }
                    fillArea((int)(particles[i].x - particles[i].vx), (int)(particles[i].y - particles[i].vy), particleSize, particleSize, 0x0000);
                    fillArea((int)particles[i].x, (int)particles[i].y, particleSize, particleSize, (rainbow) ? (color_t)getColor(particles[i].x) : 0xFFFF);
                }
                
                if(keyPressed(KEY_PRGM_MENU) || keyPressed(KEY_PRGM_EXIT)){
                    int key;
                    GetKey(&key);
                    modeSelected = 0;
                    break;
                }

                Bdisp_PutDisp_DD();
            }
        }
        if (key == KEY_CHAR_2 || (selectedMode == 2 && modeSelected == 1)) {
            fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0x0000);
            
            for (int i = 0; i < NUM_PARTICLES; i++) {
                particles[i] = generateParticleMode2();
            }

            for (;;) {
                fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0x0000);
                for (int i = 0; i < NUM_PARTICLES; i++) {
                    //if (particles[i].y <= particles[i].maxheight) {particles[i].y = particles[i].maxheight;particles[i].vy = 0;}
                    particles[i].vy += 0.8;
                    particles[i].x += particles[i].vx;
                    particles[i].y += particles[i].vy;
                    if (particles[i].x > LCD_WIDTH_PX){
                        particles[i] = generateParticleMode2();
                    }
                    if (particles[i].y + particleSize -1 > LCD_HEIGHT_PX){
                        particles[i].y -= 2*(particles[i].y - LCD_HEIGHT_PX);
                        particles[i].vy *= -1;
                    }
                    fillArea((int)(particles[i].x - particles[i].vx), (int)(particles[i].y - particles[i].vy), particleSize, particleSize, 0x0000);
                    fillArea((int)particles[i].x, (int)particles[i].y, particleSize, particleSize, (rainbow) ? (color_t)getColor(particles[i].x) : 0xFFFF);
                }
                
                if(keyPressed(KEY_PRGM_MENU) || keyPressed(KEY_PRGM_EXIT)){
                    int key;
                    GetKey(&key);
                    modeSelected = 0;
                    break;
                }
                Bdisp_PutDisp_DD();
            }
        }
        if (key == KEY_CHAR_3 || (selectedMode == 3 && modeSelected == 1)) {
            fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0x0000);
            
            for (int i = 0; i < NUM_PARTICLES; i++) {
                particles[i] = generateParticleMode3();
            }

            for (;;) {
                fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0x0000);
                for (int i = 0; i < NUM_PARTICLES; i++) {
                    particles[i].x += particles[i].vx;
                    if (particles[i].x > LCD_WIDTH_PX){
                        particles[i] = generateParticleMode2();
                    }
                    fillArea((int)(particles[i].x - particles[i].vx), (int)(particles[i].y - particles[i].vy), particleSize, particleSize, 0x0000);
                    fillArea((int)particles[i].x, (int)particles[i].y, particleSize, particleSize, (rainbow) ? (color_t)getColor(particles[i].x) : 0xFFFF);
                }
                
                if(keyPressed(KEY_PRGM_MENU) || keyPressed(KEY_PRGM_EXIT)){
                    int key;
                    GetKey(&key);
                    modeSelected = 0;
                    break;
                }
                Bdisp_PutDisp_DD();
            }
        }
        
        if (key == KEY_CHAR_4 || (selectedMode == 4 && modeSelected == 1)) {
            fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0x0000);
            
            for (int i = 0; i < NUM_PARTICLES; i++) {
                particles[i] = generateParticleMode4();
            }

            for (;;) {
                fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0x0000);
                for (int i = 0; i < NUM_PARTICLES; i++) {
                    particles[i].x += particles[i].vx;
                    particles[i].y += particles[i].vy;
                    particles[i].vx *= 1 + 0.005 * speed;
                    particles[i].vy *= 1 + (0.005 * ASPECT_RATIO * speed);
                    if (particles[i].x > LCD_WIDTH_PX || particles[i].x < -1 * particleSize || particles[i].y > LCD_HEIGHT_PX || particles[i].y < -1 * particleSize){
                        particles[i] = generateParticleMode4();
                    }
                    fillArea((int)(particles[i].x - particles[i].vx), (int)(particles[i].y - particles[i].vy), particleSize, particleSize, 0x0000);
                    fillArea((int)particles[i].x, (int)particles[i].y, particleSize, particleSize, (rainbow) ? (color_t)getColor(particles[i].x) : 0xFFFF);
                }
                
                if(keyPressed(KEY_PRGM_MENU) || keyPressed(KEY_PRGM_EXIT)){
                    int key;
                    GetKey(&key);
                    modeSelected = 0;
                    break;
                }
                Bdisp_PutDisp_DD();
            }
        }
        if (key == KEY_CHAR_5 || (selectedMode == 5 && modeSelected == 1)) {
            int cursor = 0;
            for (;;) {
                fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0xFFFF);
                PrintXY(1, 1, "  Settings", cursor == 0, 0);
                PrintXY(1, 2, "  1: Size       ", cursor == 1, 0);
                printNumber(particleSize, 15, 2);
                PrintXY(1, 3, rainbow ? "  2: Rainbow    YES" : "  2: Rainbow    NO", cursor == 2, TEXT_COLOR_BLUE);
                PrintXY(1, 4, "  3: Speed (%)  ", cursor == 3, 0);
                printNumber((int)(speed * 10), 15, 4);
                GetKey(&key);
                if (key == KEY_CTRL_DOWN) {
                    cursor += 1;
                    if (cursor > 3) cursor = 0;
                }
                if (key == KEY_CTRL_UP) {
                    cursor -= 1;
                    if (cursor < 0) cursor = 3;
                }
                if (key == KEY_CTRL_RIGHT) {
                    if (cursor == 1) particleSize += 1;
                    if (cursor == 2) rainbow = rainbow ? 0 : 1;
                    if (cursor == 3) speed += 1;

                }
                if (key == KEY_CTRL_LEFT) {
                    if (cursor == 1) particleSize -= (particleSize > 1) ? 1 : 0;
                    if (cursor == 2) rainbow = rainbow ? 0 : 1;
                    if (cursor == 3) speed -= (speed > 1) ? 1 : 0.;
                }
                if (key == KEY_CTRL_EXIT) {
                    modeSelected = 0;
                    break;
                }
            }
        }

        fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0xFFFF);
        showModes(selectedMode);
        GetKey(&key);
        if (key == KEY_CTRL_DOWN) {
            selectedMode += 1;
            if (selectedMode > 5) selectedMode = 0;
        }
        if (key == KEY_CTRL_UP) {
            selectedMode -= 1;
            if (selectedMode < 0) selectedMode = 5;
        }
        if (key == KEY_CTRL_EXE) {
            if (selectedMode != 0) modeSelected = 1;
        }
        fillArea(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, 0xFFFF);
    }
    return;
}