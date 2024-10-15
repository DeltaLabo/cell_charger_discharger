/**
 * @file charger_discharger.c
 * @author Juan J. Rojas
 * @date 7 Aug 2018
 * @brief Hardware related functions source file.
 * @par Institution:
 * LaSEINE / CeNT. Kyushu Institute of Technology.
 * @par Mail (after leaving Kyutech):
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://bitbucket.org/juanjorojash/cell_charger_discharger
 */

#include "charger_discharger.h"

/**@brief Function to initialize all the PIC16F1787 registers
*/
void initialize()
{
    /** <b> GENERAL </b>*/
    CLRWDT(); /// * Clear WDT by calling @p CLRWDT()
    nWPUEN = 0; /// * Allow change of individual WPU
    WPUE3 = 1;      //Enable pull up for MCLR
    /** <b> SYSTEM CLOCK </b>*/
    OSCCONbits.IRCF = 0b1111; /// * Set clock to 32MHz (PLL is enabled)
    OSCCONbits.SCS = 0b00; /// * Clock determined by FOSC<2:0> in Configuration Words
    OSCCONbits.SPLLEN = 1; /// * Enable PLL. 
    //According to errata this shall not be done in the Configuration Words
    /** <b> RELAY OUPUTS </b> */
    //PORTC doesn't have ANSELC
    TRISC3 = 0; /// * Set RC3 as output. Set for discharge
    WPUC3 = 0; /// * Deactivate weak pull up in RC3
    TRISC4 = 0; /// * Set RC4 as output. Set for charge
    WPUC4 = 0; /// * Deactivate weak pull up in RC4    
    TRISC5 = 0; /// * Set RC5 as output. ON/OFF relay
    WPUC5 = 0; /// * Deactivate weak pull up in RC5
    /** <b> CELL SWITCHER OUPUTS </b>*/
    TRISB2 = 0; /// * Set RB2 as output. Set for Cell #1
    ANSB2 = 0; /// * Set RB2 as digital
    WPUB2 = 0; /// * Deactivate weak pull up in RB2
    TRISB3 = 0; /// * Set RB3 as output. Set for Cell #2
    ANSB3 = 0; /// * Set RB3 as digital
    WPUB3 = 0; /// * Deactivate weak pull up in RB3
    TRISB4 = 0; /// * Set RB4 as output. Set for Cell #3
    ANSB4 = 0; /// * Set RB4 as digital  
    WPUB4 = 0; /// * Deactivate weak pull up in RB4
    TRISB5 = 0; /// * Set RB5 as output. Set for Cell #4
    ANSB5 = 0; /// * Set RB5 as digital  
    WPUB5 = 0; /// * Deactivate weak pull up in RB5
    Cell_OFF();
    /** @b TIMER1 */
    nT1SYNC = 0; /// * Synchronize asynchronous clock input with system clock (FOSC)
    T1OSCEN = 0; /// * Dedicated Timer1 oscillator circuit disabled    
    TMR1ON = 0; /// * Enables Timer1
    TMR1GE = 0; /// * Timer1 counts regardless of Timer1 gate function
    TMR1CS0 = 0; // TMR1CS=0b00      
    TMR1CS1 = 0; /// * Timer1 clock source is instruction clock (FOSC/4)
    T1CKPS0 = 0; // T1CKPS=0b00  
    T1CKPS1 = 0; /// * 1:1 Prescale value
    TMR1H = 0xE1; //TMR1 Fosc/4= 8Mhz (Tosc= 0.125us). TMR1 counts: 7805 x 0.125us = 0.97562 ms
    TMR1L = 0x83; /// * Set Timer1 register to overflow in 0.97562 ms
    /** <b> PROGRAMMABLE SWITCH MODE CONTROL (PSMC) </b> */
    PSMC1CON = 0x00; /// * Clear PSMC1 configuration
    PSMC1MDL = 0x00; /// * No modulation
    PSMC1CLK = 0x01; /// * Driven by 64MHz PLL system clock
    PSMC1PRH = 0x01; //
    PSMC1PRL = 0xFF; /// * Set period register to overflow in 8us (125kHz)
    // 511 + 1 clock cycles for period that is 8us (125kHz)
    // This set the PWM with 9 bit of resolution
    PSMC1DCH = 0x00;    //   
    PSMC1DCL = 0x32;    // * Set duty cycle register to a 10% duty cycle (0x0032)
    PSMC1PHH = 0x00; 
    PSMC1PHL = 0x00; /// * Rising event starts from the beginning, i.e duty cycle is in-phase with the period
    P1STRC = 1; /// * Single PWM activated in PSMC1C (RC2)
    P1POLC = 0; /// * PWM PSMC1 output is active-high (RC2)
    P1OEC = 1; /// * PSMC output is active in PSMC1C (RC2)
    P1PRST = 1; /// * Period event occurs when PSMC1TMR = PSMC1PR
    P1PHST = 1; /// * Rising edge event occurs when PSMC1TMR = PSMC1PH
    P1DCST = 1; /// * Falling edge event occurs when PSMC1TMR = PSMC1DC
    PSMC1CON = 0x80; /// * PSMC1 module is enable
    /// * Disable buffer load
    /// * Disable dead band
    /// * Operating mode: Single PWM
    TRISC2 = 0; /// * Set RC2 as output. PWM output
    WPUC2 = 0; /// * Deactivate weak pull up in RC2    
    /** @b ADC*/
    TRISA3 = 1; /// * Set RA3 as input. VREF+
    ANSA3 = 1; /// * Set RA3 as analog
    WPUA3 = 0; /// * Deactivate weak pull up in RA3
    TRISB1 = 1; /// * Set RB1 as input. Voltage sense
    ANSB1 = 1; /// * Set RB1 as analog
    WPUB1 = 0; /// * Deactivate weak pull up in RB1
    TRISB0 = 1; /// * Set RB0 as input. Current sense
    ANSB0 = 1; /// * Set RB0 as analog
    WPUB0 = 0; /// * Deactivate weak pull up in RB0
    TRISA5 = 1; /// * Set RA5 as input. Temperature sense
    ANSA5 = 1; /// * Set RA5 as analog
    WPUA5 = 0; /// * Deactivate weak pull up in RA5
    ADCON0bits.ADRMD = 0; /// * 12-bit result
    ADCON1bits.ADCS = 0b010; /// * Clock selected as FOSC/32
    ADCON1bits.ADNREF = 0; /// * Negative reference connected to VSS
    ADCON1bits.ADPREF = 0b01; /// * Positive reference connected to VREF+
    ADCON1bits.ADFM = 1; /// * 2's compliment result
    ADCON2bits.CHSN = 0b1111; /// * Negative differential input given by ADNREF
    ADCON0bits.ADON = 1; /// * ADC is enabled
    /** @b UART*/
    TXSEL = 0; /// * RC6 selected as TX
    RXSEL = 0; /// * RC7 selected as RX
    SP1BRGH = 0x00; 
    SP1BRGL = 0x8A; // * Baud rate register set to 57600 bps    
    BRGH  = 1;  /// * High baud rate set
    BRG16 = 1;  /// * 16-bit timer set
    SYNC  = 0;  /// * Asynchronous serial
    SPEN  = 1;  /// * Enable serial port pins
    TXEN  = 1;  /// * Enable transmission
    CREN  = 1;  /// * Enable reception
    TX9   = 0;  /// * 8-bit transmission selected
    RX9   = 0;  /// * 8-bit reception selected
    RCIE = 0; /// * Disable UART reception interrupts
    TXIE = 0; /// * Disable UART transmission interrupts
    /** @b FINAL */
    SET_DISC();
    __delay_ms(100);
    STOP_CONVERTER();
}
bool command_interpreter()
{
    bool test = true;
    uint8_t operation = 0x00;
    uint8_t code = 0x00;
    uint8_t length = 0x00;
    uint8_t data[20] = {0x00};
    uint16_t checksum = 0x0000;
    uint16_t calc_checksum = 0x0000;
    basic_configuration_ptr = &basic_configuration;
    test_configuration_ptr = &test_configuration;
    converter_configuration_ptr = &converter_configuration;
    if (!start)
    {
        if(UART_get_byte()==0xDD)
        {
            operation = UART_get_byte();
            code = UART_get_byte();
            length = UART_get_byte();
            if (length>0) UART_get_some_bytes(length, (uint8_t*)data);
            checksum = UART_get_byte();
            checksum += UART_get_byte()* 256;
            calc_checksum = calculate_checksum(code, length, (uint8_t*)data);
            if(UART_get_byte() != 0x77)
            {
                test = false;
            }
            if (checksum != calc_checksum)
            {
                test = false;
            }        
            if(!start)
            {
                switch (operation)
                {
                    case 0xA5:
                        UART_send_header(0xDD, operation, code);
                        switch (code)
                        {
                            case 0x03:
                                length = sizeof(basic_configuration);
                                UART_send_byte(length);
                                UART_send_some_bytes(length, (uint8_t*)basic_configuration_ptr);
                                calc_checksum = calculate_checksum(code, length, (uint8_t*)basic_configuration_ptr);
                                break;
                            case 0x05:
                                length = sizeof(test_configuration);
                                UART_send_byte(length);
                                UART_send_some_bytes(length, (uint8_t*)test_configuration_ptr);
                                calc_checksum = calculate_checksum(code, length, (uint8_t*)test_configuration_ptr);
                                break;
                            case 0x07:
                                length = sizeof(converter_configuration);
                                UART_send_byte(length);
                                UART_send_some_bytes(length, (uint8_t*)converter_configuration_ptr);
                                calc_checksum = calculate_checksum(code, length, (uint8_t*)converter_configuration_ptr);
                                break;
                        }
                        UART_send_some_bytes(2,(uint8_t*)&calc_checksum);
                        UART_send_byte(0x77);                
                        break;   
                    case 0x5A:
                        switch (code)
                        {
                            case 0x03: //BASIC CONFIGURATION
                                put_data_into_structure(length, (uint8_t*)data, (uint8_t*)basic_configuration_ptr);
                                vref = ( ( (float) basic_configuration.const_voltage * 4096.0 ) / 5000.0 ) + 0.5 ; //Scale the voltage reference to be compare with v;
                                i_char = (uint16_t) ( ( ( (float) basic_configuration.const_current_char * 4096.0 ) / (5000.0 * 2.5 ) ) + 0.5 );
                                i_disc = (uint16_t) ( ( ( (float) basic_configuration.const_current_disc * 4096.0 ) / (5000.0 * 2.5 ) ) + 0.5 );
                                capacity = basic_configuration.capacity;
                                EOC_variable = basic_configuration.end_of_charge;
                                EOPC_variable = basic_configuration.end_of_precharge;
                                EOD_voltage = basic_configuration.end_of_discharge;
                                EOPD_capacity = basic_configuration.end_of_postdischarge;
                                break;
                            case 0x05: // TEST CONFIGURATION
                                put_data_into_structure(length, (uint8_t*)data, (uint8_t*)test_configuration_ptr);
                                break;
                            case 0x07: // CONVERTER CONFIGURATION
                                put_data_into_structure(length, (uint8_t*)data, (uint8_t*)converter_configuration_ptr);
                                CV_kp = (float) ((converter_configuration.CVKp) / 1000000.0);
                                CV_ki = (float) ((converter_configuration.CVKi) / 1000000.0);
                                CV_kd = (float) ((converter_configuration.CVKd) / 1000.0);
                                CC_char_kp = (float) ((converter_configuration.CCKpC) / 1000000.0);
                                CC_char_ki = (float) ((converter_configuration.CCKiC) / 1000000.0);
                                CC_disc_kp = (float) ((converter_configuration.CCKpD) / 1000000.0);
                                CC_disc_ki = (float) ((converter_configuration.CCKiD) / 1000000.0);
                                break;
                        }
                        break;
                    case 0x0F:  // START CONVERTER
                        if (code == 0x05)
                        {
                            counter_state = 0;
                            state = test_configuration.order_of_states[counter_state];
                            cell_count = 0x01;
                            repetition_counter = 0x01;
                            converter_settings();
                            start = true;
                        }
                        break;
                }
            }
        }else test = false;
    }else 
    {
        code = UART_get_byte();
        switch (code)
        {
            case 0x03: // RESET
                state = IDLE;
                break;
            case 0x07: // NEXT CELL
                counter_state = test_configuration.number_of_states + 1;
                wait_count = getTime();
                state = WAIT;
                break;
            case 0x09: // NEXT STATE
                wait_count = getTime();
                state = WAIT;
                break;
            default:
                test = false;
                break;
        }
    }
    return (test);
}

