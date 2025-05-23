#pragma once
/**\class Hyd_Coupling_Point_GW2GW_List
	\author Bastian Winkels et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_COUPLING_POINT_GW2GW_LIST_H
#define HYD_COUPLING_POINT_GW2GW_LIST_H

//system_hyd
#include "Hyd_Coupling_Point_GW2GW.h"
//base class
#include "_Hyd_Coupling_Point_List_GW.h"

///List-class where the coupling points between a groundwater- (GW) and a other groundwater-models (GW) are stored and managed \ingroup hyd
/**
In this class the coupling points between a groundwater- (GW) and a other groundwater-models (GW) are stored and managed in a list.
The related polysegment to the list is the intercepting boundary polysegment between the two groundwater-models (Hyd_Model_Groundwater).
This boundary polysegment is evaluated via geometrical interception.
This list is managed in Hyd_Coupling_GW2GW.
\see Hyd_Coupling_PointGW2GW, Hyd_Model_Groundwater
*/
class Hyd_Coupling_Point_GW2GW_List : public _Hyd_Coupling_Point_List_GW
{


public:
	///Default constructor
	Hyd_Coupling_Point_GW2GW_List(void);
	///Default destructor
	~Hyd_Coupling_Point_GW2GW_List(void);

	//methods
	///Get the number of coupling points
	int get_number_couplings(void);
	///Get the total list as a pointer
	Hyd_Coupling_Point_GW2GW* get_total_list(void);
	///Get a specific coupling points with the given index
	Hyd_Coupling_Point_GW2GW get_couplingpoint(const int index);
	///Get a specific coupling points pointer with the given index
	Hyd_Coupling_Point_GW2GW* get_ptr_couplingpoint(const int index);
	///Get the first coupling points of the list
	Hyd_Coupling_Point_GW2GW get_first_couplingpoint(void);
	///Get the last coupling points of the list
	Hyd_Coupling_Point_GW2GW get_last_couplingpoint(void);

	///Output the setted members
	void output_setted_members(ostringstream *cout);

	///Set a new coupling points
	void set_new_couplingpoint(Hyd_Coupling_Point_GW2GW *new_point);

	///Delete the list
	void delete_list(void);

	///Sort the points after the distance to the beginning of the defining polysegment
	void sort_distance_along_polysegment(void);

	///Calculate for all points the distance to the next upward point and downward point
	void calculate_alldistance_up_down(void);

	///Complete the coupling points with the coupled element indices
	void complete_couplingpoints(Hyd_Model_Groundwater *gw1, Hyd_Model_Groundwater *gw2);

	///Transfer the information of the coupled elements/profiles etc to the coupling points
	void transfer_informations2points(void);

	///Reset the coupling disscharges of the coupling points
	void reset_coupling_discharge(void);
	///Reset the coupling points for starting a new simulation without deleting the object
	void reset_points(void);

	///Syncronisation of the coupled models with the stored couplingspoints in the list
	void syncronisation_models_bylistpoints(const double timepoint, const double delta_t, const bool time_check, const int internal_counter);

	///Clone the coupling point list
	void clone_list(Hyd_Coupling_Point_GW2GW_List *list, Hyd_Model_Groundwater *gw1, Hyd_Model_Groundwater *gw2);

	//Output the discharge list to file (just for testing)
	//void output_discharge_lists(void);


private:

	//members
	///Coupling points in list
	Hyd_Coupling_Point_GW2GW *points;


	//methods
	///Delete the allocated coupling points
	void delete_couplingpoints(void);
	///Allocate the coupling points
	void allocate_couplingpoints(void);

	///Calculate the distance to the startpoint
	void calculate_distances(void);


	///Set error(s)
	Error set_error(const int err_type);
};
#endif
