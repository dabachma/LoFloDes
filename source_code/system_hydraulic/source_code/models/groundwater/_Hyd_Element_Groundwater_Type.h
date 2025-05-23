#pragma once
/**\class _Hyd_Element_Groundwater_Type
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef _HYD_ELEMENT_GROUNDWATER_TYPE_H
#define _HYD_ELEMENT_GROUNDWATER_TYPE_H

//hyd_system class
#include "_Hyd_River_Profile_Type.h"
#include "Hyd_Param_Conductivity.h"
#include "Hyd_Param_Porosity.h"
#include "Hyd_Param_GW.h"
#include "Hyd_Instationary_Boundary.h"
#include "Hyd_Element_Bound_GW_Members.h"
#include "Hyd_Element_Coup_GW_Members.h"

//sys_system class
#include "Common_Const.h"
class Hyd_Element_Groundwater;

///Enumerator for the direction of the neighbouring elements (just orthogonal) \ingroup hyd
enum _hyd_neighbouring_direction_gw {
	///x-direction [0°]
	X_DIR_GW,
	///y-direction [90°]
	Y_DIR_GW,
	///Negative x-direction [180°]
	MINUS_X_DIR_GW,
	///Negative y-direction [270°]
	MINUS_Y_DIR_GW
};
///Enumerator for all direction of neighbouring elements (orthogonal and diagonal) \ingroup hyd
enum _hyd_all_element_direction_gw {
	///x-direction [0°] 
	X_DIRECT_GW,
	///Half direction between x and y [45°]
	XY_DIRECT_GW,
	///y-direction [90°]
	Y_DIRECT_GW,
	///Half direction between negative x-direction and y [135°]
	YMINUS_X_DIRECT_GW,
	///Negative x-direction [180°]
	MINUS_X_DIRECT_GW,
	///Half direction between negative x-direction and negative y-direction [225°]
	MINUS_X_MINUS_Y_DIRECT_GW,
	///Negative y-direction [270°]
	MINUS_Y_DIRECT_GW,
	///Half direction between x-direction and negative y-direction [315°]
	MINUS_Y_X_DIRECT_GW
};
///Data structure for storing the flow relevant parameters \ingroup hyd
struct _hyd_flowdata_groundwater_elem {
	///Integervalue for the conductivity (kf-value)
	int con_type;
	///kf-value kf of the element [calculation variables]
	double kf_value;
	///Integervalue for the porosity (p-value)
	int por_type;
	///p-value of the element [calculation variables]
	double p_value;
	///thickness of the groundwater layer
	double thickness;
	///Initialcondition
	double init_condition;
	///Element flag for flow in x-direction (true=> flow in x-direction; false=> noflow in x-direction)
	bool no_flow_x_flag;
	///Element flag for flow in y-direction (true=> flow in y-direction; false=> noflow in y-direction)
	bool no_flow_y_flag;


};

//
struct _hyd_groundwater_barrier_data {
	///
	bool barrier_flag;
	///
	bool barrier_x;
	bool barrier_x_minus;
	bool barrier_y;
	bool barrier_y_minus;
	///
	double barrier_kf_x;
	double barrier_kf_x_minus;
	double barrier_kf_y;
	double barrier_kf_y_minus;
};

//forward declaration
struct _hyd_neighbouring_gw_elems;

///Base class for all element types. The groundwatermodel consists of elements, which have a specific type \ingroup hyd
//Most methods are overwritten by the child classes. However every method has to be declared here to be accessible for other classes
/**
The elements (Hyd_Element_Groundwater) define the groundwater, every element has a specific type. The flow between the elements is calculated via the Darcy-formula, interactions between river and groundwater are calculated via leakage formula.
There are different types of elements:
	- standard elements (see Hyd_Element_Groundwater_Type_Standard);
	- river elements; they are intercepted by a river midline (see Hyd_Element_Groundwater_Type_Standard);
	- line element; they are intercepted dy a line, else like standard elements (see Hyd_Element_Groundwater_Type_Standard)
	- noflow elements; they are inside a user defined noflow-polygon (see Hyd_Element_Groundwater_Type_Noflow)
	- noinfo elements; there are no information like the geodetical height (see Hyd_Element_Groundwater_Type_Noflow)
Right now, only noflow and standard type have their own child classes. River elements are only considered internally for coupling but they are included in the standard class.
The geometrical attributes and the flow specific informations (e.g. kf value, pressure height) are stored here, the flow between the elements is calculated in the this class.
*/
class _Hyd_Element_Groundwater_Type
{

