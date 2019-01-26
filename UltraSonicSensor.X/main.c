/*
 * File:   main.c
 * Author: ryo wakabayashi
 *
 * Created on 19/1/4, 20:46
 */

#include "main.h"

void HC_SR04_PortInit(void)    
{ 
    TRISB |= (1<<0);//T1G pin,connected echo pin
    TRISA &= ~(1<<0);//RA0 pin connected trigger pin
    TRISA &= ~(1<<1);//RA1 pin connected trigger pin
}

void UART_PortInit(void)    
{ 
    TRISB |=  (1<<1);//RX
    TRISB &= ~(1<<2);//TX
}

void ADC_PortInit(void)
{
    TRISA  |= (1<<2);
    ANSELA |= (1<<2);
}

void main(void) 
{
    Device_Startup();
    
    UART_Init(UART_PortInit,BAUD_115_2K);
    
    ADC_Init(ADC_PortInit,ADC_FOSC_32);
    
    HC_SR40_CONFIG_t sonic[2];
    
    sonic[0].Echo_PORT = &LATA;//RA0 is Trigger pin 
    sonic[0].Echo_PIN = 0;
    sonic[1].Echo_PORT = &LATA;//RA1 is Trigger pin 
    sonic[1].Echo_PIN = 1;
    HC_SR04_Init(HC_SR04_PortInit,sonic);

    uint16_t adc_raw;
    uint8_t temp;
    uint8_t temp_p;
    float distance;
    
    while(1)
    {
        adc_raw = ADC_Scan_Voltage(2);
        
        /* Vin = (Vdd * ADC Value) /1024
         * Temperature = Vin * 100
         */
        
        HC_SR04_Measure_PulseWidth(0);
        distance = HC_SR04_Get_CorrectedDistance(0,(float)adc_raw / 2.048);        

        printf("%d\n",(uint16_t)distance);
//        printf("Distan:%d.%02dcm ",(uint8_t)(distance),(uint8_t)(distance%100));
    }  
}

void Device_Startup(void)
{
    OSCCON  = (0x0E<<3); //internal clock 32MHz
    
    TRISA   = 0x00;
    ANSELA  = 0x00;   
    WPUA    = 0x00;
    
    TRISB   = 0x00;
    ANSELB  = 0x00;
    WPUB    = 0x00;
    
    APFCON0 = 0x00;
    APFCON1 = 0x00;
    
    LATA = 0x00;
    LATB = 0x00;   
}

void interrupt Handler(void)
{
    UART_Interrupt();  
}