/* Hardware related definitions for Charge and Discharge System. */

/* Kyutech Institute of Technology. LaSEINE. Supervisor: Mengu CHO.*/
/* 
 * File:  hardware.h 
 * Author: Juan J. Rojas.
 * Comments: Version control under Git
 */


// PIC16F1786 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Disabled (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)//If this is enabled, the Timer0 module will not work properly.
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bit (Vcap functionality is disabled on RA6.)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low Power Brown-Out Reset Enable Bit (Low power brown-out is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>         //To include uint8_t and uint16_t

#define		ERR_MAX					5000        //This is given by the maximum valued that can be sensed
#define		ERR_MIN					-5000       //This is given by the maximum valued that can be sensed
#define		SET_VOLTAGE(x)			{ vref = x; }
#define		SET_CURRENT(x)			{ iref = x; }

#define 	_XTAL_FREQ 				32000000
#define		BAUD_RATE               9600

#define		V_CHAN                  0b01101 //AN13 (RB5) 
#define		I_CHAN                  0b01011 //AN11 (RB4)
//#define		T_CHAN                  0b010011 //RC3

#define		CELL1_ON				PORTAbits.RA7 = 1
#define		CELL2_ON				PORTAbits.RA6 = 1
#define		CELL3_ON				PORTCbits.RC0 = 1
#define		CELL4_ON				PORTCbits.RC1 = 1
#define		CELL1_OFF				PORTAbits.RA7 = 0
#define		CELL2_OFF				PORTAbits.RA6 = 0
#define		CELL3_OFF				PORTCbits.RC0 = 0
#define		CELL4_OFF				PORTCbits.RC1 = 0

#define		AD_SET_CHAN(x)          { ADCON0bits.CHS = x; __delay_us(10); }
#define		AD_CONVERT()            { GO_nDONE = 1; while(GO_nDONE);}
#define     AD_RESULT()             { ad_res = 0; ad_res = (ADRESL & 0xFF)|((ADRESH << 8) & 0xF00);} 
//CONTROL LOOP RELATED DEFINITION
#define     CURRENT_MODE            4                               //Number of times the voltage should be equal to the CV voltage in order to change to CV mode.

#define     LINEBREAK               UART_send_char(10)

//DC-DC CONVERTER RELATED DEFINITION
#define		STOP_CONVERTER()		{ dc = 0; set_DC(); RA1 = 1; Cell_OFF(); LOG_OFF();}
#define  	START_CONVERTER()		{ dc = DC_MIN; set_DC(); RA1 = 0; __delay_ms(100); Cell_ON();}

#define 	LOG_ON()				{ log_on = 1; }
#define 	LOG_OFF()				{ log_on = 0; }

#define 	DC_MIN         25		// DC = 0.05
#define 	DC_MAX         486    // DC = 0.95

 
#define     COUNTER        976

unsigned int 						ad_res;
int                                 v;  //ADDED
int                                 i;  //ADDED
int                                 t;  //ADDED
unsigned int 						count;             //ADDED
short long                          iprom;
short long                          vprom;
short long                          tprom;
unsigned int 						vref;
unsigned int 						iref;
char 								cmode;
unsigned char 						cc_cv;
unsigned int 						second;
unsigned char 						esc;
int 								proportional;
int 								integral;
float 								kp;				//Proportional constant, seems too big data type
float 								ki;				//Integral constant, seems too big data type
unsigned int                        dc = 0;         //Duty, change data size for 125Khz
unsigned char 						spb;			//Baud rate set
unsigned int            			log_on; 

void Initialize_Hardware(void);
void Init_Registers(void);
void Shutdown_Peripherals(void);
void pid(unsigned int feedback, unsigned int setpoint);
void set_DC(void);
void read_ADC(void);
void log_control(void);
void display_value(long value);
void cc_cv_mode(void);
void char_disc(void);
void control_loop(void);
void calculate_avg(void);
void Init_UART(void);
void UART_interrupt_enable(void);
void UART_send_char(char bt);
char UART_get_char(void); 
void UART_send_string(char* st_pt);
void Cell_ON(void);
void Cell_OFF(void);
void timing(void);