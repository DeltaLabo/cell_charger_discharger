/**
 * @file hardware.c
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

#include "hardware.h"
#include "state_machine.h"

void Init_general()
{
	CLRWDT();
	STOP_CONVERTER();
    TMR0IF = 0;                        //Clear timer 0
	ad_res = 0;                        //Clear ADC result variable
	cmode = 1;                         //Start in CC mode
	iref = 0;                  
	vref = 0;
    state = STANDBY;       
    count = COUNTER; 
    iprom = 0;
    vprom = 0;
    tprom = 0;
    wait_count = 0;
    dc_res_count = 0;
    //esc = 0;     
}

void Init_registers()
{
    //-----------------------GENERAL-------------------------------------------
    nWPUEN = 0;           //Allow change of individual WPU
    //-----------------------SYSTEM CLOCK--------------------------------------
    //PLL is always enabled because of configuration bits.
    OSCCONbits.IRCF = 0b1111;           //Set clock to 32MHz (with PLL)
    OSCCONbits.SCS = 0b00;              //Clear to use the result of IRCF
    OSCCONbits.SPLLEN = 1;              //Enable PLL, it gives a problem if is done in the CONFWords
    //System clock set as 32MHz
    //--------------------OUPUTS FOR RELAYS------------------------------------
    TRISA0 = 0;                         //Set RA0 as output. C/D relay
    ANSA0 = 0;                          //Digital
    TRISA1 = 0;                         //Set RA3 as output.ON/OFF relay
    ANSA1 = 0;                          //Digital   
//    //----------------OUTPUTS FOR CELL SWITCHER------------------------------
//    TRISAbits.TRISA7 = 0;               //Set RA7 as output. Cell #1
//    ANSELAbits.ANSA7 = 0;               //Digital
//    TRISAbits.TRISA6 = 0;               //Set RA6 as output. Cell #2
//    ANSELAbits.ANSA6 = 0;               //Digital   //DOES NOT EXIST
//    TRISCbits.TRISC0 = 0;               //Set RC0 as output. Cell #3
//    ANSELCbits.ANSC0 = 0;               //Digital   //DOES NOT EXIST
//    TRISCbits.TRISC1 = 0;               //Set RC1 as output. Cell #4
//    ANSELCbits.ANSC1 = 0;               //Digital   //DOES NOT EXIST
    //-----------TIMER0 FOR CONTROL AND MEASURING LOOP-------------------------
    TMR0IE = 0;                         //Disable timer interruptions
    TMR0CS = 0;                         //Timer set to internal instruction cycle
    OPTION_REGbits.PS = 0b110;          //Prescaler set to 128
    OPTION_REGbits.PSA = 0;             //Prescaler activated
    TMR0IF = 0;                         //Clear timer flag
    TMR0 = 0x07;                        //Counter set to 255 - 250 + 2 (delay for sync) = 7
    //Timer set to 32/4/128/250 = 250Hz
    
    //---------------------PSMC/PWM SETTING------------------------------------
    TRISA4 = 1;                         //[Temporary]Set RA4 as input to let it drive from RB3. 
    WPUA4 = 0;                          //Disable WPU for RA4.  
    WPUC0 = 0;                          //Disable WPU for RC0.  //SEE IF THIS SOLVE THE ERROR
   
    PSMC1CON = 0x00;                    //Clear configuration to start 
    PSMC1MDL = 0x00;                    //No modulation
    PSMC1CLK = 0x01;                    //Driven by 64MHz system clock
    //PSMC1CLKbits.P1CSRC = 0b01;         //Driven by 64MHz system clock
    //PSMC1CLKbits.P1CPRE = 0b00;         //No prescaler (64MHz)
    //Period
    PSMC1PRH = 0x01;                    //0x01
    PSMC1PRL = 0xFF;                    //0xFF
    //511 + 1 clock cycles for period that is 8us (125KHz)
    //This set the PWM with 9 bit of resolution
    //Duty cycle
    PSMC1DCH = 0x00;                    //Duty cycle starts in 0   
    PSMC1DCL = 0x00;                    //Duty cycle starts in 0   
    //Phase or rising event
    PSMC1PHH = 0x00;                    //Rising event starts from the beginning
    PSMC1PHL = 0x00;                    //Rising event starts from the beginning
    
    PSMC1STR0bits.P1STRA = 1;           //Single PWM activated in PSMC1A (RCO))
    PSMC1POLbits.P1POLA = 0;            //Active high
    PSMC1OENbits.P1OEA = 1;             //PSMC1A activated in PSMC1A (RC0)
    
    PSMC1PRSbits.P1PRST = 1;            //Period event occurs when PSMC1TMR = PSMC1PR
    PSMC1PHSbits.P1PHST = 1;            //Rising edge event occurs when PSMC1TMR = PSMC1PH
    PSMC1DCSbits.P1DCST = 1;            //Falling edge event occurs when PSMC1TMR = PSMC1DC
    
    PSMC1CON = 0x80;                    //Enable|Load Buffer|Dead band disabled|Single PWM
    //PSMC1TIE = 1;                       //Enable interrupts for Time Based 
    TRISC0 = 0;                         //Set RC0 as output
    
    //---------------------ADC SETTINGS----------------------------------------
   
    //ADC INPUTS//check this after final design
    TRISA3 = 1;                         //RA3, Positive voltage reference 
    ANSA3 = 0;                          //RA3 analog
    WPUA3 = 0;                          //Weak pull up Deactivated
    TRISB4 = 1;                         //RB4, current sensing input    
    ANSB4 = 1;                          //RB4 analog      
    WPUB4 = 0;                          //Weak pull up Deactivated
    TRISB5 = 1;                         //RB5, voltage sensing input
    ANSB5 = 1;                          //RB5 analog
    WPUB5 = 0;                          //Weak pull up Deactivated
    //TEMP SENSING IS MISSING
    ADCON0bits.ADRMD = 0;               //12 bits result
    ADCON1bits.ADCS = 0b010;            //Clock selected as FOSC/32
    ADCON1bits.ADNREF = 0;              //Connected to Vss
    ADCON1bits.ADPREF = 0b00;           //Connected to VDD, change after to Connected to Vref+ (01)
    ADCON1bits.ADFM = 1;                //2's compliment result
    ADCON2bits.CHSN = 0b1111;           //Negative differential input as ADNREF
    ADCON0bits.ADON = 1;                //Turn on the ADC

    //---------------------INTERRUPTS----------------------------------------
    PEIE = 1;                           //Activate pehierals Interrupts
    GIE = 1;                            //Activate Global Interrupts
}

void pid(float feedback, unsigned int setpoint)
{
float 	er;
float   pi;
	er = setpoint - feedback;

	if(er > ERR_MAX) er = ERR_MAX;
	if(er < ERR_MIN) er = ERR_MIN;
    
	proportional = (kp * er);
	integral += (ki * er)/COUNTER; //time base is 0.5Khz 

	pi = proportional + integral; 
    
    if (dc + pi >= DC_MAX){
        dc = DC_MAX;
    }else if (dc + pi <= DC_MIN){
        dc = DC_MIN;
    }else{
        dc += (int)(pi + 0.5); //This is the point in which a mix the PWM with the PID
    }   
}

void set_DC()
{
    PSMC1DCL = dc & 0x00FF;
    PSMC1DCH = (dc >> 8) & 0x01;
    PSMC1CONbits.PSMC1LD = 1; //Load Buffer
}

void cc_cv_mode(float current_voltage, unsigned int reference_voltage, char CC_mode_status)
{
    if(current_voltage > reference_voltage && CC_mode_status == 1)
    {        
//        if (!CV_count)
//        {
            proportional = 0;
            integral = 0;
//            if (cmode){
//                ki = 0.001;
//                kp = 0.1;
//            }               
            cmode = 0;
            kp = 0.4;  //0.4 with 0.3 produces a very good regulation at the end
            ki = 0.5;
            //if (ki < 0.04) ki = ki + 0.001;
//        }else CV_count--;
    }     
}

void log_control()
{  
    if (log_on)
    {
        switch (count){
            case 0:
                ip_buff = (int) iprom;
                vp_buff = (int) vprom; 
                tp_buff = (int) tprom;  
                qp_buff = (unsigned) ((qprom * 10) + 0.05);
                LINEBREAK;
                break;
            case COUNTER:
                itoa(log_buffer,minute,10);
                break;
            case COUNTER - 1:
                if (minute < 10) UART_send_char('0');
                else UART_send_char(log_buffer[0]);
                break;
            case COUNTER - 2:
                if (minute < 10) UART_send_char(log_buffer[0]);
                else UART_send_char(log_buffer[1]);
                break;
            case COUNTER - 3:
                UART_send_char(colons);
                break;
            case COUNTER - 4:
                memset(log_buffer, '0', 8);
                break;
            case COUNTER - 5:
                itoa(log_buffer,second,10);
                break;
            case COUNTER - 6:
                if (second < 10) UART_send_char('0');
                else UART_send_char(log_buffer[0]);
                break;
            case COUNTER - 7:
                if (second < 10) UART_send_char(log_buffer[0]);
                else UART_send_char(log_buffer[1]);
                break;
            case COUNTER - 8:
                UART_send_char(comma);
                break;
            case COUNTER - 9:
                memset(log_buffer, '0', 8);
                break;   
            case COUNTER - 10:    
                UART_send_char(C_str);
                break;
            case COUNTER - 11:
                UART_send_char(cell_count);
                break;
            case COUNTER - 12:
                UART_send_char(comma);
                break;
            case COUNTER - 13:
                UART_send_char(S_str);
                break;
            case COUNTER - 14:
                itoa(log_buffer,(int)state,10);
                break;
            case COUNTER - 16:
                UART_send_char(log_buffer[0]);
                break;
            case COUNTER - 17:
                if (state >= 10) UART_send_char(log_buffer[1]);
                break;   
            case COUNTER - 18:
                UART_send_char(comma);
                break;
            case COUNTER - 19:
                UART_send_char(V_str);
                break;
            case COUNTER - 20:
                itoa(log_buffer,vp_buff,10);
                break;
            case COUNTER - 21:
                UART_send_char(log_buffer[0]);
                break;
            case COUNTER - 22:
                UART_send_char(log_buffer[1]);
                break;
            case COUNTER - 23:
                UART_send_char(log_buffer[2]);
                break;
            case COUNTER - 24:
                if (vp_buff >= 1000) UART_send_char(log_buffer[3]);
                break;
            case COUNTER - 25:
                UART_send_char(comma);
                break;
            case COUNTER - 26:
                memset(log_buffer, '0', 8);
                break;
            case COUNTER - 27:
                UART_send_char(I_str);
                break;
            case COUNTER - 28:
                itoa(log_buffer,ip_buff,10);
                break;
            case COUNTER - 29:
                UART_send_char(log_buffer[0]);
                break;
            case COUNTER - 30:
                UART_send_char(log_buffer[1]);
                break;
            case COUNTER - 31:
                UART_send_char(log_buffer[2]);
                break;
            case COUNTER - 32:
                if (ip_buff >= 1000) UART_send_char(log_buffer[3]);
                break;
            case COUNTER - 33:
                UART_send_char(comma);
                break;
            case COUNTER - 34:
                memset(log_buffer, '0', 8);
                break;
            case COUNTER - 35:
                UART_send_char(T_str);
                break;
            case COUNTER - 36:
                itoa(log_buffer,tp_buff,10);
                break;
            case COUNTER - 37:
                UART_send_char(log_buffer[0]);
                break;
            case COUNTER - 38:
                UART_send_char(log_buffer[1]);
                break;
            case COUNTER - 39:
                UART_send_char(log_buffer[2]);
                break;
            case COUNTER - 40:
                if (tp_buff >= 1000) UART_send_char(log_buffer[3]);  //IT IS NEEDED ??
                break;
            case COUNTER - 41:
                UART_send_char(comma);
                break;
            case COUNTER - 42:
                memset(log_buffer, '0', 8);
                break;
            case COUNTER - 43:
                UART_send_char(Q_str);
                break;
            case COUNTER - 44:
                utoa(log_buffer,qp_buff,10);
                break;
            case COUNTER - 45:
                UART_send_char(log_buffer[0]);
                break;
            case COUNTER - 46:
                if (qp_buff >= 10) UART_send_char(log_buffer[1]);
                break;
            case COUNTER - 47:
                if (qp_buff >= 100) UART_send_char(log_buffer[2]);
                break;
            case COUNTER - 48:
                if (qp_buff >= 1000) UART_send_char(log_buffer[3]);
                break;
            case COUNTER - 49:
                if (qp_buff >= 10000) UART_send_char(log_buffer[4]);
                break;  
            case COUNTER - 50:
                UART_send_char('<');
                break;    
        } 
    }
    if (!log_on) RESET_TIME();
}
//THIS ADC IS WORKING NOW
void read_ADC()
{
    float opr = 0;
    AD_SET_CHAN(V_CHAN);
    AD_CONVERT();
    AD_RESULT();
    //v = ad_res * 1.23779; //* 1.2207;    
    opr = (float)(1.28655 * ad_res);   //1051/1000 with 5014/4096
    v = opr;    //0 as offset
    AD_SET_CHAN(I_CHAN);
    AD_CONVERT();
    AD_RESULT();
    opr = (float)(1.22412 * ad_res);     //with 5014/4096
    //i = opr;
    opr = opr - 2525;
    if (state == CHARGE | state == POSTCHARGE){
        opr = -opr;
    }
    i = (float)(opr * 2.5); //HALL EFFECT ACS723LL
    AD_SET_CHAN(T_CHAN);
    AD_CONVERT();
    AD_RESULT();     
    opr = (float)(1.22412 * ad_res);
    opr = (float)(1866.3 - opr);
    t = (float) (opr/1.169);
}

void control_loop()
{
    if(!cmode)
    {
        pid(v, vref + 18);  //offset of 18
    }else
    {
        pid(i, iref);
    }
    set_DC();
}

void timing()
{
    if(count)
    {
        count--;
    }else
    {
        count = COUNTER;
        if(second < 59) second++;
        else{second = 0; minute++;}
    }
}
//THIS NEXT FUNCTION SEEMS LIKE A GOOD SOLUTION I TESTED IT AGAINST OTHERS AND STILL THE BEST
void calculate_avg()
{
    switch(count)
    {
        case COUNTER:
            iprom = 0;
            vprom = 0;
            tprom = 0;
            break;
        case 0:
            iprom /= COUNTER;
            vprom /= COUNTER;
            tprom /= COUNTER;
            #if (NI_MH_CHEM) 
            if ((int) vprom > vmax) vmax = (int) vprom;
            #endif
            qprom += (iprom/3600);
            break;
        default:
            iprom += i;
            vprom += v;
            tprom += t;
            //tprom += dc * 1.953125;
            break;
    }   
}

//**Beginning of the UART related functions. 
void Init_UART()
{
    //****Setting I/O pins for UART****//
    TXSEL = 0;      //RC6 selected as TX
    RXSEL = 0;      //RC7 selected as RX
    //________I/O pins set __________//
    
    /**Initialize SPBRG register for required 
    baud rate and set BRGH for fast baud_rate**/
    SP1BRGH = 0x00; 
    SP1BRGL = 0x8A;    
    
    BRGH  = 1;  // for high baud_rate
    BRG16 = 1;  // for 16 bits timer
    //_________End of baud_rate setting_________//
    
    //****Enable Asynchronous serial port*******//
    SYNC  = 0;    // Asynchronous
    SPEN  = 1;    // Enable serial port pins
    //_____Asynchronous serial port enabled_______//

    //**Lets prepare for transmission & reception**//
    TXEN  = 1;    // enable transmission
    CREN  = 1;    // enable reception
    //__UART module up and ready for transmission and reception__//
    
    //**Select 8-bit mode**//  
    TX9   = 0;    // 8-bit reception selected
    RX9   = 0;    // 8-bit reception mode selected
    //__8-bit mode selected__//    

    //INTERRUPTS
    RCIE = 0;                   //disable reception interrupts
    TXIE = 0;                   //disable transmision interrupts
}

 void UART_interrupt_enable()
 {
    char clear_buffer = 0;
    while(RCIF){                //clear the reception register
        clear_buffer = RC1REG;
    }
    RCIE = 1;                   //enable reception interrupts
    TXIE = 0;                   //disable transmision interrupts
 }

