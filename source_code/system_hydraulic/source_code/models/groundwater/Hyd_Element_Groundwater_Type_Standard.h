#pragma once
/**\class Hyd_Element_Groundwater_Type_Standard
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_ELEMENT_GROUNDWATER_TYPE_STANDARD_H
#define HYD_ELEMENT_GROUNDWATER_TYPE_STANDARD_H

//hyd system
//base class
#include "_Hyd_Element_Groundwater_Type.h"


///This class represents the standard elements of a groundwater model, with the associated geometrical and flow characteristics \ingroup hyd
/**

*/
class Hyd_Element_Groundwater_Type_Standard : public _Hyd_Element_Groundwater_Type
{
	///Class where the element type is decided and which works with the element type
	friend class Hyd_Element_Groundwater;

public:
	///Default constructor
	Hyd_Element_Groundwater_Type_Standard(void);
	///Default destructor
	~Hyd_Element_Groundwater_Type_Standard(void);

	//member




	//methods
	///Initialize the element (call it backward: not from the origin; start at the diagonal corner of the origin)
	void init_element(_hyd_neighbouring_gw_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Conductivity *con_table, Hyd_Param_Porosity *por_table, const bool clone);

	//Connect the element with a instationary boundary curve if requiered
	void connect_instat_boundarycurve(Hyd_Instationary_Boundary_GW *bound_curve);
	///set barrier data
	void set_barrier_data(_hyd_all_element_direction_gw direction, const double kf);
	
	///Calculate the hydolocigal balance for boundary condition (every syncronisation step)
	void calculate_hydrolocigal_balance_boundary(const double delta_t, bool flag);
	///Calculate the hydolocigal balance for coupling (every syncronisation step)
	void calculate_hydrolocigal_balance_coupling(const double delta_t);


	///Calculate the boundary value to a given time point
	void calculate_boundary_value(const double time_point);
	
	///Calculate the maximum values 
	void calc_max_values(const double time_point);



	///Set the result value from the solver
	void set_solver_result_value(const double result);
	///Get the global waterlevel (s_value)
	double get_s_value(void);
	///Get the local waterlevel (h_value)
	double get_h_value(void);


	///Calculate the variation of the global waterlevel over time
	void calculate_ds_dt(void);
	/*
	///Calculate the flow velocities over time for output
	void calculate_v_out(const double sin_value, const double cos_value);
	*/
	///Set a value to the the variation of the global waterlevel over time
	void set_ds2dt_value(const double value);
	
	///Get the variation of the global waterlevel over time as result value ds_dt_value
	double get_ds2dt_value(void);

	///set the variation of the global waterlevel over time as result value ds_dt_value per type id
	void set_ds2dt_per_type(int id, const double value);

	///Get the variation of the global waterlevel over time as result value ds_dt_value per type id
	double get_ds2dt_per_type(int id);


