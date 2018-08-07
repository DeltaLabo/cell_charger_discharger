/* State machine source file for Charge and Discharge System. */
/* Kyutech Institute of Technology. LaSEINE. Supervisor: Mengu CHO.*/
/* 
 * File:  hardware.c 
 * Author: Juan J. Rojas.
 * Mail (after leaving Kyutech): juan.rojas@tec.ac.cr
 * Version control in Git: https://bitbucket.org/juanjorojash/cell_charger_discharger
 */

/**
 * @file state_machine.c
 * @author Juan J. Rojas
 * @date 7 Aug 2018
 * @brief File containing example of doxygen usage for quick reference.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol @@.
 * @see https://bitbucket.org/juanjorojash/cell_charger_discharger
 */
#include "state_machine.h"
#include "hardware.h"
/**
 * @brief Example showing how to document a function with Doxygen.
 *
 * Description of what the function does. This part may refer to the parameters
 * of the function, like @p param1 or @p param2. A word of code can also be
 * inserted like @c this which is equivalent to <tt>this</tt> and can be useful
 * to say that the function returns a @c void or an @c int. If you want to have
 * more than one word in typewriter font, then just use @<tt@>.
 * We can also include text verbatim,
 * @verbatim like this@endverbatim
 * Sometimes it is also convenient to include an example of usage:
 * @code
 * BoxStruct *out = Box_The_Function_Name(param1, param2);
 * printf("something...\n");
 * @endcode
 * Or,
 * @code{.py}
 * pyval = python_func(arg1, arg2)
 * print pyval
 * @endcode
 * when the language is not the one used in the current source file (but
 * <b>be careful</b> as this may be supported only by recent versions
 * of Doxygen). By the way, <b>this is how you write bold text</b> or,
 * if it is just one word, then you can just do @b this.
 * @param param1 Description of the first parameter of the function.
 * @param param2 The second one, which follows @p param1.
 * @return Describe what the function returns.
 * @see Box_The_Second_Function
 * @see Box_The_Last_One
 * @see http://website/
 * @note Something to note.
 * @warning Warning.
 */
void State_machine()
{
    switch(state){
            case STANDBY:
                fSTANDBY();             /**<Standy function.*/
                break;                
            case IDLE:
                fIDLE();
                break;
            case PRECHARGE:
            case CHARGE:   
                fCHARGE();
                break;
            case DISCHARGE:
                fDISCHARGE();
                break;
            case CS_DC_res:
            case DS_DC_res:
                fDC_res();
                break;
            case WAIT:    
                fWAIT();
                break;
            case ISDONE:    
                fISDONE();
                break;
            case FAULT:    
                fFAULT();
                break;
    }
}

//!Stand-by state function
/*!
More elaborated desc
@param void
@return void
*/
void fSTANDBY()
{    
    STOP_CONVERTER();   
    Define_param();
    if (cell_max != 0x1B)
    {
        state = IDLE;
    }
}

void fIDLE()
{
    STOP_CONVERTER();               //When switching between cells, the converter need to be stopped, maybe...
    Start_state_machine();          
    UART_interrupt_enable();        //I CHANGE THE POSITION AND IT WAS A MESS PLEASE THINK BEFORE DOING ANYTHING TO THIS    
    if (start != 0x1B)
    {
        count = COUNTER;
        switch (option){
            case 49:
                state = PRECHARGE;
                Converter_settings();
                break;
            case 50:
            case 52:
                state = DISCHARGE;
                Converter_settings();
                break;
            case 51:
                state = CHARGE;
                Converter_settings(); 
                break;
        }                
    }
}

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
    if (iprom < EOC_current)
    {                
        if (!EOCD_count)
        {
            previous_state = state;
            if (state == CHARGE && option == 51) state = ISDONE;
            else state = WAIT;                
            wait_count = wait_time;
            STOP_CONVERTER();                       
        }else EOCD_count--;
    }
}

