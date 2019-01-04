#include "PIC_I2C.h"

#if !I2C1_DONOT_USE
I2Cx_Configuration I2C1_Config;
#endif
#if !I2C2_DONOT_USE
I2Cx_Configuration I2C2_Config;
#endif

int8_t I2C_Transmit(I2Cx_Module I2Cx,uint8_t address,uint8_t *pData,uint8_t size)
{
    int8_t result = -1;
    
     switch(I2Cx)
    {
#if !I2C1_DONOT_USE
        case I2C1_MODULE:
            result = I2C1_Transmit(address,pData,size);
        break;
#endif
#if !I2C2_DONOT_USE
        case I2C2_MODULE:
            result = I2C2_Transmit(address,pData,size);
        break;
#endif
    }
    return result;
}

int8_t I2C_Receive(I2Cx_Module I2Cx,uint8_t address,uint8_t *pData,uint8_t size)
{
    int8_t result = -1;
    
    switch(I2Cx)
    {
#if !I2C1_DONOT_USE
        case I2C1_MODULE:
             result = I2C1_Receive(address,pData,size);
        break;
#endif
#if !I2C2_DONOT_USE        
        case I2C2_MODULE:
             result = I2C2_Receive(address,pData,size);
        break;
#endif
    }
    return result;
}

void I2C_Interrupt(void)
{
#if !I2C1_DONOT_USE
    I2C1_Interrupt();
#endif
#if !I2C2_DONOT_USE        
    I2C2_Interrupt();
#endif
}