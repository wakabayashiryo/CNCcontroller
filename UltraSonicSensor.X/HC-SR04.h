/* 
 * File:   HC-SR04.h
 * Author: evaota
 *
 * Created on 2018/04/12, 12:46
 */

#ifndef HC_SR04_H
#define	HC_SR04_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "main.h"
    
typedef struct
{
    struct
    {
        volatile uint8_t *Echo_PORT;
        uint8_t  Echo_PIN;
        uint16_t count;
    };
    
}HC_SR40_CONFIG_t;

typedef void (*HC_SR04_Init_PORTTypedef)(void);

void HC_SR04_Init(HC_SR04_Init_PORTTypedef HC_SR04_InitPort,HC_SR40_CONFIG_t *conf);

void  HC_SR04_Measure_PulseWidth(uint8_t ch);

uint16_t HC_SR04_Get_Count(uint8_t ch);
uint16_t HC_SR04_Get_PulseWidth(uint8_t ch);
float HC_SR04_Get_Distance(uint8_t ch);
float HC_SR04_Get_CorrectedDistance(uint8_t ch,float temp);

void HC_SR04_Show_Distance(uint8_t NumofCh);

#ifdef	__cplusplus
}
#endif

#endif	/* HC_SR04_H */

