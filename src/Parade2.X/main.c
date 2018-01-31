#include <xc.h>

#pragma config FOSC = INTOSCIO
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = OFF
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = ON
#pragma config IESO = OFF
#pragma config FCMEN = OFF

#define _XTAL_FREQ 8000000

#define DO1 1911
#define RE1 1702
#define MI1 1517
#define FA1 1432
#define SO1 1275
#define RA1 1136
#define SI1 1012

#define DO2 955
#define RE2 851
#define MI2 758
#define FA2 716
#define SO2 637
#define RA2 568
#define SI2 506

#define DO3 477

#define TMP 120.0

void playDO1(float time){
    int count = (int)(time * 400000.0 / DO1);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(DO1);
        PORTBbits.RB0 = 0;
        __delay_us(DO1);
    }
    __delay_ms(time * 200);
}

void playRE1(float time){
    int count = (int)(time * 400000.0 / RE1);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(RE1);
        PORTBbits.RB0 = 0;
        __delay_us(RE1);
    }
    __delay_ms(time * 200);
}

void playMI1(float time){
    int count = (int)(time * 400000.0 / MI1);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(MI1);
        PORTBbits.RB0 = 0;
        __delay_us(MI1);
    }
    __delay_ms(time * 200);
}

void playFA1(float time){
    int count = (int)(time * 400000.0 / FA1);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(FA1);
        PORTBbits.RB0 = 0;
        __delay_us(FA1);
    }
    __delay_ms(time * 200);
}

void playSO1(float time){
    int count = (int)(time * 400000.0 / SO1);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(SO1);
        PORTBbits.RB0 = 0;
        __delay_us(SO1);
    }
    __delay_ms(time * 200);
}

void playRA1(float time){
    int count = (int)(time * 400000.0 / RA1);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(RA1);
        PORTBbits.RB0 = 0;
        __delay_us(RA1);
    }
    __delay_ms(time * 200);
}

void playSI1(float time){
    int count = (int)(time * 400000.0 / SI1);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(SI1);
        PORTBbits.RB0 = 0;
        __delay_us(SI1);
    }
    __delay_ms(time * 200);
}

void playDO2(float time){
    int count = (int)(time * 400000.0 / DO2);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(DO2);
        PORTBbits.RB0 = 0;
        __delay_us(DO2);
    }
    __delay_ms(time * 200);
}

void playRE2(float time){
    int count = (int)(time * 400000.0 / RE2);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(RE2);
        PORTBbits.RB0 = 0;
        __delay_us(RE2);
    }
    __delay_ms(time * 200);
}

void playMI2(float time){
    int count = (int)(time * 400000.0 / MI2);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(MI2);
        PORTBbits.RB0 = 0;
        __delay_us(MI2);
    }
    __delay_ms(time * 200);
}

void playFA2(float time){
    int count = (int)(time * 400000.0 / FA2);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(FA2);
        PORTBbits.RB0 = 0;
        __delay_us(FA2);
    }
    __delay_ms(time * 200);
}

void playSO2(float time){
    int count = (int)(time * 400000.0 / SO2);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(SO2);
        PORTBbits.RB0 = 0;
        __delay_us(SO2);
    }
    __delay_ms(time * 200);
}

void playRA2(float time){
    int count = (int)(time * 400000.0 / RA2);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(RA2);
        PORTBbits.RB0 = 0;
        __delay_us(RA2);
    }
    __delay_ms(time * 200);
}

void playSI2(float time){
    int count = (int)(time * 400000.0 / SI2);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(SI2);
        PORTBbits.RB0 = 0;
        __delay_us(SI2);
    }
    __delay_ms(time * 200);
}

void playDO3(float time){
    int count = (int)(time * 400000.0 / DO3);
    for (int i = 0; i < count; i++){
        PORTBbits.RB0 = 1;
        __delay_us(DO3);
        PORTBbits.RB0 = 0;
        __delay_us(DO3);
    }
    __delay_ms(time * 200);
}

