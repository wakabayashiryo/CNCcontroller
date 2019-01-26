#include "PIC_ADC.h"

void ADC_Init(ADC_Init_PORTTypedef ADC_Init_PORT,ADC_CONVERSION_CLOCK clock) 
{
    ADC_Init_PORT();
    
    ADCON0 = 0X00;
    ADCON1 |= (1<<7);               //A/D result format is right justified
    ADCON1 |= ((uint8_t)clock<<4);  //set A/D conversion clock 
    ADCON1 &=~(1<<2);               //A/D Negative Voltage Reference is AVss
    ADCON1 &=~(3<<0);               //A/D Negative Positive Reference is AVdd
    
    ADRESL = 0x00;
    ADRESH = 0x00;
    
    ADCON0 |= (1<<0);               //Enable ADC
    
}

uint16_t ADC_Scan_Voltage(uint8_t channel)
{
    ADCON0 &= (uint8_t)~(0x1F<<2);        //clear channel select bits
    ADCON0 |= (uint8_t) (channel<<2);     //set channel select bits
    
    __delay_us(20);
    
    GO_nDONE = 1;
   
    while(GO_nDONE);
    
    return (uint16_t)((ADRESH<<8)|ADRESL);
}   