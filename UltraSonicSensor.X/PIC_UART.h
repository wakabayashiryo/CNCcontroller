/*
 * File:   PIC_UART.h
 * Author: Wakabayashi
 *
 * Created on 2018/09/21
 */

#ifndef PIC_UART_H
#define	PIC_UART_H

#include "main.h"

#define UART_BUFFER_SIZE 32

typedef enum
{
    BAUD_300    = 0,
    BAUD_1200   = 1,
    BAUD_2400   = 2,
    BAUD_9600   = 3,
    BAUD_10417  = 4,
    BAUD_19_2K  = 5,
    BAUD_57_6K  = 6,
    BAUD_115_2K = 7,
}BAUDRATE_u;

typedef void (*UART_Init_PORTTypedef)(void);

void UART_Init(UART_Init_PORTTypedef UART_Init_PORT,BAUDRATE_u brg);

void UART_StringTransmit(const uint8_t *dt);
void UART_Transmit(uint8_t dt);
uint16_t UART_Receive(void);

int UART_Available(void);
void UART_Flush(void);

void UART_Interrupt(void);

#endif	/* UART_DRIVER_H */