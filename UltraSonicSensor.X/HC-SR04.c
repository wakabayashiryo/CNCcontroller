#include "HC-SR04.h"

static HC_SR40_CONFIG_t *static_conf;

void HC_SR04_Init(HC_SR04_Init_PORTTypedef HC_SR04_InitPort,HC_SR40_CONFIG_t *conf)
{
    HC_SR04_InitPort();
    static_conf = conf;
    
    T1CON &= ~(0x03<<6);//Timer1 clock suorce is instruction  clock (FOSC/4)
    T1CON |=  (0x03<<4);//Timer1 input prescaler is 1:8 
    T1CON |=  (0x01<<2);//Do not synchronize external clock
    
    T1GCON |=  (0x01<<6);//Gate polarity is active high 
    T1GCON |=  (0x01<<4);//Gate single-pulse mode
    T1GCON &= ~(0x03<<0);//Gate source is T1G pin
           
    TMR1H = 0x00;
    TMR1L = 0x00;
    
    TMR1IF = 0;//Not use interrupt
    TMR1IE = 0;
    
    T1CON |= (1<<0);//enable timer1
}

void  HC_SR04_MeasureDistance(uint8_t ch)
{
    TMR1 = 0x0000;//Clear measuremented value
    TMR1GE = 1;   //Gate enable
    
    *static_conf[ch].Echo_PORT |= (uint8_t)(1<<static_conf[ch].Echo_PIN);//issue trigger to ultrasonic sensor      
    __delay_us(10);
    *static_conf[ch].Echo_PORT &= (uint8_t)~(1<<static_conf[ch].Echo_PIN);    

    T1GGO = 1;     //Start count pulse width
    
    while(!T1GVAL); //wait rising edge from sensor
    while(T1GVAL);  //wait falling edge
    
    static_conf[ch].count = TMR1;    //pulse width value (unit:micro seconds)
}

void HC_SR04_Show_Distance(uint8_t NumofCh)
{
    for(uint8_t i=0;i<NumofCh;i++)
    {
        HC_SR04_MeasureDistance(i);
        printf("| ch[%d] Echo:%04dus Distan:%d.%02dcm ",i,static_conf[i].count,static_conf[i].count/58,static_conf[i].count%58);
    }
    printf("|\n");
}
