/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "ADC_Config.h"    /* ADC Channel selects and read function */
#include "can.h"           /* CAN */
#include "i2c.h"           /* CAN */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */
typedef int bool;
#define true 1
#define false 0

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
/*
a. Position within tube (X, Y, and Z)

b. Velocity within tube (X, Y, and Z)

c. Acceleration within tube (X, Y, and Z)

d. Vehicle attitude (roll, pitch, and yaw)

e. Pod pressure (only applicable if Pod has any pressurized sections)

f. Temperature from at least two points on the Pod

g. Power consumption
 */

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    
    //Initialize PIC SPI for accelerometer/gyroscope
    ecan_init();
    i2c_init();
    
    
    short xAccelRaw = 0;
    short yAccelRaw = 0;
    short zAccelRaw = 0;
    
    
    unsigned int tempOne = 0;
    unsigned int tempTwo = 0;
    
    //Configure accelerometer CONTRL_REG1_XM
    char slaveAddr0 = 0x1E;
    char destAddr0 = 0x20;
    char sendData0 = 0b01100111; //== 0x67
    i2c_writeToReg(slaveAddr0, destAddr0, sendData0);

    //CONTROL_REG5_XM
    slaveAddr0 = 0x1E;
    destAddr0 = 0x24;
    sendData0 = 0b11110000;
    i2c_writeToReg(slaveAddr0, destAddr0, sendData0);
    
    while(1)
    {
        
        //Temperature Reading from Analog device------------------------------
        //CHANNEL 0 - TempOne
        //Start ADC reading Channel 0
        /*selectAN0();
        tempOne = readADC();
        
        
        //CHANNEL 1 - TempTwo
        //Start ADC reading Channel 1
        selectAN1();
        tempTwo = readADC();
        
        */
        //Accelerometer Reading from slave device-----------------------------
        
        xAccelRaw = i2c_get_byte(0x1E ,0x28);
        xAccelRaw += i2c_get_byte(0x1E,0x29) << 8;
        
        
        Message accelPack;
        accelPack.sid = 0x000;
        accelPack.len = 2;
        accelPack.data[0] = xAccelRaw;
        accelPack.data[1] = xAccelRaw >> 8;
        
        ecan_send(&accelPack);
        
    }
}