	friend class Hyd_Element_Groundwater;

public:

	///Default constructor
	_Hyd_Element_Groundwater_Type(void);
	///Default destructor
	virtual ~_Hyd_Element_Groundwater_Type(void);

	//method

	///Initialize the element (call it backward: not from the origin; start at the diagonal corner of the origin)
	virtual void init_element(_hyd_neighbouring_gw_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Conductivity *con_table, Hyd_Param_Porosity *por_table, const bool clone) = 0;
	///Connect the element with a instationary boundary curve if requiered (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
	virtual void connect_instat_boundarycurve(Hyd_Instationary_Boundary_GW *bound_curve);

	///Calculate the hydolocigal balance for boundary condition (every syncronisation step)
	virtual void calculate_hydrolocigal_balance_boundary(const double delta_t, bool flag);
	///Calculate the hydolocigal balance for coupling (every syncronisation step)
	virtual void calculate_hydrolocigal_balance_coupling(const double delta_t);


	///Calculate the boundary value to a given time point
	virtual void calculate_boundary_value(const double time_point);

	///Calculate the maximum values and the wet duration REVIEW This is either not necessary or should logically be changed to minimum values
	virtual void calc_max_values(const double time_point);


	///Set the result value (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
	virtual void set_solver_result_value(const double value);
	///Set a value to the the variation of the global waterlevel over time
	virtual void set_ds2dt_value(const double value);

	///set the variation of the global waterlevel over time as result value ds_dt_value per type id
	virtual void set_ds2dt_per_type(int id, const double value);

	///Get the variation of the global waterlevel over time as result value ds_dt_value per type id
	virtual double get_ds2dt_per_type(int id);

	//Get the global waterlevel (s_value)
	virtual double get_s_value(void);
	///Get the local groundwaterlevel (h_value)
	virtual double get_h_value(void);

	///Get the relevant area of the element
	virtual double get_relevant_area(void);

	//Get the ds2dt value due to coupling
	virtual double get_coupling_ds2dt(void);

	///Rewind the element
	virtual void rewind_element(const double h_value);

	//Calculate the variation of the global waterlevel over time (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
	virtual void calculate_ds_dt(void);
	//Calculate the flow velocities over time for output (just needed in child-class Hyd_Element_Groundwater_Type_Standard). Currently there is no need to calculate velocities
	//virtual void calculate_v_out(const double sin_value, const double cos_value);
	//Add a value to the the variation of the global waterlevel over time (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
	virtual void add_ds_dt(const double value);

	//Get the variation of the global waterlevel over time as result value (ds_dt_value)
	virtual double get_ds2dt_value(void);

	//Get the Transmissivity
	virtual double get_T(void);

	//Get the flow velocity in x-direction
	//virtual double get_flowvelocity_vx(void);
	//Get the flow velocity in y-direction
	//virtual double get_flowvelocity_vy(void);

	//Get the flow velocity in x-direction for output
	//virtual double get_flowvelocity_vx_out(void);
	//Get the flow velocity in y-direction for output
	//virtual double get_flowvelocity_vy_out(void);

	//Get the total flow velocity
	//virtual double get_flowvelocity_vtotal(void);
	//Get the if the element is wet
	//virtual double get_wet_flag(void);


	///Reset the boundary discharge
	virtual void reset_boundary_discharge_gw(void);
	///get boundary discharge from h and q boundary condition 
	virtual double get_boundary_dsicharge_gw(void);
	///Add boundary discharge, which comming from coupled groundwater models during syncronisation 
	virtual void add_boundary_discharge_gw(const double discharge);


	///Add the hydrological balance value of the boundary condition discharges to the given pointer
	virtual void add_hydro_balance_boundary(_hyd_hydrological_balance *given);
	
