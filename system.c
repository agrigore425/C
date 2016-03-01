/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "system.h"
#define _XTAL_FREQ 10000000


/* Refer to the device datasheet for information about available
oscillator configurations. */
void ConfigureOscillator(void)
{
    /* Typical actions in this function are to tweak the oscillator tuning
    register, select new clock sources, and to wait until new clock sources
    are stable before resuming execution of the main project. */
    OSCCONbits.IRCF2 = 1;       // All set -> 8 MHz
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;
    
    OSCCONbits.SCS1 = 0;        // All clear -> select primary oscillator
    OSCCONbits.SCS0 = 0;
    
    OSCTUNEbits.PLLEN = 1;      // Enable PLL x4
}

void setAnalogIn()
{
    //Set Port AN0-AN10 to Analog
    ADCON1bits.PCFG = 0;            
    ADCON2bits.ADFM = 1;    //Right justified - values converted to 0-1023
                            //Registers ADRESH and ADRESL hold this converted value
    
    //V-REF+ is VDD and V-REF- is VSS
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    
    //Set Acquisition Time to be 4T_AD
    ADCON2bits.ACQT2 = 0;
    ADCON2bits.ACQT1 = 1;
    ADCON2bits.ACQT0 = 0;
    
    //Set A/D Conversion clock frequency F_OSC/32
    ADCON2bits.ADCS2 = 0;
    ADCON2bits.ADCS1 = 1;
    ADCON2bits.ADCS0 = 0;

    
    //Set Analog ports as Input
    TRISAbits.TRISA0 = 1;   //AN0
    TRISAbits.TRISA1 = 1;   //AN1
    TRISAbits.TRISA2 = 1;   //AN2
    TRISAbits.TRISA3 = 1;   //AN3
    TRISAbits.TRISA5 = 1;   //AN4
    TRISEbits.TRISE0 = 1;   //AN5
    TRISEbits.TRISE1 = 1;   //AN6
    TRISEbits.TRISE2 = 1;   //AN7
    TRISBbits.TRISB1 = 1;   //AN8
    TRISBbits.TRISB4 = 1;   //AN9
    TRISBbits.TRISB0 = 1;   //AN10
    
    //Enable ADC
    ADCON0bits.ADON = 1; 
}

void setActuatorCntrl()
{
    //Set RD0-RD7 and RC0-RC7 as Digital outputs
    TRISDbits.TRISD0 = 0;   //A2_0
    TRISDbits.TRISD1 = 0;   //A2_1
    TRISDbits.TRISD2 = 0;   //A3_0
    TRISDbits.TRISD3 = 0;   //A3_1
    TRISDbits.TRISD4 = 0;   //A6_0
    TRISDbits.TRISD5 = 0;   //A6_1
    TRISDbits.TRISD6 = 0;   //A7_0
    TRISDbits.TRISD7 = 0;   //A7_1

    TRISCbits.TRISC0 = 0;   //A0_0
    TRISCbits.TRISC1 = 0;   //A0_1
    TRISCbits.TRISC2 = 0;   //A1_0
    //SPI TRISCbits.TRISC3 = 0;   //A1_1
    //SPI TRISCbits.TRISC4 = 0;   //A4_0
    //SPI TRISCbits.TRISC5 = 0;   //A4_1
    

}


void init_spi()
{
    TRISCbits.RC4 = 1;          // SDI as input
    TRISCbits.RC5 = 0;          // SDO as output
    TRISCbits.RC3 = 0;          // SCK/SCL as output (cleared)
    
    TRISCbits.TRISC6 = 0;   //CSG
    TRISCbits.TRISC7 = 0;   //CSXM
    
    SSPCON1bits.SSPEN = 0;      // Clear bit to reset, SPI pins
    
    SSPCON1bits.CKP = 1;        // Set polarity to idle at 1
    SSPSTATbits.CKE = 0;        // Set transmit on rising edge
    SSPSTATbits.SMP = 0;        // Set Sample bit to 0
    SSPCON1bits.SSPM0 = 0;      // Set SSPM[3:0] as stated in manual
    SSPCON1bits.SSPM1 = 0;      // In order to divide by 4 (Fosc/4)
    SSPCON1bits.SSPM2 = 0;
    SSPCON1bits.SSPM3 = 0;
    SSPCON1bits.WCOL = 0;       // Clear the write buffer
    
    SSPCON1bits.SSPEN = 1;      // Set bit to re enable SPI
       
}

unsigned int spi_data_ready()
{
    return SSPSTATbits.BF;
}


char spi_read()
{
    while(!SSPSTATbits.BF);
    return SSPBUF;
}


void spi_write(char message)
{
    //while(SSPCON1bits.WCOL);
    SSPBUF = message;
}

unsigned long spi_read_long()
{
    unsigned long total = 0;
    for(int i = 0; i < 4; i++)
    {
        total += spi_read() << (i * 8);
    }
    return total;
}

char oneReadCycle(char num)
{
    delay_milli(30);
    spi_write(num);                       // Send command to slave
    char measure;
    delay_milli(30);
    if(spi_data_ready())
    {
        measure = spi_read();
    }
    return measure; 
}

void delay_milli(unsigned int time)
{
    for (int i = 0; i < time; i++)
        __delay_ms(1);
}

int readX()
{
    //Read X low byte - Address 10 1000
    spi_write(0xA8);
    spi_write(0x00);
    int x = spi_read();
    //Read X high byte - Address 10 1001
    spi_write(0xA9);
    spi_write(0x00);
    x += spi_read() << 8;
    return x;
}

int readY()
{
    spi_write(0xAA);
    spi_write(0x00);
    int y = spi_read();
    //Read Y high byte - Address 10 1011
    spi_write(0xAB);
    spi_write(0x00);
    y += spi_read() << 8;
    return y;
}

int readZ()
{
    spi_write(0xAC);
    spi_write(0x00);
    int z = spi_read();
    //Read Z high byte - Address 10 1101
    spi_write(0xAD);
    spi_write(0x00);
    z += spi_read() << 8;
    return z;
}

void init_AccelFS()
{
    //CTRL_REG2_XM = 0x08 to allow +-4g measurement
    spi_write(0x21); // we are writing to CTRL_REG2_XM
    spi_write(0x08); // we are storing 00001000 for +-4g
    
}