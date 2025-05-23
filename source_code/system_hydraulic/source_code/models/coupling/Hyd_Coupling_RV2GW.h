#pragma once
/**\class Hyd_Coupling_RV2GW
	\author Bastian Winkels et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_COUPLING_RV2GW_H
#define HYD_COUPLING_RV2GW_H

//hyd-class
//Coupling to a coast model of the last profile of a river model
#include "groundwater/Hyd_Model_Groundwater.h"
//river model class
#include "Hyd_Model_River.h"
//parameter container
#include "Hyd_Param_RV.h"
//list of coupling points
//TODO add
//#include "Hyd_Coupling_Point_RV2GW_List.h"

//system_sys_ classes
#include "_Sys_Common_System.h"


///Coupling management-class for the coupling between a river- (RV) and one groundwater-model (GW) \ingroup hyd
/**
The coupling between a river- and one groundwater-model is a linear coupling. The coupling points (Hyd_Coupling_Point_RV2GW)
are gathered and managed in a list (Hyd_Coupling_Point_RV2GW_List). The flow between the models is evaluated as an leakage
via the leakage-formula asA river-model requieres two lists:
One for the left bank-line, one for the right bank-line (in flow direction). These lines are represented
by the outer points (left/right) of the river profiles (_Hyd_River_Profile_Type). In this class just the coupling
between one groundwater- to one river-model is managed. Thus, it gets necessary to merge the lists after evaluation
all couplings of one river-model to diverse groundwater-models. This is done in Hyd_Coupling_RV2GW_Merged. Alternatively the midline can be used instead of the banks

\see Hyd_Coupling_Point_RV2GW, Hyd_Coupling_Point_RV2GW_List
*/
class Hyd_Coupling_RV2GW
{
public:
	///Default constructor
	Hyd_Coupling_RV2GW(void);
	///Default destructor
	~Hyd_Coupling_RV2GW(void);

	//members
	///The relevant coupling points are stored here for the left river bank (in flow direction)
	Hyd_Coupling_Point_RV2GW_List list_left;
	///The relevant coupling points are stored here for the right river bank (in flow direction)
	Hyd_Coupling_Point_RV2GW_List list_right;
	///The relevant coupling points are stored here for the river midline
	Hyd_Coupling_Point_RV2GW_List list_mid;

	//methods
	///The pointer of the models for coupling are set
	void set_ptr_coupling(Hyd_Model_Groundwater *groundwater, Hyd_Model_River *river);
	///The couplings are initialized
	void init_coupling(void);
	///Output the header for the coupled model indizes
	static void output_header_coupled_indices(ostringstream *cout);
	///Output the indices of the coupled model
	void output_index_coupled_models(ostringstream *cout, const int number);
	///Output the setted coupling points in the list
	void output_setted_coupling_points(void);
	///initiate output file for couplingspoints
	void init_output2file_csv(void);

	///Get a pointer to the coupled river model
	Hyd_Model_River* get_river_model(void);
	///Get a pointer to the coupled groundwater model
	Hyd_Model_Groundwater* get_groundwater_model(void);
	///Get the coupled river index
	int get_river_index(void);
	///Get the coupled groundwater index
	int get_groundwater_index(void);
	///Set that the couplings are already merged
	void set_is_merged(void);
	///Get that the couplings are already merged
	bool get_is_merged(void);


private:
	//members
	///Pointer to the coupled coast model
	Hyd_Model_Groundwater *groundwater_model;
	///Pointer to the coupled river model
	Hyd_Model_River *river_model;
	///Flag if the couplings are already merged
	bool is_merged_flag;

	//methods
	///Set error(s)
	Error set_error(const int err_type);

};
#endif
#pragma once
