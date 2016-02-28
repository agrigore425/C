/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* TODO Define system operating frequency */

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        8000000L
#define FCY             SYS_FREQ/4

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/
void setAnalogIn();
void setActuatorCntrl();
void delay_milli(unsigned int time);
void init_spi();
void init_AccelFS();
unsigned int spi_data_ready();
char spi_read();
void spi_write(char message);
unsigned long spi_read_long();
char oneReadCycle(char num);
int readX();
int readY();
int readZ();


/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void ConfigureOscillator(void); /* Handles clock switching/osc initialization */
