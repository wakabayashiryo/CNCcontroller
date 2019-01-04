/*
 * File:   main.c
 * Author: ryo wakabayashi
 *
 * Created on 19/1/4, 20:46
 */

#include "main.h"


void main(void) 
{
    Device_Startup();
    
    //peripheral initialize functions etc...
    
    
    while(1)
    {
        
    }       
}

void Device_Startup(void)
{
/*
    OSCCON = 0xF0;      //Oscilator configrations
    
    TRISA = 0x00;       //All PORTA are set output 
    ANSELA = 0x00;      //All PORTA  are set digital

    PORTA = 0x00;       //zero clear
 */
    
}

void interrupt Handler(void)
{
/*
    if(Timer0_CheckFlag())
    {
        process...
    }
 */
    
}