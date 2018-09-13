/**
 * @file state_machine.c
 * @author Juan J. Rojas
 * @date 7 Aug 2018
 * @brief State machine source file for Charge and Discharge System.
 * @par Institution:
 * LaSEINE / CeNT. Kyushu Institute of Technology.
 * @par Mail (after leaving Kyutech):
 * juan.rojas@tec.ac.cr
 * @par Git repository:
 * https://bitbucket.org/juanjorojash/cell_charger_discharger
 */

#include "state_machine.h"
#include "hardware.h"

/**@brief This function contain the transition definition for the states of the machine.
*/
void State_machine()
{
    switch(state){
    /**The @link STANDBY @endlink state goes to the @link fSTANDBY() @endlink function.*/
            case STANDBY:
                fSTANDBY();  
                break;   
    /**The @link IDLE @endlink state goes to the @link fIDLE() @endlink function.*/             
            case IDLE:
                fIDLE();  
                break;
    /**The @link PREDISCHARGE @endlink and @link DISCHARGE @endlink states go to the @link fDISCHARGE() @endlink function.*/ 
            case PREDISCHARGE:
            case DISCHARGE:
                fDISCHARGE();
                break;
    /**And the @link POSTCHARGE @endlink and @link CHARGE @endlink states go to the @link fCHARGE() @endlink function.*/  
            case POSTCHARGE:  
            case CHARGE:   
                fCHARGE();  
                break;
    /**The @link CS_DC_res @endlink, @link DS_DC_res @endlink and @link PS_DC_res @endlink states go to the @link fDC_res() @endlink function.*/
            case CS_DC_res:
            case DS_DC_res:
            case PS_DC_res:
                fDC_res();
                break;
    /**The @link WAIT @endlink state goes to the @link fWAIT() @endlink function.*/
            case WAIT:    
                fWAIT();
                break;
    /**The @link ISDONE @endlink state goes to the @link fISDONE() @endlink function.*/
            case ISDONE:    
                fISDONE();
                break;
    /**The @link FAULT @endlink state goes to the @link fFAULT() @endlink function.*/
            case FAULT:    
                fFAULT();
                break;
    }
}

/**@brief This function define the @link STANDBY @endlink state of the state machine.
*/
void fSTANDBY()
{   
    /**First, the function will stop the converter usign @link STOP_CONVERTER() @endlink macro*/
    STOP_CONVERTER();
    /**Then, it will disable the USART reception interrupts to avoid interference with the setting of
    parameters in the @p STANDBY state*/  
    RCIE = 0;                   //disable reception interrupts
    /**Next, the variables related with the setting of parameters are initizalized: 
    <tt>option = 0, cell_max = 0, cell_count = '1'</tt>*/
    option = 0;
    cell_max = 0;
    cell_count = '1';
    /**The following message will be printed @code ---Parameter definition for charger and discharger--- @endcode*/
    LINEBREAK; 
    UART_send_string((char*)param_def_str);
    LINEBREAK;
    /**If @link LI_ION_CHEM @endlink is set to @b 1, the folowing message will be displayed:*/
    #if (LI_ION_CHEM) 
    /**> Chemistry defined as Li-Ion*/
    UART_send_string((char*)chem_def_liion);
    LINEBREAK;
    LINEBREAK;
    /**If @link NI_MH_CHEM @endlink is set to @b 1 and LI_ION_CHEM @endlink is set to @b 0, 
    the folowing message will be displayed:*/
    #elif (NI_MH_CHEM)
    /**> Chemistry defined as Ni-MH*/
    UART_send_string((char*)chem_def_nimh);
    LINEBREAK;
    LINEBREAK; 
    #endif
    /** Then the function will call the @link param() @endlink function*/
    param();
}

/**@brief This function define the IDLE state of the state machine.
*/
void fIDLE()
{
    /**At first, the function will call the @link Start_state_machine() @endlink function.*/
    Start_state_machine();
    /**Then, it will call the @link Converter_settings() @endlink function.*/
    Converter_settings(); 
    /**Then, it will enable the USART reception interrupts to give the possibility to the user to press
    @b ESC to cancel or @b n to go to the next cell, at any time during the testing process*/            
    UART_interrupt_enable();
}

