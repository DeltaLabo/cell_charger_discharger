/* State machine source file for Charge and Discharge System. */
/* Kyutech Institute of Technology. LaSEINE. Supervisor: Mengu CHO.*/
/* 
 * File:  state_machine.c 
 * Author: Juan J. Rojas.
 * Version control in Git
 */
#include "state_machine.h"
#include "hardware.h"

char const              press_s_str[] = "Press 's' to start: ";
char const              starting_str[] = "Starting...";
char const              done_str[] = "DONE";
char const              state_res_str[] = "-------------S-";
char const              end_state_res_str[] = "-S-------------";
char const              DC_res_str[] = "------------>R";
char const              end_str[] = "<------------";
char const              num_1and2_str[] = "Please input a number between 1 and 2";
char const              num_1and3_str[] = "Please input a number between 1 and 3";
char const              num_1and4_str[] = "Please input a number between 1 and 4";         
char const              param_def_str[] = "---Parameter definition for charger and discharger---";
char const              restarting_str[] = "Restarting...";
char const              chem_def_liion[] = "Chemistry defined as Li-Ion";
char const              chem_def_nimh[] = "Chemistry defined as Ni-MH";
char const              mv_str[] = " mV";
char const              mAh_str[] = " mAh";
char const              mA_str[] = " mA";
char const              EOD_V_str[] = "End of discharge voltage: ";
char const              EOC_I_str[] = "End of charge current: ";
char const              cho_bet_str[] = "Chose between following options: ";
char const              quarter_c_str[] = "(1) 0.25C";
char const              half_c_str[] = "(2) 0.50C";
char const              one_c_str[] = "(3) 1C";
char const              cell_str[] = "Cell "; 
char const              dis_def_quarter_str[] = "Discharge current defined as 0.25C";
char const              dis_def_half_str[] = "Discharge current defined as 0.5C";
char const              dis_def_one_str[] = "Discharge current defined as 1C";
char const              char_def_quarter_str[] = "Charge current defined as 0.25C";
char const              char_def_half_str[] = "Charge current defined as 0.5C";
char const              char_def_one_str[] = "Charge current defined as 1C";
char const              cv_val_str[] = "Constant voltage value: ";
char const              nom_cap_str[] = "Nominal capacity: ";
char const              def_char_curr_str[] = "Define charge current (input the number): ";
char const              def_disc_curr_str[] = "Define discharge current (input the number): ";
char const              def_num_cell_str[] = "Define number of cells to be tested (input the number): ";
char const              num_cell_str[] = "Number of cells to be tested: ";
char const              one_str[] = "1";
char const              two_str[] = "2";
char const              three_str[] = "3";
char const              four_str[] = "4";
char const              li_ion_op_1_str[] = "(1) Precharge->Discharge->Charge";
char const              li_ion_op_2_str[] = "(2) Discharge->Charge";
char const              li_ion_op_3_str[] = "(3) Only Charge";
char const              li_ion_op_4_str[] = "(4) Only Discharge";
char const              li_ion_op_1_sel_str[] = "Precharge->Discharge->Charge selected...";
char const              li_ion_op_2_sel_str[] = "Discharge->Charge selected...";
char const              li_ion_op_3_sel_str[] = "Only Charge selected...";
char const              li_ion_op_4_sel_str[] = "Only Discharge selected...";
char const              cell_below_str[] = "Cell below 0.9V or not present";
char const              in_wait_str[] = "------------W-";
char const              end_wait_str[] = "-W------------";
 
void Init_State_Machine()
{
    INTCONbits.GIE = 0;     //probar ponerlo en los registros
    start = 0;
    option = 0;
    cell_max = 0;
    c_char = 0;
    c_disc = 0;
//    vmax = 0;
    cell_count = 49;
    dc_res_count = 0;
    LOG_OFF();    
}

void State_Machine()
{
    switch(state){
            case STANDBY:
                fSTANDBY();
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

void fSTANDBY()
{
    STOP_CONVERTER();   
    Init_State_Machine();
    Define_Parameters();
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
    LOG_ON();
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
        dc_res_val = (v_1_dcres - v_2_dcres) * 10000;
        dc_res_val = dc_res_val / (i_2_dcres - i_1_dcres);
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
    kp=0.03; 
    ki=0.003;
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

void Define_Parameters()
{
    c_char = 0;
    c_disc = 0;
    option = 0;
    cell_max = 0;
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

void Li_Ion_param ()
{         
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
    //-------CHARGE CURRENT
    UART_send_string((char*)def_char_curr_str);
    LINEBREAK;
    UART_send_string((char*)quarter_c_str);
    LINEBREAK;
    UART_send_string((char*)half_c_str);
    LINEBREAK;
    UART_send_string((char*)one_c_str);
    LINEBREAK;
    LINEBREAK;
    while(c_char == 0)                               //Wait until the value of c_char is defined by the user
    {
        c_char = UART_get_char();                    //Get the value in the terminal.
        switch(c_char)
        {
            case 49:
                i_char = capacity/4;
                UART_send_string((char*)char_def_quarter_str);
                LINEBREAK;
                break;
            case 50:
                i_char = capacity/2;
                UART_send_string((char*)char_def_half_str);
                LINEBREAK;
                break;
            case 51:
                i_char = capacity;
                UART_send_string((char*)char_def_one_str);
                LINEBREAK;
                break;
            case 0x1B:
                c_disc = 0x1B;
                option = 0x1B;
                cell_max = 0x1B;
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

void Start_state_machine()
{   
    switch (cell_count){
        case 49:
            UART_send_string((char*)press_s_str);
            LINEBREAK;
            while(start == 0)
            {
                start = UART_get_char();                    //Get the value in the terminal.
                switch(start)
                {
                    case 115:
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
            if (start != 0x1B)
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
            display_value((long)(cell_count - 48));
            LINEBREAK;   
            break;
    }
}

