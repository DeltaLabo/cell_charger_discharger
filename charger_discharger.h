/**
 * @file charger_discharger.h
 * @author Juan J. Rojas
 * @date 7 Aug 2018
 * @brief State machine header file for Charge and Discharge System.
 * @par Institution:
 * LaSEINE / CeNT. Kyushu Institute of Technology.
 * @par Mail (after leaving Kyutech):
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://bitbucket.org/juanjorojash/cell_charger_discharger
 */

#ifndef CHARGER_DISCHARGER_H
    #define CHARGER_DISCHARGER_H
    // PIC16F1786 Configuration Bit Settings

    // 'C' source line config statements
    #pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
    #pragma config WDTE = OFF       // Watchdog Timer Disabled (WDT disabled)
    #pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
    #pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)//If this is enabled, the Timer0 module will not work properly.
    #pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
    #pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
    #pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset enabled)
    #pragma config CLKOUTEN = ON    // Clock Out Negative Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
    #pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
    #pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

    // CONFIG2
    #pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
    #pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bit (Vcap functionality is disabled on RA6.)
    #pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
    #pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
    #pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
    #pragma config LPBOR = OFF      // Low Power Brown-Out Reset Enable Bit (Low power brown-out is disabled)
    #pragma config LVP = OFF        // Low-Voltage Programming Enable (Low-voltage programming disabled)//IF THIS IN ON MCLR is always enabled

    #include <xc.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <stdint.h> // To include uint8_t and uint16_t
    #include <string.h>
    #include <stdbool.h> // Include bool type
    /** This is the State Machine enum*/
	enum states { 
        IDLE = 0x01, ///< "Idle" state, defined by function @link fIDLE() @endlink
        CHARGE = 0x03, ///< "Charge" state, defined by function @link fCHARGE() @endlink
        PRECHARGE = 0x05, ///< "Precharge" state, defined by function @link fCHARGE() @endlink
        DISCHARGE = 0x07, ///< "Discharge" state, defined by function @link fDISCHARGE() @endlink
        POSTDISCHARGE = 0x09, ///< "Postdischarge" state, defined by function @link fDISCHARGE() @endlink
        DC_res = 0x0B, ///< "DC resistance" state, defined by function @link fDC_res() @endlink
        WAIT = 0x0D ///< "Wait" state, defined by function @link fWAIT() @endlink
    };
    bool command_interpreter(void);
    void fIDLE(void);
    void fCHARGE(void);
    void fDISCHARGE(void);
    void fDC_res(void);
    void fWAIT(void);
    
    void fNEXTSTATE(void);
    void fNEXTCELL(void);
    void fNEXTREPETITION(void);
    uint8_t getTime();
    
    void state_machine(void);
    void converter_settings(void);
    void initialize(void);
    void pid(float feedback, float setpoint);
    void set_DC();
    uint16_t read_ADC(uint16_t channel);
    void scaling(void);
    void log_control(void);
    void cc_cv_mode(uint16_t current_voltage, uint16_t reference_voltage, bool CC_mode_status);
    void control_loop(void);
    void calculate_avg(void);
    void interrupt_enable(void);
    
    void UART_send_char(char bt);
    uint8_t UART_get_byte(void);
    void UART_send_header(uint8_t start, uint8_t operation, uint8_t code);
    void UART_send_byte(uint8_t byte);
    void UART_get_some_bytes(uint8_t length, uint8_t* data);
    void UART_send_some_bytes(uint8_t length, uint8_t* data);
    uint16_t calculate_checksum(uint8_t code, uint8_t length, uint8_t* data);
    void put_data_into_structure(uint8_t length, uint8_t* data, uint8_t* structure);
    void UART_send_string(char* st_pt);
    void Cell_ON(void);
    void Cell_OFF(void);
    void timing(void);
    
    #define     _XTAL_FREQ              32000000 ///< Frequency to coordinate delays, 32 MHz
    #define     ERR_MAX                 1000 ///< Maximum permisible error, useful to avoid ringing
    #define     ERR_MIN                 -1000 ///< Minimum permisible error, useful to avoid ringing
    #define     V_CHAN                  0b01010 ///< Definition of ADC channel for voltage measurements. AN10(RB1) 
    #define     I_CHAN                  0b01100 ///< Definition of ADC channel for current measurements. AN12(RB0)
    #define     CELL1_ON()              { RB2 = 1; } ///< Turn on Cell #1
    #define     CELL2_ON()              { RB3 = 1; } ///< Turn on Cell #2
    #define     CELL3_ON()              { RB4 = 1; } ///< Turn on Cell #3
    #define     CELL4_ON()              { RB5 = 1; } ///< Turn on Cell #4
    #define     CELL1_OFF()             { RB2 = 0; } ///< Turn off Cell #1
    #define     CELL2_OFF()             { RB3 = 0; } ///< Turn off Cell #2
    #define     CELL3_OFF()             { RB4 = 0; } ///< Turn off Cell #3
    #define     CELL4_OFF()             { RB5 = 0; } ///< Turn off Cell #4

    /** @brief Stop the converter*/
    /** Set @p conv to zero, turn off the main relay (@p RC5), set the duty cycle in @p DC_MIN, 
    turn off all the cell relays in the switcher board, disable the logging of data to the terminal 
    and the UART reception interrupts.
    */
    #define     STOP_CONVERTER()        { RC3 = 0; RC4 = 0; conv = 0; RC5 = 0; pidt = DC_MIN; set_DC(); Cell_OFF();}
   //It seems that above 0.8 of DC the losses are so high that I don't get anything similar to the transfer function 
    #define     DC_MIN                  50.0  ///< Minimum possible duty cycle, set around @b 0.1 
    #define     DC_MAX                  300.0  ///< Maximum possible duty cycle, set around @b 0.8
    #define     COUNTER                 1024  ///< Counter value, needed to obtained one second between counts.
    ////////////////////////////////////////////////////////////////////////////////////
    //General definitions
    #define     WAIT_TIME               5 ///< Time to wait before states, set to 5 seconds as default
    #define     DC_RES_SECS             220 ///< How many seconds the DC resistance process takes
    //Li-Ion definitions
    #define     Li_Ion_CV               4200 ///< Li-Ion constant voltage setting in mV
    #define     Li_Ion_CAP              3250 ///< Li-Ion capacity setting in mAh
    #define     Li_Ion_EOC_I            100 ///< Li-Ion end-of-charge current in mA
    #define     Li_Ion_EOD_V            2500 ///< Li_Ion end-of-discharge voltage in mV
    //Ni-MH definitions
    #define     Ni_MH_CV                1750 ///< Ni-MH constant voltage setting in mV
    #define     Ni_MH_CAP               2000 ///< Ni-MH capacity setting in mAh
    #define     Ni_MH_EOC_DV            10 ///< Ni-MH end-fo-charge voltage drop in mV
    #define     Ni_MH_EOD_V             1000 ///< Ni-MH end-of-discharge voltage in mV

    #define     SET_DISC()              { RC3 = 0; RC4 = 0; __delay_ms(100); RC3 = 1; __delay_ms(100); RC3 = 0; __delay_ms(100); RC5 = 1; __delay_ms(100); kp = CC_disc_kp; ki = CC_disc_ki; kd = (float) (CC_char_disc_kd); pidi = 0.0;}
    #define     SET_CHAR()              { RC3 = 0; RC4 = 0; __delay_ms(100); RC4 = 1; __delay_ms(100); RC4 = 0; __delay_ms(100); RC5 = 1; __delay_ms(100); kp = CC_char_kp; ki = CC_char_ki; kd = (float) (CC_char_disc_kd); pidi = 0.0;}
    //Structs
    typedef struct basic_configuration_struct {
        uint8_t version;
        uint16_t const_voltage;
        uint16_t const_current_char;
        uint16_t const_current_disc; 
        uint16_t capacity;
        uint16_t end_of_charge;
        uint16_t end_of_precharge;
        uint16_t end_of_discharge;
        uint16_t end_of_postdischarge;   
    }basic_configuration_type, *basic_configuration_type_ptr;
    
    typedef struct test_configuration_struct {
        uint8_t number_of_cells;
        uint8_t number_of_states;
        uint8_t number_of_repetitions;
        uint16_t wait_time;
        uint16_t end_wait_time;
        uint8_t order_of_states[10];
    }test_configuration_type, *test_configuration_type_ptr;
    
    typedef struct converter_configuration_struct {
        uint16_t CVKp;
        uint16_t CVKi;
        uint16_t CVKd;
        uint16_t CCKpC;
        uint16_t CCKiC;
        uint16_t CCKpD;
        uint16_t CCKiD;
    }converter_configuration_type, *converter_configuration_type_ptr;
    
    typedef struct log_data_struct {
        uint8_t cell_counter;
        uint8_t repetition_counter;
        uint8_t state;
        uint16_t elapsed_time;
        uint16_t voltage;
        uint16_t current; 
        uint16_t capacity;
        uint16_t temperature;
        uint16_t duty_cycle;
    }log_data_type, *log_data_type_ptr;
    
    //Variables
    
    basic_configuration_type            basic_configuration;
    basic_configuration_type_ptr        basic_configuration_ptr;  
    test_configuration_type             test_configuration;
    test_configuration_type_ptr         test_configuration_ptr; 
    converter_configuration_type        converter_configuration;
    converter_configuration_type_ptr    converter_configuration_ptr; 
    log_data_type                       log_data;
    log_data_type_ptr                   log_data_ptr;
    bool                                start = false;
    bool                                SECF = 1; ///< 1 second flag
    bool                                SRXF = 0; ///< Serial Reception Flag
    uint16_t                            capacity; ///< Definition of capacity per cell according to each chemistry
    uint16_t                            i_char; ///< Charging current in mA
    uint16_t                            i_disc; ///< Discharging current in mA
    unsigned char                       cell_count = 1; ///< Cell counter from '1' to '4'. Initialized as '1'
    unsigned char                       cell_max = 0; ///< Number of cells to be tested. Initialized as 0
    uint16_t                            wait_count = 0; ///< Counter for waiting time between states. Initialized as 0
    unsigned char                       dc_res_count = 0; ///< Counter for DC resistance. Initialized as 0
    unsigned char                       state = IDLE; ///< Used with store the value of the @link states @endlink enum. Initialized as @link STANDBY @endlink 
    
    uint8_t                             counter_state = 0; ///< Used to move trough the diferent states.
    uint8_t                             repetition_counter = 0; ///< Used to move trough repetitions.
    
    // last test with LI_ION gave this constants
    float                               CV_kp = 0.0018;  ///< Proportional constant for CV mode
    float                               CV_ki = 0.0005;  ///< Integral constant for CV mode 
    float                               CV_kd = 0.020; ///< Diferential constant for CV mode 
    
    // last test with LI_ION gave this constants MAYBE OK ALEX
    float                               CC_char_kp = 0.0130;  ///< Proportional constant divider for CC mode
    float                               CC_char_ki = 0.0025;  ///< Integral constant for CC mode 
    float                               CC_disc_kp = 0.006;   ///< Proportional constant for CC mode
    float                               CC_disc_ki = 0.001;   ///< Integral constant for CC mode
    uint8_t                             CC_char_disc_kd = 0;  ///< Diferential constant for CC mode 
    
                                
    uint16_t                            EOC_variable; ///< End-of-charge current in mA
    uint16_t                            EOPC_variable; ///< End-of-precharge variable in mA or mV
    uint16_t                            EOD_voltage; ///< End-of-dischage voltage in mV
    uint16_t                            EOPD_capacity; ///< End-of-postdischarge capacity
    bool                                conv = 0; ///< Turn controller ON(1) or OFF(0). Initialized as 0
    uint16_t                            count = COUNTER; ///< Counter that should be cleared every second. Initialized as #COUNTER 
    /**< Every control loop cycle this counter will be decreased. This variable is used to calculate the averages and to trigger
    all the events that are done every second.*/
    //uint16_t                            ad_res; ///< Result of an ADC measurement.
    uint16_t                            v;  ///< Last voltage ADC measurement.
    uint16_t                            i;  ///< Last current ADC measurement.
    uint24_t                            vacum = 0; ///< accumulator dor v
    uint24_t                            iacum = 0;
    //qavg does not need accumulator
    uint16_t                            vavg = 0;  ///< Last one-second-average of #v . Initialized as 0
    uint16_t                            iavg = 0;  ///< Last one-second-average of #i . Initialized as 0
    float                               qavg = 0.0;  ///< Integration of #i . Initialized as 0
    uint16_t                            vmax = 0;   ///< Maximum recorded average voltage. 
    float                               pidi;   ///< Integral acumulator of PI compensator
    float                               kp;  ///< Proportional compesator gain
    float                               ki;  ///< Integral compesator gain
    float                               kd;  ///< Diferential compesator gain
    float                               vref = 0;  ///< Scaled voltage setpoint. Initialized as 0
    uint16_t                            iref = 0;  ///< Current setpoint. Initialized as 0
    bool                                cmode = 1;  ///< CC / CV selector. CC: <tt> cmode = 1 </tt>. CV: <tt> cmode = 0 </tt>   
    float                               pidt = 0;  ///< Duty cycle
    float                               er = 0; /// < Define er for calculating the error on dc calculus    
    uint16_t                            second = 0; ///< Seconds counter
    uint16_t                            timeout = 0;
#endif /* CHARGER_DISCHARGER_H */


