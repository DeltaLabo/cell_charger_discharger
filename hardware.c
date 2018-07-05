/* Hardware related source file for Charge and Discharge System. */
/* Kyutech Institute of Technology. LaSEINE. Supervisor: Mengu CHO.*/
/* 
 * File:  hardware.c 
 * Author: Juan J. Rojas.
 * Version control in Git
 */
#include <stdio.h>
#include <math.h>
#include "hardware.h"
#include "state_machine.h"

unsigned int            ad_res;
unsigned int            v;                  //ADDED
unsigned int            i;                  //ADDED
unsigned int            t;                  //ADDED
unsigned int            count;              //ADDED
unsigned short long     iprom;
unsigned short long     vprom;
unsigned short long     tprom;
unsigned int            vref;
unsigned int            iref;
unsigned char           cmode;
unsigned char           cc_cv;
unsigned int            second;
unsigned char           esc;                    //Escape character
int                     pp;
int                     pi;
float                   kp;
float                   ki;
unsigned long           inc;
unsigned char           spb;
unsigned int            log_on; 
char const              comma_str[] = ",";
char const              in_arr_str[] = "->";
char const              end_arr_str[] = "<-";
char const              in_wait_str[] = "------------W-";
char const              end_wait_str[] = "-W------------";
char const              hip_str[] = "-";
char const              in_sta_str[] = "S-";
char const              end_sta_str[] = "-S";
char const              C_str[] = "C";
char const              V_str[] = "V";
char const              I_str[] = "I";
char const              T_str[] = "T";


void Initialize_Hardware()
{
	CLRWDT();
	TMR0IF = 0;                          //Clear timer 0
	ad_res = 0;                        //Clear ADC result variable
	cmode = 1;                         //Start in CC mode
	iref = 0;                  
	vref = 0;
	STOP_CONVERTER();
}

