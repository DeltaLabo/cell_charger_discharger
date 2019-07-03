/**
 * @file charger_discharger.c
 * @author Juan J. Rojas
 * @date 7 Aug 2018
 * @brief Hardware source file for Charge and Discharge System.
 * @par Institution:
 * LaSEINE / CeNT. Kyushu Institute of Technology.
 * @par Mail (after leaving Kyutech):
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://bitbucket.org/juanjorojash/cell_charger_discharger
 */

#include "charger_discharger.h"

/**@brief Function to define initialize the system
*/
void initialize()
{
    /** @b GENERAL*/
    CLRWDT(); /// * Clear WDT by calling @p CLRWDT()
    nWPUEN = 0; /// * Allow change of individual WPU
    /** @b SYSTEM @b CLOCK*/
    /** PLL is always enabled because of configuration bits*/
    OSCCONbits.IRCF = 0b1111; /// * Set clock to 32MHz (with PLL)
    OSCCONbits.SCS = 0b00; /// * Clock determined by FOSC<2:0> in Configuration Words
    OSCCONbits.SPLLEN = 1; /// * Enable PLL. According to Errata this shall not be done in the Configuration Words
    /** @b RELAY @b OUPUTS*/
    //PORTC doesn't have ANSELC
    TRISC3 = 0; /// * Set RC3 as output. Discharge set
    WPUC3 = 0; /// * Weak pull up deactivated
    TRISC4 = 0; /// * Set RC4 as output. Charge set
    WPUC4 = 0; /// * Weak pull up deactivated    
    TRISC5 = 0; /// * Set RC5 as output. ON/OFF relay
    WPUC5 = 0; /// * Weak pull up deactivated
    /** @b CELL @b SWITCHER @b OUPUTS*/
    TRISB2 = 0; /// * Set RB2 as output. Cell #1
    ANSB2 = 0; /// * Set RB2 as digital
    WPUB2 = 0; /// * Weak pull up deactivated
    TRISB3 = 0; /// * Set RB3 as output. Cell #2
    ANSB3 = 0; /// * Set RB3 as digital
    WPUB3 = 0; /// * Weak pull up deactivated
    TRISB4 = 0; /// * Set RB4 as output. Cell #3
    ANSB4 = 0; /// * Set RB4 as digital  
    WPUB4 = 0; /// * Weak pull up deactivated
    TRISB5 = 0; /// * Set RB5 as output. Cell #4
    ANSB5 = 0; /// * Set RB% as digital  
    WPUB5 = 0; /// * Weak pull up deactivated
    Cell_OFF();
   /** @b TIMER0 for control and measuring loop*/
   TMR0IE = 0; /// * Disable timer0 interruptions
//    TMR0CS = 0; /// * Timer set to internal instruction cycle
//    OPTION_REGbits.PS = 0b110; /// * Prescaler set to 128
//    OPTION_REGbits.PSA = 0; /// * Prescaler activated
//    TMR0IF = 0; /// * Timer flag cleared
//    TMR0 = 0x07; /// * Counter set to 255 - @b 250 + 2 (delay for sync) = 7
   /** Timer set to 32Mhz/4/128/250 = 250Hz*/
    /** @b TIMER 1 for control and measuring loop using interruption
    /* Preload TMR1 register pair for 1us overflow */
    /* T1OSCEN = 1, nT1SYNC = 1, TMR1CS = 0 and TMR1ON = 1*/
    nT1SYNC = 0;     //Synchronized
    T1OSCEN = 0;
    TMR1ON = 0;       //ON
    TMR1GE = 0;      //Dont care about gate
    TMR1CS0 = 0;       
    TMR1CS1 = 0;    //FOSC/4
    T1CKPS0 = 0;
    T1CKPS1 = 1;
    TMR1H = 0xE0;//TMR1 Fosc/4= 8Mhz (Tosc= 0.125us)
    TMR1L = 0xC0;//TMR1 counts: 8000 x 0.125us = 1ms
    /** @b PSMC/PWM @b SETTINGS*/
    /** Programmable switch mode control (PSMC)*/
    PSMC1CON = 0x00; /// * Clear PSMC1 configuration to start
    PSMC1MDL = 0x00; /// * No modulation
    PSMC1CLK = 0x01; /// * Driven by 64MHz PLL system clock
    PSMC1PRH = 0x01; /// * Set period high register to 0x01
    PSMC1PRL = 0xFF; /// * Set period low register to 0xFF
    /** 511 + 1 clock cycles for period that is 8us (125KHz)*/
    /** This set the PWM with 9 bit of resolution*/
    /** Duty cycle*/
    PSMC1DCH = 0x00;                    // * Set duty cycle high register to 0x00   
    PSMC1DCL = 0x00;                    // * Set duty cycle low register to 0x00
    /* Duty cycle starts in 0 */  
    /** Phase or rising event*/
    PSMC1PHH = 0x00;                    /// * Rising event starts from the beginning
    PSMC1PHL = 0x00;                    /// * Rising event starts from the beginning
    P1STRC = 1;           /// * Single PWM activated in PSMC1C (RC2)
    P1POLC = 0;            /// * Active high (RC2)
    P1OEC = 1;             /// * PSMC activated in PSMC1C (RC2)
    P1PRST = 1;            /// * Period event occurs when PSMC1TMR = PSMC1PR
    P1PHST = 1;            /// * Rising edge event occurs when PSMC1TMR = PSMC1PH
    P1DCST = 1;            /// * Falling edge event occurs when PSMC1TMR = PSMC1DC
    PSMC1CON = 0x80;                    /// * Enable|Load Buffer|Dead band disabled|Single PWM
    //PSMC1TIE = 1;                       //Enable interrupts for Time Based 
    WPUC2 = 0; /// * Disable WPU for RC0.
    TRISC2 = 0;                         /// * Set RC2 as output
    /** @b ADC*/
    /** ADC INPUTS*///check this after final design
    TRISA3 = 1; /// * RA3, Positive voltage reference
    ANSA3 = 1; /// * RA3 analog
    WPUA3 = 0; /// * Weak pull up deactivated
    TRISB1 = 1; /// * RB1, voltage sensing input
    ANSB1 = 1; /// * RB1 analog
    WPUB1 = 0; /// * RB1 weak pull up deactivated
    TRISB0 = 1; /// * RB0, current sensing input
    ANSB0 = 1; /// * RB0 analog
    WPUB0 = 0; /// * RB0 weak pull up deactivated
    TRISA5 = 1; /// * RA5, temperature sensing input
    ANSA5 = 1; /// * RA5 analog
    WPUA5 = 0; /// * RA5 weak pull up deactivated   
    /** Configs*/
    ADCON0bits.ADRMD = 0; /// * 12 bits result
    ADCON1bits.ADCS = 0b010; /// * Clock selected as FOSC/32
    ADCON1bits.ADNREF = 0; /// * Connected to Vss
    ADCON1bits.ADPREF = 0b01; /// * Connected to Vref+
    ADCON1bits.ADFM = 1; /// * 2's compliment result
    ADCON2bits.CHSN = 0b1111; /// * Negative differential input as ADNREF
    ADCON0bits.ADON = 1; /// * Turn on the ADC
    /** @b UART*/
    //**Setting I/O pins for UART*/
    TXSEL = 0;      /// * RC6 selected as TX
    RXSEL = 0;      /// * RC7 selected as RX
    //________I/O pins set __________//
    
    /**Initialize SPBRG register for required 
    baud rate and set BRGH for fast baud_rate**/
    SP1BRGH = 0x00; 
    SP1BRGL = 0x8A;    
    
    BRGH  = 1;  /// * for high baud_rate
    BRG16 = 1;  /// * for 16 bits timer
    //_________End of baud_rate setting_________//
    
    //****Enable Asynchronous serial port*******//
    SYNC  = 0;    /// * Asynchronous
    SPEN  = 1;    /// * Enable serial port pins
    //_____Asynchronous serial port enabled_______//

    //**Lets prepare for transmission & reception**//
    TXEN  = 1;    /// * enable transmission
    CREN  = 1;    /// * enable reception
    //__UART module up and ready for transmission and reception__//
    //**Select 8-bit mode**//  
    TX9   = 0;    /// * 8-bit reception selected
    RX9   = 0;    /// * 8-bit reception mode selected
    //__8-bit mode selected__/
    RCIE = 0; /// * Disable UART reception interrupts
    TXIE = 0; /// * Disable UART transmission interrupts
    /** @bFINAL CHECK ALL!!*/
    STOP_CONVERTER(); ///* Call #STOP_CONVERTER()
    ad_res = 0; /// * Clear ADC result variable
    cmode = 1; /// * Start in CC mode    
    wait_count = 0; /// * CHECK!!!
    dc_res_count = 0; /// * CHECK!!
    RC3 = 0; /// * RELAY OUTPUT DOWN 
    RC4 = 0; /// * RELAY OUTPUT DOWN 
}
/**@brief This function defines the PI controller
*  @param   feedback average of measured values for the control variable
*  @param   setpoint desire controlled output for the variable
*/
void pid(float feedback, unsigned setpoint)
{ 
float 	er; /// * Define @p er for calculating the error
float   pi; /// * Define @p pi for storing the PI compesator value
    er = setpoint - feedback; /// * Calculate the error by substracting the @p feedback from the @p setpoint and store it in @p er
    if(er > ERR_MAX) er = ERR_MAX; /// * Make sure error is never above #ERR_MAX
    if(er < ERR_MIN) er = ERR_MIN; /// * Make sure error is never below #ERR_MIN
    proportional = (kp * er); /// * Calculate #proportional component of compensator
	integral += (ki * er)/COUNTER; /// * Calculate #integral component of compensator
    pi = proportional + integral; /// * Sum them up and store in @p pi*/
    if (dc + pi >= dcmax){ /// * Make sure duty cycle is never above #DC_MAX
        dc = dcmax;
    }else if (dc + pi <= dcmin){ /// * Make sure duty cycle is never below #DC_MIN
        dc = dcmin;
    }else{
        dc += (int)(pi + 0.5); /// * Store the new value of the duty cycle with operation @code dc = dc + pi @endcode
    }   
}
/**@brief This function sets the desired duty cycle
*/
void set_DC()
{
/// This function can set the duty cycle from 0x0 to 0x1FF
    PSMC1DCL = dc & 0x00FF; /// * Lower 8 bits of #dc are stored in @p PSMC1DCL
    PSMC1DCH = (dc >> 8) & 0x01; /// * Higher 1 bit of #dc are stored in @p PSMC1DCH
    PSMC1CONbits.PSMC1LD = 1; /// * Set the load register. This will load all the setting as once*/
}
/**@brief This function switches between CC and CV mode.
* @param current_voltage average of current voltage
* @param referece_voltage voltage setpoint
* @param CC_mode_status current condition of #cmode variable
*/
void cc_cv_mode(float current_voltage, unsigned int reference_voltage, char CC_mode_status)
{
/// If the current voltage is bigger than the voltage setpoint and the system is in CC mode, then:
    if(current_voltage > reference_voltage && CC_mode_status == 1)
    {        
            proportional = 0; /// * The #proportional is set to zero
            integral = 0; /// * The #integral is set to zero
            cmode = 0; /// * The system is set in CV mode by clearing the #cmode variable
            kp = CV_kp; /// * The proportional constant, #kp is set to #CV_kp 
            ki = CV_ki; /// * The integral constant, #ki is set to #CV_ki 
    }    
}
/**@brief This function takes care of printing the test data using the UART
*/
void log_control()
{
/**The code in this function is only excecuted if the #log_on variable is set*/
/**This funtion takes care of sending the logging data in pieces to avoid disturbing the control loop. 
This problem can be avoided with the use of interruptions for the control loop; however this was not implemented
and could be considered as some future improvement*/  
    if (log_on)
    {
                ip_buff = (int) iprom; /// * Define @p ip_buff for storing #i_prom
                vp_buff = (int) vprom; /// * Define @p vp_buff for storing #v_prom
                tp_buff = (int) tprom; /// * Define @p tp_buff for storing #tprom
                qp_buff = (unsigned) ((qprom * 10) + 0.05); /// Define @p qp_buff for storing #qprom @c * @c 10*/
                LINEBREAK;
                itoa(log_buffer,minute,10); /// * Convert #minute into a string and store it in #log_buffer
                UART_send_char(log_buffer[0]); /// * Send #log_buffer[0]
                if (minute >= 10) UART_send_char(log_buffer[1]); /// * If #minute is bigger than 10 send #log_buffer[1]
                if (minute >= 100) UART_send_char(log_buffer[2]); /// * If #minute is bigger than 100 send #log_buffer[2]
                UART_send_char(colons); /// * Send a colons character
                memset(log_buffer, '0', 8); /// * Clear #log_buffer
                itoa(log_buffer,second,10); /// * Convert #second into a string and store it in #log_buffer
                if (second < 10) UART_send_char('0'); /// * If #second is smaller than 10 send a '0'
                else UART_send_char(log_buffer[0]); /// * Else, send #log_buffer[0]
                if (second < 10) UART_send_char(log_buffer[0]); /// * If #second is smaller than 10 send #log_buffer[0]
                else UART_send_char(log_buffer[1]); /// * Else, send #log_buffer[1]
                UART_send_char(comma); /// * Send a comma character
                memset(log_buffer, '0', 8); /// * Clear #log_buffer
                UART_send_char(C_str); /// * Send a 'C'
                UART_send_char(cell_count); /// * Send a #cell_count variable
                UART_send_char(comma); /// * Send a comma character
                UART_send_char(S_str); /// * Send an 'S'
                itoa(log_buffer,(int)state,10); /// * Convert #state into a string and store it in #log_buffer
                UART_send_char(log_buffer[0]); /// * Send #log_buffer[0]
                if (state >= 10) UART_send_char(log_buffer[1]); /// * If #state is bigger than 10, send #log_buffer[0]   
                UART_send_char(comma); /// * Send a comma character
                UART_send_char(V_str); /// * Send a 'V'
                itoa(log_buffer,vp_buff,10); /// * Convert @p vp_buff into a string and store it in #log_buffer
                UART_send_char(log_buffer[0]); /// * Send #log_buffer[0]
                UART_send_char(log_buffer[1]); /// * Send #log_buffer[1]
                UART_send_char(log_buffer[2]); /// * Send #log_buffer[2]
                if (vp_buff >= 1000) UART_send_char(log_buffer[3]); /// * If @p vp_buff is bigger than 1000, send #log_buffer[3]
                UART_send_char(comma); ///* Send a comma character
                memset(log_buffer, '0', 8);  /// * Clear #log_buffer
                UART_send_char(I_str); /// * Send an 'I'
                itoa(log_buffer,ip_buff,10); /// * Convert @p ip_buff into a string and store it in #log_buffer
                UART_send_char(log_buffer[0]); /// * Send #log_buffer[0]
                UART_send_char(log_buffer[1]); /// * Send #log_buffer[1]
                if (ip_buff >= 100) UART_send_char(log_buffer[2]); /// * Send #log_buffer[2]
                if (ip_buff >= 1000) UART_send_char(log_buffer[3]); /// * If @p ip_buff is bigger or equal to 1000, send #log_buffer[3]
                UART_send_char(comma); ///* Send a comma character
                memset(log_buffer, '0', 8);  /// * Clear #log_buffer
                UART_send_char(T_str); /// * Send a 'T'
                itoa(log_buffer,tp_buff,10); /// * Convert @p tp_buff into a string and store it in #log_buffer
                UART_send_char(log_buffer[0]); /// * Send #log_buffer[0]
                UART_send_char(log_buffer[1]); /// * Send #log_buffer[1]
                UART_send_char(log_buffer[2]); /// * Send #log_buffer[1]
                if (tp_buff >= 1000) UART_send_char(log_buffer[3]);  // * If @p tp_buff is bigger or equal to 1000, send #log_buffer[3]
                UART_send_char(comma); ///* Send a comma character
                memset(log_buffer, '0', 8);  /// * Clear #log_buffer
                UART_send_char(Q_str); /// * Send a 'Q'
                utoa(log_buffer,qp_buff,10); /// * Convert @p qp_buff into a string and store it in #log_buffer
                UART_send_char(log_buffer[0]); /// * Send #log_buffer[0]
                if (qp_buff >= 10) UART_send_char(log_buffer[1]); /// * If @p qp_buff is bigger or equal to 10, send #log_buffer[1]
                if (qp_buff >= 100) UART_send_char(log_buffer[2]); /// * If @p qp_buff is bigger or equal to 100, send #log_buffer[2]
                if (qp_buff >= 1000) UART_send_char(log_buffer[3]); /// * If @p qp_buff is bigger or equal to 1000, send #log_buffer[3]
                if (qp_buff >= 10000) UART_send_char(log_buffer[4]); /// * If @p qp_buff is bigger or equal to 10000, send #log_buffer[4]
                UART_send_char('<'); /// * Send a '<'  
    }
    if (!log_on) RESET_TIME(); /// If #log_on is cleared, call #RESET_TIME()
}
/**@brief This function read the ADC and store the data in the coresponding variables
*/
void read_ADC()
{
    float opr = 0; /// Define @p opr to store the operations inside the function
    AD_SET_CHAN(V_CHAN); /// Select the #V_CHAN channel usign #AD_SET_CHAN(x)
    AD_CONVERT(); /// Make the conversion by calling #AD_CONVERT()
    AD_RESULT(); /// Store the result in #ad_res with #AD_RESULT()   
    opr = (float)(1.2207 * ad_res); /// Apply the operation @code opr = ad_res * [(Vref)/(2^12)] = ad_res * (5000/4096) @endcode
    v = opr; /// Make #v equal to @p opr
    AD_SET_CHAN(I_CHAN); /// Select the #I_CHAN channel usign #AD_SET_CHAN(x)
    AD_CONVERT(); /// Make the conversion by calling #AD_CONVERT()
    AD_RESULT(); /// Store the result in #ad_res with #AD_RESULT()
    opr = (float)(1.22412 * ad_res); /// Apply the operation @code opr = [(Vref)/(2^12)] * ad_res @endcode
    //i = opr;
    opr = opr - 2525; /// Apply the operation @code opr = opr - 2525 @endcode
    if (state == CHARGE | state == POSTCHARGE){
        opr = -opr; ///If the #state is #CHARGE or #POSTCHARGE change the sign of the result
    }
    i = (float)(opr * 2.5); /// Apply the operation @code opr = opr * 2.5 @endcode which is the sensitivity of the ACS723LL
    AD_SET_CHAN(T_CHAN); /// Select the #T_CHAN channel usign #AD_SET_CHAN(x)
    AD_CONVERT(); /// Make the conversion by calling #AD_CONVERT()
    AD_RESULT(); /// Store the result in #ad_res with #AD_RESULT()
    opr = (float)(1.22412 * ad_res); /// Apply the operation @code opr = [(Vref)/(2^12)] * ad_res @endcode
    opr = (float)(1866.3 - opr); /// Apply the operation @code opr = 1866.3 - opr @endcode. Sensor STLM20 Datasheet p.6
    t = (float) (opr/1.169); /// Apply the operation @code t = opr/1.169 @endcode. Sensor STLM20 Datasheet p.6
}
/**@brief This function is the PI control loop
*/
void control_loop()
{   
    if(!cmode) /// If #cmode is cleared then
    {
        pid(v, vref);  /// * The #pid() function is called with @p feedback = #v and @p setpoint = #vref
    }else /// Else,
    {
        pid(i, iref); /// * The #pid() function is called with @p feedback = #i and @p setpoint = #iref
    }
    set_DC(); /// The duty cycle is set by calling the #set_DC() function
}
/**@brief This function control the timing
*/
void timing()
{
    if(!count) /// If #count is other than zero, then
    {
        SECF = 1;
        count = COUNTER; /// * Make #count equal to #COUNTER
        if(second < 59) second++; /// * If #second is smaller than 59 then increase it
        else{second = 0; minute++;} /// * Else, make #second zero and increase #minute
    }else /// Else,
    {
        count--; /// * Decrease it
    }
}
/**@brief This function calculate the averages
*/
void calculate_avg()
{
    switch(count)
    {
        case COUNTER: /// If #count = #COUNTER
            iprom = 0; /// * Make #iprom zero
            vprom = 0; /// * Make #vprom zero
            tprom = 0; /// * Make #tprom zero
            break;
        case 0: /// If #count = 0
            iprom /= (COUNTER - 1); /// * Divide the value stored in #iprom between COUNTER to obtain the average
            vprom /= (COUNTER - 1); /// * Divide the value stored in #vprom between COUNTER to obtain the average
            tprom /= (COUNTER - 1); /// * Divide the value stored in #tprom between COUNTER to obtain the average
            if (iprom > 0) qprom += (iprom/3600); /// * Divide #iprom between 3600 and add it to #qprom to integrate the current over time
            else qprom += 0;
            #if (NI_MH_CHEM)  
            if ((int) vprom > vmax) vmax = (int) vprom; /// * If is the chemistry is Ni-MH and #vprom is bigger than #vmax then set #vmax = #vprom
            #endif
            break;
        default: /// If #count is not any of the previous cases then
            iprom += i; /// * Accumulate #i in #iprom
            vprom += v; /// * Accumulate #v in #vprom
            tprom += t; /// * Accumulate #t in #tprom
            //tprom += dc * 1.953125; // TEST FOR DC Is required to deactivate temperature protection
            break;
    }   
}
/**@brief This function activate the UART reception interruption 
*/
void interrupt_enable()
{
    char clear_buffer = 0; /// * Define the variable @p clear_buffer, used to empty the UART buffer
    while(RCIF){
        clear_buffer = RC1REG; /// * Clear the reception buffer and store it in @p clear_buffer
    }
    RCIE = 1; /// * Enable UART reception interrupts
    TXIE = 0; /// * Disable UART transmission interrupts
    TMR1IE = 1;   //enable T1 interrpt
    PEIE = 1;       //enable peripherals interrupts
    GIE = 1;        //enable global interrupts
    TMR1ON = 1;    //turn on timer
    TMR1IF = 0; //Clear timer1 interrupt flag
    count = 0; /// The timing counter #count will be intialized to zero, to start a full control loop cycle
}
/**@brief This function send one byte of data to UART
* @param bt character to be send
*/
void UART_send_char(char bt)  
{
    while(0 == TXIF)
    {
    }/// * Hold the program until the transmission buffer is free
    TX1REG = bt; /// * Load the transmission buffer with @p bt
}
/**@brief This function receive one byte of data from UART
* @return RC1REG reception register
*/
char UART_get_char()
{
    if(OERR) /// If there is error
    {
        CREN = 0; /// * Clear the error
        CREN = 1; /// * Restart
    }    
    while(!RCIF);  /// Hold the program until the reception buffer is free   
    return RC1REG; /// Receive the value and return it
}
/**@brief This function send a string using UART
* @param st_pt pointer to string to be send
*/
void UART_send_string(char* st_pt)
{
    while(*st_pt) /// While there is a byte to send
        UART_send_char(*st_pt++); /// * Send it usign #UART_send_char() and then increase the pointer possition
}
/**@brief This function convert a number to string and then send it using UART
* @param value integer to be send
*/
void display_value(int value)
{   
    char buffer[6]; /// * Define @p buffer to used it for store character storage
    itoa(buffer,value,10);  /// * Convert @p value into a string and store it in @p buffer
    UART_send_string((char*)buffer); /// * Send @p buffer using #UART_send_string()
}

