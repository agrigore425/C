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
    init_spi();
    init_AccelFS();

    char checkRegReady = 0xA7; //Read config & Address of STATUS_REG_A/G
    
    int16_t xAccelRaw = 0;
    int16_t yAccelRaw = 0;
    int16_t zAccelRaw = 0;
    
    float xAccel = 0;
    float yAccel = 0;
    float zAccel = 0;
    
    
    int16_t xGyroRaw = 0;
    int16_t yGyroRaw = 0;
    int16_t zGyroRaw = 0;
    
    double loopPeriod = 0.01;

    
    float xGyro = 0;
    float yGyro = 0;
    float zGyro = 0;
    
    unsigned int tempOne = 0;
    unsigned int tempTwo = 0;
    
    while(1)
    {
        //Temperature Reading from Analog device------------------------------
        //CHANNEL 0 - TempOne
        //Start ADC reading Channel 0
        selectAN0();
        tempOne = readADC();
        
        //CHANNEL 1 - TempTwo
        //Start ADC reading Channel 1
        selectAN1();
        tempTwo = readADC();
        
        //Accelerometer Reading from slave device-----------------------------
        
        //Set Accelerometer Chip Select (Active Low)
        LATCbits.LATC6 = 1; // Set CSG high to not read G
        LATCbits.LATC7 = 0; // Set CSXM Active low to read XM
        
        //Check for SATUS_REG_A of device to say data ready
        spi_write(checkRegReady); //asks for accel status
        spi_write(0x00);            //send blanks to acquire read value
        char accelStat = spi_read();
        accelStat = (accelStat & 0x08) >> 3; //bit3 is the bit that states data ready
        
        if(accelStat)
        {
            //Read acceleration X, Y, Z - these are 2 bytes per axis
            //TWOs COMP left justified
            //Read X acceleration
            xAccelRaw = readX();
            //Read Y acceleration
            yAccelRaw = readY();
            //Read Z low byte - Address 10 1100
            zAccelRaw = readZ();
        }
        LATCbits.LATC6 = 1; // Set CSG high to not read G
        LATCbits.LATC7 = 1; // Set CSXM high to not read XM
        
        // Reads 0.122mg/LSB --> 0.000122 g/LSB
        xAccel = xAccelRaw * 0.000122;
        yAccel = yAccelRaw * 0.000122;
        zAccel = zAccelRaw * 0.000122;
        
        
        //Gyroscope reading from slave device----------------------------------
        
        //Set Gyroscope Chip Select (Active Low))
        LATCbits.LATC6 = 0; // Set CSG Active low to read G
        LATCbits.LATC7 = 1; // Set CSXM high to not read XM
       
        //Check for SATUS_REG_G of device to say data ready
        spi_write(checkRegReady); //asks for gyro status
        spi_write(0x00);            //send blanks to acquire read value
        char gyroStat = spi_read();
        gyroStat = (gyroStat & 0x08) >> 3; //bit3 is the bit that states data ready
        
        if(gyroStat)
        {
            //Read acceleration X, Y, Z - these are 2 bytes per axis
            //TWOs COMP left justified
            //Read X gyro
            xGyroRaw = readX();
            //Read Y gyro
            yGyroRaw = readY();
            //Read Z gyro
            zGyroRaw = readZ();
        }
        LATCbits.LATC6 = 1; // Set CSG high to not read G
        LATCbits.LATC7 = 1; // Set CSXM high to not read XM
        
        //Convert raw gyro data to usable angles. Since we use default 250dps
        //the sensitivity level is 0.00875
        float xGyroRate = (float) xGyroRaw * 0.00875;
        float yGyroRate = (float) yGyroRaw * 0.00875;
        float zGyroRate = (float) zGyroRaw * 0.00875;
        
        delay_milli(10); //10 milliseconds delay to use with gyroscope reading
        
        xGyro = xGyroRate*loopPeriod;
        yGyro = yGyroRate*loopPeriod;
        zGyro = zGyroRate*loopPeriod;
        
        //Barometric Pressure Reading from slave device------------------------
        //FUCK I2C
        
    }

}