	///Add the hydrological balance value of the coupling discharges to the element due to a coupling between two groundwater models to the given pointer
	virtual void add_hydro_balance_coupling_gw(_hyd_hydrological_balance *given);
	//Add the hydrological balance value of the coupling discharges to the element due to river coupling via a hydraulic structure to the given pointer
	//virtual void add_hydro_balance_structure_coupling_rv(_hyd_hydrological_balance *given);
	//Add the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile to the given pointer
	//virtual void add_hydro_balance_direct_coupling_rv(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to river coupling via leakage to the given pointer
	virtual void add_hydro_balance_leakage_coupling_rv(_hyd_hydrological_balance *given);

	///Get the hydrological balance value of the coupling discharges to the element due to a coupling between two groundwater models 
	virtual _hyd_hydrological_balance get_hydro_balance_coupling_gw(void);
	//Get the hydrological balance value of the coupling discharges to the element due to river coupling via a hydraulic structure 
	//virtual _hyd_hydrological_balance get_hydro_balance_structure_coupling_rv(void);
	//Get the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile 
	//virtual _hyd_hydrological_balance get_hydro_balance_direct_coupling_rv(void);
	//Get the total coupling inflow volume (without boundary conditions)
	//virtual double get_total_coupling_inflow_volume(void);
	//Get the total coupling outflow volume (without boundary conditions)
	//virtual double get_total_coupling_outflow_volume(void);


	///Get the watervolume in the element
	virtual double get_watervolume_element(void);
	///Get the watervolume in of the init-conditions
	virtual double get_watervolume_init(void);

	///Get the area
	virtual double get_area(void);
	///Get the width (x-direction)
	virtual double get_width_x(void);
	///Get the width (y-direction)
	virtual double get_width_y(void);

	///Get the conveance in x-direction
	virtual double get_cx(void);
	///Get the conveance in y-direction
	virtual double get_cy(void);
	///Get the conveance in x-direction for boundaries
	virtual double get_cx_bound(void);
	///Get the conveance in y-direction for boundaries
	virtual double get_cy_bound(void);
	///Get the conveance in top-direction for boundaries
	virtual double get_ctop_bound(void);
	///Get the maximum of geodetic height in x-direction
	virtual double get_smax_x(void);
	///Get the maximum of geodetic height in y-direction
	virtual double get_smax_y(void);

	///Get the boundary discharge
	virtual double get_bound_discharge(bool flag);
	///Get the coupling discharge
	virtual double get_coupling_discharge(void);

	///Get the head flag
	virtual bool get_h_flag(void);
	///Get the discharge flag
	virtual bool get_q_flag(void);

	///Set the maximum results values directly
	virtual void set_max_results_directly(const double waterlevel, const double max_v, const double duration, const double dh_dt);

	///Get the maximum local waterlevel (h_value)
	virtual _hyd_max_values get_max_h_value(void);

	//Set the waterlevel to the init-condition
	void _Hyd_Element_Groundwater_Type::set_waterlevels2init_condition(const double h_init);
	
	///Get the maximal variation of the global waterlevel over time as result value (ds_dt_value)
	virtual _hyd_max_values get_max_ds2dt(void);
	
	///Reset the discharge, which comming from coupled groundwater models (coupling_q_gw)
	virtual void reset_coupling_discharge_gw(void);
	///Add coupling discharge, which comming from coupled groundwater models (coupling_q_gw) during syncronisation 
	virtual void add_coupling_discharge_gw(const double discharge);
	///Reset the discharge, which comming from coupled river models due to hydraulic leakage coupling (coupling_q_rv_leakage)
	virtual void reset_coupling_discharge_rv_leakage(void);
	///Add coupling discharge, which comming from coupled river models due to hydraulic leakage coupling (coupling_q_rv_leakage) during syncronisation 
	virtual void add_coupling_discharge_rv_leakage(const double discharge);
	//Reset the discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure)
	//virtual void reset_coupling_discharge_rv_structure(void);
	//Add coupling discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure) during syncronisation 
	//virtual void add_coupling_discharge_rv_structure(const double discharge);
	//Reset the discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
	//virtual void reset_coupling_discharge_rv_direct(void);
	//Add coupling discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
	//virtual void add_coupling_discharge_rv_direct(const double discharge);

