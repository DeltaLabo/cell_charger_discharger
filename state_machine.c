///**
// * @file state_machine.c
// * @author Juan J. Rojas
// * @date 7 Aug 2018
// * @brief State machine source file for Charge and Discharge System.
// * @par Institution:
// * LaSEINE / CeNT. Kyushu Institute of Technology.
// * @par Mail (after leaving Kyutech):
// * juan.rojas@tec.ac.cr
// * @par Git repository:
// * https://bitbucket.org/juanjorojash/cell_charger_discharger
// */
//
//#include "charger_discharger.h"
//
///**@brief This function contain the transition definition for the states of the machine.
//*/
//void state_machine()
//{
//    switch(state){
//    /**The #IDLE  state goes to the #fIDLE()  function.*/             
//            case IDLE:
//                fIDLE();
//                break;
//    /**The #PREDISCHARGE  and #DISCHARGE  states go to the #fDISCHARGE()  function.*/ 
//            case POSTDISCHARGE:
//            case DISCHARGE:
//                fDISCHARGE();
//                break;
//    /**And the #POSTCHARGE  and #CHARGE  states go to the #fCHARGE()  function.*/  
//            case PRECHARGE:  
//            case CHARGE:   
//                fCHARGE();  
//                break;
//    /**The #CS_DC_res , #DS_DC_res  and #PS_DC_res  states go to the #fDC_res()  function.*/
//            case DC_res:
//                fDC_res();
//                break;
//    /**The #WAIT  state goes to the #fWAIT()  function.*/
//            case WAIT:    
//                fWAIT();
//                break;
//    }
//}
//
///**@brief This function define the #STANDBY  state of the state machine.
//*/
//void fSTANDBY()
//{   
//    STOP_CONVERTER(); /// * Stop the converter by calling the #STOP_CONVERTER() macro
//    RCIE = 0; /// * Disable the USART reception interrupts to avoid interference with the setting of parameters in the #STANDBY state
//    TMR1ON = 0; ///* Disable the Timer1 to avoid interference
//    option = 0; /// * Initialize #option to 0
//    cell_max = 0; /// * Initialize #cell_max to 0
//    cell_count = 1; /// * Initialize #cell_count to '1'
//    LINEBREAK;
//    #if (LI_ION_CHEM) /// If #LI_ION_CHEM  is set to @b 1 and #NI_MH_CHEM  is set to @b 0, the folowing message will be displayed:
//    //UART_send_string((char*)chem_def_liion); /// * <tt> Chemistry defined as Li-Ion </tt>
//    //LINEBREAK;
//    #elif (NI_MH_CHEM) /// If #NI_MH_CHEM  is set to @b 1 and #LI_ION_CHEM  is set to @b 0, the folowing message will be displayed:
//    //UART_send_string((char*)chem_def_nimh); /// * <tt> Chemistry defined as Ni-MH </tt>
//    //LINEBREAK;
//    #endif
//    param(); /// Call the #param() function
//}
//
///**@brief This function define the IDLE state of the state machine.
//*/
//
////This function define the IDLE state of the state machine.
//void fIDLE(){
//    //IN PROGRESS
//    
//    /**At first, the function will call the #Start_state_machine()  function.*/
//    start_state_machine();
//    /**Then, it will call the #Converter_settings()  function.*/
//    converter_settings(); 
//    
//}
//
//
//void fCHARGE()
//{
//    LOG_ON(); /// * Activate the logging by calling #LOG_ON() macro
//    conv = 1; /// * Activate control loop by setting #conv
//    if (vavg < 900) //&& (qavg > 1)) /// If #vavg is below 0.9V
//    {
////        state = FAULT; /// * Go to #FAULT state
////        //UART_send_string((char*)cell_below_str); /// * Send a warning message
////        //LINEBREAK;
//    }
//    if (state == CHARGE){ /// If the #state is #CHARGE
//        #if (LI_ION_CHEM) 
//        /// If the chemistry is Li-Ion
//        if ((iavg < EOC_current)  && (qavg > 100)) /// * If #iavg is below #EOC_current then
//        {                
//            prev_state = state; /// -# Set #prev_state equal to #state
//            state = WAIT; /// -# Else, go to #WAIT state
//            wait_count = WAIT_TIME; /// -# Set #wait_count equal to #WAIT_TIME
//            STOP_CONVERTER(); /// -# Stop the converter by calling #STOP_CONVERTER() macro
//        }
//        #elif (NI_MH_CHEM) 
//        /// If the chemistry is Ni-MH
//        if (((vavg < (vmax - Ni_MH_EOC_DV)) && (qavg > 100)) || minute >= timeout)
//        {
//            prev_state = state; /// -# Set #prev_state equal to #state
//            state = WAIT; /// -# Else, go to #WAIT state
//            wait_count = WAIT_TIME; /// -# Set #wait_count equal to #WAIT_TIME
//            STOP_CONVERTER(); /// -# Stop the converter by calling #STOP_CONVERTER() macro
//        }
//        #endif   
//    } 
//    if (state == PRECHARGE){
//        #if (LI_ION_CHEM) 
//        if (qavg >= ( (capacity * 10) / 2 ) && ((minute + second) >= 1)){
//            prev_state = state;
//            state = WAIT;
//            wait_count = WAIT_TIME;
//            STOP_CONVERTER();
//        }
//        #elif (NI_MH_CHEM)
//        if (qavg >= ( (capacity * 10) / 2 ) || (unsigned) minute >= timeout){
//            prev_state = state;
//            state = WAIT;
//            wait_count = WAIT_TIME;
//            STOP_CONVERTER();
//        }
//        #endif  
//    }    
//}
//
///**@brief This function define the IDLE state of the state machine.
//*/
//
//void fDISCHARGE()
//{
//    LOG_ON(); /// * Activate the logging by calling #LOG_ON() macro
//    conv = 1; /// * Activate control loop by setting #conv
//    if ( vavg < EOD_voltage ) /// * If #vavg is below #EOD_voltage then
//    {
//        prev_state = state; /// -# Set #prev_state equal to #stat
//        state = WAIT; /// -# Else, go to #WAIT state                  
//        wait_count = WAIT_TIME; /// -# Set #wait_count equal to #WAIT_TIME
//        STOP_CONVERTER(); /// -# Stop the converter by calling #STOP_CONVERTER() macro  
//    }
//}
//
///**@brief This function define the IDLE state of the state machine.
//*/
//void fDC_res() //can be improved a lot!!
//{
//    conv = 1; /// * Activate control loop by setting #conv
//    if (dc_res_count == 4)  /// * If #dc_res_count is equal to 4 (CHANGE), then:
//    {
//        v_1_dcres = vavg;
//        i_1_dcres = iavg;
//        iref = (uint16_t) ( ( ( capacity * 4096.0 ) / (5000.0 * 2.5 * 1.0 ) ) + 0.5 );     //1C            
//    }
//    if (dc_res_count == 1)
//    {
//        v_2_dcres = vavg;
//        i_2_dcres = iavg;
//        STOP_CONVERTER();            
//        dc_res_val = (uint24_t)(v_1_dcres - v_2_dcres) * 10000;    
//        dc_res_val = dc_res_val /(uint24_t)(i_2_dcres - i_1_dcres);
//    }
//    if (!dc_res_count)
//    {   
////        LINEBREAK;
////        UART_send_char(C_str);
////        display_value_u((uint16_t)cell_count);
////        UART_send_char(comma);
////        UART_send_char(S_str);
////        display_value_u((uint16_t)state);
////        UART_send_char(comma);
////        UART_send_char(R_str);
////        display_value_u((uint16_t)dc_res_val);
////        UART_send_char('<');
////        LINEBREAK;
//        LOG_OFF();   ///I dont like this 
//        prev_state = state;
//        state = WAIT;
//        wait_count = WAIT_TIME;              
//    }else dc_res_count--;
//}
//
///**@brief This function define the IDLE state of the state machine.
//*/
//void fWAIT()
//{
//    STOP_CONVERTER();  ///MAYBEOK
//    if (wait_count)
//    {   
////        LINEBREAK;
////        UART_send_char(C_str);
////        display_value_u((uint16_t)cell_count);
////        UART_send_char(comma);
////        UART_send_char(S_str);
////        display_value_u((uint16_t)state);
////        UART_send_char(comma);
////        UART_send_char(W_str);
////        display_value_u(wait_count);
////        UART_send_char('<');
//        wait_count--;             
//    }
//    
//    if(!wait_count)
//    {           
////        switch(prev_state)
////        {
////            case PREDISCHARGE:
////                state = CHARGE;
////                converter_settings();
////                break;
////            case CHARGE:
////                state = CS_DC_res; 
////                converter_settings();
////                break;
////            case DISCHARGE:
////                state = DS_DC_res; 
////                converter_settings();
////                break;
////            case POSTCHARGE:
////                state = PS_DC_res;
////                converter_settings();
////                break;
////            case DS_DC_res:
////                if (option == '2'| option == '4') state = ISDONE; /// -# If #option is '2' or '4' then go to #ISDONE state
////                else state = POSTCHARGE;
////                converter_settings();
////                break;
////            case CS_DC_res:
////                if (option == '3') state = ISDONE; /// -# If #option is '3' then go to #ISDONE state
////                else state = DISCHARGE;
////                converter_settings();
////                break;
////            case PS_DC_res:
////                state = ISDONE;
////                STOP_CONVERTER();
////                break;
////        }
//    }
//}
//
/////**@brief This function is executed every time a whole test process for one cell is finished
////*/
////void fISDONE()
////{
////    /**The function will check if the current cell number (@p cell_count) is smaller than the 
////    number of cells to be tested (@p cell_max)*/
////    if (cell_count < cell_max)
////    {
////        UART_send_string((char*)">END<");
////        __delay_ms(500);
////        /**If the condition is @b TRUE the counter will be incremented */       
////        cell_count++;
////        /**And the testin process of the next cell will be started by going to the @p IDLE state*/
////        state = IDLE;   
////    }else
////    {
////        UART_send_string((char*)">END<");
////        state = STANDBY;
////    }    
////}
//
/////**@brief This function define the FAULT state of the state machine.
////*/
////void fFAULT()
////{   
////    /**The function will stop the converter using #STOP_CONVERTER()  macro*/
////    STOP_CONVERTER();
////    /**The @p state will be set to @p STANDBY*/
////    state = STANDBY;
////}
//
///**@brief Function to start the state machine.
//*/
////void start_state_machine()
////{
////    /**First,*/
////    switch(option)
////    {
////        /**Check the option (CHANGE)*/
////        case '1':
////            /**> if @option is equal to @b 1 it will set the @p state as @p PREDISCHARGE*/
////            state = PREDISCHARGE;
////            break;
////        case '2':
////            /**> if @option is equal to @b 2 it will set the @p state as @p CHARGE*/
////            state = CHARGE;
////            break;
////        case '3':
////            /**> if @option is equal to @b 3 it will set the @p state as @p CHARGE*/
////            state = CHARGE;            
////            break;
////        case '4':
////            /**> if @option is equal to @b 4 it will set the @p state as @p DISCHARGE*/
////            state = DISCHARGE;                
////            break;
////    }
////    /**First, this function will declare and initialized to zero a variable called @p start, which will
////    be used to store the input of the user.*/ 
////    unsigned char start = 0;
////    switch (cell_count){
////        /**If the current cell is the first (<tt>cell_count</tt>) , it will ask for user intervention to start.*/
////        case 0x01:
////            /**It will prompt the user to press @b s.*/
////            UART_send_string((char*)press_s_str);
////            LINEBREAK;                  
////            while(start == 0)                                               
////            {
////                /**`The key pressed by the user will be assigned to @p start.*/
////                start = UART_get_char();
////                switch(start)
////                {
////                    /**If the user press @b s, the program will start.*/
////                    case 's': 
////                        break;
////                    /**The user also can press @b ESC and the program will be restarted to the @p STANBY state.*/ 
////                    case 0x1B:
////                        state = STANDBY;
////                        goto NOSTART;  //go to the end of the function 
////                    /**If the user press something different from @b s, or @b ESC the program will print 
////                    a warning message and wait for a valid input.*/
////                    default:
////                        LINEBREAK;
////                        UART_send_string((char*)press_s_str);
////                        LINEBREAK;
////                        start = 0;  //Keep the program inside the while loop 
////                        break;
////                }
////            }
////            break;
////        /**If the current cell is @b not @b '1' the program will start without 
////        the user intervention.*/
////        default: 
////            break;
////    }    
////    /**Before starting, the program will print the following:*/
////    LINEBREAK; 
////    /**Starting...*/
////    UART_send_string((char*)starting_str);                                            
////    LINEBREAK; 
////    LINEBREAK;
////    /**Cell {1,2,3 or 4}*/
////    UART_send_string((char*)cell_str);
////    display_value_u((uint16_t)(cell_count));
////    LINEBREAK; 
////    NOSTART: ;  //label to goto the end of the function 
////}
//
///**@brief Function to set the configurations of the converter.
//*/
//void converter_settings()
//{
//    cmode = 1; /// * Start in constant current mode by setting. #cmode
//    intacum = 0; /// * The #integral component of the compensator is set to zero.*/
//    qavg = 0; /// * Average capacity, #q_prom is set to zero.*/
//    vmax = 0; /// * Maximum averaged voltage, #vmax is set to zero.*/
//    dc = DC_MIN;
//    set_DC(&dc);  /// * The #set_DC() function is called
//    Cell_ON(); /// * The #Cell_ON() function is called
//    switch(state)
//    {
//        case PRECHARGE:
//        case CHARGE: /// If the current state is @p POSTCHARGE or @p CHARGE
//            iref = basic_configuration.const_current_char; /// * The current setpoint, #iref is defined as #i_char
//            timeout = (uint16_t)(((float)capacity / (float)ccref) * 66.0); /// * Charging #timeout is set to 10% more @b only_for}_NIMH
//            SET_CHAR(); /// * The charge/discharge relay is set in charge position by calling the #SET_CHAR() macro
//            break;
//        case POSTDISCHARGE:
//        case DISCHARGE: /// If the current state is @p PREDISCHARGE or @p DISCHARGE
//            iref = basic_configuration.const_current_disc; /// * The current setpoint, #iref is defined as #i_disc
//            SET_DISC(); /// * The charge/discharge relay is set in discharge position by calling the #SET_DISC() macro
//            break;
//        case DC_res: /// If the current state is #CS_DC_res, #DS_DC_res or #PS_DC_res
//            iref = (uint16_t) ( ( ( capacity * 4096.0 ) / (5000 * 2.5 * 5 ) ) + 0.5 ); /// * The current setpoint, #iref is defined as <tt> capacity / 5 </tt>
//            dc_res_count = DC_RES_SECS; /// * The #dc_res_count is set to #DC_RES_SECS
//            SET_DISC(); /// * The charge/discharge relay is set in discharge position by calling the #SET_DISC() macro
//            break;
//    }
//    __delay_ms(10);   
//}
