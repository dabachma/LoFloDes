#include "source_code\Hyd_Headers_Precompiled.h"
//#include "_Hyd_Element_Groundwater_Type.h"

//constructor
_Hyd_Element_Groundwater_Type::_Hyd_Element_Groundwater_Type(void) {
	this->z_value = NULL;
}
//destructor
_Hyd_Element_Groundwater_Type::~_Hyd_Element_Groundwater_Type(void) {
}
//________________
//public
//Connect the element with a instationary boundary curve if requiered (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
void _Hyd_Element_Groundwater_Type::connect_instat_boundarycurve(Hyd_Instationary_Boundary_GW *bound_curve) {
	Hyd_Instationary_Boundary_GW *dummy;
	dummy = bound_curve;
}
//Calculate the hydolocigal balance for boundary condition (every syncronisation step)
void _Hyd_Element_Groundwater_Type::calculate_hydrolocigal_balance_boundary(const double, bool flag) {

}
//Calculate the hydolocigal balance for coupling (every syncronisation step)
void _Hyd_Element_Groundwater_Type::calculate_hydrolocigal_balance_coupling(const double) {

}

//Calculate the boundary value to a given time point
void _Hyd_Element_Groundwater_Type::calculate_boundary_value(const double) {

}
//Calculate the maximum values
void _Hyd_Element_Groundwater_Type::calc_max_values(const double) {

}
//Set the result value (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
void _Hyd_Element_Groundwater_Type::set_solver_result_value(const double) {

}

///Set the variation of the global waterlevel over time as result value ds_dt_value per type id
void _Hyd_Element_Groundwater_Type::set_ds2dt_per_type(int id, const double value) {
	
}

///Get the variation of the global waterlevel over time as result value ds_dt_value per type id
double _Hyd_Element_Groundwater_Type::get_ds2dt_per_type(int id) {
	return 0.0;
}

//Set a value to the the variation of the global waterlevel over time
void _Hyd_Element_Groundwater_Type::set_ds2dt_value(const double) {

}

//Calculate the variation of the global waterlevel over time(just needed in child-class Hyd_Element_Groundwater_Type_Standard)
void _Hyd_Element_Groundwater_Type::calculate_ds_dt(void) {

}

//Distribute the flows (ds_dt) to the neighbouring x- and y-elements (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
//Add a value to the the variation of the global waterlevel over time (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
void _Hyd_Element_Groundwater_Type::add_ds_dt(const double) {

}
//Get the global waterlevel (s_value); 
double _Hyd_Element_Groundwater_Type::get_s_value(void) {
	return 0.0;
}

///Get the local waterlevel (h_value)
double _Hyd_Element_Groundwater_Type::get_h_value(void) {
	return 0.0;
}
//Get the relevant area of the element
double _Hyd_Element_Groundwater_Type::get_relevant_area(void) {
	return 0.0;
}

//Get the ds2dt value due to coupling
double _Hyd_Element_Groundwater_Type::get_coupling_ds2dt(void) {
	return 0.0;
}

//Rewind the element
void _Hyd_Element_Groundwater_Type::rewind_element(const double) {


}

//Get the variation of the global waterlevel over time as result value ds_dt_value
double _Hyd_Element_Groundwater_Type::get_ds2dt_value(void) {
	return 0.0;
}

//Add the hydrological balance value of the boundary condition discharges to the given pointer
void _Hyd_Element_Groundwater_Type::add_hydro_balance_boundary(_hyd_hydrological_balance *given) {
	_hyd_hydrological_balance *buffer;
	buffer = given;
}
//Add the hydrological balance value of the coupling discharges to the elements due to a coupling between two floodplain models
void _Hyd_Element_Groundwater_Type::add_hydro_balance_coupling_gw(_hyd_hydrological_balance *given) {
	_hyd_hydrological_balance *buffer;
	buffer = given;
}
//Add the hydrological balance value of the coupling discharges to the elements due to river coupling via a hydraulic structure
/*
void _Hyd_Element_Groundwater_Type::add_hydro_balance_structure_coupling_rv(_hyd_hydrological_balance *given) {
	_hyd_hydrological_balance *buffer;
	buffer = given;
}
//Add the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile to the given pointer
void _Hyd_Element_Groundwater_Type::add_hydro_balance_direct_coupling_rv(_hyd_hydrological_balance *given) {
	_hyd_hydrological_balance *buffer;
	buffer = given;
}
*/
//Add the hydrological balance value of the coupling discharges to the elements due to river coupling via leakage
void _Hyd_Element_Groundwater_Type::add_hydro_balance_leakage_coupling_rv(_hyd_hydrological_balance *given) {
	_hyd_hydrological_balance *buffer;
	buffer = given;
}
//Get the hydrological balance value of the coupling discharges to the element due to a coupling between two groundwater models 
_hyd_hydrological_balance _Hyd_Element_Groundwater_Type::get_hydro_balance_coupling_gw(void) {
	_hyd_hydrological_balance buff;
	buff.volume_in = 0.0;
	buff.volume_out = 0.0;
	buff.volume_total = 0.0;
	return buff;
}