/**@brief This function define the IDLE state of the state machine.
*/
void fCHARGE()
{
    LOG_ON();
    conv = 1;
    if (vprom < 900)
    {
        state = FAULT;
        UART_send_string((char*)cell_below_str);
        LINEBREAK;
    }
    if (state == CHARGE){
        #if (LI_ION_CHEM)
        if (iprom < EOC_current)
        {                
            if (!EOCD_count)//evaluate this, is really needed
            {
                prev_state = state;
                if (option == '3') state = ISDONE;
                else state = WAIT;                
                wait_count = WAIT_TIME;
                STOP_CONVERTER();                       
            }else EOCD_count--;
        }
        #elif (NI_MH_CHEM)
        if (vprom < (vmax - Ni_MH_EOC_DV) || minute >= timeout)
        {
            if (!EOCD_count)//evaluate this, is really needed
            {
                prev_state = state;
                if (option == '3') state = ISDONE;
                else state = WAIT;                
                wait_count = WAIT_TIME;
                STOP_CONVERTER();    
            }else EOCD_count--;
        }
        #endif   
    } 
    if (state == POSTCHARGE){
        if (qprom >= (capacity/2)){
            prev_state = state;
            state = WAIT;
            wait_count = WAIT_TIME;
            STOP_CONVERTER();
        }
    }    
}

/**@brief This function define the IDLE state of the state machine.
*/
void fDISCHARGE()
{
    LOG_ON();
    conv = 1;
    if (vprom < EOD_voltage)
    {
        if (!EOCD_count)//evaluate this, is really needed
        { 
            prev_state = state;
            if (option == '2'| option == '4') state = ISDONE;
            else state = WAIT;                
            wait_count = WAIT_TIME;
            STOP_CONVERTER();
        }else EOCD_count--;
    }
}

/**@brief This function define the IDLE state of the state machine.
*/
void fDC_res() //can be improved a lot!!
{
    //LOG_ON();
    conv = 1;
    if (dc_res_count == 4)  //Check all this timming
    {
        v_1_dcres = vprom;
        i_1_dcres = iprom;
        iref = capacity;                  
    }
    if (dc_res_count == 1)
    {
        v_2_dcres = vprom;
        i_2_dcres = iprom;
        STOP_CONVERTER();            
        dc_res_val = ((v_1_dcres - v_2_dcres)/(i_2_dcres - i_1_dcres));
        dc_res_val = dc_res_val * 10000;
    }
    if (!dc_res_count)
    {   
        LINEBREAK;
        UART_send_char(C_str);
        UART_send_char(cell_count);
        UART_send_char(comma);
        UART_send_char(S_str);
        display_value((int)state);
        UART_send_char(comma);
        UART_send_char(R_str);
        display_value((int)dc_res_val);
        UART_send_char('<');
        LINEBREAK;
        LOG_OFF();   ///I dont like this 
        prev_state = state;
        state = WAIT;
        wait_count = WAIT_TIME;              
    }else dc_res_count--;
}

/**@brief This function define the IDLE state of the state machine.
*/
void fWAIT()
{
    STOP_CONVERTER();  ///MAYBEOK
    if (wait_count)
    {   
        LINEBREAK;
        UART_send_char(C_str);
        UART_send_char(cell_count);
        UART_send_char(comma);
        UART_send_char(S_str);
        display_value((int)state);
        UART_send_char(comma);
        UART_send_char(W_str);
        display_value(wait_count);
        UART_send_char('<');
        wait_count--;             
    }
    if(!wait_count)
    {           
        switch(prev_state)
        {
            case PREDISCHARGE:
                state = CHARGE;
                Converter_settings();
                break;
            case CHARGE:
                state = CS_DC_res; 
                Converter_settings();
                break;
            case DISCHARGE:
                state = DS_DC_res; 
                Converter_settings();
                break;
            case POSTCHARGE:
                state = PS_DC_res;
                Converter_settings();
                break;
            case DS_DC_res:
                state = POSTCHARGE;
                Converter_settings();
                break;
            case CS_DC_res:
                state = DISCHARGE;
                Converter_settings();
                break;
            case PS_DC_res:
                state = ISDONE;
                STOP_CONVERTER();
                break;
        }
    }
}

/**@brief This function is executed every time a whole test process for one cell is finished
*/
void fISDONE()
{
    /**The function will check if the current cell number (@p cell_count) is smaller than the 
    number of cells to be tested (@p cell_max)*/
    if (cell_count < cell_max)
    {
        __delay_ms(500);
        /**If the condition is @b TRUE the counter will be incremented */       
        cell_count++;
        /**And the testin process of the next cell will be started by going to the @p IDLE state*/
        state = IDLE;   
    }else
    {
        state = STANDBY;
    }    
}