void main() {
    OSCCON = 0x70;
    TRISB = 0x02;
    
    //while(PORTBbits.RB1 == 1);
    
    for (int i = 0; i < 2; i++){
        playDO1(TMP / 60.0 / 8.0);
        for (int j = 0; j < 7; j++)
            playMI1(TMP / 60.0 / 8.0);
    }

    playRE1(TMP / 60.0 / 8.0);
    for (int j = 0; j < 7; j++)
        playFA1(TMP / 60.0 / 8.0);
    
    __delay_ms(TMP / 0.06 / 16.0);
    playRA1(TMP / 60.0 / 8.0);
    playRA1(TMP / 60.0 / 8.0);
    playSI1(TMP / 60.0 / 8.0);
    playSI1(TMP / 60.0 / 8.0);
    
    __delay_ms(TMP / 0.06 / 16.0);
    playSO1(TMP / 60.0 / 6.0);
    playSO1(TMP / 60.0 / 16.0);
    playDO2(TMP / 60.0 / 2);
    
    playDO2(TMP / 60.0 / 6.0);
    playMI2(TMP / 60.0 / 9.0);
    playSO2(TMP / 60.0 / 9.0);
    playDO3(TMP / 60.0 / 9.0);
    playSI2(TMP / 60.0 / 9.0);
    playRA2(TMP / 60.0 / 9.0);
    playSO2(TMP / 60.0 / 4.0);
    
    //__delay_ms(TMP / 0.06 / 32.0);
    playMI2(TMP / 60.0 / 8.0);
    playSO2(TMP / 60.0 / 9.0);
    playFA2(TMP / 60.0 / 6.0);
    //__delay_ms(TMP / 0.06 / 64.0);
    playRE2(TMP / 60.0 / 8.0);
    playMI2(TMP / 60.0 / 9.0);
    playRE2(TMP / 60.0 / 5.0);
    //__delay_ms(TMP / 0.06 / 64.0);
    playMI2(TMP / 60.0 / 7.0);
    playRE2(TMP / 60.0 / 9.0);
    playDO2(TMP / 60.0 / 2.0);
    
    __delay_ms(TMP / 0.06 / 16.0);
    playSO1(TMP / 60.0 / 6.0);
    playSO1(TMP / 60.0 / 16.0);
    playDO2(TMP / 60.0 / 2);
    
    playDO2(TMP / 60.0 / 6.0);
    playMI2(TMP / 60.0 / 9.0);
    playSO2(TMP / 60.0 / 9.0);
    playDO3(TMP / 60.0 / 9.0);
    playSI2(TMP / 60.0 / 9.0);
    playRA2(TMP / 60.0 / 9.0);
    playSO2(TMP / 60.0 / 4.0);
    
    //__delay_ms(TMP / 0.06 / 32.0);
    playMI2(TMP / 60.0 / 8.0);
    playSO2(TMP / 60.0 / 9.0);
    playFA2(TMP / 60.0 / 6.0);
    //__delay_ms(TMP / 0.06 / 32.0);
    playRE2(TMP / 60.0 / 8.0);
    playMI2(TMP / 60.0 / 9.0);
    playRE2(TMP / 60.0 / 5.0);
    //__delay_ms(TMP / 0.06 / 32.0);
    playMI2(TMP / 60.0 / 7.0);
    playRE2(TMP / 60.0 / 9.0);
    playDO2(TMP / 60.0 / 1.0);
    
    __delay_ms(TMP / 0.06 / 16.0);
    playDO2(TMP / 60.0 / 6.0);
    playDO2(TMP / 60.0 / 9.0);
    playRE2(TMP / 60.0 / 9.0);
    playMI2(TMP / 60.0 / 9.0);
    __delay_ms(TMP / 0.06 / 8.0);
    playDO2(TMP / 60.0 / 9.0);
    playRE2(TMP / 60.0 / 9.0);
    playDO2(TMP / 60.0 / 9.0);
    
}
