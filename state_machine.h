/* Hardware related definitions for Charge and Discharge System. */

/* Kyutech Institute of Technology. LaSEINE. Supervisor: Mengu CHO.*/
/* 
 * File:  state_machine.h 
 * Author: Juan J. Rojas.
 * Comments: Release
 * Date: 06/14/17. Version: Git
 */

#ifndef STATE_MACHINE_H
    #define STATE_MACHINE_H
	enum states { STANDBY = 0, IDLE = 1, FAULT = 2, ISDONE = 3, WAIT = 4, PRECHARGE = 5, DISCHARGE = 6, CHARGE = 7, POSTDISCHARGE = 8, DS_DC_res = 9, CS_DC_res = 10};
    
    void fSTANDBY(void);
    void fIDLE(void);
    void Init_State_Machine(void);
    void Start_state_machine(void);
    void Parameter_Set(void);
    void State_Machine(void);
    void Li_Ion_states_p1(void);
    void Li_Ion_states_p2(void);
    void option_50(void);
    void Define_Parameters(void);
    void Li_Ion_param(void);
    void Ni_MH_param(void);
    void Calculate_DC_res(void);


   
    //General definitions
    #define CV_loops            4
    #define EOCD_loops           4
    #define OCV_time            976
    #define wait_time           10//600

    //Li-Ion definitions
    #define Li_Ion_CV           4200
    #define Li_Ion_CAP          3250
    #define Li_Ion_EOC_curr     100
    #define Li_Ion_EOD_volt     3000

    //Ni-MH definitions
    #define Ni_MH_CAP           2500
    #define Ni_MH_EOC_DV        10
    #define Ni_MH_EOD_volt      1000

    unsigned char           start;                      //variable to receive the start command
    //unsigned char           chem;                       //chem = 49 -> Li-Ion, chem = 50-> Ni-MH || Other options can be included in the future  
    unsigned char           c_char;                     //option for charging current in C
    unsigned char           c_disc;                     //option for discharging current in C
    unsigned char           option;                     //choose between four options, different for each chemistry, look into Define_Parameter for details
    unsigned int            capacity;                   //definition of capacity per cell according to each chemistry
    unsigned int            i_char;                     //charging current in mA
    unsigned int            i_disc;                     //discharging curretn in mA
    unsigned char           cell_count;                 //cell counter from 1 to 4.
    unsigned char           cell_max;                   //number of cells to be tested
    unsigned int            EOCD_count;                 //counter for EOC 
    unsigned char           CV_count = CV_loops;        //counter for CV
    //unsigned int            ocv_count;                  //counter for OCV measurement
    unsigned int            wait_count;                 //counter for waiting time between states
    //unsigned int            small_count;                //counter for waiting time inside states
    unsigned int            dc_res_count;               //counter for DC resistance step
    unsigned char           state;                      //used with enum "states", defined in state_machine.h
    unsigned char           previous_state;             //used with enum "states" 
    //unsigned int            state_counter;              //used to show numerical value of the state
    unsigned int            EOC_current;                //end of charge current in mA
    unsigned int            EOD_voltage;                //end of dischage voltage in mV
    //double                  vmax;                       //maximum recorded voltage
    //double                  In_ocv;
    //double                  Pre_ocv;
    unsigned int            v_1_dcres;
    unsigned int            i_1_dcres;
    unsigned int            v_2_dcres;
    unsigned int            i_2_dcres;
    unsigned short long     dc_res_val;
    
#endif /* STATE_MACHINE_H*/


