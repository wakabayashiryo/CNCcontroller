#include"PIC_I2C.h"

#if !I2C2_DONOT_USE
//Check idle status use R_nW,SEN,RSEN,PEN,RCEN,ACKEN 
#define I2C2_IdleCheck() while((SSP2CON2&0x1F)|(SSP2STAT&0x05))

extern I2Cx_Configuration I2C2_Config;

void I2C2_Init(I2C2_Init_PORTTypedef I2C2_Init_PORT,uint8_t address,I2Cx_Mode mode,I2Cx_Speed Speed)
{
    I2C2_Init_PORT();                         //Intialize Port for I2C
    I2C2_Config.Mode = mode;                //Save I2C2 mode

    switch(mode)
    {
        case I2C_MODE_MASTER:
            SSP2CON1 |= (0x08<<0);          //I2C mode is master
            if(Speed==I2C_SPEED_STANDARD)
            {
                SSP2STAT |= (1<<7);         //Slew rate control disabled for standard speed mode
                SSP2ADD = 0x4F;             //Set baud rate(100kHz)
            }
            else if(Speed==I2C_SPEED_HIGH)
            {
                SSP2STAT &= ~(1<<7);        //Slew rate control enabled for high speed mode 
                SSP2ADD = 0x13;             //Set baud rate(400kHz)
            }
            SSP2CON1bits.SSPEN = 1;         //Enable I2C
        break;
        
        case I2C_MODE_SLAVE:
            SSP2STAT = 0x00;                //Clear all flag
            if(Speed==I2C_SPEED_STANDARD)
                SSP2STAT |= (1<<7);         //Slew rate control disabled for standard speed mode
            else if(Speed==I2C_SPEED_HIGH)
                SSP2STAT &= ~(1<<7);        //Slew rate control enabled for high speed mode 
            
            SSP2CON1 |= (0x06<<0);          //I2C slave mode 7bit                
            SSP2CON2bits.SEN= 1;            //clock streching is enable
            SSP2ADD = address << 1;         //Set own address
            SSP2MSK = 0xFE;                 //Compared on address bit from 1bit to 7bit
            SSP2CON1bits.SSPEN = 1;         //Enable I2C
        break;
    }
    SSP2IE = 1;                             //Enable SSP interrupt
    BCL2IE = 1;                             //Enable Bus collisioni interrupt
    SSP2IF = 0;                             //Clear SSP interrupt flag
    BCL2IF = 0;                             //Clear Bus collisioni interrupt flag

    PEIE = 1;
    GIE = 1;
}

int8_t I2C2_Transmit(uint8_t address,uint8_t *pData,uint8_t size)
{
    int8_t result = 0;

    switch(I2C2_Config.Mode)
    {
        case I2C_MODE_MASTER:
            I2C2_IdleCheck();               //Waitting bus will become idle status 
            SSP2CON2bits.SEN = 1;           //Issue Start Condition

            I2C2_IdleCheck();               //Waitting bus will become idle status 
            SSP2BUF = address << 1;         //Send Slave address
            while(SSP2STATbits.R_nW);       //Waitting to send address 

            result = SSP2CON2bits.ACKSTAT;     //Check acknowledge from slave
            if (!result)                       //if Slave send acknowledge to master
            {
                for(uint8_t i=0;i<size;i++)        //Send data sequence
                {
                    I2C2_IdleCheck();       //Waitting bus will become idle status  
                    SSP2BUF = *(pData+i);   //send Data to slave
                    while(SSP2STATbits.R_nW);//Waitting to send data 

                    result = SSP2CON2bits.ACKSTAT;//Check acknowledge from slave
                    if (result)                // if master revceived Not acknowledge
                    {
                        result = 2;
                        break;
                    }
                }
            }

            I2C2_IdleCheck();               //Waitting bus will become idle status 
            SSP2CON2bits.PEN = 1;           //Issue Stop condition
        break;
        
        case I2C_MODE_SLAVE:
            if((SSP2STATbits.S==0)&&(SSP2STATbits.P==1))
            {
                for(uint8_t i = 0;i<size;i++)
                    I2C2_Config.SlaveTransmitBuff[i] = pData[i];
                result = size;
            }
            else
                result = 0;
        break;
    }
    
    return result;
}