/**@brief This function calls the PI control loop for current or voltage depending on the value of the #cmode variable.
*/
void control_loop()
{   
    if(!cmode) /// If #cmode is cleared then
    {
        pid(v, vref);/// * The #pid() function is called with @p feedback = #v and @p setpoint = #vref
    }else /// Else,
    {
        pid(i, iref); /// * The #pid() function is called with @p feedback = #i and @p setpoint = #iref
    }
    set_DC();
}

/**@brief This function defines the PI controller
*  @param   feedback average of measured values for the control variable
*  @param   setpoint desire controlled output for the variable
*/
void pid(float feedback, float setpoint)
{  
    pidt += kd * (setpoint - feedback - er); /// * Calculate #diferential component of compensator
    
    er = setpoint - feedback; /// * Calculate the error by substract the @p feedback from the @p setpoint and store it in @p er
    if(er > ERR_MAX) er = ERR_MAX; /// * Make sure error is never above #ERR_MAX
    if(er < ERR_MIN) er = ERR_MIN; /// * Make sure error is never below #ERR_MIN
    
    
	pidi += (ki * er); /// * Calculate #integral component of compensator
    pidt += (er * kp + pidi); /// * Calculate #proportional component of compensator
    
    if (pidt >= DC_MAX) pidt = DC_MAX;
    else if (pidt <= DC_MIN) pidt = DC_MIN;
}
/**@brief This function sets the desired duty cycle
*/
void set_DC()
{
/// This function can set the duty cycle from 0x0 to 0x1FF
    uint16_t    dc = (uint16_t) pidt;
    
    PSMC1DCL = dc & 0x00FF; /// * Lower 8 bits of #dc are stored in @p PSMC1DCL
    PSMC1DCH = (dc >> 8) & 0x01; /// * Higher 1 bit of #dc are stored in @p PSMC1DCH
    PSMC1CONbits.PSMC1LD = 1; /// * Set the load register. This will load all the setting as once*/
}

