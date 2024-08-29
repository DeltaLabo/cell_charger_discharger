/**
 * @file state_machine_b.c
 * @author Juan J. Rojas
 * @date 13 Feb 2024
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


void fIDLE() //@brief This function define the IDLE state of the state machine.
{
    // REVISION DE FUNCIONALIDAD
    if (start) STOP_CONVERTER(); // MAYBE OK ALEX
    start = false;
    
}

void fCHARGE()
{    
    if ( ( ( iavg < basic_configuration.end_of_charge ) && ( basic_configuration.version == 0x01 ) ) || ( ( ( vavg < (vmax - 5) ) || (timeout < second) ) && ( basic_configuration.version == 0x02 ) ) ) /// * If #vavg is below #EOD_voltage
    {
        if (second > 5)
        {
            state = WAIT; /// -# Else, go to #WAIT state          
            wait_count = getTime(); /// -# Set #wait_count equal to the time set
            STOP_CONVERTER();
        }
    }
    if (state == PRECHARGE){
        if (basic_configuration.version == 1)
        { 
            if (qavg >= ( (capacity) / 2 ) && (second >= 1))
            {
                state = WAIT;
                wait_count = getTime(); /// -# Set #wait_count equal to the time set
                STOP_CONVERTER();
            }
        }
        else if (basic_configuration.version == 2)
        {
            if (vavg >= basic_configuration.end_of_precharge || qavg >= ( (capacity) / 2 ) || (second >= timeout))
            {
                state = WAIT;
                wait_count = getTime(); /// -# Set #wait_count equal to the time set
                STOP_CONVERTER();
            }
        }
    }
}

void fDISCHARGE()
{    
    if ( (((uint16_t) ( ( ( (float)vavg * 5000.0 ) / 4096.0 ) + 0.5 )) < basic_configuration.end_of_discharge) ) /// * If #vavg is below #EOD_voltage then
    {
        if (second > 5)
        {
            state = WAIT; /// -# Else, go to #WAIT state                  
            wait_count = getTime(); /// -# Set #wait_count equal to #WAIT_TIM
            STOP_CONVERTER();
        }
    }
    if (state == POSTDISCHARGE)
    {
        if (basic_configuration.version == 1) // If Lition Ion
        {
            if (qavg >= ( (capacity) / 2 ) && (second >= 1))
            {
                state = WAIT;
                wait_count = getTime(); /// -# Set #wait_count equal to the time set
                STOP_CONVERTER();
            }
        }
        else if (basic_configuration.version == 2) // If NiMH
        {
            if (((uint16_t) ( ( ( (float)vavg * 5000.0 ) / 4096.0 ) + 0.5 )) <= basic_configuration.end_of_postdischarge || qavg >= ( (capacity) / 2 ) || (second >= timeout))
            {
                state = WAIT;
                wait_count = getTime(); /// -# Set #wait_count equal to the time set
                STOP_CONVERTER();
            }
        }
    }
}

void fDC_res() //can be improved a lot!!
{
    // POR VERIFICAR
    
    if (dc_res_count == 202)  /// * If #dc_res_count is equal to 4 (CHANGE), then:
    {
        iref = (uint16_t) ( ( ( capacity * 4096.0 ) / (5000.0 * 2.5) ) * 0.75 + 0.5 );     //0.75 C Discharge            
    }
    if (dc_res_count == 100)  /// * If #dc_res_count is equal to 4 (CHANGE), then:
    {
        iref = 0;
    }
    if (dc_res_count == 60)  /// * If #dc_res_count is equal to 4 (CHANGE), then:
    {
        iref = (uint16_t) ( ( ( capacity * 4096.0 ) / (5000.0 * 2.5) ) * 0.75 + 0.5 );      //0.75 C Charge
        SET_CHAR();         
    }
    if (dc_res_count == 40)  /// * If #dc_res_count is equal to 4 (CHANGE), then:
    {
        iref = (uint16_t) ( ( ( capacity * 4096.0 ) / (5000.0 * 2.5) ) + 0.5 );      //1 C Charge        
    }
    if (!dc_res_count)
    {   
        STOP_CONVERTER();
        wait_count = getTime(); 
        state = WAIT;
                     
    }else dc_res_count--;
}

/**@brief This function define the IDLE state of the state machine.
*/
void fWAIT()
{
    STOP_CONVERTER();
    second = wait_count;
    if (wait_count) wait_count--;
    if(!wait_count) fNEXTSTATE();
}

