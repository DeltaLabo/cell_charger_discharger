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

/**@brief This is the main function of the program.
*/
void main(void)
{   
    /**initially the @link Initialize() @endlink function is called*/ 
    Initialize();
    __delay_ms(10);
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

    /**Then the main loop enters*/ 
    while(1)
	{   
    /**If the flag of the Timer0 is set then:*/
        if(TMR0IF){
        /** - The Timer0 period register is set to 7, which gives 252 instructions (including 2 of delay) to overflow*/
        /** - Which gives @b Instruction @b clock {32MHz/4} / @b Prescaler {128} / @b Counter {250} = 250Hz = 4ms of period.*/
            TMR0 = 0x07;
        /** - Then, the Timer0 flag is cleared*/
            TMR0IF = 0;
        /** - Then, the ADC channels are read by calling the @link read_ADC() @endlink function*/
            read_ADC();
        /** - Then, averages for the 250 values available each second are calculated by calling the @link calculate_avg() @endlink function*/
            calculate_avg();
        /** - Then, the log is printed in the serial terminal by calling the @link log_control() @endlink function*/
            log_control();
        /** - If the counter in the variable @link count @endlink is cleared it means that 1 second has passed:*/
            if (!count)
            {
        /**     + If the chemistry is Li Ion the @link cc_cv_mode() @endlink function is called.*/           
                #if (LI_ION_CHEM) 
                cc_cv_mode(vprom, vref, cmode);
                #endif
        /**     + Then the @link State_machine() @endlink function is called.*/
                State_machine(); 
        /**     .*/              
            }
        /** - If the variable @link conv @endlink is set then:*/
            if (conv)
            {
        /**     - The main relay is closed.*/
                RA1 = 0;            //close main relay
        /**     - Then the @link control_loop() @endlink function is called.*/
                control_loop();     //start controlling
                if (TMR0IF) UART_send_string((char*)"T_ERROR");
        /**     .*/
            }else RA1 = 1;             
            timing(); 
        /** .*/           
		}        
	}
}

void interrupt serial_interrupt(void) 
{
    volatile char esc = 0;
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