void fDISCHARGE()
{
    LOG_ON();
    conv = 1;
    if (vprom < EOD_voltage)
    {
        if (!EOCD_count)
        { 
            previous_state = state;
            if (option == 52) state = ISDONE;
            else state = WAIT;                
            wait_count = wait_time;
            STOP_CONVERTER();
        }else EOCD_count--;
    }
}

void fDC_res()
{
    //LOG_ON();
    conv = 1;
    if (dc_res_count == 4)  //Check all this timming
    {
        v_1_dcres = vprom;
        i_1_dcres = iprom;
        SET_CURRENT(capacity);                  
    }
    if (dc_res_count == 1)
    {
        v_2_dcres = vprom;
        i_2_dcres = iprom;
        STOP_CONVERTER();            
        dc_res_val = (unsigned short long)((v_1_dcres - v_2_dcres) * 10000);
        dc_res_val = dc_res_val / (unsigned short long)(i_2_dcres - i_1_dcres);
    }
    if (!dc_res_count)
    {   
        LINEBREAK;
        UART_send_string((char*)DC_res_str);
        display_value(dc_res_val);
        UART_send_string((char*)end_str);
        LINEBREAK;
        LOG_OFF();   ///I dont like this 
        previous_state = state;
        state = WAIT;
        wait_count = wait_time;              
    }else dc_res_count--;
}

void fWAIT()
{
    STOP_CONVERTER();  ///MAYBEOK
    if (wait_count)
    {   
        LINEBREAK;
        UART_send_string((char*)in_wait_str);
        display_value(wait_count);
        UART_send_string((char*)end_wait_str);
        wait_count--;             
    }
    if(!wait_count)
    {           
        switch(previous_state)
        {
            case PRECHARGE:
                state = DISCHARGE;
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
            case DS_DC_res:
                state = CHARGE;
                Converter_settings();
                break;
            case CS_DC_res:
                state = ISDONE;
                STOP_CONVERTER();
                break;
        }
    }
}

void fISDONE()
{
    if (cell_count < cell_max)
    {
        __delay_ms(500);       
        cell_count++;
        state = IDLE;   
    }else
    {
        state = STANDBY;
    }    
}

void fFAULT()
{
    STOP_CONVERTER();
    state = STANDBY;
}


void Converter_settings()
{
    kp=0.025; //0.025 with 0.01 is good
    ki=0.02;
    cmode = 1;
    integral = 0;
    EOCD_count = EOCD_loops;
    CV_count = CV_loops;
    dc = DC_START; 
    set_DC(); 
    Cell_ON();
    switch(state)
    {
        case PRECHARGE:
        case CHARGE:
            SET_CURRENT(i_char); 
            RA0 = 0;
            break;
        case DISCHARGE:
            SET_CURRENT(i_disc);
            RA0 = 1;            
            break;
        case CS_DC_res:
        case DS_DC_res:
            SET_CURRENT(capacity / 5);
            dc_res_count = 14;
            RA0 = 1;            
            break;
    }
    __delay_ms(10);
}

