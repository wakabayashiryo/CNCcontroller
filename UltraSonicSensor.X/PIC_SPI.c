#include "PIC_SPI.h"

static uint8_t UART_Buffer[UART_BUFFER_SIZE];
static uint8_t UART_Buffer_inptr;
static uint8_t UART_Buffer_outptr;

#if _XTAL_FREQ==32000000
static const uint16_t baudtable[] = {
   6666,    //300bps
   3332,    //1200bps
    832,    //2400bps
    207,    //9600bps
    191,    //10417bps
    103,    //19.2kbps
     34,    //57.6kbos
     16     //115.2kbps
};

#elif _XTAL_FREQ==20000000
static const uint16_t baudtable[] = {
   4166,    //300bps
   1041,    //1200bps
    520,    //2400bps
    129,    //9600bps
    119,    //10417bps
     64,    //19.2kbps
     21,    //57.6kbos
     10     //115.2kbps
};

#elif _XTAL_FREQ==160000000
static const uint16_t baudtable[] = {
   3332,    //300bps
    832,    //1200bps
    415,    //2400bps
    103,    //9600bps
     94,    //10417bps
     51,    //19.2kbps
     16,    //57.6kbos
      7     //115.2kbps
};
#endif

void SPI_Init(SPI_Init_PORTTypedef SPI_Init_PORT,BAUDRATE_u brg) 
{
    SPI_Init_PORT();
    
    TXSTA  = 0x20;       //Transmit Enable bit is set 1 
    RCSTA  = 0x10;       //Repeatation Receiving
    
    BRG16  = 1;          //baudrate generate of 16bit
    SPBRGL =  baudtable[(uint8_t)brg]&0x00FF;     //Set baudrate value
    SPBRGH = (baudtable[(uint8_t)brg]>>8)&0x00FF;
    
    RCIF   = 0;           //Clear Receive Interrupt flag
    RCIE   = 1;           //Receive Interrupt enable
    PEIE   = 1;           //Peripheral Interrupt enable
    GIE    = 1;           //Global Interrupt enable
    
    SPEN   = 1;           //Serial Port enable
    
    UART_Buffer_inptr  = 0; 
    UART_Buffer_outptr = 0; 
}

void UART_StringTransmit(const uint8_t *dt) 
{
    while(1) 
    {
        while(TXIF == 0); 
        TXREG = *dt; 
        if(*dt == 0)break;
        *dt++;
    }
}

void UART_Transmit(uint8_t dt) 
{
    while(TXIF == 0); 
    TXREG = dt; 
}

void putch(char c)
{
    UART_Transmit(c);
}

int UART_Available(void) 
{
    return (UART_Buffer_outptr + UART_BUFFER_SIZE - UART_Buffer_inptr) % UART_BUFFER_SIZE;
}

/* ans : Number of Receive Data,if it is empty return value is 0xFFFF*/
uint16_t UART_Receive(void) 
{
    uint16_t ans;

    if(UART_Buffer_inptr == UART_Buffer_outptr) return 0xFFFF;

    ans = UART_Buffer[UART_Buffer_inptr];

    UART_Buffer_inptr = (uint8_t)((UART_Buffer_inptr + 1) % UART_BUFFER_SIZE);

    return ans;
}

/* Clear buffers */
void UART_Flush(void)
{
    UART_Buffer_inptr = 0; 
    UART_Buffer_outptr = 0; 
}

/* Set This function in interrupt function*/
void UART_Interrupt(void)
{
    uint8_t x;
    uint8_t dt;

    if(RCIF == 1) 
    {
        dt = RCREG;
        x = (uint8_t)((UART_Buffer_outptr + 1) % UART_BUFFER_SIZE);
        if(x != UART_Buffer_inptr) 
        {
            UART_Buffer[UART_Buffer_outptr] = dt;
            UART_Buffer_outptr = x;
        }

        if(OERR == 1)
        {
            CREN = 0;
            while(OERR);
            CREN = 1;
        }
        RCIF = 0;
    }
}