void fNEXTSTATE(){
    counter_state = counter_state + 1;
    if ((counter_state <= test_configuration.number_of_states) && (test_configuration.order_of_states[counter_state] != 0x00)){
        state = test_configuration.order_of_states[counter_state];
        converter_settings();
    }
    else{
        fNEXTCELL();
    }
}

void fNEXTCELL(){
    counter_state = 0;
    if (cell_count < test_configuration.number_of_cells){
        state = test_configuration.order_of_states[counter_state];
        cell_count = cell_count + 1;
        converter_settings();
    }
    else {
        fNEXTREPETITION();
    }
}

void fNEXTREPETITION(){
    if (repetition_counter < test_configuration.number_of_repetitions){
        counter_state = 0;
        state = test_configuration.order_of_states[counter_state];
        cell_count = 0x01;
        repetition_counter = repetition_counter + 1;
        converter_settings();
        }
    else{
        state = IDLE;
    }
}

/**@brief Function to set the configurations of the converter.
*/
void converter_settings()
{
    // POR VERIFICAR 
    cmode = 1; /// * Start in constant current mode by setting. #cmode
    pidi = 0; /// * The #integral component of the compensator is set to zero.*/
    qavg = 0; /// * Average capacity, #q_prom is set to zero.*/
    vmax = 0; /// * Maximum averaged voltage, #vmax is set to zero.*/
    pidt = DC_MIN;
    set_DC();  /// * The #set_DC() function is called
    Cell_ON(); /// * The #Cell_ON() function is called
    switch(state)
    {
        case CHARGE: /// If the current state is @p POSTCHARGE or @p CHARGE
            iref = i_char; /// * The current setpoint, #iref is defined as #i_char
            if(basic_configuration.version == 0x02) timeout = (uint16_t)(((float)capacity / (float)basic_configuration.const_current_char) * 3600 * 1.1); /// * Charging #timeout is set to 10% more @b only_for}_NIMH
            SET_CHAR(); /// * The charge/discharge relay is set in charge position by calling the #SET_CHAR() macro
            break;
        case DISCHARGE: /// If the current state is @p PREDISCHARGE or @p DISCHARGE
            iref = i_disc; /// * The current setpoint, #iref is defined as #i_disc
            SET_DISC(); /// * The charge/discharge relay is set in discharge position by calling the #SET_DISC() macro
            break;
        case DC_res: /// If the current state is #DC_RES
            iref = (uint16_t) ( ( ( capacity * 4096.0 ) / (5000 * 2.5) ) + 0.5 ); /// * The current setpoint, #iref is defined as <tt> capacity </tt>
            dc_res_count = DC_RES_SECS; /// * The #dc_res_count is set to #DC_RES_SECS
            SET_DISC(); /// * The charge/discharge relay is set in discharge position by calling the #SET_DISC() macro
            break;
        case POSTDISCHARGE:
            iref = (uint16_t) ( ( ( capacity * 4096.0 ) / (5000 * 2.5 * 2 ) ) + 0.5 ); /// * The current setpoint, #iref is defined as <tt> capacity / 5 </tt>
            if(basic_configuration.version == 0x02) timeout = (uint16_t)(((float)capacity / (float)basic_configuration.const_current_char) * 3600 * 1.1);
            SET_DISC();
            break;
        case PRECHARGE:
            iref = (uint16_t) ( ( ( capacity * 4096.0 ) / (5000 * 2.5 * 2 ) ) + 0.5 ); /// * The current setpoint, #iref is defined as <tt> capacity / 5 </tt>
            if(basic_configuration.version == 0x02) timeout = (uint16_t)(((float)capacity / (float)basic_configuration.const_current_char) * 3600 * 1.1);
            SET_CHAR();
            break;
    }
    __delay_ms(10); 
    second = 0;
    conv = 1;
}