/**@brief This function define the FAULT state of the state machine.
*/
void fFAULT()
{   
    /**The function will stop the converter usign @link STOP_CONVERTER() @endlink macro*/
    STOP_CONVERTER();
    /**The @p state will be set to @p STANDBY*/
    state = STANDBY;
}

/**@brief Function to start the state machine.
*/
void Start_state_machine()
{
    /**First,*/
    switch(option)
    {
        /**Check the option (CHANGE)*/
        case '1':
            /**> if @option is equal to @b 1 it will set the @p state as @p PREDISCHARGE*/
            state = PREDISCHARGE;
            break;
        case '2':
            /**> if @option is equal to @b 2 it will set the @p state as @p CHARGE*/
            state = CHARGE;
            break;
        case '3':
            /**> if @option is equal to @b 3 it will set the @p state as @p CHARGE*/
            state = CHARGE;            
            break;
        case '4':
            /**> if @option is equal to @b 4 it will set the @p state as @p DISCHARGE*/
            state = DISCHARGE;                
            break;
    }
    /**First, this function will declare and initialized to zero a variable called @p start, which will
    be used to store the input of the user.*/ 
    unsigned char start = 0;
    switch (cell_count){
        /**If the current cell is the first (<tt>cell_count</tt>) , it will ask for user intervention to start.*/
        case '1':
            /**It will prompt the user to press @b s.*/
            UART_send_string((char*)press_s_str);
            LINEBREAK;                  
            while(start == 0)                                               
            {
                /**`The key pressed by the user will be assigned to @p start.*/
                start = UART_get_char();
                switch(start)
                {
                    /**If the user press @b s, the program will start.*/
                    case 's': 
                        break;
                    /**The user also can press @b ESC and the program will be restarted to the @p STANBY state.*/ 
                    case 0x1B:
                        state = STANDBY;
                        goto NOSTART;  //go to the end of the function 
                    /**If the user press something different from @b s, or @b ESC the program will print 
                    a warning message and wait for a valid input.*/
                    default:
                        LINEBREAK;
                        UART_send_string((char*)press_s_str);
                        LINEBREAK;
                        start = 0;  //Keep the program inside the while loop 
                        break;
                }
            }
            break;
        /**If the current cell is @b not @b '1' the program will start without 
        the user intervention.*/
        default: 
            break;
    }    
    /**Before starting, the program will print the following:*/
    LINEBREAK; 
    /**Starting...*/
    UART_send_string((char*)starting_str);                                            
    LINEBREAK; 
    LINEBREAK;
    /**Cell {1,2,3 or 4}*/
    UART_send_string((char*)cell_str);
    display_value((int)(cell_count - '0'));
    LINEBREAK; 
    NOSTART: ;  //label to goto the end of the function 
}

