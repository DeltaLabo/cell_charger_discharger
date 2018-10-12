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
    initialize(); /// * Call the #initialize() function
    __delay_ms(10);
    //WPUE3 = 1;      //Enable pull up for MCLR

    while(1) /// <b> Repeat the following steps forever </b>
    {
        if(TMR0IF){ /// If the flag of the Timer0 is set then:
            TMR0 = 0x07; /// * The Timer0 period register is set to 7, which gives 252 instructions (including 2 of delay) to overflow
            TMR0IF = 0; /// * Then, the Timer0 flag is cleared
            read_ADC(); /// * Then, the ADC channels are read by calling the #read_ADC() function
            calculate_avg(); /// * Then, averages for the 250 values available each second are calculated by calling the #calculate_avg() function
            log_control(); /// *  Then, the log is printed in the serial terminal by calling the #log_control() function
            if (!count) /// * The following tasks are excuted every second:
            {          
                #if (LI_ION_CHEM) /// -# If the chemistry is Li Ion the #cc_cv_mode() function is called
                cc_cv_mode(vprom, vref, cmode);
                #endif
                state_machine(); /// -# Then the #state_machine() function is called
            }
            if (conv) /// * If the variable #conv is set it means the converter shall be started, then:
            {
                RC5 = 1; /// -# The main relay is closed
                control_loop(); /// -# The #control_loop() function is called*/
                if (TMR0IF) UART_send_string((char*)"T_ERROR"); /// -# If by that point the timer flag was set again and error message is printed
            }else RC5 = 0; /// Else, the main relay is keep closed         
            timing(); /// * Timing control is executed by calling the #timing() function    
		}        
	}
}

/**@brief This is the interruption service function. It will stop the process if an @b ESC or a @b "n" is pressed. 
*/
void interrupt serial_interrupt(void) 
{
    volatile char recep = 0; /// Define and initialize @p recep variable to store the received character


    if(RCIF)/// If the UART reception flag is set then:
    {
        if(RC1STAbits.OERR) /// * Check for errors and clear them
        {
            RC1STAbits.CREN = 0;  
            RC1STAbits.CREN = 1; 
        }
        while(RCIF) recep = RC1REG; /// * Empty the reception buffer and assign its contents to the variable @p recep
        if (recep == 0x1B) /// * If @p recep received an @b ESC, then:
        {   
            STOP_CONVERTER(); /// -# Stop the converter by calling the #STOP_CONVERTER() macro.
            state = STANDBY; /// -# Go to the #STANDBY state.
        }else if  (recep == 'n') /// * Else If @p recep received an @b "n", then:
        {
            STOP_CONVERTER(); /// -# Stop the converter by calling the #STOP_CONVERTER() macro.
            state = ISDONE; /// -# Go to the #ISDONE state.
        }else /// * Else:
        {
           recep = 0; /// -# Clear the @p recep variable.
        }
    }  
}