//**Function to send one byte of date to UART**//
void UART_send_char(char bt)  
{
    while(0 == TXIF)
    {
    }// hold the program till TX buffer is free
    TX1REG = bt; //Load the transmitter buffer with the received value
}
//_____________End of function________________//

//**Function to get one byte of date from UART**//
char UART_get_char()   
{
    if(OERR) // check for Error 
    {
        CREN = 0; //If error -> Reset 
        CREN = 1; //If error -> Reset 
    }
    
    while(!RCIF);  // hold the program till RX buffer is free
    
    return RC1REG; //receive the value and send it to main function
}
//_____________End of function________________//

//**Function to convert string to byte**//
void UART_send_string(char* st_pt)
{
    while(*st_pt) //if there is a char
        UART_send_char(*st_pt++); //process it as a byte data
}

void display_value(int value)
{   
    char buffer[6]; 
  
    itoa(buffer,value,10);  
  
    UART_send_string((char*)buffer);
}

void Cell_ON()
{
//    if (cell_count == 49)
//    {
//        CELL1_ON;
//        CELL2_OFF;
//        CELL3_OFF;
//        CELL4_OFF;
//    }else if (cell_count == 50)
//    {
//        CELL1_OFF;
//        CELL2_ON;
//        CELL3_OFF;
//        CELL4_OFF;        
//    }else if (cell_count == 51)
//    {
//        CELL1_OFF;
//        CELL2_OFF;
//        CELL3_ON;
//        CELL4_OFF;        
//    }else if (cell_count == 52)
//    {
//        CELL1_OFF;
//        CELL2_OFF;
//        CELL3_OFF;
//        CELL4_ON;        
//    }
}

void Cell_OFF()
{
//    CELL1_OFF;
//    CELL2_OFF;
//    CELL3_OFF;
//    CELL4_OFF;
}