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
	enum states { STANDBY = 0, IDLE = 1, FAULT = 2, ISDONE = 3, WAIT = 4, PRECHARGE = 5, DISCHARGE = 6, CHARGE = 7, POSTDISCHARGE = 8, DS_DC_res = 9, CS_DC_res = 10};
    
    void fSTANDBY(void);
    void fIDLE(void);
    void fCHARGE(void);
    void fDISCHARGE(void);
    void fDC_res(void);
    void fWAIT(void);
    void fISDONE(void);
    void fFAULT(void);
    void Init_state_machine(void);
    void Start_state_machine(void);
    void Parameter_set(void);
    void State_machine(void);
    void option_50(void);
    void Li_Ion_param(void);
    void Ni_MH_param(void);
    void Converter_settings(void);
    
    /** @def LI_ION_CHEM
    Set to one to choose Li_Ion chemistry
    */
    #define LI_ION_CHEM         1
     /** @def NI_MH_CHEM 
    Set to one to choose Ni-MH chemistry
    */
    #define NI_MH_CHEM          0
   
    //General definitions
    #define CV_LOOPS            2
    #define EOCD_LOOPS          4
    #define WAIT_TIME           600
    #define DC_RES_TIME

    //Li-Ion definitions
    #define Li_Ion_CV           4200
    #define Li_Ion_CAP          3250
    #define Li_Ion_EOC_curr     100
    #define Li_Ion_EOD_volt     3000

    //Ni-MH definitions
    #define Ni_MH_CAP           2500
    #define Ni_MH_EOC_DV        10
    #define Ni_MH_EOD_volt      1000

    //unsigned char           chem;                         //chem = 49 -> Li-Ion, chem = 50-> Ni-MH || Other options can be included in the future  
    unsigned char           option = 0;                     //choose between four options, different for each chemistry, look into Define_Parameter for details
    unsigned int            capacity;                       //definition of capacity per cell according to each chemistry
    unsigned int            i_char;                         //charging current in mA
    unsigned int            i_disc;                         //discharging curretn in mA
    unsigned char           cell_count = 49;                //cell counter from 1 to 4. Initialized as '1'
    unsigned char           cell_max = 0;                   //number of cells to be tested
    unsigned int            EOCD_count;                     //counter for EOC 
    unsigned char           CV_count = CV_LOOPS;            //counter for CV
    //unsigned int            ocv_count;                    //counter for OCV measurement
    unsigned int            wait_count;                     //counter for waiting time between states
    //unsigned int            small_count;                  //counter for waiting time inside states
    unsigned int            dc_res_count = 0;               //counter for DC resistance step
    unsigned char           state;                          //used with enum "states", defined in state_machine.h
    unsigned char           previous_state;                 //used with enum "states" 
    //unsigned int            state_counter;                //used to show numerical value of the state
    unsigned int            EOC_current;                    //end of charge current in mA
    unsigned int            EOD_voltage;                    //end of dischage voltage in mV
    //double                  vmax;                         //maximum recorded voltage
    //double                  In_ocv;
    //double                  Pre_ocv;
    float                   v_1_dcres;
    float                   i_1_dcres;
    float                   v_2_dcres;
    float                   i_2_dcres;
    float                   dc_res_val;
    unsigned int            conv = 0;                       //Turn converter ON and OFF.
    
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
    char const              def_num_cell_str[] = "Define number of cells to be tested (input the number, maximum of 4): ";
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

#endif /* STATE_MACHINE_H*/