/**@brief Function to set the configurations of the converter.
*/
void Converter_settings()
{
    /**Initially, the function set the proportional (@p kp) and integral (@p ki) constants for the PI loop.*/
    kp = 0.025;//kp = 0.025;  
    ki = 0.04;//ki = 0.02;
    /**Then, the system is configured to start in constant current mode by setting. <tt> cmode = 1 </tt>*/        
    cmode = 1;
    /**The integral component of the PI (@p integral) is set to zero.*/
    integral = 0;
    /**Capacity (@p q_prom) is set to zero.*/
    qprom = 0;
    /**Maximum averaged voltage (@p vmax) is set to zero.*/
    vmax = 0;
    /**Clean all the log buffers*/
    ip_buff = 0;
    vp_buff = 0; 
    tp_buff = 0; 
    qp_buff = 0;
    /**@p EOCD_count is defined as @p EOCD_LOOPS. That is, after the system is in EOC condition, the condition 
    needs to be mantained for @p EOCD_LOOPS loop cycles before the system stop the charge process.*/
    EOCD_count = EOCD_LOOPS;
    /**@p CV_count is defined as @p CV_LOOPS. That is, after the system reach the CV voltage, the condition 
    needs to be mantained for @p CV_LOOPS loop cycles before the system change to <b> constant voltage mode </b>.*/
    CV_count = CV_LOOPS;
    /**The initial <b> duty cycle </b> of the PWM is set to @p DC_START*/
    dc = DC_START;
    /**The @link set_DC() @endlink function is called.*/  
    set_DC();
    /**The @link Cell_ON() @endlink function is called.*/
    Cell_ON();
    switch(state)
    {
        /**If the current state is @p POSTCHARGE or @p CHARGE*/
        case POSTCHARGE:
        case CHARGE:
            /**> The current setpoint (@p iref) is defined as @p i_char*/
            iref = i_char; 
            /**> Time out is calculated*/
            timeout = ((capacity / iref) * 66); //10% more than an hour 
            /**> The charge/discharge relay (@p RA0) will be set to the charge position (low)*/
            RA0 = 0;
            break;
        /**If the current state is @p PREDISCHARGE or @p DISCHARGE*/
        case PREDISCHARGE:
        case DISCHARGE:
            /**> The current setpoint (@p iref) is defined as @p i_disc*/
            iref = i_disc;
            /**> The charge/discharge relay (@p RA0) will be set to the discharge position (high)*/
            RA0 = 1;            
            break;
        /**If the current state is @p CS_DC_res, @p DS_DC_res or @p PS_DC_res*/
        case CS_DC_res:
        case DS_DC_res:
        case PS_DC_res:
            /**> The current setpoint (@p iref) is defined as <tt> capacity / 5 </tt>*/
            iref = capacity / 5;
            dc_res_count = 14;
            /**> The charge/discharge relay (@p RA0) will be set to the discharge position (high)*/
            RA0 = 1;            
            break;
    }
    __delay_ms(10);
    /**The timing counter @p count will be intialized to @p COUNTER, to start a full control loop cycle.*/    
    count = 0;
}

