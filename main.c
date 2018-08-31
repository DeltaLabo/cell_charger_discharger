/**
 * @file main.c
 * @author Juan J. Rojas
 * @date 7 Aug 2018
 * @brief Main source file for Charge and Discharge System.
 * @par Institution:
 * LaSEINE / CeNT. Kyushu Institute of Technology.
 * @par Mail (after leaving Kyutech):
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://bitbucket.org/juanjorojash/cell_charger_discharger
 */

#include "hardware.h"
#include "state_machine.h"

void main(void)
{    
    Init_registers();
	Init_general();
    Init_UART(); 
    __delay_ms(10);
    //UART_send_string("\r\nPCON: "); 
    //display_value(PCON);
    //UART_send_string(" \r\nSTATUS:");
    //display_value(STATUS);
    //WPUE3 = 1;      //Enable pull up for MCLR
    //HACKS FOR THIS BOARD
    TRISB0 = 1;     //Set RB1 as input
    WPUB0 = 0;      //Disable pull up
    TRISB1 = 1;     //Set RB1 as input
    WPUB1 = 0;      //Disable pull up
    TRISC3 = 1;     //As input to avoid control //old position
    WPUC3 = 0;      //Disable pull up
    TRISC4 = 1;     //As input to avoid control //old position
    WPUC4 = 0;      //Disable pull up
    TRISC5 = 1;     //As input to avoid control //old position
    WPUC5 = 0;      //Disable pull up

    while(1)
	{   
        if(TMR0IF){
            TMR0 = 0x07;
            TMR0IF = 0;
            read_ADC();
            calculate_avg();
            log_control();      //Log control shall be before the state machine
            if (!count)
            {           
                #if (LI_ION_CHEM) 
                cc_cv_mode(vprom, vref, cmode);
                #endif
                State_machine();                  
            }
            if (conv)
            {
                RA1 = 0;            //close main relay
                control_loop();     //start controlling
                if (TMR0IF) UART_send_string((char*)"T_ERROR");
            }else RA1 = 1;             
            timing();            
		}        
	}
}

void interrupt serial_interrupt(void) 
{
    char esc = 0;
    if(RCIF)
    {

        if(RC1STAbits.OERR) // check for Error 
        {
            RC1STAbits.CREN = 0; //If error -> Reset 
            RC1STAbits.CREN = 1; //If error -> Reset 
        }
        
        while(RCIF) esc = RC1REG; //receive the value and put it to esc

        if (esc == 0x1B)
        {
            STOP_CONVERTER();
            esc = 0;
            wait_count = 0;
            dc_res_count = 0;
            __delay_ms(50);
            state = STANDBY;
        }else if  (esc == 'n')      //if the user press 'n' to go to next cell
        {
            STOP_CONVERTER();
            __delay_ms(50);
            cell_count++;
            state = IDLE;  
        }else
        {
            esc = 0;
        }
    }    
}