void Init_Registers()
{
     //SYSTEM CLOCK 
    OSCFRQbits.HFFRQ = 0b110;           //Set HFINTOSC at 32Mhz (000 is for 1MHz)
    OSCCON1bits.NOSC = 0b110;           //Set internal clock to HFINTOSC or FOSC=32MHz
    OSCCON1bits.NDIV = 0b0001;          //Set divider to 2
    //SYSTEM CLOCK IS AT 16MHz

    //PINS FOR SWITCHES
    TRISBbits.TRISB5 = 1;               //Set RB5 as input. C/D signal
    ANSELBbits.ANSB5 = 0;               //Digital switch
    WPUBbits.WPUB5 = 1;                 //Weak pull up activated

    //THIS ONE WAS INTERRUPTS
    //TRISBbits.TRISB4 = 1;               //Set RB4 as input. ON/OFF signal
    //ANSELBbits.ANSB4 = 0;               //Digital switch
    //PIE0bits.IOCIE = 1;                 //enable interrupt on change global
    //IOCBNbits.IOCBN4 = 1;               //negative detection on B4, enter the ISR
    //IOCBPbits.IOCBP4 = 1;               //positive detection on B4, enter the ISR
     
    WPUBbits.WPUB4 = 1;                 //Weak pull up activated
    TRISBbits.TRISB2 = 1;               //Set RB2 as input. MAN/AUTO signal
    ANSELBbits.ANSB2 = 0;               //Digital switch
    WPUBbits.WPUB2 = 1;                 //Weak pull up activated
    //PINS FOR OPAMPS
    TRISBbits.TRISB3 = 1;               //Set RB3 as input. BATT_IN signal
    ANSELBbits.ANSB3 = 0;               //Digital switch
    WPUBbits.WPUB3 = 0;                 //Weak pull up DEactivated   
    
    //OUTPUTS FOR RELAYS
    TRISAbits.TRISA0 = 0;               //Set RA0 as output. C/D relay
    ANSELAbits.ANSA0 = 0;               //Digital
    TRISAbits.TRISA1 = 0;               //Set RA3 as output.ON/OFF relay
    ANSELAbits.ANSA1 = 0;               //Digital   

    //OUTPUTS FOR CELL SWITCHER
    TRISAbits.TRISA7 = 0;               //Set RA7 as output. Cell #1
    ANSELAbits.ANSA7 = 0;               //Digital
    TRISAbits.TRISA6 = 0;               //Set RA6 as output. Cell #2
    ANSELAbits.ANSA6 = 0;               //Digital
    TRISCbits.TRISC0 = 0;               //Set RC0 as output. Cell #3
    ANSELCbits.ANSC0 = 0;               //Digital
    TRISCbits.TRISC1 = 0;               //Set RC1 as output. Cell #4
    ANSELCbits.ANSC1 = 0;               //Digital
    //TIMER0 FOR CONTROL AND MEASURING LOOP
    // T0OUTPS 1:1; T0EN disabled; T016BIT 8-bit; 
    // T0CON0 = 0x00;
    // FOSC/4 , NOT SYNC, 1:4096
    // T0CON1 = 0x59; 
    T0CON0bits.T0EN = 0;                // Turn off the timer module
    T0CON0bits.T016BIT = 0;             // 8 bit timer
    T0CON0bits.T0OUTPS = 0b0000;        // Use 1:1 postscaler
    T0CON1bits.T0CS = 0b010;            // FOSC/4 as clock source
    T0CON1bits.T0ASYNC = 1;             // Input of TMR0 is syncronized with Fosc/4
    T0CON1bits.T0CKPS = 0b0101;         // Prescaler as 1:16  ///I just changed to 1:32 (0b0101)  //Now to 1:64 (0b0110) // Now to 1:128 (0b0111)
    PIR0bits.TMR0IF = 0;                // Clear interrupt flag before enable the interrupt
    //PIE0bits.TMR0IE = 1;                // Enable automatic interrupt //THIS GAVE ERROR WITH THE OTHER INTERRUPT
    // This gives a TIMER of 16Mhz/4/32/256, aprox 976.56Hz/2
    //TURN IT ON
    T0CON0bits.T0EN = 1;
    //T0CON0 = 0x80;
    
    //TIMER2 (FOR PWM)
    T2CLKCONbits.CS = 0b0001;           //TIMER2 is driven by FOSC/4 (4Mhz). This instruction is also required for correct operation of PWM.
    T2CONbits.TMR2ON = 1;               //TIMER2 is ON
    T2CONbits.CKPS = 0b010;             //1:4 Prescaler
    T2CONbits.OUTPS = 0b0000;           //1:1 Postcaler (Postscaler has no effect in PWM operation)
    T2HLTbits.PSYNC = 1;                //TIMER2 Presclaler is sinchronized with FOSC/4
    T2HLTbits.MODE =  0b000;            //TIMER2 mode is Free run
    PR2 = 1;                            //TIMER2 output is divided again by 2. This makes TIMER2 output overflow at FOSC/32 or 500Khz
    
    //PWM 
    CCP5CONbits.EN = 1;                 //Enable CCP5 module
    CCP5CONbits.MODE = 0b1111;          //PWM MODE
    CCPTMRS1bits.C5TSEL = 0b01;         //PWM5 based on TMR2
    CCP5CONbits.FMT = 0;                //Right aligned
    PIR4bits.TMR2IF = 0;                //Clear the flag (I'm not sure if this is needed)
    CCPR5L = 0b00000001;                //Lest significant eight bits     
    CCPR5H = 0b00000000;                //Most significant eight bits (only two <7:6> matter)
    //PWM DC = 1/8. PWM period = 2 microseconds
    
    //LOGIC CELL CONFIGURATION FOR CONVERTING PWM IN CLOCK
    CLC1CON = 0x80;                     //ON, AND-OR
    CLC1GLS0 = 0x02;                    //PWM SEL
    CLC1GLS1 = 0x08;                    //FOSC SEL    
    CLC1GLS2 = 0x08;                    //FOSC SEL
    CLC1GLS3 = 0x10;                    //NCO OUT SEL
    CLC1POL = 0x00;                     //INVERTED OUTPUT POLARITY
    CLC1SEL0 = 0x17;                    //PWM IN
    CLC1SEL1 = 0x04;                    //FOSC IN
    CLC1SEL2 = 0x1A;                    //NCO OUT IN
    CLC1SEL3 = 0x00;                    //LOGIC ZERO
    
    //NCO DRIVEN BY LOGIC CELL CLOCK
    NCO1CONbits.N1EN = 1;               //Turn on the NCO1
    NCO1CONbits.N1POL = 0;              //Polarity is not inverted
    //This bit is going to be changed inside the main program when DC>50%
    NCO1CONbits.N1PFM = 1;              //Activate Pulse Frequency mode
    TRISAbits.TRISA4 = 1;               //NCO output in RA4, not set yet as output, wait for it... 
    NCO1CLKbits.N1PWS = 0b001;          //NCO1 output is active for 1 input clock period
    NCO1CLKbits.N1CKS = 0b010;          //Clock source is LC1 Out
    
    //NCO OUTPUT
    RA4PPS = 0x19;                      //NCO in RA4 selected as PPS output pin
    //TRISAbits.TRISA4 = 0;             //Enable RA4 output PIN----THIS IS TURNED ON WITH START_CONVERTER()
    
    //******ADC******
    //REFERENCE FOR ADC
    FVRCONbits.FVREN = 1;               //Fixed Voltage Reference is enabled
    //FVRCONbits.CDAFVR = 0b11;         //Comparator set to 4.096V
    FVRCONbits.ADFVR = 0b11;            //ADC set to 4.096V
    //INTERRUPTS 
    //PIE1bits.ADIE = 1;                  //Activate interrupts
    //INTCONbits.PEIE =1;                 //Pehierals interrupts
    
    //ADC INPUTS//check this after final design
    TRISCbits.TRISC4 = 1;               //RC4, voltage sensing input    
    ANSELCbits.ANSC4 = 1;               //RC4 analog      
    WPUCbits.WPUC4 = 0;                 //Weak pull up Deactivated
    TRISCbits.TRISC5 = 1;               //RC5, current sensing input
    ANSELCbits.ANSC5 = 1;               //RC5 analog
    WPUCbits.WPUC5 = 0;                 //Weak pull up Deactivated
    TRISCbits.TRISC3 = 1;               //RC3, temperature sensing input
    ANSELCbits.ANSC3 = 1;               //RC3 analog
    WPUCbits.WPUC3 = 0;                 //Weak pull up Deactivated
    
    //PROVISIONAL CURRENT CHANNEL
    TRISCbits.TRISC2 = 1;               //RC2, current sensing input
    ANSELCbits.ANSC2 = 1;               //RC2 analog
    WPUCbits.WPUC2 = 0;                 //Weak pull up Deactivated
    
    //CHANGE TO USE FRC
    //OSCENbits.ADOEN = 1;                //Activate FRC
    //ADCON0bits.ADCS = 1;                //Clock supplied by FRC
    //ADC INITIALIZATI0
    ADCON0bits.ADCS = 0;                //Clock supplied by FOSC, divided according to ADCLK register
    ADCLKbits.ADCCS = 0b111111;         //Clock set as FOSC/16 THAT 1us. 0b000101
    ADREFbits.ADNREF = 0;               //Negative reference as Vss
    ADREFbits.ADPREF = 0b11;            //Positive reference as FVR_buffer
    ADCON0bits.ADFRM0 = 1;              //8 MSB are in ADRESH ///CALCULATION ALWAYS RIGHT SHIFT THE DATA
    //this is not here
    ADPREbits.ADPRE = 0;                //0 clock cycles of precharge
    ADACQbits.ADACQ = 0;
    //ADCON1bits.ADDSEN = 1;            //THIS IS NOT NEEDED
    //some clock cycles of adquisition
    //NUEVO
    //ADCON1bits.ADPPOL = 1;            //Polarity 
    ADCON2bits.ADMD = 3;                //Burst average (3) Low pass filter mode (4)
    ADCON2bits.ADCRS = 1;               //For 8 samples this is 3
    ADRPT = 2;                          //8 samples
    //ADCAPbits.ADCAP = 0b1111;         //31pF of extra capacitance THIS DOES NOT HELP
    ADCON3bits.ADTMD = 0b111;           //SET ADTIF At the end of calculation
    PIR1bits.ADTIF = 0;                 //Clear the flag before start
    ADCON0bits.ADON = 1;                //Turn on the ADC

}