	///Get the relevant area of the element
	double get_relevant_area(void);
	//Get the ds2dt value due to coupling
	//double get_coupling_ds2dt(void);

	
	///Add the hydrological balance value of the boundary condition discharges to the given pointer
	void add_hydro_balance_boundary(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to a coupling between two groundwater models to the given pointer
	void add_hydro_balance_coupling_gw(_hyd_hydrological_balance *given);

	///Get the hydrological balance value of the coupling discharges to the element due to a coupling between two groundwater models 
	_hyd_hydrological_balance get_hydro_balance_coupling_gw(void);
	///Get the total coupling inflow volume (without boundary conditions)
	double get_total_coupling_inflow_volume(void);
	///Get the total coupling outflow volume (without boundary conditions)
	double get_total_coupling_outflow_volume(void);

	///Reset all hydrologicla balance value
	void reset_hydro_balance(void);
	///Get the watervolume in the element
	double get_watervolume_element(void);
	///Get the watervolume in of the init-conditions
	double get_watervolume_init(void);

	///Set the maximum results values directly
	void set_max_results_directly(const double waterlevel, const double dh_dt);


	///Get the maximum local waterlevel (h_value)
	_hyd_max_values get_max_h_value(void);
	///Get the maximal variation of the global waterlevel over time as result value (ds_dt_value)
	_hyd_max_values get_max_ds2dt(void);
	
	///Get the area
	double get_area(void);
	///Get the width (x-direction)
	double get_width_x(void);
	///Get the width (y-direction)
	double get_width_y(void);
	///Get the conveance in x-direction REVIEW
	double get_cx(void);
	///Get the conveance in y-direction
	double get_cy(void);
	///Get the conveance in x-direction for boundaries REVIEW
	double get_cx_bound(void);
	///Get the conveance in y-direction for boundaries
	double get_cy_bound(void);
	///Get the conveance in top-direction for boundaries
	double get_ctop_bound(void);
	
	///Get the maximum of geodetic height in x-direction
	double get_smax_x(void);
	///Get the maximum of geodetic height in y-direction
	double get_smax_y(void);
	
	///Get the boundary discharge(true, return h; false, return q)
	double get_bound_discharge(bool flag);
	///Get the coupling discharge
	double get_coupling_discharge(void);

	///Get the flag of h_boundary
	bool get_h_flag(void);
	///Get the flag of q_boundary
	bool get_q_flag(void);

	///Get the Transmissivity
	double get_T(void);
	

	///Reset the discharge, which comming from coupled groundwater models (coupling_q_gw)
	void reset_coupling_discharge_gw(void);
	///Reset the boundary discharge
	void reset_boundary_discharge_gw(void);

	///Add coupling discharge, which comming from coupled groundwater models (coupling_q_gw) during syncronisation 
	void add_coupling_discharge_gw(const double discharge);
	///get boundary discharge from h and q boundary condition 
	double get_boundary_dsicharge_gw(void);
	///Add boundary discharge, which comming from coupled groundwater models during syncronisation 
	void add_boundary_discharge_gw(const double discharge);
	///Add coupling discharge, which comming from coupled river models due to leakage coupling (coupling_q_rv_leakage) during syncronisation 
	void add_coupling_discharge_rv_leakage(const double discharge);
	//Reset the discharge, which comming from a coupled river model due to leakage coupling (coupling_q_rv_leakage) during syncronisation 
	void reset_coupling_discharge_rv_leakage(void);
	//Add coupling discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
	//void add_coupling_discharge_rv_direct(const double discharge);
	
	///Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Groundwater
	void set_maximum_value2querystring(ostringstream *query_string);

	///Set the instationary result values of an element to an query string to transfer them into a database table (Hyd_Element_Groundwater
	void set_instat_value2querystring(ostringstream *query_string);


	///Reset the hydrological balance value and the maximum values
	void reset_hydrobalance_maxvalues(void);

	///Get the maximum change in a element 
	void get_max_changes_elements(double *max_change_h, const bool timecheck);

	
	///Get the coupling flag
	bool get_coupling_flag(void);
	///Get boundary condition flag
	bool get_bound_flag(void);

	///Delete the data of boundary condition
	void delete_bound_data(void);

	///Set coupling data
	void set_coupling_data(void);

	///set vertex flag
	void set_vertex_flag(void);

	///get vertex flag
	bool get_vertex_flag(void);
	

	///Calculate hydrological balance
	void calculate_h_hydro_balance(const double delta_t);

	//set rv coupled flag
	void set_rv_coupled_flag(bool flag);

	//set gw coupled flag
	void set_gw_coupled_flag(bool flag);

	//return rv coupled flag
	bool get_rv_coupled_flag(void);

	//return gw coupled flag
	bool get_gw_coupled_flag(void);


private:
	//members
	///Pointer to width of the element in x-direction [calculation variables]
	double *x_width;
	///Pointer to width of the element in x-direction [calculation variables]
	double *y_width;
	///Pointer to element area
	double *area;
	/// pointer to element thickness
	double *thickness;
	///Maximal global geodetic height in x-direction [calculation variables]
	double s_max_x;
	///Maximal global geodetic height in y-direction [calculation variables]
	double s_max_y;
	///Summarized flowparameter in x-direction [calculation variables]
	double c_x;
	///Summarized flowparameter in y-direction [calculation variables]
	double c_y;
	///Summarized Transmissivity [variation during the time]
	double T;
	///Summarized flow parameter for h-boundaries in x-direction (only use for adjacent groundwater)
	double c_x_bound;
	///Summarized flow parameter for h-boundaries in y-direction (only use for adjacent groundwater)
	double c_y_bound;
	///Summarized flow parameter for h-boundaries in top-direction
	double c_top_bound;

	///Summearized 
	///Pointer to the element in x-direction
	Hyd_Element_Groundwater *element_x;
	///Pointer to the element in y-direction
	Hyd_Element_Groundwater *element_y;
	///Pointer to the element in minus x-direction
	Hyd_Element_Groundwater *element_x_minus;
	///Pointer to the element in minus y-direction
	Hyd_Element_Groundwater *element_y_minus;


	///Data structure of the flow data of the element (_hyd_flowdata_groundwater_elem) [calculation variables]
	_hyd_flowdata_groundwater_elem flow_data;
	///Data structure of the barrier data of the element (_hyd_groundwater_barrier_data) [calculation variables]
	_hyd_groundwater_barrier_data barrier_data;


	///Gloabl waterlevel:= geodetic height (z) + waterlevel (h); it is also the result of the solver [result variables]
	double s_value;
	///Local waterlevel:= waterlevel (h) [result variables]
	double h_value;
	///Variation of the global waterlevel over time [result variables]
	double ds_dt_value;
	
	///Maximum local waterlevel [maximum result variables]
	_hyd_max_values max_h_value;
	///Maximum of the variation of the global waterlevel over time [maximum result variables]
	_hyd_max_values max_ds_dt;
	
	///Flag if the element is coupled
	bool coupling_flag;
	///Coupling mebers REVIEW
	Hyd_Element_Coup_GW_Members *coup_mem;

	///Flag if the element has a boundary condition
	bool boundary_flag;
	///Boundary mebers
	Hyd_Element_Bound_GW_Members *bound_mem;

	///Flag if the element is the vertex of the raster 
	bool vertex_flag;


	//flag if the element is coupled with river model
	bool rv_coupled;

	//flag if the element is coupled with another gw model
	bool gw_coupled;

	///Variation of the waterlevel over time for darcy
	double ds_dt_darcy;
	///Variation of the waterlevel over time for the internal timestep before for darcy
	double ds_dt_darcy_old;



	///Variation of the waterlevel over time for coupling
	double ds_dt_coup;
	///Variation of the waterlevel over time for the internal timestep before for coupling
	double ds_dt_coup_old;


	///Variation of the waterlevel over time for boundary
	double ds_dt_bound;
	///Variation of the waterlevel over time for the internal timestep before for coupling
	double ds_dt_bound_old;

	///Variation of the waterlevel over time for darcy in x direction
	double ds_dt_x_darcy;
	///Variation of the waterlevel over time for darcy in y direction
	double ds_dt_y_darcy;
	///Variation of the waterlevel over time for river coupling
	double ds_dt_rv_coup;
	///Variation of the waterlevel over time for groundwater coupling
	double ds_dt_gw_coup;
	///Variation of the waterlevel over time for h boundary
	double ds_dt_h_bound;
	///Variation of the waterlevel over time for q boundary
	double ds_dt_q_bound;
	
	///The watervolume in the element
	double watervolume;
	///The watervolume of the init-conditions
	double watervolume_init;

	///Infiltration rate
	const double infiltration_rate;

	///Make a hydrological balance for the h boundary condition discharge [boundarycondition variables]
	_hyd_hydrological_balance h_boundary_volume;





	//method
	///Get the flow data structure
	_hyd_flowdata_groundwater_elem get_flow_data(void);
	///Get the boundary data structure
	_hyd_boundarydata_groundwater_elem get_boundary_data(void);

	///Output the setted members
	void output_setted_members(ostringstream *cout);
	///Ouput the result members per internal timestep
	void output_result_values(ostringstream *cout);
	///Ouput the maximum calculated values
	void output_maximum_calculated_values(ostringstream *cout);

	///Set the data buffers of _hyd_boundarydata_groundwater_elem, _hyd_flowdata_groundwater_elem and the global geodetic height from class Hyd_Groundwater_Element 
	void set_data_buffers(const _hyd_boundarydata_groundwater_elem boundary, const _hyd_flowdata_groundwater_elem flow, double *z_value);


	///Set new boundary condition data
	void set_boundary_condition_data(const _hyd_boundarydata_groundwater_elem boundary);

	///calculate the variation of the global waterlevel (s-value) 

	///Set element flag for flow in x-direction
	void set_x_noflow_flag(const bool flag);
	///Set element flag for flow in y-direction
	void set_y_noflow_flag(const bool flag);

	///Calculate the variation of the global waterlevel (s-value) over time for flow in x-direction with the Darcy-formula
	void calculate_ds_dt_darcy_x(void);
	///Calculate the variation of the global waterlevel (s-value) over time for flow in y-direction with the Darcy-formula
	void calculate_ds_dt_darcy_y(void);

	///Calculate the maximum values to a given time point
	void calculate_maximum_values(const double time_point);
	///Calculate the watervolume in the element
	void calculate_watervolume_element(const bool infiltration = false);


	///Check if there are unphysical waterlevel calculated by the solver
	void check_unphysical_waterlevels(void);

	///calculate the T over time
	void calculate_T(void);



	///Allocate the data of coupling
	void allocate_coup_data(void);
	///Delete the data of coupling
	void delete_coup_data(void);
	///Allocate the data of boundary condition
	void allocate_bound_data(void);
	
	///set the h boundary
	void set_h_boundary_value(const double time);


	///Set error(s)
	Error set_error(const int err_type);

	

	


};

#endif

