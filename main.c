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
        if (SECF) /// * The following tasks are executed every second:
        {     
            SECF = 0;
            log_control(); /// *  Then, the log is printed in the serial terminal by calling the #log_control() function
            cc_cv_mode(vprom, cvref, cmode);
            state_machine(); /// -# Then the #state_machine() function is called
            //temp_protection(); /// -# If at any point the temperature is higher than 35 degrees the process is stopped
            LINEBREAK;
            display_value_s((int)v);
            LINEBREAK;
            display_value_s((int)i);
            LINEBREAK;
            display_value_s((int)t);
        }     
	}
}

/**@brief This is the interruption service function. It will stop the process if an @b ESC or a @b "n" is pressed. 
*/
void interrupt ISR(void) 
{
    char recep = 0; /// Define and initialize @p recep variable to store the received character
    
    if(TMR1IF)
    {
        TMR1H = 0xE1;//TMR1 Fosc/4= 8Mhz (Tosc= 0.125us)
        TMR1L = 0x83;//TMR1 counts: 7805 x 0.125us x 4 = 3.9025ms
        TMR1IF = 0; //Clear timer1 interrupt flag
        v = read_ADC(V_CHAN); /// * Then, the ADC channels are read by calling the #read_ADC() function
        i = read_ADC(I_CHAN); /// * Then, the ADC channels are read by calling the #read_ADC() function
        i = (uint16_t) (abs((int)i - 2048 )); ///If the #state is #CHARGE or #POSTCHARGE change the sign of the result  
        t = read_ADC(T_CHAN); /// * Then, the ADC channels are read by calling the #read_ADC() function 
        if (conv) control_loop(); /// -# The #control_loop() function is called*/
        calculate_avg(); /// * Then, averages for the 250 values available each second are calculated by calling the #calculate_avg() function
        timing(); /// * Timing control is executed by calling the #timing() function 
        //if (TMR1IF) UART_send_string((char*)"T_ERROR1");
    }

    if(RCIF)/// If the UART reception flag is set then:
    {
        if(RC1STAbits.OERR) /// * Check for errors and clear them
        {
            RC1STAbits.CREN = 0;  
            RC1STAbits.CREN = 1; 
        }
        while(RCIF) recep = RC1REG; /// * Empty the reception buffer and assign its contents to the variable @p recep
        switch (recep)
        {
        case 0x63: /// * If @p recep received a @b "c", then:
            STOP_CONVERTER(); /// -# Stop the converter by calling the #STOP_CONVERTER() macro.
            state = STANDBY; /// -# Go to the #STANDBY state.
            break;
        case 0x6E: /// * If @p recep received an @b "n", then:
            STOP_CONVERTER(); /// -# Stop the converter by calling the #STOP_CONVERTER() macro.
            state = ISDONE; /// -# Go to the #ISDONE state.
        default:
            recep = 0;
            break;
        }
    }  
}