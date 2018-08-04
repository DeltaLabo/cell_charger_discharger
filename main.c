/* Main loop for Charge and Discharge System. */
/* Kyutech Institute of Technology. LaSEINE. Supervisor: Mengu CHO.*/
/* 
 * File:  main.c 
 * Author: Juan J. Rojas
 * Version control: Git
 */

#include "hardware.h"
#include "state_machine.h"

char const              next_cell_str_main[] = "---------->NEXT_CELL<----------";

void main(void)
{    
    Init_Registers();
	Initialize_general();
    Init_UART(); 
    __delay_ms(10);
    //UART_send_string("\r\nPCON: "); 
    //display_value(PCON);
    //UART_send_string(" \r\nSTATUS:");
    //display_value(STATUS);
    //WPUE3 = 1;      //Enable pull up for MCLR
    //HACKS FOR THIS BOARD
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
        if(TMR0IF)
        {
            TMR0IF = 0;
            read_ADC();
            calculate_avg();
            if (conv)
            {
                RA1 = 0;            //close main relay
                control_loop();     //start controlling
            }else RA1 = 1; 
            if (!count)
            {                     
                cc_cv_mode();
                log_control();      //Log control shall be before the state machine
                State_Machine();                  
            }
            timing();       
		}        
	}
}

void interrupt serial_interrupt(void) 
{
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
            state = STANDBY;
            esc = 0;
            wait_count = 0;
            dc_res_count = 0;
        }else if  (esc == 110)
        {
            __delay_ms(50);
            cell_count++;
            state = IDLE;  
        }else
        {
            esc = 0;
        }
    }    
}