/*
//Get the hydrological balance value of the coupling discharges to the element due to river coupling via a hydraulic structure 
_hyd_hydrological_balance _Hyd_Element_Groundwater_Type::get_hydro_balance_structure_coupling_rv(void) {
	_hyd_hydrological_balance buff;
	buff.volume_in = 0.0;
	buff.volume_out = 0.0;
	buff.volume_total = 0.0;
	return buff;
}
//Get the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
_hyd_hydrological_balance _Hyd_Element_Groundwater_Type::get_hydro_balance_direct_coupling_rv(void) {
	_hyd_hydrological_balance buff;
	buff.volume_in = 0.0;
	buff.volume_out = 0.0;
	buff.volume_total = 0.0;
	return buff;
}
//Get the total coupling inflow volume (without boundary conditions)
double _Hyd_Element_Groundwater_Type::get_total_coupling_inflow_volume(void) {
	double buff = 0.0;
	return buff;
}
//Get the total coupling outflow volume (without boundary conditions)
double _Hyd_Element_Groundwater_Type::get_total_coupling_outflow_volume(void) {
	double buff = 0.0;
	return buff;
}
*/

//Get the watervolume in the element
double _Hyd_Element_Groundwater_Type::get_watervolume_element() {
	return 0.0;
}
//Get the watervolume in of the init-conditions
double _Hyd_Element_Groundwater_Type::get_watervolume_init(void) {
	return 0.0;
}

//Get the area
double _Hyd_Element_Groundwater_Type::get_area(void) {
	return 0.0;
}

//Get the width (x-direction)
double _Hyd_Element_Groundwater_Type::get_width_x(void) {
	return 0.0;
}

//Get the width (y-direction)
double _Hyd_Element_Groundwater_Type::get_width_y(void) {
	return 0.0;
}

//Get the conveyance in x-direction
double _Hyd_Element_Groundwater_Type::get_cx(void) {
	return 0.0;
}
//Get the conveyance in y-direction
double _Hyd_Element_Groundwater_Type::get_cy(void) {
	return 0.0;
}
//Get the convyeance in x-direction for boundaries
double _Hyd_Element_Groundwater_Type::get_cx_bound(void) {
	return 0.0;
}
//Get the conveyance in y-direction
double _Hyd_Element_Groundwater_Type::get_cy_bound(void) {
	return 0.0;
}
//Get the conveance in top-direction
double _Hyd_Element_Groundwater_Type::get_ctop_bound(void) {
	return 0.0;
}
//Get the maximum of geodetic height in x-direction
double _Hyd_Element_Groundwater_Type::get_smax_x(void) {
	return 0.0;
}
//Get the maximum of geodetic height in y-direction
double _Hyd_Element_Groundwater_Type::get_smax_y(void) {
	return 0.0;
}
//Get the boundary discharge
double _Hyd_Element_Groundwater_Type::get_bound_discharge(bool flag) {
	return 0.0;

}
//Get the coupling discharge
double _Hyd_Element_Groundwater_Type::get_coupling_discharge(void) {
	return 0.0;

}
//Get the Transmissivity
double _Hyd_Element_Groundwater_Type::get_T(void) {
	return 0.0;

}

//Check  whether a boundary condition is a head-boundary
bool _Hyd_Element_Groundwater_Type::get_h_flag(void) {
	return false;
}