void pid(unsigned int feedback, unsigned int setpoint)
{
int 	er;
int		ipid;
	er = setpoint - feedback;

	if(er > ERR_MAX) er = ERR_MAX;
	if(er < ERR_MIN) er = ERR_MIN;
    
	if(cmode) pp = er; else
	pp = er;

	pi += er;
	if(pi > ERR_MAX) pi = ERR_MAX;
	if(pi < ERR_MIN) pi = ERR_MIN;

	ipid = kp*pp; //Im going to put a constant here only to see
	ipid += ki*(pi / 256); //It takes 256 instructions to overflow
    
	if(ipid > ERR_MAX) ipid = ERR_MAX;
	if(ipid < ERR_MIN) ipid = ERR_MIN;
                 
	inc += ipid; //This is the point in which a mix the PWM with the PID

    set_NCO();
}

void set_NCO()
{
    if(inc < INC_MIN) inc = INC_MIN;                //Respect the limits of the increment
    if(inc > INC_MAX) inc = INC_MAX;

    NCO1INCU = (inc >> 16) & 0xFF;                  //INIT_INCU; //8 bit register MSB
    NCO1INCH = (inc >> 8) & 0xFF;                   //INIT_INCH; //8 bit register 
    NCO1INCL = inc & 0xFF;                          //INIT_INCL; //8 bit register LSB
}

