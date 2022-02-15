/*
 * proyectofinal.c
 *
 * Created: 06/02/2022 12:02:06 p. m.
 * Author: hfran
 */
#asm
    .equ __lcd_port=0x0B   //LCD en PuertoD
    .equ __lcd_EN=3
    .equ __lcd_RS=2
    .equ __lcd_D4=4
    .equ __lcd_D5=5
    .equ __lcd_D6=6
    .equ __lcd_D7=7
   #endasm

#include <io.h>
#include <delay.h>
#include <display.h>
#include <stdio.h>
#include <stdlib.h>
#include <mega328p.h>

#define INTERVAL 750

flash int du=262,re= 294, ri=312, mi =330,fa=349, fi=370;
unsigned char highScore = 0, score = 0, iterator = 0, current = 0;
char printSc[2], printHsc[2];
char specChar [8] = { 0x04,0x0E,0x0E,0x1F,0x0E,0x0E,0x04,0x00 };
char pattern[21];
bit isDefeat = 0;

void cls(){
    MoveCursor(0,0);
    StringLCD("                ");
    MoveCursor(0,1);
    StringLCD("                ");
}

void tono (char sel) {
    float f, Cuentas;
    unsigned int CuentasEnt;
    switch (sel){
        case 0:
            f = du*2;
            PORTC.4 = 1;
            break;

        case 1:
            f = re*2;
            PORTC.5 = 1;
            break;
        case 2:
            f = ri*2;
            PORTB.5 = 1;
            break;
        case 3:
            f = mi*2;
            PORTB.4 = 1;
            break;
        case 4:
            f = fa;
            break;
        case 5:
            f = fi*2;
            break;
    }

    DDRB.1 = 1;
    Cuentas=500000.0/f;
    CuentasEnt=Cuentas;
    if ((Cuentas-CuentasEnt)>=0.5)  //Redondear
       CuentasEnt++;
    TCCR1A=0x40;
    TCCR1B=0x09;
    OCR1AH=(CuentasEnt-1)>>8;
    OCR1AL=(CuentasEnt-1)&0xFF;
}

void noTono() {
    TCCR1A=0x00;
    TCCR1B=0x00;
    PORTC = 0x0F;
    PORTB=0;  //Apaga puerto manualmente
}

void greeting() {
    MoveCursor(3, 0);
    StringLCD("WELCOME TO");
    MoveCursor(2, 1);
    CharLCD(0);
    MoveCursor(3, 1);
    StringLCD("SIMON SAYS");
    MoveCursor(13, 1);
    CharLCD(0);
    delay_ms(1500);
    cls();
}

void autoGeneratePattern(){
    srand(TCNT0);
    for(iterator = 0; iterator < 21; iterator++){
        pattern[iterator] = rand() % 4;
    }
}

void displayToStart(){
    MoveCursor(0,0);
    StringLCD("   PRESS ANY   ");
    MoveCursor(0,1);
    StringLCD("BUTTON TO START");
}

void pressToStart(){
    TCCR0B = 0x01;
    displayToStart();
    PORTB = 0x0F;
    while(PINC & 0x0F){
        if(PINC.0 == 0 || PINC.1 == 0 || PINC.2 == 0 || PINC.3 == 0){
            break;
        }
    }
    autoGeneratePattern();
    cls();
}

void printScore(){
    MoveCursor(0,1);
    StringLCD("SCORE: ");
    sprintf(printSc, "%02u", score);
    StringLCDVar(printSc);
}

void refreshScore(){
    MoveCursor(0,0);
    StringLCD("HIGHSCORE: ");
    highScore = highScore > score ? highScore : score;
    sprintf(printHsc,"%02u", highScore);
    StringLCDVar(printHsc);
    printScore();
}


void showPattern(){
    delay_ms(INTERVAL);
    for(iterator = 0; iterator <= current; iterator++){
        switch(pattern[iterator]){
            case 0:
                tono(0);
                delay_ms(INTERVAL);
                noTono();
                delay_ms(INTERVAL/2);
                break;
            case 1:
                tono(1);
                delay_ms(INTERVAL);
                noTono();
                delay_ms(INTERVAL/2);
                break;
            case 2:
                tono(2);
                delay_ms(INTERVAL);
                noTono();
                delay_ms(INTERVAL/2);
                break;
            case 3:
                tono(3);
                delay_ms(INTERVAL);
                noTono();
                delay_ms(INTERVAL/2);
                break;
        }
    }
}

void blinkLeds(char tone){
    PORTB.4 = 1;
    PORTB.5 = 1;
    PORTC = 0x3F;
    tono(tone);
    delay_ms(INTERVAL/2);
    noTono();
    delay_ms(INTERVAL);
}

void showLoss(){
    MoveCursor(0,0);
    StringLCD("  YOU LOST!!!  ");
    printScore();
    delay_ms(2000);
    cls();
    pressToStart();
}

void showWin(){
    MoveCursor(0,0);
    StringLCD("   YOU WON!!!  ");
    printScore();
    delay_ms(2000);
    cls();
    pressToStart();
}

void loseGame(){
    char n;
    isDefeat = 1;
    for(n = 0; n < 4; n++){
        blinkLeds(4);
    }
    showLoss();
}

void winGame(){
    char n;
    isDefeat = 1;
    for(n = 0; n < 4; n++){
        blinkLeds(5);
    }
    showWin();
}

char individualSelection(){
    char selection;
    unsigned char lect;
    PORTB = 0x0F;
    while(PINC & 0x0F){
        if(PINC.0 == 0){
            lect = 0x0E;
            while(PINC.0 == 0);
            delay_ms(10);
            break;
        }
        if(PINC.1 == 0){
            lect = 0x0D;
            while(PINC.1 == 0);
            delay_ms(10);
            break;
        }
        if(PINC.2 == 0){
            lect = 0x0B;
            while(PINC.2 == 0);
            delay_ms(10);
            break;
        }
        if(PINC.3 == 0){
            lect = 0x07;
            while(PINC.3 == 0);
            delay_ms(10);
            break;
        }
    }
    switch(lect){
        case 0x0E:
            selection = 0;
            break;
        case 0x0D:
            selection = 1;
            break;
        case 0x0B:
            selection = 2;
            break;
        case 0x07:
            selection = 3;
            break;
    }
    tono(selection);
    delay_ms(INTERVAL);
    noTono();
    delay_ms(INTERVAL/2);
    return selection;
}

void selectPattern(){
    char s;
    for(iterator = 0; iterator <= current;iterator++){
        s = individualSelection();
        if(pattern[iterator] == s){
            continue;
        } else {
            loseGame();
            break;
        }
    }
}

void playSequence(){
    showPattern();
    selectPattern();
    if(!isDefeat){
        score++;
        blinkLeds(5);
    }
}

void resetGame(){
    score = 0;
    current = 0;
    isDefeat = 0;
}

void main(void) {
    CLKPR=0x80;
    CLKPR=0x04;
    DDRC = 0x30; // 4 - 5 salidas
    PORTC = 0x0F; // 0 - 3 pull-ups
    DDRB = 0x30; // 4 - 5 salidas
    SetupLCD();
    CreateChar(0, specChar);
    greeting();
    pressToStart();
    while (1) {
        refreshScore();
        playSequence();
        current++;
        if(current == 20){
            winGame();
        }
        if(isDefeat){
            resetGame();
        }
    }
}
