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

#include "charger_discharger.h"

/**@brief This function contain the transition definition for the states of the machine.
*/
void state_machine()
{
    switch(state){
    /* The #IDLE  state goes to the #fIDLE()  function.*/             
            case IDLE:
                fIDLE();
                break;
    /* The #POSTDISCHARGE  and #DISCHARGE  states go to the #fDISCHARGE()  function.*/ 
            case POSTDISCHARGE:
                fDISCHARGE();
                break;
            case DISCHARGE:
                fDISCHARGE();
                break;
                
    /**And the #PRECHARGE  and #CHARGE  states go to the #fCHARGE()  function.*/  
            case PRECHARGE: 
                fCHARGE();  
                break;
            case CHARGE:   
                fCHARGE();  
                break;
                
    /**The #CS_DC_res , #DS_DC_res  and #PS_DC_res  states go to the #fDC_res()  function.*/
            case DC_res:
                fDC_res();
                break;
    /**The #WAIT  state goes to the #fWAIT()  function.*/
            case WAIT:    
                fWAIT();
                break;
    }
}

void fIDLE()
{
    //Por Desarrollar
}

void fCHARGE()
{
    LOG_ON(); /// * Activate the logging by calling #LOG_ON() macro
    conv = 1; /// * Activate control loop by setting #conv
    if ( vavg < EOD_voltage ) /// * If #vavg is below #EOD_voltage then
    {
        prev_state = state; /// -# Set #prev_state equal to #stat
        state = WAIT; /// -# Else, go to #WAIT state                  
        wait_count = WAIT_TIME; /// -# Set #wait_count equal to #WAIT_TIME
        STOP_CONVERTER(); /// -# Stop the converter by calling #STOP_CONVERTER() macro  
    }
}

void fDISCHARGE()
{
    LOG_ON(); /// * Activate the logging by calling #LOG_ON() macro
    conv = 1; /// * Activate control loop by setting #conv
    if ( vavg < EOD_voltage ) /// * If #vavg is below #EOD_voltage then
    {
        prev_state = state; /// -# Set #prev_state equal to #stat
        state = WAIT; /// -# Else, go to #WAIT state                  
        wait_count = WAIT_TIME; /// -# Set #wait_count equal to #WAIT_TIME
        STOP_CONVERTER(); /// -# Stop the converter by calling #STOP_CONVERTER() macro  
    }
}

/**@brief This function define the IDLE state of the state machine.
*/
void fDC_res() //can be improved a lot!!
{
    conv = 1; /// * Activate control loop by setting #conv
    if (dc_res_count == 4)  /// * If #dc_res_count is equal to 4 (CHANGE), then:
    {
        v_1_dcres = vavg;
        i_1_dcres = iavg;
        iref = (uint16_t) ( ( ( capacity * 4096.0 ) / (5000.0 * 2.5 * 1.0 ) ) + 0.5 );     //1C            
    }
    if (dc_res_count == 1)
    {
        v_2_dcres = vavg;
        i_2_dcres = iavg;
        STOP_CONVERTER();            
        dc_res_val = (uint24_t)(v_1_dcres - v_2_dcres) * 10000;    
        dc_res_val = dc_res_val /(uint24_t)(i_2_dcres - i_1_dcres);
    }
    if (!dc_res_count)
    {   
//        LINEBREAK;
//        UART_send_char(C_str);
//        display_value_u((uint16_t)cell_count);
//        UART_send_char(comma);
//        UART_send_char(S_str);
//        display_value_u((uint16_t)state);
//        UART_send_char(comma);
//        UART_send_char(R_str);
//        display_value_u((uint16_t)dc_res_val);
//        UART_send_char('<');
//        LINEBREAK;
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
//        LINEBREAK;
//        UART_send_char(C_str);
//        display_value_u((uint16_t)cell_count);
//        UART_send_char(comma);
//        UART_send_char(S_str);
//        display_value_u((uint16_t)state);
//        UART_send_char(comma);
//        UART_send_char(W_str);
//        display_value_u(wait_count);
//        UART_send_char('<');
        wait_count--;             
    }
    if(!wait_count)
    {           
//        switch(prev_state)
//        {
//            case PREDISCHARGE:
//                state = CHARGE;
//                converter_settings();
//                break;
//            case CHARGE:
//                state = CS_DC_res; 
//                converter_settings();
//                break;
//            case DISCHARGE:
//                state = DS_DC_res; 
//                converter_settings();
//                break;
//            case POSTCHARGE:
//                state = PS_DC_res;
//                converter_settings();
//                break;
//            case DS_DC_res:
//                if (option == '2'| option == '4') state = ISDONE; /// -# If #option is '2' or '4' then go to #ISDONE state
//                else state = POSTCHARGE;
//                converter_settings();
//                break;
//            case CS_DC_res:
//                if (option == '3') state = ISDONE; /// -# If #option is '3' then go to #ISDONE state
//                else state = DISCHARGE;
//                converter_settings();
//                break;
//            case PS_DC_res:
//                state = ISDONE;
//                STOP_CONVERTER();
//                break;
//        }
    }
}

/**@brief Function to set the configurations of the converter.
*/
void converter_settings()
{
    cmode = 1; /// * Start in constant current mode by setting. #cmode
    intacum = 0; /// * The #integral component of the compensator is set to zero.*/
    qavg = 0; /// * Average capacity, #q_prom is set to zero.*/
    vmax = 0; /// * Maximum averaged voltage, #vmax is set to zero.*/
    dc = DC_MIN;
    set_DC(&dc);  /// * The #set_DC() function is called
    Cell_ON(); /// * The #Cell_ON() function is called
    switch(state)
    {
        case PRECHARGE:
        case CHARGE: /// If the current state is @p POSTCHARGE or @p CHARGE
            iref = basic_configuration.const_current_char; /// * The current setpoint, #iref is defined as #i_char
            timeout = (uint16_t)(((float)capacity / (float)ccref) * 66.0); /// * Charging #timeout is set to 10% more @b only_for}_NIMH
            SET_CHAR(); /// * The charge/discharge relay is set in charge position by calling the #SET_CHAR() macro
            break;
        case POSTDISCHARGE:
        case DISCHARGE: /// If the current state is @p PREDISCHARGE or @p DISCHARGE
            iref = basic_configuration.const_current_disc; /// * The current setpoint, #iref is defined as #i_disc
            SET_DISC(); /// * The charge/discharge relay is set in discharge position by calling the #SET_DISC() macro
            break;
        case DC_res: /// If the current state is #CS_DC_res, #DS_DC_res or #PS_DC_res
            iref = (uint16_t) ( ( ( capacity * 4096.0 ) / (5000 * 2.5 * 5 ) ) + 0.5 ); /// * The current setpoint, #iref is defined as <tt> capacity / 5 </tt>
            dc_res_count = DC_RES_SECS; /// * The #dc_res_count is set to #DC_RES_SECS
            SET_DISC(); /// * The charge/discharge relay is set in discharge position by calling the #SET_DISC() macro
            break;
    }
    __delay_ms(10);   
}