void cc_cv_mode()
{
    if (!count)
    {
        if(v > vref && cmode == 1)
        {
            pi = 0;
            cmode = 0;
            kp = 3;
            ki = 0.5;
        }
    }         
}

void log_control()
{
        if (!count)
        {
            if (wait_count < wait_time && wait_count > 1)
            {   
                LINEBREAK;
                UART_send_string(in_wait_str);
                display_value(wait_count);
                UART_send_string(end_wait_str);             
            }
            if (wait_count) wait_count--;
            /*if (dc_res_count)
            {
                UART_send_string(hip_str);
                display_value(dc_res_count);
                UART_send_string(hip_str);               
            }*/
            if (log_on)
            {
                LINEBREAK;
                UART_send_string(in_sta_str);
                display_value(state);
                UART_send_string(end_sta_str);
                UART_send_string(in_arr_str);
                UART_send_string(C_str);
                display_value(cell_count - 48);
                UART_send_string(comma_str);
                UART_send_string(V_str);
                display_value(vprom*10);
                UART_send_string(comma_str);
                UART_send_string(I_str);
                display_value(iprom*10);   
                UART_send_string(comma_str);
                UART_send_string(T_str);
                display_value(tprom);  
                //UART_send_string(comma_str); //
                //display_value(t);           //
                /*UART_send_string(comma_str);
                UART_send_string("Inc");
                display_value(inc);*/
                UART_send_string(end_arr_str);
            }
            count = COUNTER;
            iprom = 0;
            vprom = 0;
            tprom = 0;
        }
}
//THIS ADC IS WORKING NOW
void read_ADC()
{
    unsigned long opr;
    AD_SET_CHAN(V_CHAN);
    AD_CONVERT();
    AD_RESULT();
    opr = 4UL * 1051UL * ad_res;
    v = opr / 1000UL + 0UL;    //0 as offset   
    AD_SET_CHAN(I_CHAN);
    AD_CONVERT();
    AD_RESULT();
    opr = 4UL * ad_res;    
    if(opr > 2500UL)
    {
        opr = opr - 2500UL;
    }
    else if(i == 2500UL)
    {
        opr = 0UL;
    }
    else if(i < 2500UL)
    {
        opr = 2500UL - opr;
    }
    //i=i/0.4;       //A mOhms resistor
    //i = (200/37) * i; //Hall effect sensor  37/200=0.185
    opr = 25UL * opr;
    i = opr / 10UL; //HALL EFFECT ACS723LL    
    AD_SET_CHAN(T_CHAN);
    AD_CONVERT();
    AD_RESULT();
    opr = 4000UL * ad_res;
    opr = 1866300UL - opr;
    t = opr / 1169UL;
    opr = 0UL;     
}
//***PROBABLY THIS IS NOT GOOD ANYMORE BECAUSE OF MANUAL AUTO OPERATION

void control_loop()
{
    cc_cv_mode();
	if(!cmode)
    {
        pid(v, vref);
    }else
    {
        pid(i, iref);
    }
}

