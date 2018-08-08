/* Hardware header file for Charge and Discharge System. */
/* Kyutech Institute of Technology. LaSEINE. Supervisor: Mengu CHO.*/
/* 
 * File:  hardware.c 
 * Author: Juan J. Rojas.
 * Mail (after leaving Kyutech): juan.rojas@tec.ac.cr
 * Version control in Git: https://bitbucket.org/juanjorojash/cell_charger_discharger
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
#pragma config BOREN = OFF       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bit (Vcap functionality is disabled on RA6.)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low Power Brown-Out Reset Enable Bit (Low power brown-out is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (Low-voltage programming disabled)//IF THIS IN ON MCLR is always enabled

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>         //To include uint8_t and uint16_t
#include <string.h>
#include <stdbool.h>			//Include bool type

#define		ERR_MAX					250        //!< Maximum permisible error, useful to avoid ringing
#define		ERR_MIN					-250       //!< Minimum permisible error, useful to avoid ringing
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

#define		AD_SET_CHAN(x)          { ADCON0bits.CHS = x; __delay_us(5); }
#define		AD_CONVERT()            { GO_nDONE = 1; while(GO_nDONE);}
#define     AD_RESULT()             { ad_res = 0; ad_res = (ADRESL & 0xFF)|((ADRESH << 8) & 0xF00);} 
//CONTROL LOOP RELATED DEFINITION
#define     CURRENT_MODE            4                               //Number of times the voltage should be equal to the CV voltage in order to change to CV mode.

#define     LINEBREAK               UART_send_char(10)

//DC-DC CONVERTER RELATED DEFINITION
#define		STOP_CONVERTER()		{ conv = 0; RA1 = 1; dc = DC_MIN; set_DC(); Cell_OFF(); LOG_OFF(); }

#define 	LOG_ON()				{ log_on = 1; }
#define 	LOG_OFF()				{ log_on = 0; }

#define 	DC_MIN         25		// DC = 0.05
#define     DC_START       51      //DC = 0.1
#define 	DC_MAX         486    // DC = 0.95

 
#define     COUNTER        244

unsigned int 						count = COUNTER;             //ADDED
unsigned int 						ad_res;
float                               v;  //ADDED
float                               i;  //ADDED
float                               t;  //ADDED
float                               iprom;
float                               vprom;
float                               tprom;
float 								proportional;
float 								integral;
float 								kp;							//Proportional constant, seems too big data type
float 								ki;		
unsigned int 						vref;
unsigned int 						iref;
char 								cmode;
unsigned char 						cc_cv;
unsigned int 						second;
unsigned char 						esc;					
unsigned int                        dc = 0;         			//Duty, change data size for 125Khz
unsigned char 						spb;						//Baud rate set
unsigned int            			log_on = 0;					//Variable to indicate if the log is activated  
char                                log_buffer[5]={0};   		//for printing data in the log
int                                 ip_buff = 0;  //Buffer for 
int                                 vp_buff = 0;  
int                                 tp_buff = 0; 

char const              comma = ',';
char const              S_str = 'S';
char const              C_str = 'C';
char const              V_str = 'V';
char const              I_str = 'I';
char const              T_str = 'T';

void Init_general(void);
void Init_registers(void);
void Shutdown_Peripherals(void);
void pid(float feedback, unsigned int setpoint);
void set_DC(void);
void read_ADC(void);
void log_control(void);
void display_value(int value);
void cc_cv_mode(float current_voltage, unsigned int reference_voltage, char CC_mode_status);
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