/**@brief This function switches between CC and CV mode.
* @param current_voltage average of current voltage
* @param referece_voltage voltage setpoint
* @param CC_mode_status current condition of #cmode variable
*/
void cc_cv_mode(uint16_t current_voltage, uint16_t reference_voltage, bool CC_mode_status)
{
/// If the current voltage is bigger than the CV setpoint and the system is in CC mode, then:
    if( ( ( (uint16_t) ( ( ( (float)current_voltage * 5000.0 ) / 4096.0 ) + 0.5 ) ) > reference_voltage ) && CC_mode_status )
    {        
        pidi = 0;       /// <ol> <li> The integral acummulator is cleared
        cmode = 0;      /// <li> The system is set in CV mode by clearing the #cmode variable
        kp = CV_kp;     /// <li> The proportional constant is set to #CV_kp 
        ki = CV_ki;     /// <li> The integral constant is set to #CV_ki
        kd = CV_kd;     /// <li> The derivative constant is set to #CV_kd
    }    
}
/**@brief This function takes care of scaling the average values to correspond with their real values.
*/
void scaling() /// This function performs the folowing tasks:
{
    log_data.current = (uint16_t) ( ( ( (float)iavg * 2.5 * 5000.0 ) / 4096.0 ) + 0.5 ); /// <ol><li> Scale #iavg according to the 12-bit ADC resolution (4096) and the sensitivity of the sensor (0.4 V/A). 
    log_data.voltage = (uint16_t) ( ( ( (float)vavg * 5000.0 ) / 4096.0 ) + 0.5 ); /// <li> Scale #vavg according to the 12-bit ADC resolution (4096)
    qavg += (float)( ( ( (float)iavg * 2.5 * 5000.0 ) / 4096.0 ) + 0.5 ) / 3600.0; /// <li> Perform the discrete integration of #iavg over one second and accumulate in #qavg 
    log_data.capacity = (uint16_t) (qavg);
    if (basic_configuration.version == 2)
    {
        if (vavg > vmax)
        {
            (vmax = vavg); /// <li> If the chemistry is Ni-MH and #vavg is bigger than #vmax then set #vmax equal to #vavg
        }
    } 
}
/**@brief This function takes care of calculating the average values printing the log data using the UART.
*/
void log_control()
{
    if(start)
    {
        log_data_ptr = &log_data;
        log_data.cell_counter = cell_count;
        log_data.state = state;
        log_data.repetition_counter = repetition_counter;
        log_data.elapsed_time = second;
        log_data.duty_cycle = (uint16_t) pidt;
        UART_send_byte(0xDD);
        UART_send_some_bytes(sizeof(log_data),(uint8_t*)log_data_ptr);
        UART_send_byte(0x77);
    }else second = 0;
}