void calculate_avg()
{
    if(!PORTAbits.RA1)
    {
        if (count)
        {
            iprom += i;
            vprom += v;
            tprom += t;
            count--;
        }
        if (!count)
        {
            iprom = iprom / COUNTER;
            vprom = vprom / COUNTER;
            tprom = tprom / COUNTER;
        }      
    }else
    {
        count--;
        iprom = 0;
        vprom = 0;
        tprom = 0;
    }
}

//**Beginning of the UART related functions. 
void Init_UART()
{
    //****Setting I/O pins for UART****//
    RC6PPS = 0x0010; // RC6 -> EUSART:TX
    RXPPSbits.RXPPS = 0x17;
    TRISCbits.TRISC6 = 1; // Set to 1 according to page 574
    ANSELCbits.ANSC7 = 0; // Clear ANSEL bit for RX
    TRISCbits.TRISC7 = 1; // Set to 1 according to page 574
    //________I/O pins set __________//
    
    /**Initialize SPBRG register for required 
    baud rate and set BRGH for fast baud_rate**/
    spb = ((_XTAL_FREQ/16)/BAUD_RATE) - 1;
    SP1BRGH = 0x00; 
    SP1BRGL = spb;
                    
    
    
    TX1STAbits.BRGH  = 1;  // for high baud_rate
    BAUD1CONbits.BRG16 = 0;  // for 16 bits timer
    //_________End of baud_rate setting_________//
    
    //****Enable Asynchronous serial port*******//
    TX1STAbits.SYNC  = 0;    // Asynchronous
    RC1STAbits.SPEN  = 1;    // Enable serial port pins
    //_____Asynchronous serial port enabled_______//

    //**Lets prepare for transmission & reception**//
    TX1STAbits.TXEN  = 1;    // enable transmission
    RC1STAbits.CREN  = 1;    // enable reception
    //__UART module up and ready for transmission and reception__//
    
    //**Select 8-bit mode**//  
    TX1STAbits.TX9   = 0;    // 8-bit reception selected
    RC1STAbits.RX9   = 0;    // 8-bit reception mode selected
    //__8-bit mode selected__//    
    //INTERRUPTS
}

void UART_interrupt_enable()
{
    while(RCIF){
        esc = RC1REG;
        esc = 0;
    }
    PIE3bits.RCIE = 1;                  //enable reception interrupts
    PIE3bits.TXIE = 0;                  //disable transmision interrupts
    INTCONbits.PEIE = 1;                //enable peripherals interrupts
    INTCONbits.GIE = 1;                 //enable global interrupts   
}

//**Function to send one byte of date to UART**//
void UART_send_char(char bt)  
{
    while(0 == PIR3bits.TXIF)
    {
    }// hold the program till TX buffer is free
    TX1REG = bt; //Load the transmitter buffer with the received value
}
//_____________End of function________________//

//**Function to get one byte of date from UART**//
char UART_get_char()   
{
    if(RC1STAbits.OERR) // check for Error 
    {
        RC1STAbits.CREN = 0; //If error -> Reset 
        RC1STAbits.CREN = 1; //If error -> Reset 
    }
    
    while(!PIR3bits.RCIF);  // hold the program till RX buffer is free
    
    return RC1REG; //receive the value and send it to main function
}
//_____________End of function________________//

//**Function to convert string to byte**//
void UART_send_string(char* st_pt)
{
    while(*st_pt) //if there is a char
        UART_send_char(*st_pt++); //process it as a byte data
}

void display_value(unsigned int value)
{   
    char buffer[6]; 
  
    utoa(buffer,value,10);  
  
    UART_send_string(buffer);
}

void Cell_ON()
{
    if (cell_count == 49)
    {
        CELL1_ON;
        CELL2_OFF;
        CELL3_OFF;
        CELL4_OFF;
    }else if (cell_count == 50)
    {
        CELL1_OFF;
        CELL2_ON;
        CELL3_OFF;
        CELL4_OFF;        
    }else if (cell_count == 51)
    {
        CELL1_OFF;
        CELL2_OFF;
        CELL3_ON;
        CELL4_OFF;        
    }else if (cell_count == 52)
    {
        CELL1_OFF;
        CELL2_OFF;
        CELL3_OFF;
        CELL4_ON;        
    }
}

void Cell_OFF()
{
    CELL1_OFF;
    CELL2_OFF;
    CELL3_OFF;
    CELL4_OFF;
}