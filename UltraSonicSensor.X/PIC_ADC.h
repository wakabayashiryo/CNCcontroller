/*
 * File:   PIC_ADC.h
 * Author: Wakabayashi
 *
 * Created on 2019/01/24
 */

#ifndef PIC_ADC_H
#define	PIC_ADC_H

#include "main.h"

typedef enum
{
    ADC_FOSC_2 = 0,
    ADC_FOSC_8 = 1,
    ADC_FOSC_32 = 2,
    ADC_FOSC_FRC = 3,
    ADC_FOSC_4 = 4,
    ADC_FOSC_16 = 5,
    ADC_FOSC_64 = 6,
}ADC_CONVERSION_CLOCK;

typedef void (*ADC_Init_PORTTypedef)(void);

void ADC_Init(ADC_Init_PORTTypedef ADC_Init_PORT,ADC_CONVERSION_CLOCK clock) ;
uint16_t ADC_Scan_Voltage(uint8_t channel);

#endif	/* PIC_ADC_H */