int8_t I2C2_Receive(uint8_t address,uint8_t *pData,uint8_t size)
{
    int8_t result;

    switch(I2C2_Config.Mode)
    {
        case I2C_MODE_MASTER:
            I2C2_IdleCheck();               //Waitting bus will become idle status
            SSP2CON2bits.SEN = 1;           //Issue Start Condition

            I2C2_IdleCheck();               //Waitting bus will become idle status
            SSP2BUF = ((address << 1)|0x01);//Sned address to slave
            while(SSP2STATbits.R_nW);       //Waitting to send address 
            
            result = SSP2CON2bits.ACKSTAT;     //Check acknowledge from slave
            if (!result)                       // if master revceived Not acknowledge
            {
                for (uint8_t i=0;i<size;i++) 
                {
                    I2C2_IdleCheck();       //Waitting bus will become idle status
                    SSP2CON2bits.RCEN = 1;  //Enable Receive (wait sendding data from slave)
                    while(SSP2CON2bits.RCEN);//wait to receive Data
                    *(pData+i) = SSP2BUF;   //Read Data that sent from slave
                    I2C2_IdleCheck();       //Waitting bus will become idle status
                    if (i == size-1) 
                        SSP2CON2bits.ACKDT = 1;//acknowledge will send to slave is not acknowledge  
                    else 
                        SSP2CON2bits.ACKDT = 0;//acknowledge will send to slave is acknowledge  
                    SSP2CON2bits.ACKEN = 1;    //send acknowledge 
                }
            }

            I2C2_IdleCheck();                  //Waitting bus will become idle status
            SSP2CON2bits.PEN = 1;              //Issue Start Condition
        break;
        
        case I2C_MODE_SLAVE:
            if((SSP2STATbits.S==0)&&(SSP2STATbits.P==1))
            {
                for(uint8_t i = 0;i<size;i++)
                     pData[i] = I2C2_Config.SlaveReceiveBuff[i];
                result = I2C2_Config.ReceivePoint - I2C2_Config.SlaveReceiveBuff;
            }            
        break;
    }
    
    return result;
}

void I2C2_Interrupt(void) 
{
    if(SSP2IF&&SSP2IE)
    {   //Check Received data is data or address
        if(SSP2STATbits.D_nA)           //Received Data from master
        {
            if(SSP2STATbits.R_nW)    //Read responce
            {
                if(!SSP2CON2bits.ACKSTAT)   //If Number of Read Data is 2 over
                {
                    while((SSP2CON1bits.CKP)|(SSP2STATbits.BF));
                    SSP2BUF = *I2C2_Config.TransmitPoint;       //Set Data to buffer
                    I2C2_Config.TransmitPoint++;            //increment buffer pointer
                    SSP2CON1bits.CKP = 1;           //return acknowledge to master
                }
            }
            else        //Write responce
            {
                *I2C2_Config.ReceivePoint = SSP2BUF;    //Read Data from buffer
                I2C2_Config.ReceivePoint++;         //increment buffer pointer
                SSP2CON1bits.CKP = 1;           //return acknowledge to master
            }
        }   
        else if(!SSP2STATbits.D_nA)         //Received Address from master
        {
            SSP2BUF;            //Access buffer
            
            if(SSP2STATbits.R_nW)       //Read resopnce
            {   
                I2C2_Config.TransmitPoint = I2C2_Config.SlaveTransmitBuff;      //Set first buffer pointer

                while((SSP2CON1bits.CKP)|(SSP2STATbits.BF));
                SSP2BUF = *I2C2_Config.TransmitPoint;           //Set Data to buffer
                I2C2_Config.TransmitPoint++;                //increment buffer pointer
            }
            else            //Write resopnce
            {
                I2C2_Config.ReceivePoint = I2C2_Config.SlaveReceiveBuff;        //Set first buffer pointer
            }
            SSP2CON1bits.CKP = 1;           //return acknowledge to master
        } 
        SSP2IF = 0;
    }

    if(BCL2IE&&BCL2IF)      //If occured bus collision
        BCL2IF = 0;
}
#endif 