/**@brief This function read the ADC and store the data in the coresponding variable
*/
uint16_t read_ADC(uint16_t channel)
{
    uint16_t ad_res = 0;
    __delay_us(10);
    ADCON0bits.CHS = channel;
    __delay_us(10);
    GO_nDONE = 1;
    while(GO_nDONE);
    ad_res = (uint16_t)((ADRESL & 0xFF)|((ADRESH << 8) & 0xF00));
    return ad_res;
}

/**@brief This function control the timing
*/
void timing()
{
    if(!count) /// If #count is other than zero, then
    {
        SECF = 1;
        count = COUNTER; /// * Make #count equal to #COUNTER
        second++; /// * always increase second, no more minutes
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
            iacum = (uint24_t) i; /// * Make #iavg zero
            vacum = (uint24_t) v; /// * Make #vavg zero
            break;
        case 0: /// If #count = 0
            iavg = ((iacum >> 10) + ((iacum >> 9) & 0x01)); /// * Divide the value stored in #iavg between COUNTER to obtain the average   
            vavg = ((vacum >> 10) + ((vacum >> 9) & 0x01)); /// * This is equivalent to vacum / 1024 = vacum / 2^10 
            break;
        default: /// If #count is not any of the previous cases then
            iacum += (uint24_t) i; /// * Accumulate #i in #iavg
            vacum += (uint24_t) v; /// * Accumulate #v in #vavg
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
    RCIE = 1;           /// * Enable UART reception interrupts
    TXIE = 0;           /// * Disable UART transmission interrupts
    TMR1IE = 1;         //enable T1 interrupt
    PEIE = 1;           //enable peripherals interrupts
    GIE = 1;            //enable global interrupts
    count = COUNTER;    /// The timing counter #count will be initialized to zero, to start a full control loop cycle
    TMR1IF = 0;         //Clear timer1 interrupt flag
    TMR1ON = 1;         //turn on timer 
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

void UART_send_header(uint8_t start, uint8_t operation, uint8_t code)
{
    UART_send_byte(start);
    UART_send_byte(operation);
    UART_send_byte(code);
}

/**@brief This function receive one byte of data from UART
* @return RC1REG reception register
*/
uint8_t UART_get_byte()
{
    if(OERR) /// If there is error
    {
        CREN = 0; /// * Clear the error
        CREN = 1; /// * Restart
    }    
    while(!RCIF);  /// Hold the program until the reception buffer is free   
    return RC1REG; /// Receive the value and return it
}

void UART_get_some_bytes(uint8_t length, uint8_t* data)
{
    if(OERR) /// If there is error
    {
        CREN = 0; /// * Clear the error
        CREN = 1; /// * Restart
    }    
    while(length--)
    {
        *data++ = UART_get_byte(); /// * Get a byte      
    }
}

void UART_send_byte(uint8_t byte)  
{
    while(0 == TXIF)
    {
    }/// * Hold the program until the transmission buffer is free
    TX1REG = byte; /// * Load the transmission buffer with @p bt
}

void UART_send_some_bytes(uint8_t length, uint8_t* data)
{
    while(length--)
    {
        UART_send_byte(*data++); /// * send a byte      
    }
}

uint16_t calculate_checksum(uint8_t code, uint8_t length, uint8_t* data)
{
    uint16_t result = 0x00;
    result = (uint16_t)code + (uint16_t)length;
    while(length--)
    {
        result += *data++;
    }
    return (result);
}

void put_data_into_structure(uint8_t length, uint8_t* data, uint8_t* structure)
{
    while(length--)
    {
        *structure++ = *data++;
    }
}

/**@brief This function send a string using UART
* @param st_pt pointer to string to be send
*/
void UART_send_string(char* st_pt)
{
    while(*st_pt) /// While there is a byte to send
        UART_send_char(*st_pt++); /// * Send it using #UART_send_char() and then increase the pointer possition
}

/**@brief This function activate the desired relay in the switcher board according to the value
* of #cell_count
*/
void Cell_ON()
{
    if (cell_count == 1) /// If cell_count = '1'
    {
        CELL1_ON(); /// * Turn ON cell #1 by calling #CELL1_ON
        __delay_ms(10);
        CELL2_OFF(); /// * Turn OFF cell #2 by calling #CELL2_OFF
        __delay_ms(10);        
        CELL3_OFF(); /// * Turn OFF cell #3 by calling #CELL3_OFF
        __delay_ms(10);
        CELL4_OFF(); /// * Turn OFF cell #4 by calling #CELL4_OFF
        __delay_ms(10);
    }else if (cell_count == 2)
    {
        CELL1_OFF(); /// * Turn OFF cell #1 by calling #CELL1_OFF
        __delay_ms(10);
        CELL2_ON(); /// * Turn ON cell #2 by calling #CELL2_ON
        __delay_ms(10);
        CELL3_OFF(); /// * Turn OFF cell #3 by calling #CELL3_OFF
        __delay_ms(10);
        CELL4_OFF(); /// * Turn OFF cell #4 by calling #CELL4_OFF
        __delay_ms(10);
    }else if (cell_count == 3)
    {
        CELL1_OFF(); /// * Turn OFF cell #1 by calling #CELL1_OFF
        __delay_ms(10);
        CELL2_OFF(); /// * Turn OFF cell #2 by calling #CELL2_OFF
        __delay_ms(10);
        CELL3_ON(); /// * Turn ON cell #3 by calling #CELL3_ON
        __delay_ms(10);
        CELL4_OFF(); /// * Turn OFF cell #4 by calling #CELL4_OFF    
    }else if (cell_count == 4)
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

uint8_t getTime(){
    if ((counter_state + 1 <= test_configuration.number_of_states) && (test_configuration.order_of_states[counter_state + 1] != 0x00)){
        return test_configuration.wait_time;
    }
    else if (cell_count < test_configuration.number_of_cells){
        return test_configuration.wait_time;
    }
    else{
        return test_configuration.end_wait_time;
    }
}