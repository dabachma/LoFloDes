#pragma once
/**\class Hyd_Coupling_Point_RV2GW
	\author Bastian Winkels et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_COUPLING_POINT_RV2GW_H
#define HYD_COUPLING_POINT_RV2GW_H


//system hyd class
//base class
#include "_Hyd_Coupling_Point.h"
//Class of the coupling partner (Base class of the profile types)
#include "_Hyd_River_Profile.h"

///Coupling point-class for the coupling between a river- (RV) and groundwater-model (GW) \ingroup hyd
/**
The coupling between the river- and a groundwater-model is a linear coupling. The founded points by a
a geometrical model interception are related to the bank-lines of the river-model (right and left bank lines in flow direction) (Hyd_Model_River).
TODO Rephrase
The flow over the bank-lines is evaluated by the Poleni-formula (overflow). The discharge to/out of the coupling related floodplain standard
element (Hyd_Floodplain_Element_Type_Standard) is set as a discharge boundary condition to this element, constant
over the syncronisation timestep. The discharge to/out of the coupling related profiles is equivalent, but with a opposite sign (+ - /- +).
Generally, there are two related river profiles.
The points are managed and stored in a list (Hyd_Coupling_Point_RV2FP_List).
\see Hyd_Model_Groundwater
*/
class Hyd_Coupling_Point_RV2GW : public _Hyd_Coupling_Point
{
	///Class for managing this points in a list
	friend class Hyd_Coupling_Point_RV2GW_List;

public:
	///Default constructor
	Hyd_Coupling_Point_RV2GW(void);
	//Copy constructor
	Hyd_Coupling_Point_RV2GW(const Hyd_Coupling_Point_RV2GW& object);
	///Default destructor
	~Hyd_Coupling_Point_RV2GW(void);





	//methods
	///Set the pointer/index members (without the geometrical members) from the given point
	void set_couplingpoint_members(Hyd_Coupling_Point_RV2GW *object);

	///Set file name
	void set_file_name(void);

	///Output the header for the setted member (static)
	static void output_header_setted_member(ostringstream *cout);
	///Output the setted  members
	void output_setted_members(ostringstream *cout);

	///Set the indices for the upwards river profile (corresponding to the index of the polysegment of the river bank line) and the element index of the Hyd_Groundwater_Element, which is used for calculation
	void set_indices(const int up_river_profile, const int groundwater_model_elem);
	///Set the indices for the element index of the Hyd_Groundwater_Element, which is used for calculation
	void set_gw_indices(const int groundwater_model_elem);
	///Set the indices for the upwards river profile (corresponding to the index of the polysegment of the river bank line) 
	void set_rv_indices(const int up_river_profile);

	///Set the pointers to the river profiles with help of the indices
	void set_pointer_river_profiles(Hyd_Model_River *river_model);

	///Set the indexof the first found Hyd_Groundwater_Element; it is not necessarily used for calculation (e.g. river elements)
	void set_first_found_elem_index(const int index);
	///Set the index of the groundwater element
	void set_index_groundwater_element(const int index);
	///Get the index of the groundwater element
	int get_index_groundwater_element(void);
	///Set the pointers to the groundwater elements
	void set_pointer_groundwater_element(Hyd_Element_Groundwater *groundwater);

	///Set the index of the coupled groundwater
	void set_groundwater_index(const int index);
	///Get the index of the coupled groundwater
	int get_groundwater_index(void);

	///Get the index of the coupled gw-section
	int get_gw_section_index(void);

	///Get the mid of waterlevel of the two profiles (global) in the river
	double get_mid_waterlevel(void);
	///Calculate the current mid-waterlevel via the factors in the river
	void calculate_mid_waterlevel(void);

	///Get the mid height for the overflow-coupling (global)
	double get_mid_height(void);

	///Get the pointer to the downstream river profile
	_Hyd_River_Profile* get_downstream_rvprofile(void);
	///Get the pointer to the upstream river profile
	_Hyd_River_Profile* get_upstream_rvprofile(void);

	///Get the index of the downstream river profile
	int get_index_downstream_rvprofile(void);
	///Get the index to the upstream river profile
	int get_index_upstream_rvprofile(void);

	///Get the pointer to the groundwater element
	Hyd_Element_Groundwater* get_groundwater_element(void);
	///Get the upwards factor for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
	double get_upwards_mid_factor(void);
	///Get the downwards factor for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
	double get_downwards_mid_factor(void);

	///Get the distance from the coupling point to the element mid point
	double get_distancecoupling2elem_mid(void);

	///Transfer the coupling characteristics of the coupled elements
	void transfer_coupling_characteristics(const bool left_river_flag);

	///Transfer the coupling characteristics of the coupled elements for the midline
	void transfer_coupling_characteristics_mid(void);

	///Reset the current coupling discharge of the points and the coupled element
	void reset_coupling_discharge(const bool left_river_flag);
	///Syncronisation of the coupled models with the couplingspoint
	void syncronisation_coupled_models(const double timepoint, const double delta_t, const bool left_river_flag, const bool time_check, const int internal_counter);

	///Set the flag if a coupling due to a leakage is applicable
	void set_leakage_flag(const bool flag);
	///Set the fixed leakage flag to true; it is not more dependent of the profiles
	void set_fixed_leakage_flag(void);

