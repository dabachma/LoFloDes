#pragma once
/**\class Risk_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef RISK_SYSTEM_H
#define RISK_SYSTEM_H

//QT libs
#include <QThread>
//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"
#include "Sys_Multi_CheckBox_Dia.h"
#include "Sys_Project.h"

//system hyd
#include "Hyd_Multiple_Hydraulic_Systems.h"
//system dam
#include "Dam_Damage_System.h"


///Enumerator to distinguish the type of the risk calculation, e.g. peforming a nobreak-risk analysis or a risk analysis for a catchment area \ingroup risk
enum _risk_type {
	///A risk analysis without any break of a fpl-section. Risk is defined as: annuality of flood event times damage
	nobreak_risk,
	///A risk analysis with one break in a fpl-section (scenario based). Hydraulic- and fpl-calculation are independent. Risk is defined as: annuality of sectionfailure times damage
	scenario_risk,
	///A risk analysis with one/multiple break(s) in a  different fpl-section (catchment area based). Hydraulic- and fpl-calculation are connected via the fragility curve.
	catchment_risk
};



///Enumerator to distinguish the type of the thread, e.g. calculation or data handling etc.\ingroup risk
enum _risk_thread_type :short{
	///Thread for performing a nobreak risk analysis
	risk_calc_nobreak,
	///Thread for performing a scenario-based risk analysis
	risk_calc_scenario,
	///Thread for generating the risk states for the catchment area approach (detailed risk result) for the nobreak states
	risk_generate_catchment_state,
	///Thread for performing a catchment area risk analysis (detailed risk result) for the nobreak states
	risk_calc_catchment_nobreak,
	///Thread for performing a catchment area risk analysis (detailed risk result) for the one-break states
	risk_calc_catchment_one_break,
	///Thread for performing a catchment area risk analysis (detailed risk result) for user defined states
	risk_calc_catchment_hand,
	///Thread for performing a catchment area risk analysis (detailed risk result) for automatically generated states
	risk_calc_catchment_auto,
	///Thread for performing a catchment area risk analysis for the cumulation of the risk states
	risk_calc_catchment_cumulated,
	///Thread for calculation and output of the catchment are risk approach
	risk_catchment_statistic,
	///Thread is creating the risk tables for a database project
	risk_create_tab,
	///Thread is checking the risk tables of a database project
	risk_check_tab,
	///Thread for deleting the risk data
	risk_del,
	///Thread for deleting the risk results data of a system state in database
	risk_del_risk_result,
	///Thread for deleting all risk relevant results (fpl, dam, hyd, risk) data of a system state in database 
	risk_del_all_result,
	///Thread for outputing the statistcis of already generated risk-results to display/console
	risk_output_stat_result,
	///Thread for computing different steps of the risk analysis serial
	risk_calc_serial_steps,
	///Thread for a unknown action
	risk_thread_unknown

};
///Namespace for the keystring in a database table of the control parameters for a risk calculation \ingroup risk
namespace risk_control_param{
	///Keystring for identifier of the main outflow river 
	const string main_outflow_rv("MAIN_OUTFLOW_RIVER");
	///Keystring for the maximum of hydraulic calculation for the catchment risk calculation per hydraulic boundary scenario
	const string max_hydraulic("MAX_HYDRAULIC_SIM");
	///Keystring for the boundary of cummulated probability when a search for further break scenarios is stopped (maximum is 1.0)
	const string boundary_prob("BOUNDARY_PROBABILITY");
	///Keystring for number of monte-carlo simulation 
	const string number_mc_sim("NUMBER_MC");
	///Keystring for type of random generator
	const string rand_generator("RAND_GENERATOR");
	///Keystring for number of intervals for dividing the frc-cruves (minimum is 2) 
	const string number_intervals("NUMBER_FRC_INTERVALS");
	///Keystring if predicted values should be used 
	const string use_prediction("USE_PREDICTION");


	///Default control parameter: maximum of hydraulic calculation for the catchment risk calculation
	const int max_hydraulic_def=500;
	///Default control parameter: boundary of cummulated probability when a search for further break scenarios is stopped [-] (maximum is 1.0)
	const double boundary_prob_def=0.9;

	///Default control parameter: boundary of the predicted risk when a search for further break scenarios is stopped [%] (maximum is 100.0 %)
	const double boundary_risk_predicted_def=90.0;

	///Default control parameter: Maximum number of monte-carlo simulation for the generation of the catchment area scenarios
    const int max_mc_def=50000000;
	///Default control parameter: Minimum number of monte-carlo simulation for the generation of the catchment area scenarios
    const int min_mc_def=1000;
	///Default control parameter: Number of monte-carlo simulation for the generation of the catchment area scenarios
	const int no_mc_def=1000000;	
};
///Data structure for gathering the risk results \ingroup risk
struct _risk_risk_results{
	///Results of the damage calculation
	_dam_damage_results dam_results;
	///Maximum outflow of the main river
	double max_outflow;
	///Probability of ths break scenario
	double prob_break_sc;
	///Probability of the hydraulic boundary scenario
	double prob_hyd_bound_sc;
	///Reoccurence probability of the hydraulic boundary scenario
	double reocc_prob_hyd_bound_sc;
	///Annuality of the hydraulic boundary scenario
	int annuality_hyd_bound_sc;
	///Number of applied risk results
	int number_applied_result;
};

///Class for the calculation of the flood risk \ingroup risk
/**
There three types of risk calculation available:
	- a risk analysis without any break of a fpl-section,
	- a pure sceneario based risk analysis,
	- a risk analysis for the catchment area.

*/
class Risk_System: public QThread, public _Sys_Common_System
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT 

public:
	///Default constructor
	Risk_System(void);
	///Default destructor
	~Risk_System(void);


	
};
#endif