//Check  whether a boundary condition is a discharge-boundary
bool _Hyd_Element_Groundwater_Type::get_q_flag(void) {
	return false;
}

//Set the waterlevel to the init-condition REVIEW
void _Hyd_Element_Groundwater_Type::set_waterlevels2init_condition(const double h_init) {
	//this->h_value = h_init;
	//this->s_value = this->z_value + h_init;
}
//Set the maximum results values directly
void _Hyd_Element_Groundwater_Type::set_max_results_directly(const double, const double, const double, const double) {

}
//Get the maximum local waterlevel (h_value)
_hyd_max_values _Hyd_Element_Groundwater_Type::get_max_h_value(void) {
	_hyd_max_values buffer;
	buffer.maximum = 0.0;
	buffer.time_point = -1.0;
	return buffer;
}

//Get the maximal variation of the global waterlevel over time as result value (ds_dt_value)
_hyd_max_values _Hyd_Element_Groundwater_Type::get_max_ds2dt(void) {
	_hyd_max_values buffer;
	buffer.maximum = 0.0;
	buffer.time_point = -1.0;
	return buffer;
}

//Reset the discharge, which comming from coupled groundwater models (coupling_q_gw)
void _Hyd_Element_Groundwater_Type::reset_coupling_discharge_gw(void) {

}
//Add coupling discharge, which comming from coupled groundwater models (coupling_q_gw) during syncronisation 
void _Hyd_Element_Groundwater_Type::add_coupling_discharge_gw(const double discharge) {
	double dummy;
	dummy = discharge;
}
//Reset the discharge, which comming from coupled river models due to hydraulic leakage coupling (coupling_q_rv_leakage)
void _Hyd_Element_Groundwater_Type::reset_coupling_discharge_rv_leakage(void) {

}
//Add coupling discharge, which comming from coupled river models due to hydraulic leakage coupling (coupling_q_rv_leakage) during syncronisation 
void _Hyd_Element_Groundwater_Type::add_coupling_discharge_rv_leakage(const double discharge) {
	double dummy;
	dummy = discharge;
}

/*
//Reset the discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure)
void _Hyd_Element_Groundwater_Type::reset_coupling_discharge_rv_structure(void) {

}
//Add coupling discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure) during syncronisation 
void _Hyd_Element_Groundwater_Type::add_coupling_discharge_rv_structure(const double discharge) {
	double dummy;
	dummy = discharge;
}
//Reset the discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
void _Hyd_Element_Groundwater_Type::reset_coupling_discharge_rv_direct(void) {
}
//Add coupling discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
void _Hyd_Element_Groundwater_Type::add_coupling_discharge_rv_direct(const double discharge) {
	double dummy;
	dummy = discharge;
}
*/

//Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Groundwater)
void _Hyd_Element_Groundwater_Type::set_maximum_value2querystring(ostringstream *query_string) {
	//max-values
	*query_string << 0.0 << " , ";
	*query_string << (*this->z_value) << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << -1.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	//volumes
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";

	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";

	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";

	//*query_string << 0.0 << " , ";
	//*query_string << 0.0 << " , ";
	//*query_string << 0.0 << " , ";
	//*query_string << 0.0 << " , ";

	//*query_string << 0.0 << " , ";
	//*query_string << 0.0 << " , ";

}
///Set the instationary result values of an element to an query string to transfer them into a database table (Hyd_Element_Groundwater)
void _Hyd_Element_Groundwater_Type::set_instat_value2querystring(ostringstream *query_string) {
	//instat-values
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
}
//Get the next _hyd_all_element_direction_gw to the given direction counterclockwise (static)
_hyd_all_element_direction_gw _Hyd_Element_Groundwater_Type::get_next_element_direction_counterclockwise(const _hyd_all_element_direction_gw dir) {
	_hyd_all_element_direction_gw next_dir;
	switch (dir) {
	case _hyd_all_element_direction_gw::X_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::XY_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::XY_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::Y_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::Y_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::YMINUS_X_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::YMINUS_X_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::MINUS_X_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::MINUS_X_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::MINUS_X_MINUS_Y_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::MINUS_X_MINUS_Y_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::MINUS_Y_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::MINUS_Y_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::MINUS_Y_X_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::MINUS_Y_X_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::X_DIRECT_GW;
		break;
	default:
		break;
	}

	return next_dir;
}
///Get the next _hyd_all_element_direction_gw to the given direction clockwise (static)
_hyd_all_element_direction_gw _Hyd_Element_Groundwater_Type::get_next_element_direction_clockwise(const _hyd_all_element_direction_gw dir) {
	_hyd_all_element_direction_gw next_dir;
	switch (dir) {
	case _hyd_all_element_direction_gw::X_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::MINUS_Y_X_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::XY_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::X_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::Y_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::XY_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::YMINUS_X_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::Y_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::MINUS_X_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::YMINUS_X_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::MINUS_X_MINUS_Y_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::MINUS_X_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::MINUS_Y_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::MINUS_X_MINUS_Y_DIRECT_GW;
		break;
	case _hyd_all_element_direction_gw::MINUS_Y_X_DIRECT_GW:
		next_dir = _hyd_all_element_direction_gw::MINUS_Y_DIRECT_GW;
		break;
	default:
		break;
	}


	return next_dir;
}
//Reset the hydrological balance value and the maximum values
void _Hyd_Element_Groundwater_Type::reset_hydrobalance_maxvalues(void) {
	
}
//Set the connected element (just relevant for river element types)
void _Hyd_Element_Groundwater_Type::set_connected_element(_hyd_neighbouring_gw_elems neigh_elem) {

}
//Get the maximum change in a element 
void _Hyd_Element_Groundwater_Type::get_max_changes_elements(double *max_change_h, const bool) {
	*max_change_h = 0.0;
}
///Get a pointer to the flow data
_hyd_groundwater_barrier_data _Hyd_Element_Groundwater_Type::get_barrier_data(void) {
	_hyd_groundwater_barrier_data buffer;
	this->init_barrierdata_structures(&buffer);
	return buffer;
}
///set barrier data
void _Hyd_Element_Groundwater_Type::set_barrier_data(_hyd_all_element_direction_gw direction, const double kf) {

}
///Get a pointer to the flow data
_hyd_flowdata_groundwater_elem _Hyd_Element_Groundwater_Type::get_flow_data(void) {
	_hyd_flowdata_groundwater_elem buffer;
	this->init_flowdata_structures(&buffer);
	return buffer;
}

///Get a pointer to the boundary data
_hyd_boundarydata_groundwater_elem _Hyd_Element_Groundwater_Type::get_boundary_data(void) {
	_hyd_boundarydata_groundwater_elem  buffer;
	Hyd_Element_Bound_GW_Members::init_boundarydata_structures(&buffer);
	return buffer;
}
//Get the coupling flag
bool _Hyd_Element_Groundwater_Type::get_coupling_flag(void) {
	return false;
}
//Get boundary condition flag
bool _Hyd_Element_Groundwater_Type::get_bound_flag(void) {
	return false;
}
//Delete the data of boundary condition
void _Hyd_Element_Groundwater_Type::delete_bound_data(void) {
}
//Set coupling data
void _Hyd_Element_Groundwater_Type::set_coupling_data(void) {

}

///Reset the boundary discharge
void _Hyd_Element_Groundwater_Type::reset_boundary_discharge_gw(void) {

}
///get boundary discharge from h and q boundary condition 
double _Hyd_Element_Groundwater_Type::get_boundary_dsicharge_gw(void) {
	return 0.0;
}
///Add boundary discharge, which comming from coupled groundwater models during syncronisation 
void _Hyd_Element_Groundwater_Type::add_boundary_discharge_gw(const double discharge) {

}
//_____________
//protected
//Initialize the data structures of _hyd_flowdata_groundwater_elem
void _Hyd_Element_Groundwater_Type::init_flowdata_structures(_hyd_flowdata_groundwater_elem * data) {
	data->init_condition = 0.0;
	data->con_type = -1;
	data->por_type = -1;
	data->thickness = 0.0;
	data->no_flow_x_flag = false;
	data->no_flow_y_flag = false;
}

