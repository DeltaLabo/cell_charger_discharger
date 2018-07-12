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
uint16_t                counting = 0;
uint8_t                 PWM = 0;

void main(void)
{
    Init_Registers();
	Initialize_Hardware();
    Init_UART();
    state = STANDBY;       
    count = COUNTER; 
    iprom = 0;
    vprom = 0;
    tprom = 0;
    wait_count = 0;
    dc_res_count = 0;
    esc = 0;    
    __delay_ms(10);

    TRISBbits.TRISB0 = 0;               //Set RB0 as output. led
    ANSELBbits.ANSB0 = 0;               //Digital
    TRISB1 = 1;     //Set RB1 as input
    WPUB1 = 0;      //Disable pull up
    
    while(1)
	{        
        if(TMR0IF)
        {
            TMR0IF = 0;
            counting++;
            if (counting > 20){
                if (LATB0){
                RB0 = 0;
                }else RB0 = 1; 
                counting = 0;
//                UART_send_string("LED ON/OFF");
//                LINEBREAK;
//                if (PWM >= 255){
//                    PWM = 0;
//                }else PWM ++;
//                PSMC1DCL = PWM;     
//                PSMC1CONbits.PSMC1LD = 1; //Load Buffer
            }
            read_ADC();
            calculate_avg();      
			State_Machine();
		}
        log_control();
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
            STOP_CONVERTER();
            LINEBREAK;
            UART_send_string(next_cell_str_main);
            LINEBREAK;
            __delay_ms(50);
            cell_count++;
            state = IDLE;  
        }else
        {
            esc = 0;
        }
    }    
}