/**@brief Function to define the parameters of the testing process for both chemistries.
*/
void param()
{
    /**First, this function will declare and intialize to zero a variable called @p input, which will
    be used to store the input of the user.*/
    unsigned char input = 0;
    /**Then, it will show the pre-set parameters for charging that are:*/
    /**For Li Ion:*/
    /**@code
    Li_Ion_CV = 4200 mV
    Li_Ion_CAP = 3250 mAh 
    @endcode*/ 
    /**For Ni_MH:*/
    /**@code
    Ni_MH_CV = 1700 mV //Just for protection for now, maybe is not necessary to show it
    Ni_MH_CAP = 2500 mAh
    @endcode*/
    LINEBREAK;  
    #if (LI_ION_CHEM)    
    vref = Li_Ion_CV;
    UART_send_string((char*)cv_val_str);
    display_value(Li_Ion_CV);
    UART_send_string((char*)mV_str);
    LINEBREAK;
    /** The @p capacity will be set to @p Li_Ion_CAP.*/
    capacity = Li_Ion_CAP;
    UART_send_string((char*)nom_cap_str);
    display_value(capacity);
    UART_send_string((char*)mAh_str);
    #elif (NI_MH_CHEM) 
    vref = Ni_MH_CV;
    UART_send_string((char*)cv_val_str);
    display_value(Ni_MH_CV);
    UART_send_string((char*)mV_str);
    LINEBREAK;
    /** The @p capacity will be set to @p Ni_MH_CAP.*/
    capacity = Ni_MH_CAP;
    UART_send_string((char*)nom_cap_str);
    display_value(capacity);
    UART_send_string((char*)mAh_str);
    #endif
    LINEBREAK;
    LINEBREAK;    
    /**For the charging current it will print three options:*/
    UART_send_string((char*)def_char_curr_str);
    LINEBREAK;
    /** - 1) 0.25 C*/
    UART_send_string((char*)quarter_c_str);
    LINEBREAK;
    /** - 2) 0.5 C*/
    UART_send_string((char*)half_c_str);
    LINEBREAK;
    /** - 3) 1 C*/
    UART_send_string((char*)one_c_str);
    LINEBREAK;
    LINEBREAK;
    /** .*/
    while(input == 0)
    {
    /**The user should input the desired option.*/
        input = UART_get_char();  //Get the value in the terminal.
        switch(input)
        {   
            /**After chosing the charging current, the program will assign it to @p i_char and print it.*/
            case '1':
                i_char = capacity/4;
                UART_send_string((char*)char_def_quarter_str);  //0.25C
                LINEBREAK;
                break;
            case '2':
                i_char = capacity/2;
                UART_send_string((char*)char_def_half_str);  //0.5C
                LINEBREAK;
                break;
            case '3':
                i_char = capacity;
                UART_send_string((char*)char_def_one_str);  //0.1C
                LINEBREAK;
                break;
                /**Unless the user press @e ESC, in that case the program will be restarted to the @p STANBY state.*/
                case 0x1B:
                state = STANDBY;
                LINEBREAK;
                UART_send_string((char*)restarting_str);  //restarting...
                LINEBREAK; 
                goto ESCAPE;  //go to the end of the function 
                /**If the user press something different from @b 1, @b 2, @b 3 or @b ESC the program will print 
                a warning message and wait for a valid input.*/
                default:
                input = 0;  //stay inside the while loop.
                LINEBREAK;
                UART_send_string((char*)num_1and3_str);  //ask the user to use a number between 1 and 3.
                LINEBREAK;                
                break;
        }
    }
    /**Variable @p input is cleared*/
    input = 0; 
    /**Next, the program will show the end-of-charge parameters that are:*/
    /**For Li Ion, the end-of-charge current:*/
    /**@code
    Li_Ion_EOC_I = 100 mA
    @endcode*/ 
    /**For Ni_MH, the end-of-charge voltage drop:*/
    /**@code
    Ni_MH_EOC_DV = 10 mV
    @endcode*/
    #if (LI_ION_CHEM)
    EOC_current = Li_Ion_EOC_I;
    UART_send_string((char*)EOC_I_str);
    display_value(EOC_current);
    UART_send_string((char*)mA_str);
    #elif (NI_MH_CHEM) 
    UART_send_string((char*)EOC_DV_str);
    display_value(Ni_MH_EOC_DV);
    UART_send_string((char*)mV_str);
    #endif    
    LINEBREAK; 
    LINEBREAK;
    /**For the discharging current it will show three options:*/
    UART_send_string((char*)def_disc_curr_str);
    LINEBREAK;
    /** - 1) 0.25 C*/
    UART_send_string((char*)quarter_c_str);
    LINEBREAK;
    /** - 2) 0.5 C*/
    UART_send_string((char*)half_c_str);
    LINEBREAK;
    /** - 3) 1 C*/
    UART_send_string((char*)one_c_str);
    LINEBREAK;
    LINEBREAK;
    /** .*/
    while(input == 0)
    {
        /**The user should input the desired option.*/
        input = UART_get_char();  //Get the value in the terminal.
        switch (input)
        {
            /**After chosing the discharging current, the program will assign it to @p i_disc and print it.*/
            case '1':
                i_disc = capacity/4;
                UART_send_string((char*)dis_def_quarter_str);  //0.25 C
                LINEBREAK;            
                break;
            case '2':
                i_disc = capacity/2;
                UART_send_string((char*)dis_def_half_str);  //0.5 C
                LINEBREAK;         
                break;
            case '3':
                i_disc = capacity;
                UART_send_string((char*)dis_def_one_str);  //1C
                LINEBREAK;
                break;
            /**Unless the user press @e ESC, in that case the program will be restarted to the @p STANBY state.*/
            case 0x1B:  //ESC button was pressed
                state = STANDBY;
                LINEBREAK;
                UART_send_string((char*)restarting_str);  //restarting...
                LINEBREAK;             
                goto ESCAPE;  //go to the end of the function 
            /**If the user press something different from @e 1, @e 2, @e 3 or @e ESC the program will print 
            a warning message and wait for a valid input.*/
            default:
                input = 0;
                LINEBREAK;
                UART_send_string((char*)num_1and3_str);  //ask the user to use a number between 1 and 3.
                LINEBREAK;                
                break;
        }
    }
    /**Varible @p input is cleared*/
    input = 0;
    /**Next, the program will show the end-of-discharge voltage constant that is: ((CHANGE THIS STYLE))*/
    /**For Li Ion:*/
    /**@code
    Li_Ion_EOD_V = 3000 mV
    @endcode*/ 
    /**For Ni_MH:*/
    /**@code
    Ni_MH_EOD_V = 1000 mV
    @endcode*/
    #if (LI_ION_CHEM)
    EOD_voltage = Li_Ion_EOD_V;
    UART_send_string((char*)EOD_V_str);
    #elif (NI_MH_CHEM)
    EOD_voltage = Ni_MH_EOD_V;
    UART_send_string((char*)EOD_V_str);
    #endif
    display_value(EOD_voltage);
    UART_send_string((char*)mV_str);
    LINEBREAK;
    /**For the test cycle it will show four options:*/
    LINEBREAK;
    UART_send_string((char*)cho_bet_str);
    LINEBREAK;
    /** - 1) Precharge->Discharge->Charge (includes DC resistance measure after charge and after discharge).*/
    UART_send_string((char*)op_1_str);
    LINEBREAK;
    /** - 2) Discharge->Charge (includes DC resistance measure after charge and after discharge).*/
    UART_send_string((char*)op_2_str);
    LINEBREAK;
    /** - 3) Only Charge*/
    UART_send_string((char*)op_3_str);
    LINEBREAK;
    /** - 4) Only Discharge*/
    UART_send_string((char*)op_4_str);
    LINEBREAK;
    LINEBREAK;
    /** .*/
    while(option == 0)
    {
        /**The user should input the desired option, which will be assigned to @p option*/
        option = UART_get_char();  //Get the value in the terminal.
        switch(option)
        {
            /**After that the program will print the selected option and:*/
            case '1':
                LINEBREAK;
                UART_send_string((char*)op_1_sel_str);  //Predischarge->Charge->Discharge->Postcharge
                LINEBREAK;
                break;
            case '2':
                LINEBREAK;
                UART_send_string((char*)op_2_sel_str);  //Charge->Discharge
                LINEBREAK;
                break;
            case '3':
                LINEBREAK;
                UART_send_string((char*)op_3_sel_str);  //Only Charge
                LINEBREAK;         
                break;
            case '4':
                LINEBREAK;
                UART_send_string((char*)op_4_sel_str);  //Only Discharge
                LINEBREAK;             
                break;
            /**Unless the user press @e ESC, in that case the program will be restarted to the @p STANBY state.*/
            case 0x1B:
                state = STANDBY;
                LINEBREAK;
                UART_send_string((char*)restarting_str);
                LINEBREAK;
                goto ESCAPE;  //go to the end of the function 
            /**If the user press something different from @b 1, @b 2, @b 3, @b 4 or @b ESC the program will print 
            a warning message and wait for a valid input.*/
            default:
                option = 0;
                LINEBREAK;
                UART_send_string((char*)num_1and4_str);  //ask the user to use a number between 1 and 4.
                LINEBREAK;
                break;
        }
    }
    LINEBREAK;
    /**Then, the program will ask the user how many cells he wants to test (4 cells is the maximum):*/
    UART_send_string((char*)def_num_cell_str);
    LINEBREAK;
    while(cell_max == 0)
    {
        /**The user should input the desired number, which will be assigned to @p cell_max*/
        cell_max = UART_get_char();  //Get the value in the terminal.
        switch(cell_max)
        {
            /**After that the program will print it.*/
            case '1':
                LINEBREAK;
                UART_send_string((char*)num_cell_str);
                UART_send_string((char*)one_str);
                LINEBREAK;
                break;
            case '2':
                LINEBREAK;
                UART_send_string((char*)num_cell_str);
                UART_send_string((char*)two_str);
                LINEBREAK;
                break;
            case '3':
                LINEBREAK;
                UART_send_string((char*)num_cell_str);
                UART_send_string((char*)three_str);
                LINEBREAK;  
                break;
            case '4':
                LINEBREAK;
                UART_send_string((char*)num_cell_str);
                UART_send_string((char*)four_str);
                LINEBREAK;
                break;
            /**Unless the user press @b ESC, in that case the program will be restarted to the @p STANBY state.*/
            case 0x1B:
                state = STANDBY;
                LINEBREAK;
                UART_send_string((char*)restarting_str);
                LINEBREAK;
                goto ESCAPE;  //go to the end of the function 
            /**If the user press something different from @b 1, @b 2, @b 3, @b 4 or @b ESC the program will print 
            a warning message and wait for a valid input.*/
            default:
                cell_max = 0;   //Keep the program inside the while loop 
                LINEBREAK;
                UART_send_string((char*)num_1and4_str);  //ask the user to use a number between 1 and 4.
                LINEBREAK;
                break;
        }           
    }
    /**After the user has set the number of cells the program will go to the @p IDLE state. @see fIDLE()*/
    state = IDLE;  //go to IDLE state
    ESCAPE: ;  //label to goto the end of the function 
}