	///Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Groundwater)
	virtual void set_maximum_value2querystring(ostringstream *query_string);

	///Set the instationary result values of an element to an query string to transfer them into a database table (Hyd_Element_Groundwater)
	virtual void set_instat_value2querystring(ostringstream *query_string);

	///Get the next _hyd_all_element_direction_gw to the given direction counterclockwise
	static _hyd_all_element_direction_gw get_next_element_direction_counterclockwise(const _hyd_all_element_direction_gw dir);
	///Get the next _hyd_all_element_direction_gw to the given direction clockwise
	static _hyd_all_element_direction_gw get_next_element_direction_clockwise(const _hyd_all_element_direction_gw dir);

	//Reset the hydrological balance value and the maximum values
	virtual void reset_hydrobalance_maxvalues(void);

	///Set the connected element (just relevant for river element types)
	virtual void set_connected_element(_hyd_neighbouring_gw_elems neigh_elem);

	///Get the maximum change in a element 
	virtual void get_max_changes_elements(double *max_change_h, const bool timecheck);

	///Get a pointer to the flow data
	virtual _hyd_flowdata_groundwater_elem get_flow_data(void);
	///Get a pointer to the barrier data
	virtual _hyd_groundwater_barrier_data get_barrier_data(void);
	///Set barrier data
	virtual void set_barrier_data(_hyd_all_element_direction_gw direction, const double kf);

	//Get a pointer to the boundary data
	virtual _hyd_boundarydata_groundwater_elem get_boundary_data(void);

	
	//Get the coupling flag
	virtual bool get_coupling_flag(void);
	///Get boundary condition flag
	virtual bool get_bound_flag(void);

	///Delete the data of boundary condition
	virtual void delete_bound_data(void);

	//Set coupling data
	virtual void set_coupling_data(void);

	//set rv coupled flag
	virtual void set_rv_coupled_flag(bool flag);

	//set gw coupled flag
	virtual void set_gw_coupled_flag(bool flag);

	//return rv coupled flag
	virtual bool get_rv_coupled_flag(void);

	//return gw coupled flag
	virtual bool get_gw_coupled_flag(void);


protected:
	//members
	///Global geodetic height
	double *z_value;

	//method
	///Initialize the data structures of _hyd_flowdata_groundwater_elem
	void init_flowdata_structures(_hyd_flowdata_groundwater_elem * data);
	///Initialize the data structures of _hyd_groundwater_barrier_data
	void init_barrierdata_structures(_hyd_groundwater_barrier_data* data);


	///Get the element index of the connected element (important for Hyd_Element_Groundwater_Type_River because here the connected element number is returned)
	virtual int get_coupling_relevant_element_number(void);



private:
	//methods


	///Output the setted members
	virtual void output_setted_members(ostringstream *cout) = 0;
	///Ouput the maximum calculated values
	virtual void output_maximum_calculated_values(ostringstream *cout) = 0;
	///Ouput the result members per internal timestep
	virtual void output_result_values(ostringstream *cout) = 0;
	///Ouput the header for the maximum calculated values output
	static void output_header_result_values(ostringstream *cout);
	///Ouput the header for the maximum calculated values output
	static void output_header_maximum_calculated_values(ostringstream *cout);


	//Set the data buffers of _hyd_boundarydata_groundwater_elem, _hyd_flowdata_groundwater_elem and the global geodetic height from class Hyd_Groundwater_Element  
	virtual void set_data_buffers(const _hyd_boundarydata_groundwater_elem boundary, const _hyd_flowdata_groundwater_elem flow, double *z_value);

	//Set new boundary condition data
	virtual void set_boundary_condition_data(const _hyd_boundarydata_groundwater_elem boundary);


	///Set element flag for flow in x-direction (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
	virtual void set_x_noflow_flag(const bool flag);
	///Set element flag for flow in y-direction (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
	virtual void set_y_noflow_flag(const bool flag);

};
#endif