void Define_param()
{
    start = 0;
    c_char = 0;
    c_disc = 0;
    option = 0;
    cell_max = 0;
    cell_count = 49;
    LINEBREAK; 
    UART_send_string((char*)param_def_str);
    LINEBREAK;
    //LI-ION CASE
    UART_send_string((char*)chem_def_liion);
    LINEBREAK;
    LINEBREAK;  
    Li_Ion_param();
    //END OF LI-ION CASE
    //NI-MH CASE
        /*
        UART_send_string((char*)chem_def_nimh);
        LINEBREAK;
        LINEBREAK;  
        Ni_MH_param();
        */
    //END OF NI-MH CASE
}
/** @brief Function to define the parameters of the charge/discharge process fo Li-Ion chemistry*/
void Li_Ion_param ()
{   
    /** This function will first show the pre-set parameters for charging that are:
    - @p Li_Ion_CV = 4200 mV
    - @p Li_Ion_CAP = 3250 mAh */      
    LINEBREAK;
    SET_VOLTAGE(Li_Ion_CV);
    UART_send_string((char*)cv_val_str);
    display_value(Li_Ion_CV);
    UART_send_string((char*)mv_str);
    LINEBREAK;
    capacity = Li_Ion_CAP;
    UART_send_string((char*)nom_cap_str);
    display_value(capacity);
    UART_send_string((char*)mAh_str);
    LINEBREAK;
    LINEBREAK;
    /** For the charging current it will offer three options :
    - 0.2 C
    - 0.5 C
    - 1 C
    */    
    UART_send_string((char*)def_char_curr_str);
    LINEBREAK;
    UART_send_string((char*)quarter_c_str);
    LINEBREAK;
    UART_send_string((char*)half_c_str);
    LINEBREAK;
    UART_send_string((char*)one_c_str);
    LINEBREAK;
    LINEBREAK;
    while(c_char == 0)  //Wait until the value of c_char is defined by the user, which will change it from zero
    {
        c_char = UART_get_char();  //Get the value in the terminal.
        switch(c_char)
        {
            case '1':       
                i_char = capacity/4;  //0.25C
                UART_send_string((char*)char_def_quarter_str);
                LINEBREAK;
                break;
            case '2':
                i_char = capacity/2;  //0.5C
                UART_send_string((char*)char_def_half_str);
                LINEBREAK;
                break;
            case '3':
                i_char = capacity;  //1C      
                UART_send_string((char*)char_def_one_str);
                LINEBREAK;
                break;
            case 0x1B:  //The user pressed ESC
                //Set this three parameters in ESC to used them to jump over the rest of the function code
                c_disc = 0x1B;
                option = 0x1B;
                cell_max = 0x1B;
                //Go to STANBY 
                state = STANDBY;
                LINEBREAK;
                UART_send_string((char*)restarting_str);
                LINEBREAK; 
                break;
            default:
                c_char = 0;
                LINEBREAK;
                UART_send_string((char*)num_1and3_str);
                LINEBREAK;                
                break;
        }
    }
    if (c_char != 0x1B)     //Only show the message if the ESC was not pressed
    {
        EOC_current = Li_Ion_EOC_curr;
        UART_send_string((char*)EOC_I_str);
        display_value(EOC_current);
        UART_send_string((char*)mA_str);
        LINEBREAK; 
        LINEBREAK;
        //-------DISCHARGE CURRENT
        UART_send_string((char*)def_disc_curr_str);
        LINEBREAK;
        UART_send_string((char*)quarter_c_str);
        LINEBREAK;
        UART_send_string((char*)half_c_str);
        LINEBREAK;
        UART_send_string((char*)one_c_str);
        LINEBREAK;
        LINEBREAK;
    }
    while(c_disc == 0)
    {
        c_disc = UART_get_char();                    //Get the value in the terminal.
        switch (c_disc)
        {
            case 49:
                i_disc = capacity/4;
                UART_send_string((char*)dis_def_quarter_str);
                LINEBREAK;            
                break;
            case 50:
                i_disc = capacity/2;
                UART_send_string((char*)dis_def_half_str);
                LINEBREAK;         
                break;
            case 51:
                i_disc = capacity;
                UART_send_string((char*)dis_def_one_str);
                LINEBREAK;
                break;
            case 0x1B:
                option = 0x1B;
                cell_max = 0x1B;
                state = STANDBY;
                LINEBREAK;
                UART_send_string((char*)restarting_str);
                LINEBREAK;             
                break;
            default:
                c_disc = 0;
                LINEBREAK;
                UART_send_string((char*)num_1and3_str);
                LINEBREAK;                
                break;
        }
    }
    if (c_disc != 0x1B)
    {
        //-------EOD voltage
        EOD_voltage = Li_Ion_EOD_volt;
        UART_send_string((char*)EOD_V_str);
        display_value(EOD_voltage);
        UART_send_string((char*)mv_str);
        LINEBREAK;
        //-------Li-Ion case for options
        LINEBREAK;
        UART_send_string((char*)cho_bet_str);
        LINEBREAK;
        UART_send_string((char*)li_ion_op_1_str);
        LINEBREAK;
        UART_send_string((char*)li_ion_op_2_str);
        LINEBREAK;
        UART_send_string((char*)li_ion_op_3_str);
        LINEBREAK;
        UART_send_string((char*)li_ion_op_4_str);
        LINEBREAK;
        LINEBREAK;
    }
    while(option == 0)
    {
        option = UART_get_char();                    //Get the value in the terminal.
        switch(option)
        {
            case 49:
                LINEBREAK;
                UART_send_string((char*)li_ion_op_1_sel_str);
                LINEBREAK;
                break;
            case 50:
                LINEBREAK;
                UART_send_string((char*)li_ion_op_2_sel_str);
                LINEBREAK;
                break;
            case 51:
                LINEBREAK;
                UART_send_string((char*)li_ion_op_3_sel_str);
                LINEBREAK;            
                break;
            case 52:
                LINEBREAK;
                UART_send_string((char*)li_ion_op_4_sel_str);
                LINEBREAK;            
                break;
            case 0x1B:
                cell_max = 0x1B;
                state = STANDBY;
                LINEBREAK;
                UART_send_string((char*)restarting_str);
                LINEBREAK;
                break;
            default:
                option = 0;
                LINEBREAK;
                UART_send_string((char*)num_1and4_str);
                LINEBREAK;
                break;

        }
    }                 
    if (option != 0x1B)
    {
        LINEBREAK;
        UART_send_string((char*)def_num_cell_str);
        LINEBREAK;
    }
    while(cell_max == 0)
    {
        cell_max = UART_get_char();                 //Get the value in the terminal.
        switch(cell_max)
        {
            case 49:
                LINEBREAK;
                UART_send_string((char*)num_cell_str);
                UART_send_string((char*)one_str);
                LINEBREAK;
                break;
            case 50:
                LINEBREAK;
                UART_send_string((char*)num_cell_str);
                UART_send_string((char*)two_str);
                LINEBREAK;
                break;
            case 51:
                LINEBREAK;
                UART_send_string((char*)num_cell_str);
                UART_send_string((char*)three_str);
                LINEBREAK;  
                break;
            case 52:
                LINEBREAK;
                UART_send_string((char*)num_cell_str);
                UART_send_string((char*)four_str);
                LINEBREAK;
                break;
            case 0x1B:
                state = STANDBY;
                LINEBREAK;
                UART_send_string((char*)restarting_str);
                LINEBREAK;
                break;
            default:
                cell_max = 0;
                LINEBREAK;
                UART_send_string((char*)num_1and4_str);
                LINEBREAK;
                break;
        }           
    }
}
/**
 * @brief Function to start the State Machine
 *
 * At first, this function check which is the current cell. If @p cell_count is equal to 
 * <b>'1'</b> it will ask for user intervention to start. It will prompt the user to press <b>'s'</b>.
 * If the user press it, the program will start. The user can also press <b>'ESC'</b> and go 
 * to the @p STANDBY state. If the current cell is <b>not '1'</b> the program will start without 
 * the user intervention. 
 */
void Start_state_machine()
{
    switch (cell_count){        
        case '1':                                                           //*<In case is the cell#1
            UART_send_string((char*)press_s_str);                           //Ask the user to press "s"
            LINEBREAK;                  
            while(start == 0)                                               
            {
                start = UART_get_char();                                    //Get the value in the terminal.
                switch(start)
                {
                    case 's':
                        UART_send_string((char*)starting_str);        
                        LINEBREAK;                                 
                        LINEBREAK;   
                        break;
                    case 0x1B:
                        state = STANDBY;
                        break;
                    default:
                        UART_send_string((char*)press_s_str);
                        LINEBREAK;
                        start = 0; 
                        break;
                }
            }
            if (start != 0x1B)      //The user pressed ESC                                      
            {
                LINEBREAK;
                UART_send_string((char*)cell_str);
                LINEBREAK;           
            }
            break;
        default:
            LINEBREAK; 
            UART_send_string((char*)starting_str);                                            
            LINEBREAK; 
            LINEBREAK;
            UART_send_string((char*)cell_str);
            display_value((int)(cell_count - 48));
            LINEBREAK;   
            break;
    }
}