//Initialize the data structures of _hyd_groundwater_barrier_data
void _Hyd_Element_Groundwater_Type::init_barrierdata_structures(_hyd_groundwater_barrier_data* data) {
	
	data->barrier_flag = false;
	data->barrier_x = false;
	data->barrier_x_minus = false;
	data->barrier_y = false;
	data->barrier_y_minus = false;

	data->barrier_kf_x = 1e-7;
	data->barrier_kf_x_minus = 1e-7;
	data->barrier_kf_y = 1e-7;
	data->barrier_kf_y_minus = 1e-7;
}
//Get the element index of the connected element (important for Hyd_Element_Groundwater_Type_River because here the connected element number is returned)
int _Hyd_Element_Groundwater_Type::get_coupling_relevant_element_number(void) {
	return -1;
}

//set rv coupled flag
void _Hyd_Element_Groundwater_Type::set_rv_coupled_flag(bool flag) {
	
}

//set gw coupled flag
void _Hyd_Element_Groundwater_Type::set_gw_coupled_flag(bool flag) {
	
}

//return rv coupled flag
bool _Hyd_Element_Groundwater_Type::get_rv_coupled_flag(void) {
	return 0.0;
}

//return gw coupled flag
bool _Hyd_Element_Groundwater_Type::get_gw_coupled_flag(void) {
	return 0.0;
}


//_____________________
//private

//Set the data buffers of _hyd_boundarydata_groundwater_elem, _hyd_flowdata_groundwater_elem and the global geodetic height from class Hyd_Groundwater_Element  REVIEW
void _Hyd_Element_Groundwater_Type::set_data_buffers(const _hyd_boundarydata_groundwater_elem boundary, const _hyd_flowdata_groundwater_elem flow, double *z_value) {
	this->z_value = z_value;
	this->set_boundary_condition_data(boundary);

}

//Set new boundary condition data
void _Hyd_Element_Groundwater_Type::set_boundary_condition_data(const _hyd_boundarydata_groundwater_elem) {

}

///Set element flag for flow in x-direction (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
void _Hyd_Element_Groundwater_Type::set_x_noflow_flag(const bool) {

}
///Set element flag for flow in y-direction (just needed in child-class Hyd_Element_Groundwater_Type_Standard)
void _Hyd_Element_Groundwater_Type::set_y_noflow_flag(const bool) {

}

//(static) Ouput the header for the maximum calculated values output
void _Hyd_Element_Groundwater_Type::output_header_maximum_calculated_values(ostringstream *cout) {

	*cout << W(8) << "h" << label::m;
	*cout << W(8) << "s" << label::m;
	*cout << W(10) << "t" << label::sec;
	*cout << W(8) << "ds_dt" << label::m_per_min;
	*cout << W(6) << "t" << label::sec;
	*cout << W(6) << "t" << label::sec;
	*cout << W(8) << "hv" << label::sqm_per_sec;
	*cout << W(6) << "t" << label::sec;
	*cout << W(11) << "ariv.time" << label::sec;
	*cout << W(13) << "CVol_FP_in" << label::cubicmeter;
	*cout << W(13) << "CVol_FP_out" << label::cubicmeter;
	*cout << W(13) << "CVol_RV_Sc_in" << label::cubicmeter;
	*cout << W(13) << "CVol_RV_Sc_out" << label::cubicmeter;
	*cout << W(13) << "CVol_RV_Di_in" << label::cubicmeter;
	*cout << W(13) << "CVol_RV_Di_out" << label::cubicmeter;
	*cout << W(13) << "CVol_RV_OV_in" << label::cubicmeter;
	*cout << W(13) << "CVol_RV_OV_out" << label::cubicmeter;
	*cout << W(13) << "CVol_RV_DB_in" << label::cubicmeter;
	*cout << W(13) << "CVol_RV_DB_out" << label::cubicmeter;
	*cout << W(13) << "CVol_CO_OV_in" << label::cubicmeter;
	*cout << W(13) << "CVol_CO_OV_out" << label::cubicmeter;
	*cout << W(13) << "CVol_CO_DB_in" << label::cubicmeter;
	*cout << W(13) << "CVol_CO_DB_out" << label::cubicmeter;

	*cout << endl;
}
//(Static) Ouput the header for the maximum calculated values output
void _Hyd_Element_Groundwater_Type::output_header_result_values(ostringstream *cout) {
	*cout << W(10) << "h" << label::m;
	*cout << W(10) << "s" << label::m;
	*cout << W(10) << "ds_dt" << label::m_per_sec;
	*cout << endl;
}
