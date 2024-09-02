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
 * DELTALab. Instituto Tecnologico de Costa Rica 
 * @par Mail:
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://github.com/DeltaLabo/cell_charger_discharger
 */

#include "charger_discharger.h"

/**@brief <b> This is the main function of the program. It initializes the system in every reset and takes care of several tasks that are excecuted every second. </b>
*/

void main(void) /// This function performs the folowing tasks:                     
{           
    initialize(); /// <ul> <li> Call the #initialize function
    __delay_ms(10);
    interrupt_enable(); // this I added for the test
    while(1) /// <li> <b> The main loop repeats the following forever: </b> 
    {
        if (SECF) /// <ul> <li> Check the #SECF flag, if it is set, 1 second has passed since last execution, so the folowing task are executed:
        {
            scaling(); /// <li> Scale the average measured values by calling the #scaling function
            state_machine(); /// <li> Call the #state_machine function
            log_control(); /// <li> Print the log in the serial terminal by calling the #log_control function
            if (basic_configuration.version == 1){
                cc_cv_mode(vavg, basic_configuration.const_voltage, cmode); /// <li> Check if the system shall change to CV mode by calling the #cc_cv_mode function
            }
            SECF = 0; /// <ol> <li> Clear the #SECF flag to restart the 1 second timer
        }
	}
}

/**@brief <b> This is the interruption service function. It will interrupt the code whenever the Timer1 overflow (0.975625 milliseconds) or when any character is received from the serial terminal via UART. </b>
*/
void __interrupt() ISR(void) /// This function performs the folowing tasks: 
{
    if(RCIF)/// <li> Check the @b UART reception interrupt flag, if it is set, the folowing task are executed:
    {
        RCIF = 0;
        if(RC1STAbits.OERR) /// <ol> <li> Check for any errors and clear them
        { 
            RC1STAbits.CREN = 0;
            RC1STAbits.CREN = 1;
            //UART_send_byte(false);
            UART_send_string((char*)"OERR_ERROR");
            state = IDLE;
        }
        else
        {
            UART_send_byte(command_interpreter());
        }
    }
        
    if(TMR1IF) /// <li> Check the @b Timer1 interrupt flag, if it is set, the folowing task are executed:
    {
        TMR1H = 0xE1; // TMR1 clock is Fosc/4= 8Mhz (Tick= 0.125us). TMR1IF is set when the 16-bit register overflows. 7805 x 0.125us = 0.975625 ms.
        TMR1L = 0x83;/// <ol> <li> Load the @b Timer1 16-bit register so it overflow every 0.975625 ms 
        TMR1IF = 0; /// <li> Clear the @b Timer1 interrupt flag
        v = read_ADC(V_CHAN); /// <li> Read the ADC channel #V_CHAN and store the value in #v. Using the #read_ADC() function
        i = read_ADC(I_CHAN); /// <li> Read the ADC channel #I_CHAN and store the value in #i. Using the #read_ADC() function
        i = (uint16_t) (abs ( 2048 - (int)i ) ); /// <li> Substract the 2.5V bias from #i, store the absolute value in #i   
        if (conv) control_loop(); /// <li> Call the #control_loop() function
        else pidi = 0;
        calculate_avg(); /// <li> Call the #calculate_avg() function
        timing(); /// <li> Call the #timing() function
        if (TMR1IF) UART_send_string((char*)"TIMING_ERROR"); /// <li> If the @b Timer1 interrupt flag is set, there is a timing error, print "TIMING_ERROR" into the terminal. </ol>
    }
}