	///Get maximum waterlevel at this coupling point related to the mid base points of the profiles 
	_hyd_max_values get_max_h(void);

	///Reset the hydrological balance values and the maximum calculated values
	void reset_hydro_balance_max_values(void);
	///Reset the smoothing calculation members
	void reset_smoothing(void);

	///Get the maximum waterlevel gradient 
	double get_maximum_h_grad(void);

	///Output the result members for each coupling points at every timestep to csv file
	void output_result_members_per_timestep2csv(ofstream* file);
	///Copy operator
	Hyd_Coupling_Point_RV2GW& operator=(const Hyd_Coupling_Point_RV2GW& object);


private:
	//members
	
	///Index of the river profile upwards
	int river_profile_index_up;
	///Pointer to the river profile upwards
	_Hyd_River_Profile *river_profile_up;
	///Index of the river profile downwards
	int river_profile_index_down;
	///Pointer to the river profile downwards
	_Hyd_River_Profile *river_profile_down;
	///Index of the coupled groundwater
	int groundwater_index;
	///Index of the groundwater element
	int groundwater_elem_index;
	///Index of the first found coupling element
	int first_found_elem_index;
	///Pointer to the groundwater element
	Hyd_Element_Groundwater *groundwater_elem;

	///Pointer to the River Parameter (Container object for the model members)
	Hyd_Param_RV *Param_RV;

	


	///Index of a Fpl-section
	int gw_section_id;

	///Mid of height of the two profiles
	double mid_height;
	///Mid base point height of the two profiles or the floodplain element
	double mid_basepoint;
	///Mid base point of the two profiles
	double mid_basepoint_profile;
	///Factor (upwards) for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
	double mid_fac_up;
	///Factor (downwards) for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
	double mid_fac_down;
	///Mid of waterlevel of the two profiles in the river
	double mid_waterlevel;

	///final used groundwater height 
	double groundwater_h;
	///final used river height
	double river_h;
	///final used riverbed height
	double riverbed_h;
	///final used flow length
	double flow_length_m;
	///final used wetted perimeter
	double wetted_p;

	///output file name
	string file_name;

	///Total flow width (total width minus fixed width reduction)
	double total_flow_width;

	///Flag if an leakage is desired
	bool leakage_flag;
	///Flag if the leakage flag is fixed (e.g. structure, river junctions)
	bool leakage_flag_fixed;

	///leakage width
	double leakage_width;

	///Maximum waterlevel at this coupling point related to the mid base points of the profiles; this is essential for the coupling to the fpl-system  
	_hyd_max_values max_h;

	///Flag for a horizontal backwater from downstream
	bool horizontal_backwater_flag;
	///Flag for a horizontal backwater from upstream
	bool horizontal_backwater_flag_upstream;


	///Current gradient of leakage discharge
	double grad_q_current;
	///Gradient one syncronisation step before of leakage discharge
	double grad_q_before;
	///Smoother for the oscilations of leakage discharge
	double oscilation_smoother;
	///Counter if there are no oscilation of leakage discharge
	double no_osci_counter;
	///Counter if there are  oscilation of leakage discharge
	double number_osci_counter;


	///Current gradient of discharge due to dikebreak coupling
	double grad_q_break_current;
	///Gradient one syncronisation step before of discharge due to dikebreak coupling
	double grad_q_break_before;
	///Smoother for the oscilations of discharge due to dikebreak coupling
	double oscilation_smoother_break;
	///Counter if there are no oscilation of discharge due to dikebreak coupling
	double no_osci_counter_break;
	///Counter if there are oscilation of discharge due to dikebreak coupling
	double number_osci_counter_break;


	///Predicted waterlevel two (RV)
	double predicted_h_two;
	///Corrected waterlevel two (RV)
	double corrected_h_two;
	///Calculated waterlevel two (RV)
	double calc_h_two;
	///Old calculated waterlvel two (RV)
	double old_calc_h_two;

	///List of gradients of waterlevel two (RV)
	QList <double> gradient_list_h_two;
	///Predicted gradient of waterlevel two (RV)
	double gradient_h_two;

	///Stored gradient one syncronisation step before
	double store_grad_q_before;
	///Stored smoother for the oscilations
	double store_oscilation_smoother;
	///Stored counter if there are  oscilation of overflow discharge
	double store_number_osci_counter;
	///Stored counter if there are no oscilation
	double store_no_osci_counter;

	//methods
	///Transfer the coupling characteristics of the coupled elements for a left river bank (in flow direction)
	void transfer_coupling_characteristics_leftbank(void);
	///Transfer the coupling characteristics of the coupled elements for a right river bank (in flow direction)
	void transfer_coupling_characteristics_rightbank(void);

	///Syncronisation of the coupled models with the couplingspoint for leakage using Darcy Approach
	void syncronisation_coupled_models_Darcy(const double timepoint, const double h_one, const double h_two);
	///Syncronisation of the coupled models with the couplingspoint for leakage using Rushton Approach
	void syncronisation_coupled_models_Rushton(const double timepoint, const double h_one, const double h_two);

	///Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
	double smooth_coupling_discharge(const double q_current, double *old_q);

	///Predict the values
	void predict_values(const int int_counter);

};
#endif
