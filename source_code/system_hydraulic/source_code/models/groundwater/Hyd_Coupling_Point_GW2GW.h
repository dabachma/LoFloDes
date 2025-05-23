#pragma once
/**\class Hyd_Coupling_Point_GW2GW
	\author Bastian Winkels et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_COUPLING_POINT_GW2GW_H
#define HYD_COUPLING_POINT_GW2GW_H


//system hyd
//base class
#include "_Hyd_Coupling_Point.h"

///Coupling point-class for the coupling between a groundwater- (GW) and a groundwater-model (GW) \ingroup hyd
/**
The coupling between a groundwater- and a groundwater-model is a linear coupling. The founded points by a
a geometrical model interception are related to the intercepting boundary of the groundwater-models (Hyd_Model_Groundwater).
The flow over the boundary-line is evaluated by the Darcy-formula. The discharge to/out of the coupling related groundwater standard
element (Hyd_Groundwater_Element_Type_Standard) is set as a discharge boundary condition to this element, constant
over the syncronisation timestep. The discharge to the other groundwater-model is equivalent, but with a opposite sign (+ - /- +).
The points are managed and stored in a list (Hyd_Coupling_Point_GW2GW_List).
*/
class Hyd_Coupling_Point_GW2GW : public _Hyd_Coupling_Point
{
	///Class for managing this points in a list
	friend class Hyd_Coupling_Point_GW2GW_List;

public:
	///Default constructor
	Hyd_Coupling_Point_GW2GW(void);
	//Copy constructor
	Hyd_Coupling_Point_GW2GW(const Hyd_Coupling_Point_GW2GW& object);
	///Default destructor
	~Hyd_Coupling_Point_GW2GW(void);


	//method
	///Output the header for the setted member (static)
	static void output_header_setted_member(ostringstream *cout);
	///Output the setted members
	void output_setted_members(ostringstream *cout);

	///Set the index of the elements for the first groundwater model
	void set_first_gwelem_index(const int first_model_elem);
	///Set the index of the elements for the second groundwater model
	void set_second_gwelem_index(const int second_model_elem);
	///Set the pointer to the elements of the first groundwater model
	void set_first_gwelem_pointer(Hyd_Element_Groundwater *gw1_elem);
	///Set the pointer to the elements of the second groundwater model
	void set_second_gwelem_pointer(Hyd_Element_Groundwater *gw2_elem);
	///Set the direction of coupling
	void set_direction_coupling(const bool x_direction);
	///Set the distance for the Darcy-flow calculation from the first groundwater (smaller size of elements)
	void set_flow_distance(Hyd_Model_Groundwater *gw1);

	///Transfer the coupling characteristics of the coupled elements
	void transfer_coupling_characteristics(void);
	///Reset the current coupling discharge of the points and the coupled element
	void reset_coupling_discharge(void);
	///Syncronisation of the coupled models with the couplingspoint
	void syncronisation_coupled_models(const double timepoint, const double delta_t, const bool time_check, const int internal_counter);

	///Reset the smoothing calculation members
	void reset_smoothing(void);

	//Output the discharges  to file (just for testing)
	//void output_discharge_list(void);




	///Copy operator
	Hyd_Coupling_Point_GW2GW& operator=(const Hyd_Coupling_Point_GW2GW& object);

private:
	//members
	///Index of the first groundwater element (smaller size of elements)
	int gw1_elem_index;
	///Flag if the index of the element of the first groundwater is set (smaller size of elements)
	bool gw1_elem_is_setted;
	///Pointer to the first groundwater element (smaller size of elements)
	Hyd_Element_Groundwater *gw1_elem;
	///Index of the second groundwater element
	int gw2_elem_index;
	///Flag if the index of the element of the second groundwater is set
	bool gw2_elem_is_setted;
	///Pointer to the second groundwater element
	Hyd_Element_Groundwater *gw2_elem;
	///Direction of coupling of the first groundwater model cells for defining the distances dx or dy for the Darcy-calculation
	bool x_width_flag;
	///Distance for the flow claculation
	double flow_distance;
	///Width for the flow claculation
	double flow_width;
	///Maximum value of the element height (z-value) [corresponding to s_max in Hyd_Element_Groundwater_Standard.h]
	double max_z;
	///Mid-value of the Transmissivity (Conductivity-value)
	double mid_T;
	///Summarized flowparameter
	double c_flow;

	///Predicted waterlevel two (GW2)
	double predicted_h_two;
	///Corrected waterlevel two (GW2)
	double corrected_h_two;
	///Calculated waterlevel two (GW2)
	double calc_h_two;
	///Old calculated waterlvel two (GW2)
	double old_calc_h_two;

	///List of gradients of waterlevel two (GW2)
	QList <double> gradient_list_h_two;
	///Predicted gradient of waterlevel two (GW2)
	double gradient_h_two;


	///Current gradient of q
	double grad_q_current;
	///Gradient one syncronisation step before
	double grad_q_before;
	///Smoother for the oscilations
	double oscilation_smoother;
	///Counter if there are no oscilation
	double no_osci_counter;
	///Counter if there are oscilation
	double number_osci_counter;

	///Stored gradient one syncronisation step before
	double store_grad_q_before;
	///Stored smoother for the oscilations
	double store_oscilation_smoother;
	///Stored counter if there are no oscilation
	double store_no_osci_counter;
	///Stored counter if there are oscilation
	double store_number_osci_counter;

	///Gradient of the waterlevel difference
	double grad_delta_h;
	///Stored gradient of the waterlevel difference
	double store_grad_delta_h;
	///Old waterlevel difference
	double old_delta_h;
	///Stored old waterlevel difference
	double store_old_delta_h;


	//List of the discharges over time (just for testing)
	//_hyd_dicharge_lists q_list;

	//methods
	///Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
	double smooth_coupling_discharge(const double q_current, double *old_q);

	///Predict the values
	void predict_values(const int int_counter);

};

#endif