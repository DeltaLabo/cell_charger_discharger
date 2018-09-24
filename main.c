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

#include "charger_discharger.h"

/**@brief This is the main function of the program.
*/
void main(void)
{   
/** Initially the @link Initialize() @endlink function is called*/ 
    initialize();
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

/** Then the @p while(1) loop enters*/ 
    while(1)
	{   
/**     If the flag of the Timer0 is set then:.*/
        if(TMR0IF){
/**         - The Timer0 period register is set to 7, which gives 252 instructions (including 2 of delay) to overflow\n
            That is: @b Instruction @b clock {32MHz/4} / @b Prescaler {128} / @b Counter {250} = 250Hz = 4ms of period*/
            TMR0 = 0x07;
/**         - Then, the Timer0 flag is cleared*/
            TMR0IF = 0;
/**         - Then, the ADC channels are read by calling the @link read_ADC() @endlink function*/
            read_ADC();
/**         - Then, averages for the 250 values available each second are calculated by calling the @link calculate_avg() @endlink function*/
            calculate_avg();
/**         - Then, the log is printed in the serial terminal by calling the @link log_control() @endlink function*/
            log_control();
/**         - If the counter in the variable @link count @endlink is cleared it means that 1 second has passed.\n
            The following tasks are excuted every second:*/
            if (!count)
            {
/**             -# If the chemistry is Li Ion the @link cc_cv_mode() @endlink function is called*/           
                #if (LI_ION_CHEM) 
                cc_cv_mode(vprom, vref, cmode);
                #endif
/**             -# Then the @link State_machine() @endlink function is called*/
                state_machine();  
            }
/**         - If the variable @link conv @endlink is set it means the converter shall be stated, then:*/
            if (conv)
            {
/**             -# The main relay is closed*/
                RA1 = 0;
/**             -# Then the @link control_loop() @endlink function is called*/
                control_loop();
/**             -# If by that point the timer flag was set again and error message is printed*/
                if (TMR0IF) UART_send_string((char*)"T_ERROR");
/**         - Else, the main relay is keep closed*/
            }else RA1 = 1;             
/**         Timing control is executed by calling the @link timing() @endlink function*/
            timing();       
		}        
	}
}

/**@brief This is the interruption service function. It will stop the process if an @b ESC or a @b "n" is pressed. 
*/
void interrupt serial_interrupt(void) 
{
/** The function first define and initialize a variable (@p recep) to store the received character*/
    volatile char recep = 0;

/** If the UART reception flag is set then:*/
    if(RCIF)
    {
/**     - Check for errors and clear them*/
        if(RC1STAbits.OERR) // check for Error 
        {
            RC1STAbits.CREN = 0; //If error -> Reset 
            RC1STAbits.CREN = 1; //If error -> Reset 
        }
/**     - Empty the reception buffer and assign its contents to the variable @p recep*/
        while(RCIF) recep = RC1REG; 
/**     - If @p recep received an @b ESC, then:*/
        if (recep == 0x1B)
        {   
/**         -# Stop the converter by calling the @link STOP_CONVERTER() @endlink macro.*/
            STOP_CONVERTER();
/**         -# Go to the @link STANDBY @endlink state.*/
            state = STANDBY;
/**     - Else If @p recep received an @b "n", then:.*/
        }else if  (recep == 'n')      //if the user press 'n' to go to next cell
        {
/**         -# Stop the converter by calling the @link STOP_CONVERTER() @endlink macro.*/
            STOP_CONVERTER();
/**         -# Go to the @link ISDONE @endlink state.*/
            state = ISDONE; 
/**     - Else:*/
        }else
        {
/**         -# Clear the @p recep variable.*/
           recep = 0;
        }
    }  
}