void temp_protection()
{
    if (conv && tprom > 350){
        UART_send_string((char*)"HIGH_TEMP");
        STOP_CONVERTER(); /// -# Stop the converter by calling the #STOP_CONVERTER() macro.
        state = STANDBY; /// -# Go to the #STANDBY state.
    }
}
/**@brief This function activate the desired relay in the switcher board according to the value
* of #cell_count
*/
void Cell_ON()
{
    if (cell_count == '1') /// If cell_count = '1'
    {
        CELL1_ON(); /// * Turn ON cell #1 by calling #CELL1_ON
        __delay_ms(10);
        CELL2_OFF(); /// * Turn OFF cell #2 by calling #CELL2_OFF
        __delay_ms(10);        
        CELL3_OFF(); /// * Turn OFF cell #3 by calling #CELL3_OFF
        __delay_ms(10);
        CELL4_OFF(); /// * Turn OFF cell #4 by calling #CELL4_OFF
        __delay_ms(10);
    }else if (cell_count == '2')
    {
        CELL1_OFF(); /// * Turn OFF cell #1 by calling #CELL1_OFF
        __delay_ms(10);
        CELL2_ON(); /// * Turn ON cell #2 by calling #CELL2_ON
        __delay_ms(10);
        CELL3_OFF(); /// * Turn OFF cell #3 by calling #CELL3_OFF
        __delay_ms(10);
        CELL4_OFF(); /// * Turn OFF cell #4 by calling #CELL4_OFF
        __delay_ms(10);
    }else if (cell_count == '3')
    {
        CELL1_OFF(); /// * Turn OFF cell #1 by calling #CELL1_OFF
        __delay_ms(10);
        CELL2_OFF(); /// * Turn OFF cell #2 by calling #CELL2_OFF
        __delay_ms(10);
        CELL3_ON(); /// * Turn ON cell #3 by calling #CELL3_ON
        __delay_ms(10);
        CELL4_OFF(); /// * Turn OFF cell #4 by calling #CELL4_OFF    
    }else if (cell_count == '4')
    {
        CELL1_OFF(); /// * Turn OFF cell #1 by calling #CELL1_OFF
        __delay_ms(10);
        CELL2_OFF(); /// * Turn OFF cell #2 by calling #CELL2_OFF
        __delay_ms(10);
        CELL3_OFF(); /// * Turn OFF cell #3 by calling #CELL3_OFF
        __delay_ms(10);
        CELL4_ON(); /// * Turn ON cell #4 by calling #CELL4_ON
        __delay_ms(10);
    }
}
/**@brief This function deactivate all relays in the switcher board
*/
void Cell_OFF()
{
    CELL1_OFF(); /// * Turn OFF cell #1 by calling #CELL1_OFF
    __delay_ms(10);
    CELL2_OFF(); /// * Turn OFF cell #2 by calling #CELL2_OFF
    __delay_ms(10);
    CELL3_OFF(); /// * Turn OFF cell #3 by calling #CELL3_OFF
    __delay_ms(10);
    CELL4_OFF(); /// * Turn OFF cell #4 by calling #CELL4_OFF  
    __delay_ms(10);
}