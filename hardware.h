/**
 * @file hardware.h
 * @author Juan J. Rojas
 * @date 7 Aug 2018
 * @brief Hardware header file for Charge and Discharge System.
 * @par Institution:
 * LaSEINE / CeNT. Kyushu Institute of Technology.
 * @par Mail (after leaving Kyutech):
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://bitbucket.org/juanjorojash/cell_charger_discharger
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

#define 	_XTAL_FREQ 				32000000
#define		BAUD_RATE               9600

#define		V_CHAN                  0b01101 ///> Definition of ADC channel for voltage measurements. AN13(RB5) 
#define		I_CHAN                  0b01011 ///> Definition of ADC channel for current measurements. AN11(RB4)
#define		T_CHAN                  0b01100 ///> Definition of ADC channel for temperature measurements. AN12(RB0)
#define		CELL1_ON				PORTAbits.RA7 = 1
#define		CELL2_ON				PORTAbits.RA6 = 1
#define		CELL3_ON				PORTCbits.RC0 = 1
#define		CELL4_ON				PORTCbits.RC1 = 1
#define		CELL1_OFF				PORTAbits.RA7 = 0
#define		CELL2_OFF				PORTAbits.RA6 = 0
#define		CELL3_OFF				PORTCbits.RC0 = 0
#define		CELL4_OFF				PORTCbits.RC1 = 0
/** @def AD_SET_CHAN(x)
Set the ADC channel to @p x and wait for 5 microseconds. 
*/
#define		AD_SET_CHAN(x)          { ADCON0bits.CHS = x; __delay_us(5); }
#define		AD_CONVERT()            { GO_nDONE = 1; while(GO_nDONE);}
#define     AD_RESULT()             { ad_res = 0; ad_res = (ADRESL & 0xFF)|((ADRESH << 8) & 0xF00);} 
//DC-DC CONVERTER RELATED DEFINITION
#define		STOP_CONVERTER()		{ conv = 0; RA1 = 1; dc = DC_MIN; set_DC(); Cell_OFF(); LOG_OFF(); RCIE = 0;}
/**< @brief Stop the converter*/
/**< Set @p conv to zero, turn off the main relay (@p RA1), set the duty cycle in @p DC_MIN, 
turn off all the cell relays in the switcher board, disable the logging of data to the terminal 
and the USART reception interrupts.
*/
#define 	UART_INT_ON()			{ while(RCIF) clear = RC1REG; RCIE = 1; }  
///< Clear transmission buffer and turn ON UART transmission interrupts.
#define 	UART_INT_OFF()			{ log_on = 0; }  ///< Turn OFF UART transmission interrupts.
#define 	LOG_ON()				{ log_on = 1; }  ///< Turn OFF logging in the terminal.
#define 	LOG_OFF()				{ log_on = 0; }  ///< Turn ON logging in the terminal.
#define 	DC_MIN         			25  ///< Minimum possible duty cycle, set around @b 0.05
#define     DC_START       			51  ///< Initial duty cycle, set around @b 0.1
#define 	DC_MAX         			486  ///< Maximum possible duty cycle, set around @b 0.95
#define     COUNTER        			244  ///< Counter value, needed to obtained one second between counts. 
#define     LINEBREAK               UART_send_char(10)  ///< Send a linebreak to the terminal.
unsigned int 						count; ///< Counter that should be cleared every second.
/**< Every control loop cycle this counter will be decreased. This variable is used to calculate the averages and to trigger
all the events that are done every second.*/
unsigned int 						ad_res; ///< Result of an ADC measurement.
float                               v;  ///< Last voltage ADC measurement.
float                               i;  ///< Last current ADC measurement.
float                               t;  ///<  Last temperature ADC measurement.
float                               vprom;  ///< Last one-second-average of @link v @endlink.
float                               iprom;  ///< Last one-second-average of @link i @endlink.
float                               tprom;  ///< Last one-second-average of @link t @endlink. 
int                                 vmax = 0;   ///< Maximum recorded average voltage. 
float 								proportional;  ///< Proportional component of PI compensator
float 								integral;  ///< Integral component of PI compensator
float 								kp;  ///< Proportional compesator gain
float 								ki;  ///< Integral compesator gain		
unsigned int 						vref;  ///< Voltage setpoint
unsigned int 						iref;  ///< Current setpoint
char 								cmode;  ///< CC / CV selector. CC: <tt> cmode = 1 </tt>. CV: <tt> cmode = 0 </tt> 	
unsigned int                        dc = 0;  ///< Duty cycle
unsigned char 						spb;						//Baud rate set
char 								clear;  ///< Variable to clear the transmission buffer of UART
unsigned int            			log_on = 0;					//Variable to indicate if the log is activated  
char                                log_buffer[5]={0};   		//for printing data in the log
int                                 ip_buff = 0;  ///< Current buffer to send to the terminal usign @link log_control() @endlink.
int                                 vp_buff = 0;  ///< Voltage buffer to send to the terminal usign @link log_control() @endlink.
int                                 tp_buff = 0;  ///< Temperature buffer to send to the terminal usign @link log_control() @endlink.

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