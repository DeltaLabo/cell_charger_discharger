/**
 * @file
 * main.c
 * @author 
 * Juan J. Rojas
 * @date 
 * 7 Aug 2018
 * @brief 
 * Main source file. Includes the main loop and the interruption service routine. 
 * @par Institution:
 * LaSEINE / CeNT. Kyushu Institute of Technology.
 * @par Mail:
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://bitbucket.org/juanjorojash/cell_charger_discharger
 */

#include "charger_discharger.h"

/**@brief This is the main function of the program. It takes care of initializing the registers   
*/

void main(void) /// This function performs the folowing tasks:                     
{       
    initialize(); /// <ul> <li> the #initialize function is excecuted one time.
    __delay_ms(10);
    while(1) /// <li> then the main @b while repeats the following steps forever: 
    {
        if (SECF) /// <ul> <li> check the #SECF flag, if it is set, the folowing task are excecuted:
        {     
            SECF = 0; /// -# clear the #SECF flag to restart the 1 second timer
            log_control(); /// -# print the log in the serial terminal by calling the #log_control function
            cc_cv_mode(vprom, cvref, cmode); /// -# check if the system shall change to CV mode by calling the #cc_cv_mode function
            state_machine(); /// -# call the #state_machine function
            temp_protection(); /// -# call the #temp_protection function </ul> </ul>
        }
	}
}

/**@brief This is the interruption service function. It will interrupt the code whenever the Timer1 overflow (0.975625 milliseconds) or when any character is received from the serial terminal via UART. 
*/
void __interrupt() ISR(void) /// This function performs the folowing tasks: 
{
    char recep = 0; /// <ul> <li> define and initialize the @p recep variable to store the character received via UART
    
    if(TMR1IF) /// <li> check the @b Timer1 interrupt flag, if it is set, the folowing task are executed:
    {
        TMR1H = 0xE1; // TMR1 clock is Fosc/4= 8Mhz (Tick= 0.125us). TMR1IF is set when the 16-bit register overflows. 7805 x 0.125us = 0.975625 ms.
        TMR1L = 0x83;/// <ol> <li> load the @b Timer1 16-bit register so it overflow every 0.975625 ms 
        TMR1IF = 0; /// <li> clear the @b Timer1 interrupt flag
        v = read_ADC(V_CHAN); /// <li> read the ADC channel #V_CHAN and store the value in #v. Using the #read_ADC() function
        i = read_ADC(I_CHAN); /// <li> read the ADC channel #I_CHAN and store the value in #i. Using the #read_ADC() function
        i = (uint16_t) (abs ( 2048 - (int)i ) ); /// <li> substract the 2.5V bias from #i, use the absolute value   
        t = read_ADC(T_CHAN); /// <li> read the ADC channel #T_CHAN and store the value in #t. Using the #read_ADC() function 
        if (conv) control_loop(); /// <li> call the #control_loop() function
        calculate_avg(); /// <li> call the #calculate_avg() function
        timing(); /// <li> call the #timing() function
        if (TMR1IF) UART_send_string((char*)"TIMING_ERROR"); /// <li> if the @b Timer1 interrupt flag is set, there is a timing error, print "TIMING_ERROR" into the terminal. </ol>
    }

    if(RCIF)/// <li> check the @b UART reception interrupt flag, if it is set, the folowing task are executed:
    {
        if(RC1STAbits.OERR) /// <ol> <li> check for any errors and clear them
        {
            RC1STAbits.CREN = 0;  
            RC1STAbits.CREN = 1; 
        }
        while(RCIF) recep = RC1REG; /// <li> assign the content of the UART reception buffer to the variable @p recep
        switch (recep)
        {
        case 0x63: /// <li> if @p recep received a @b "c", the process is stop, then:
            STOP_CONVERTER(); ///                   - stop the converter by calling the #STOP_CONVERTER() macro
            state = STANDBY; ///                    - set the #state to #STANDBY
            break;
        case 0x6E: /// <li> if @p recep received an @b "n", the systems jumps to the next cell, then:
            STOP_CONVERTER(); ///                   - stop the converter by calling the #STOP_CONVERTER() macro
            state = ISDONE; ///                     - set the #state to #ISDONE 
            break;
        default: /// <li> in any other case do nothing
            recep = 0; 
        } /// </ol> </ul>
    }  
}