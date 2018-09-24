/**
 * @file state_machine.h
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

#ifndef STATE_MACHINE_H
    #define STATE_MACHINE_H
    /** This is the State Machine enum*/
	enum states { 
        STANDBY = 0, ///< "Stand by" state, defined by function @link fSTANDBY() @endlink
        IDLE = 1, ///< "Idle" state, defined by function @link fIDLE() @endlink
        FAULT = 2, ///< "Fault" state, defined by function @link fFAULT() @endlink
        ISDONE = 3, ///< "Is done" state, defined by function @link fISDONE() @endlink
        WAIT = 4, ///< "Wait" state, defined by function @link fWAIT() @endlink
        PREDISCHARGE = 5, ///< "Predischarge" state, defined by function @link fDISCHARGE() @endlink
        CHARGE = 6, ///< "Charge" state, defined by function @link fCHARGE() @endlink
        DISCHARGE = 7, ///< "Discharge" state, defined by function @link fDISCHARGE() @endlink
        POSTCHARGE = 8, ///< "Postcharge" state, defined by function @link fCHARGE() @endlink
        DS_DC_res = 9, ///< "Discharged state DC resistance" state, defined by function @link fDC_res() @endlink
        CS_DC_res = 10, ///< "Charged state DC resistance" state, defined by function @link fDC_res() @endlink
        PS_DC_res = 11 ///< "Postcharged state DC resistance" state, defined by function @link fDC_res() @endlink
    };
    
    void fSTANDBY(void);
    void fIDLE(void);
    void fCHARGE(void);
    void fDISCHARGE(void);
    void fDC_res(void);
    void fWAIT(void);
    void fISDONE(void);
    void fFAULT(void);
    void Start_state_machine(void);
    void State_machine(void);
    void param(void);
    void Converter_settings(void);
    //Chemistry definition
    #define LI_ION_CHEM         0 ///< Set this definition to 1 and NI_MH_CHEM to 0 to set the test Li-Ion cells   
    #define NI_MH_CHEM          1 ///< Set this definition to 1 and LI_ION_CHEM to 0 to set the test Ni-MH cells
    //General definitions
    #define WAIT_TIME           600 ///< Time to wait before states, set to 10 minutes
    #define DC_RES_SECS         14 ///< How many seconds the DC resistance process takes
    //Li-Ion definitions
    #define Li_Ion_CV           4200 ///< Li-Ion constant voltage setting in mV
    #define Li_Ion_CAP          3250 ///< Li-Ion capacity setting in mAh
    #define Li_Ion_EOC_I        100 ///< Li-Ion end-of-charge current in mA
    #define Li_Ion_EOD_V        3000 ///< Li_Ion end-of-discharge voltage in mV
    //Ni-MH definitions
    #define Ni_MH_CV            1700 ///< Ni-MH constant voltage setting in mV
    #define Ni_MH_CAP           2000 ///< Ni-MH capacity setting in mAh
    #define Ni_MH_EOC_DV        10 ///< Ni-MH end-fo-charge voltage drop in mV
    #define Ni_MH_EOD_V         1000 ///< Ni-MH end-of-discharge voltage in mV
    //Variables
    unsigned char               option = 0; ///< Four different options, look into @link param() @endlink for details
    unsigned                    capacity; ///< Definition of capacity per cell according to each chemistry
    unsigned                    i_char; ///< Charging current in mA
    unsigned                    i_disc; ///< Discharging current in mA
    unsigned char               cell_count = 49; ///< Cell counter from '1' to '4'. Initialized as '1'
    unsigned char               cell_max = 0; ///< Number of cells to be tested. Initialized as 0
    unsigned                    wait_count = WAIT_TIME; ///< Counter for waiting time between states. Initialized as @link WAIT_TIME @endlink
    unsigned                    dc_res_count = DC_RES_SECS; ///< Counter for DC resistance. Initialized as @link DC_RES_SECS @endlink
    unsigned char               state = STANDBY; ///< Used with store the value of the @link states @endlink enum. Initialized as @link STANDBY @endlink
    unsigned char               prev_state = STANDBY; ///< Used to store the previous state. Initialized as @link STANDBY @endlink  
    unsigned                    EOC_current; ///< End-of-charge current in mA
    unsigned                    EOD_voltage; ///< End-of-dischage voltage in mV
    float                       v_1_dcres; ///< First voltage measured during DC resistance state 
    float                       i_1_dcres; ///< First current measured during DC resistance state  
    float                       v_2_dcres; ///< Second voltage measured during DC resistance state 
    float                       i_2_dcres; ///< Second current measured during DC resistance state
    float                       dc_res_val; ///< To store the operation of obtained from the DC resistance state
    unsigned                    conv = 0; ///< Turn controller ON(1) or OFF(0). Initialized as 0
    //Strings
    char const                  press_s_str[] = "Press 's' to start: ";
    char const                  starting_str[] = "Starting...";
    char const                  done_str[] = "DONE";
    char const                  num_1and2_str[] = "Please input a number between 1 and 2";
    char const                  num_1and3_str[] = "Please input a number between 1 and 3";
    char const                  num_1and4_str[] = "Please input a number between 1 and 4";         
    char const                  param_def_str[] = "---Parameter definition for charger and discharger---";
    char const                  restarting_str[] = "Restarting...";
    char const                  chem_def_liion[] = "Chemistry defined as Li-Ion";
    char const                  chem_def_nimh[] = "Chemistry defined as Ni-MH";
    char const                  mV_str[] = " mV";
    char const                  mAh_str[] = " mAh";
    char const                  mA_str[] = " mA";
    char const                  EOD_V_str[] = "End of discharge voltage: ";
    char const                  EOC_I_str[] = "End of charge current: ";
    char const                  EOC_DV_str[] = "End of charge voltage drop: ";
    char const                  cho_bet_str[] = "Chose between following options: ";
    char const                  quarter_c_str[] = "(1) 0.25C";
    char const                  half_c_str[] = "(2) 0.50C";
    char const                  one_c_str[] = "(3) 1C";
    char const                  cell_str[] = "Cell "; 
    char const                  dis_def_quarter_str[] = "Discharge current defined as 0.25C";
    char const                  dis_def_half_str[] = "Discharge current defined as 0.5C";
    char const                  dis_def_one_str[] = "Discharge current defined as 1C";
    char const                  char_def_quarter_str[] = "Charge current defined as 0.25C";
    char const                  char_def_half_str[] = "Charge current defined as 0.5C";
    char const                  char_def_one_str[] = "Charge current defined as 1C";
    char const                  cv_val_str[] = "Constant voltage value: ";
    char const                  nom_cap_str[] = "Nominal capacity: ";
    char const                  def_char_curr_str[] = "Define charge current (input the number): ";
    char const                  def_disc_curr_str[] = "Define discharge current (input the number): ";
    char const                  def_num_cell_str[] = "Define number of cells to be tested (input the number, maximum of 4): ";
    char const                  num_cell_str[] = "Number of cells to be tested: ";
    char const                  one_str[] = "1";
    char const                  two_str[] = "2";
    char const                  three_str[] = "3";
    char const                  four_str[] = "4";
    char const                  op_1_str[] = "(1) Predischarge->Charge->Discharge->Postcharge";
    char const                  op_2_str[] = "(2) Charge->Discharge";
    char const                  op_3_str[] = "(3) Only Charge";
    char const                  op_4_str[] = "(4) Only Discharge";
    char const                  op_1_sel_str[] = "Predischarge->Charge->Discharge->Postcharge selected...";
    char const                  op_2_sel_str[] = "Charge->Discharge selected...";
    char const                  op_3_sel_str[] = "Only Charge selected...";
    char const                  op_4_sel_str[] = "Only Discharge selected...";
    char const                  cell_below_str[] = "Cell below 0.9V or not present";

#endif /* STATE_MACHINE_H*/


