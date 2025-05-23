#pragma once
/**\class Hyd_Coupling_GW2GW
	\author Bastian Winkels et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_COUPLING_GW2GW_H
#define HYD_COUPLING_GW2GW_H

//hyd-class
//groundwater model class
#include "Hyd_Model_Groundwater.h"
//list of coupling points
#include "Hyd_Coupling_Point_GW2GW_List.h"

//system_sys_ classes
#include "_Sys_Common_System.h"
//forward
class Hyd_Hydraulic_System;

///Managing-class for a coupling between one groundwater-model (GW) to one groundwater-model (GW) \ingroup hyd
/**
This class manages the coupling between one groundwater-model (GW) and one groundwater-model (GW) via a list of coupling points.
The flow between the models is over the their boundaries. For flow-calculation the Darcy-formula is used.
\see Hyd_Coupling_Point_GW2GW, Hyd_Coupling_Point_GW2GW_List
*/
class Hyd_Coupling_GW2GW
{
public:
	///Default constructor
	Hyd_Coupling_GW2GW(void);
	///Default destructor
	~Hyd_Coupling_GW2GW(void);

	//members
	///The relevant coupling points are stored here
	Hyd_Coupling_Point_GW2GW_List list;

	//methods
	///The pointer of the models for coupling are set
	void set_ptr_coupling(Hyd_Model_Groundwater *groundwater_1, Hyd_Model_Groundwater *groundwater_2);
	///The couplings are initialized
	void init_coupling(void);
	///Synchronise the models
	void synchronise_models(const double timepoint, const double delta_t, const bool time_check, const int internal_counter);

	///Output the header for the coupled model indizes
	static void output_header_coupled_indices(ostringstream *cout);
	///Output the indices of the coupled model
	void output_index_coupled_models(ostringstream *cout, const int number);
	//Output the setted coupling points in the list
	void output_setted_coupling_points(void);

	///Clone the groundwater to groundwater couplings
	void clone_couplings(Hyd_Coupling_GW2GW *coupling, Hyd_Hydraulic_System *system);


private:
	//members
	///Pointer to the groundwater model no. 1
	Hyd_Model_Groundwater *groundwater_model_1;
	///Pointer to the groundwater model no. 2
	Hyd_Model_Groundwater *groundwater_model_2;
	///Defining polysegment is the boundary of the first groundwater model
	Hyd_Groundwater_Polysegment defining_polysegment;

	//methods
	///Set the defining polysegment with the boundary of the first groundwater model
	void set_defining_polysegment(void);


};
#endif
