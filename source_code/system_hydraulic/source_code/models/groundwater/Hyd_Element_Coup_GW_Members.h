#pragma once
/**\class Hyd_Element_Coup_GW_Members
	\author Bastian Winkels et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_ELEMENT_COUP_GW_MEMBERS_H
#define HYD_ELEMENT_COUP_GW_MEMBERS_H

//hyd system


///This class groups the members for a coupling condition in a groundwater element \ingroup hyd
/**

*/
class Hyd_Element_Coup_GW_Members
{
	///Class where the members are required
	friend class Hyd_Element_Groundwater_Type_Standard;

public:
	///Default constructor
	Hyd_Element_Coup_GW_Members(void);
	///Default destructor
	~Hyd_Element_Coup_GW_Members(void);


	//method
	///Reset volumes and discharge
	void reset_vol_q(void);

	///Get the sum of inflow volume
	double get_sum_inflow(void);
	///Get the sum of outflow volume
	double get_sum_outflow(void);

	///Get sum of coupling discharge
	double sum_coupling_discharge(void);

	///Calculate hydrological balance
	void calculate_hydro_balance(const double delta_t);

private:

	//members
	///Discharge which comming from coupled groundwater models
	double coupling_q_gw;
	///Discharge which comming from coupled river models
	double coupling_q_rv_leakage;

	///Make a hydrological balance for the coupling discharge to the element due to a coupling between two groundwater models
	_hyd_hydrological_balance coupling_volume_gw;
	///Make a hydrological balance for the coupling discharge to the element due to a coupling between a groundwater and a river model
	_hyd_hydrological_balance coupling_volume_leakage_rv;

};
#endif
