#include "source_code\Hyd_Headers_Precompiled.h"
//#include "Hyd_Element_Groundwater_Type_Standard.h"

//constructor
Hyd_Element_Groundwater_Type_Standard::Hyd_Element_Groundwater_Type_Standard(void) : infiltration_rate(2.0e-6) {
	//boundary condtion and coupling members

	
	this->coupling_flag = false;
	this->coup_mem = NULL;
	this->boundary_flag = false;
	this->bound_mem = NULL;
	this->vertex_flag = false;

	this->h_boundary_volume.volume_in = 0.0;
	this->h_boundary_volume.volume_out = 0.0;
	this->h_boundary_volume.volume_total = 0.0;

	//calculation members
	this->init_flowdata_structures(&(this->flow_data));
	this->init_barrierdata_structures(&(this->barrier_data));
	this->x_width = NULL;
	this->y_width = NULL;
	this->area = NULL;
	this->thickness = NULL;
	this->s_max_x = 0.0;
	this->s_max_y = 0.0;
	this->c_x = 0.0;
	this->c_y = 0.0;
	this->T= 0.0;
	this->c_x_bound = 0;
	this->c_y_bound = 0;
	this->c_top_bound = 0;
	this->element_x = NULL;
	this->element_y = NULL;
	this->element_x_minus = NULL;
	this->element_y_minus = NULL;


	//result members
	this->s_value = 0.0;
	this->h_value = 0.0;
	this->ds_dt_value = 0.0;
	//maximum result variables
	this->max_h_value.maximum = 0.0;
	this->max_ds_dt.maximum = 0.0;
	this->max_h_value.time_point = -1.0;
	this->max_ds_dt.time_point = -1.0;

	this->ds_dt_darcy = 0.0;
	this->ds_dt_x_darcy = 0.0;
	this->ds_dt_y_darcy = 0.0;
	
	
	this->ds_dt_coup_old = 0.0;
	this->ds_dt_coup = 0.0;
	this->ds_dt_rv_coup= 0.0;
	this->ds_dt_gw_coup = 0.0;

	this->ds_dt_bound_old = 0.0;
	this->ds_dt_bound = 0.0;
	this->ds_dt_h_bound = 0.0;
	this->ds_dt_q_bound = 0.0;

	this->watervolume = 0.0;
	this->watervolume_init = 0.0;

	//coupling flag
	this->rv_coupled = false;
	this->gw_coupled = false;

	



	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Element_Groundwater_Type_Standard), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Element_Groundwater_Type_Standard::~Hyd_Element_Groundwater_Type_Standard(void) {

	this->delete_bound_data();
	this->delete_coup_data();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Element_Groundwater_Type_Standard), _sys_system_modules::HYD_SYS);
}
//____________
//public
//Initialize the element (call it backward: not from the origin; start at the diagonal corner of the origin)
void Hyd_Element_Groundwater_Type_Standard::init_element(_hyd_neighbouring_gw_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Conductivity *con_table, Hyd_Param_Porosity *por_table, const bool clone) {
	//set the width
	ostringstream cout;
	this->x_width = width_x;
	this->y_width = width_y;
	this->area = area;
	this->h_value = this->flow_data.init_condition;
	this->flow_data.p_value = por_table->get_por_value(this->flow_data.por_type, _hyd_por_coefficient_types::effective_porosity);
	//set the data for the darcy claculation
	this->flow_data.kf_value = con_table->get_con_value(this->flow_data.con_type, _hyd_con_coefficient_types::Kf_coefficient);
	this->T = this->flow_data.kf_value * min(this->h_value, this->flow_data.thickness);
	this->s_value = this->h_value + *this->z_value;


	//x-direction
	if (neigh_elem.x_direction != NULL) {
		this->element_x = neigh_elem.x_direction;
		if (this->element_x->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW) {
			//Darcy transmissivity (summarizing all parameters that do not depend on water level)
			this->c_x = 2* (*this->y_width)/ (this->flow_data.p_value * *this->area) / *this->x_width * ((this->flow_data.kf_value * min((h_value), this->flow_data.thickness)) * (this->element_x->get_flow_data().kf_value * min((this->element_x->get_flow_data().init_condition), this->element_x->get_flow_data().thickness)));
			this->c_x = this->c_x / (this->flow_data.kf_value * min(h_value, this->flow_data.thickness)+ this->element_x->get_flow_data().kf_value * min((this->element_x->get_flow_data().init_condition), this->element_x->get_flow_data().thickness));
			//this->c_x = this->c_x / (this->flow_data.p_value * *this->area) / *this->x_width;
			cout << "c_x: " << this->c_x << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//this->s_max_x = max((*this->z_value), this->element_x->get_z_value());
		}
		///if this element is noflow element
		else {
			this->flow_data.no_flow_x_flag = true;
			this->c_x = 0;
		}
	}
	///if this element_x is noflow element
	else {
		this->flow_data.no_flow_x_flag = true;
		this->c_x = 0;
	}
	
	

	if (neigh_elem.y_direction != NULL) {
		this->element_y = neigh_elem.y_direction;
		if (this->element_y->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW) {
			//Darcy transmissivity (summarizing all parameters that do not depend on water level difference)
			this->c_y = 2 * (*this->x_width)/ (this->flow_data.p_value * *this->area) / *this->y_width * ((this->flow_data.kf_value * min((h_value), this->flow_data.thickness)) * (this->element_y->get_flow_data().kf_value * min((this->element_y->get_flow_data().init_condition), this->element_y->get_flow_data().thickness)));
			this->c_y = this->c_y / (this->flow_data.kf_value * min(h_value, this->flow_data.thickness) + this->element_y->get_flow_data().kf_value * min((this->element_y->get_flow_data().init_condition), this->element_y->get_flow_data().thickness));
			//this->c_y = this->c_y / (this->flow_data.p_value * *this->area) / *this->y_width;
			cout << "c_y: " << this->c_y << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//this->s_max_x = max((*this->z_value), this->element_x->get_z_value());
		}
		///if this element is noflow element
		else {
			this->flow_data.no_flow_y_flag = true;
			this->c_y = 0;
		}
	}
	///if this element_y is noflow element
	else {
		this->flow_data.no_flow_y_flag = true;
		this->c_y = 0;
	}


	///for h-boundaries
	this->c_x_bound = *this->y_width*(this->flow_data.kf_value * min((h_value), this->flow_data.thickness));
	this->c_x_bound=this->c_x_bound / (this->flow_data.p_value * *this->area) / *this->x_width;

	this->c_y_bound = *this->x_width*(this->flow_data.kf_value * min((h_value), this->flow_data.thickness));
	this->c_y_bound = this->c_y_bound / (this->flow_data.p_value * *this->area) / *this->y_width;

	this->c_top_bound = *this->x_width;// *(this->flow_data.kf_value * *this->area);
	this->c_top_bound = this->c_top_bound / (this->flow_data.p_value * *this->area) / *this->y_width;

	//set the other element pointer
	this->element_x_minus = neigh_elem.minus_x_direction;
	this->element_y_minus = neigh_elem.minus_y_direction;

	/*if (this->element_x_minus == NULL || this->element_x_minus->get_elem_type() == _hyd_gw_elem_type::NOFLOW_ELEM_GW) {
		this->flow_data.no_flow_x_flag == true; 
	}
	if (this->element_y_minus == NULL || this->element_y_minus->get_elem_type() == _hyd_gw_elem_type::NOFLOW_ELEM_GW) {
		this->flow_data.no_flow_y_flag == true;
	}*/




	//calculate the initial watervolume for a hydrological balance
	this->watervolume_init = this->flow_data.init_condition*(*this->area);

	//set vertex flag
	//this->set_vertex_flag();
}
///set barrier data
void Hyd_Element_Groundwater_Type_Standard::set_barrier_data(_hyd_all_element_direction_gw direction, const double kf) {
	this->barrier_data.barrier_flag = true;
	if (direction == _hyd_all_element_direction_gw::X_DIRECT_GW || direction == _hyd_all_element_direction_gw::MINUS_X_DIRECT_GW) {
		this->barrier_data.barrier_x = true;
		this->barrier_data.barrier_kf_x = kf;
		
	}
	else if (direction == _hyd_all_element_direction_gw::Y_DIRECT_GW || direction == _hyd_all_element_direction_gw::MINUS_Y_DIRECT_GW) {
		this->barrier_data.barrier_y = true;
		this->barrier_data.barrier_kf_y = kf;
		
	}

}




//Connect the element with a instationary boundary curve if requiered
void Hyd_Element_Groundwater_Type_Standard::connect_instat_boundarycurve(Hyd_Instationary_Boundary_GW *bound_curve) {
	if (this->boundary_flag == false) {

		return;
	}
	
	if (this->bound_mem->boundary_data.h_boundary_flag == true) {
		if (this->bound_mem->boundary_data.h_stationary_flag == true) {
			this->bound_mem->boundary_data.found_flag = true;
		}
		else {
			
			try {
				//bound_curve->set_type(this->bound_mem->boundary_data.boundary_type);
				//set geom
				//bound_curve->set_geom(this->bound_mem->boundary_data.geom_type);
				this->bound_mem->boundary_data.h_curve_number == bound_curve->get_curve_number();
				this->bound_mem->boundary_data.h_curve_number == bound_curve->get_curve_number();
				bound_curve->set_dir(this->bound_mem->boundary_data.boundary_direction);
			}
			catch (Error msg) {
				ostringstream info;
				info << "Curve Number : " << this->bound_mem->boundary_data.h_curve_number << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			this->bound_mem->boundary_data.ptr_hboundarycurve = bound_curve;
			this->bound_mem->boundary_data.found_flag = true;
			
			
		
		}

	}

	if (this->bound_mem->boundary_data.q_boundary_flag == true) {
		if (this->bound_mem->boundary_data.q_stationary_flag == true) {
			this->bound_mem->boundary_data.found_flag = true;
		}
		else {
			if (this->bound_mem->boundary_data.q_curve_number == bound_curve->get_curve_number()) {
				try {
					//bound_curve->set_type(this->bound_mem->boundary_data.boundary_type);
					//set geom
					bound_curve->set_geom(this->bound_mem->boundary_data.geom_type);
					//bound_curve->set_dir(this->bound_mem->boundary_data.boundary_direction);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Curve Number : " << this->bound_mem->boundary_data.q_curve_number << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				this->bound_mem->boundary_data.ptr_qboundarycurve = bound_curve;
				this->bound_mem->boundary_data.found_flag = true;
			}
		}

	}

	//if (this->bound_mem->boundary_data.h_stationary_flag == true) {
	//	this->bound_mem->boundary_data.found_flag = true;
	//	return;
	//}
	//else {
	//	//check if it is the right curve
	//	if (this->bound_mem->boundary_data.h_curve_number == bound_curve->get_curve_number() && this->bound_mem->boundary_data.boundary_type == _hyd_bound_type_gw::head_boundary) {
	//		try {
	//			bound_curve->set_type(this->bound_mem->boundary_data.boundary_type);
	//			//set geom
	//			bound_curve->set_geom(this->bound_mem->boundary_data.geom_type);
	//			bound_curve->set_dir(this->bound_mem->boundary_data.boundary_direction);
	//		}
	//		catch (Error msg) {
	//			ostringstream info;
	//			info << "Curve Number : " << this->bound_mem->boundary_data.h_curve_number << endl;
	//			msg.make_second_info(info.str());
	//			throw msg;
	//		}
	//		this->bound_mem->boundary_data.ptr_boundarycurve = bound_curve;
	//		this->bound_mem->boundary_data.found_flag = true;
	//	}
	//	else if (this->bound_mem->boundary_data.curve_number == bound_curve->get_curve_number() && this->bound_mem->boundary_data.boundary_type == _hyd_bound_type_gw::discharge_boundary){
	//		try {
	//			bound_curve->set_type(this->bound_mem->boundary_data.boundary_type);
	//			//set geom
	//			bound_curve->set_geom(this->bound_mem->boundary_data.geom_type);
	//			//bound_curve->set_dir(this->bound_mem->boundary_data.boundary_direction);
	//		}
	//		catch (Error msg) {
	//			ostringstream info;
	//			info << "Curve Number : " << this->bound_mem->boundary_data.curve_number << endl;
	//			msg.make_second_info(info.str());
	//			throw msg;
	//		}
	//		this->bound_mem->boundary_data.ptr_boundarycurve = bound_curve;
	//		this->bound_mem->boundary_data.found_flag = true;
	//	}
	//}
}
//Calculate the hydolocigal balance for boundary condition (every syncronisation step)
void Hyd_Element_Groundwater_Type_Standard::calculate_hydrolocigal_balance_boundary(const double delta_t, bool flag) {
	bool h_flag = true;
	//boundary
	if (this->boundary_flag == true) {
		if (this->bound_mem->boundary_data.h_boundary_flag == true && h_flag == flag) {
			this->calculate_h_hydro_balance(delta_t);;
		}
		if (this->bound_mem->boundary_data.q_boundary_flag == true && h_flag != flag) {
			this->bound_mem->calculate_q_bound_value(delta_t, *this->area);
		}
		//this->bound_mem->calculate_hydro_balance(delta_t);
	}
}
//Calculate the hydolocigal balance for coupling (every syncronisation step)
void Hyd_Element_Groundwater_Type_Standard::calculate_hydrolocigal_balance_coupling(const double delta_t) {
	//coupling
	if (this->coupling_flag == true) {
		this->coup_mem->calculate_hydro_balance(delta_t);
	}
}
//Calculate the boundary value to a given time point
void Hyd_Element_Groundwater_Type_Standard::calculate_boundary_value(const double time_point) {

	if (this->boundary_flag == true) {//no condition
		if (this->bound_mem->boundary_data.h_boundary_flag == true) {
			this->bound_mem->calculate_h_bound_value(time_point);
		}
		if (this->bound_mem->boundary_data.q_boundary_flag == true) {
			this->bound_mem->calculate_q_bound_value(time_point, *this->area);
		}
		
	}
	//else {
	//	if (this->bound_mem->boundary_data.head_flag == false) {
	//		this->bound_mem->calculate_q_bound_value(time_point, *this->area);
	//	}
	//	else {
	//		this->set_h_boundary_value(time_point);
	//	}
	//}
}
//Calculate the maximum values and the wet duration
void Hyd_Element_Groundwater_Type_Standard::calc_max_values(const double time_point) {
	//calculate the maximum values
	this->calculate_maximum_values(time_point);
	//calculate volume
	this->calculate_watervolume_element();
}
//Set the result value REVIEW
void Hyd_Element_Groundwater_Type_Standard::set_solver_result_value(const double result) {

	//this->check_unphysical_waterlevels();
	
	this->h_value = result;
	this->s_value = *this->z_value + this->h_value;

	this->calculate_T();
}
//Get the global waterlevel (s_value)
double Hyd_Element_Groundwater_Type_Standard::get_s_value(void) {
	return this->s_value;
}
//Get the local waterlevel (h_value)
double Hyd_Element_Groundwater_Type_Standard::get_h_value(void) {
	return this->h_value;
}

//set the variation of the global waterlevel over time per type id
void Hyd_Element_Groundwater_Type_Standard::set_ds2dt_per_type(int id, const double value) {
	if (id == 0) {
		this->ds_dt_x_darcy = value;
		/*if (this->flow_data.no_flow_x_flag != true) {
			this->ds_dt_x_darcy = value;
		}
		else {
			this->ds_dt_x_darcy = 0.0;
		}*/
		
	}
	else if (id == 1) {
		this->ds_dt_y_darcy = value;
		/*if (this->flow_data.no_flow_y_flag != true) {
			this->ds_dt_y_darcy = value;
		}
		else {
			this->ds_dt_y_darcy = 0.0;
		}*/
	}
	else if (id == 2) {
		this->ds_dt_h_bound = this->ds_dt_h_bound+ value;
	}
	else if (id == 3) {
		this->ds_dt_q_bound = this->ds_dt_q_bound +value;
	}
	else if (id == 4) {
		this->ds_dt_rv_coup = value;
	}
	else if (id == 5) {
		this->ds_dt_gw_coup = value;
	}
}


//get the variation of the global waterlevel over time per type id
double Hyd_Element_Groundwater_Type_Standard::get_ds2dt_per_type(int id) {
	if (id == 0) {
		return this->ds_dt_x_darcy;
	}
	else if (id == 1) {
		return this->ds_dt_y_darcy;
	}
	else if (id == 2) {
		return this->ds_dt_h_bound;
	}
	else if (id == 3) {
		return this->ds_dt_q_bound;
	}
	else if (id == 4) {
		return this->ds_dt_rv_coup;
	}
	else if (id == 5) {
		return this->ds_dt_gw_coup;
	}
}

//Calculate the variation of the global waterlevel over time
void Hyd_Element_Groundwater_Type_Standard::calculate_ds_dt(void) {
	//flow in x-direction
	//REVIEW
	//this->ds_dt_x_darcy = 0.0;
	//this->ds_dt_x_darcy = 0.0;

	if (this->flow_data.no_flow_x_flag != true) {
		this->calculate_ds_dt_darcy_x();
	}

	//flow in y-direction
	if (this->flow_data.no_flow_y_flag != true) {
		this->calculate_ds_dt_darcy_y();
	}	
}

//Set a value to the the variation of the global waterlevel over time
void Hyd_Element_Groundwater_Type_Standard::set_ds2dt_value(const double value) {
	this->ds_dt_value = value;


	///check
	double sum_ds_dt = 0.0;
	double ds2dt_x_minus = 0.0;
	double ds2dt_y_minus = 0.0;
	double tolerance = 0.0;
	sum_ds_dt = this->ds_dt_x_darcy + this->ds_dt_y_darcy + this->ds_dt_h_bound + this->ds_dt_q_bound + this->ds_dt_rv_coup + this->ds_dt_gw_coup;

	if (this->element_x_minus != NULL) {
		if(this->element_x_minus->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW) {
			ds2dt_x_minus = -1*this->element_x_minus->element_type->get_ds2dt_per_type(0);
		}
	}
		
	if (this->element_y_minus != NULL) {
		if (this->element_y_minus->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW) {
			ds2dt_y_minus = -1*this->element_y_minus->element_type->get_ds2dt_per_type(1);
		}
	}

	sum_ds_dt = sum_ds_dt + ds2dt_x_minus + ds2dt_y_minus;

	if ((abs(sum_ds_dt/value-1.0)>= 0.1)&&(abs(sum_ds_dt-value)>=1e-5)) {
		tolerance = sum_ds_dt - value;


		this->ds_dt_value = sum_ds_dt;
		

		

	}
}
//Get the variation of the global waterlevel over time as result value ds_dt_value
double Hyd_Element_Groundwater_Type_Standard::get_ds2dt_value(void) {
	return this->ds_dt_value;
}
//Get the relevant area of the element
double Hyd_Element_Groundwater_Type_Standard::get_relevant_area(void) {
	return (*this->area);
}
/*
//Get the ds2dt value due to coupling
double Hyd_Element_Groundwater_Type_Standard::get_coupling_ds2dt(void) {
	return this->ds_dt_coup/(*this->area * this->flow_data.p_value);
}
*/
//Add the hydrological balance value of the boundary condition discharges to the given pointer
void Hyd_Element_Groundwater_Type_Standard::add_hydro_balance_boundary(_hyd_hydrological_balance *given) {
	if (this->boundary_flag == false) {
		return;
	}
	if (this->bound_mem->boundary_data.h_boundary_flag == true) {
		given->volume_in = given->volume_in + this->h_boundary_volume.volume_in;
		given->volume_out = given->volume_out + this->h_boundary_volume.volume_out;
		given->volume_total = given->volume_total + this->h_boundary_volume.volume_total;
	}
	if (this->bound_mem->boundary_data.q_boundary_flag == true) {
		given->volume_in = given->volume_in + this->bound_mem->q_boundary_volume.volume_in;
		given->volume_out = given->volume_out + this->bound_mem->q_boundary_volume.volume_out;
		given->volume_total = given->volume_total + this->bound_mem->q_boundary_volume.volume_total;
	}
	
}
//Add the hydrological balance value of the coupling discharges to the element due to a coupling between two groundwater models REVIEW
void Hyd_Element_Groundwater_Type_Standard::add_hydro_balance_coupling_gw(_hyd_hydrological_balance *given) {
	if (this->coupling_flag == false) {
		return;
	}
	given->volume_in = given->volume_in + this->coup_mem->coupling_volume_gw.volume_in;
	given->volume_out = given->volume_out + this->coup_mem->coupling_volume_gw.volume_out;
	given->volume_total = given->volume_total + this->coup_mem->coupling_volume_gw.volume_total;
}

//Get the hydrological balance value of the coupling discharges to the element due to a coupling between two groundwater models 
_hyd_hydrological_balance Hyd_Element_Groundwater_Type_Standard::get_hydro_balance_coupling_gw(void) {
	if (this->coupling_flag == false) {
		_hyd_hydrological_balance buffer;
		buffer.volume_in = 0.0;
		buffer.volume_out = 0.0;
		buffer.volume_total = 0.0;
		return buffer;
	}
	//return this->coup_mem->coupling_volume_gw;
}

//Get the total coupling inflow volume (without boundary conditions)
double Hyd_Element_Groundwater_Type_Standard::get_total_coupling_inflow_volume(void) {
	if (this->coupling_flag == false) {
		return 0.0;
	}
	double sum = this->coup_mem->get_sum_inflow();
	return sum;
}
//Get the total coupling outflow volume (without boundary conditions)
double Hyd_Element_Groundwater_Type_Standard::get_total_coupling_outflow_volume(void) {
	if (this->coupling_flag == false) {
		return 0.0;
	}
	double sum = this->coup_mem->get_sum_outflow();
	return sum;
}
//Reset all hydrologicla balance value
void Hyd_Element_Groundwater_Type_Standard::reset_hydro_balance(void) {
	if (this->boundary_flag == true) {
		this->bound_mem->reset_vol_q();
	}
	if (this->coupling_flag == true) {
		this->coup_mem->reset_vol_q();
	}

	this->watervolume = 0.0;
	this->watervolume_init = 0.0;

	this->ds_dt_coup_old = 0.0;
	this->ds_dt_coup = 0.0;

	this->ds_dt_darcy_old = 0.0;
	this->ds_dt_darcy = 0.0;

	this->ds_dt_bound_old = 0.0;
	this->ds_dt_bound = 0.0;
}
//Get the watervolume in the element
double Hyd_Element_Groundwater_Type_Standard::get_watervolume_element(void) {
	//update it
	this->calculate_watervolume_element(true);
	return this->watervolume;
}
//Get the watervolume in of the init-conditions
double Hyd_Element_Groundwater_Type_Standard::get_watervolume_init(void) {
	return this->watervolume_init;
}
//Set the maximum results values directly
void Hyd_Element_Groundwater_Type_Standard::set_max_results_directly(const double waterlevel, const double dh_dt) {
	if (waterlevel > 0.0) {
	
		this->h_value = waterlevel;
	}
	else {
		return;
	}

	this->max_h_value.maximum = waterlevel;
	this->max_ds_dt.maximum = dh_dt;

	this->max_h_value.time_point = -1.0;
	this->max_ds_dt.time_point = -1.0;
}
//Get the maximum local waterlevel (h_value)
_hyd_max_values Hyd_Element_Groundwater_Type_Standard::get_max_h_value(void) {
	return this->max_h_value;
}
//Get the maximal variation of the global waterlevel over time as result value (ds_dt_value)
_hyd_max_values Hyd_Element_Groundwater_Type_Standard::get_max_ds2dt(void) {
	return this->max_ds_dt;
}

//Get the area
double Hyd_Element_Groundwater_Type_Standard::get_area(void) {
	return *this->x_width * *this->y_width;
}
//Get the width (x-direction)
double Hyd_Element_Groundwater_Type_Standard::get_width_x(void) {
	return *this->x_width;
}
//Get the width (y-direction)
double Hyd_Element_Groundwater_Type_Standard::get_width_y(void) {
	return *this->y_width;
}
//Get the conveance in x-direction
double Hyd_Element_Groundwater_Type_Standard::get_cx(void) {
	if (this->element_x != NULL && this->element_x->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW) {
		if (this->barrier_data.barrier_x == true) {
			this->c_x = min(this->flow_data.thickness, max(this->get_h_value(), 0.001))* min(this->element_x->element_type->get_flow_data().thickness, max(this->element_x->element_type->get_h_value(), 0.001));
			this->c_x = this->c_x/(min(this->flow_data.thickness, max(this->get_h_value(), 0.001))+ min(this->element_x->element_type->get_flow_data().thickness, max(this->element_x->element_type->get_h_value(), 0.001)));
			this->c_x=this->c_x*this->barrier_data.barrier_kf_x * 2 * (*this->y_width) / (this->flow_data.p_value * *this->area) / *this->x_width;
		}
		else {
			this->c_x = (this->T * this->element_x->element_type->get_T()) / (this->T + this->element_x->element_type->get_T()) * 2 * (*this->y_width) / (this->flow_data.p_value * *this->area) / *this->x_width;
		}
	}
	else {
		this->c_x = 0.0;
	}
	return this->c_x;
}
//Get the conveance in y-direction
double Hyd_Element_Groundwater_Type_Standard::get_cy(void) {
	if (this->element_y != NULL && this->element_y->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW) {
		if (this->barrier_data.barrier_y == true) {
			this->c_y = min(this->flow_data.thickness, max(this->get_h_value(), 0.001)) * min(this->element_y->element_type->get_flow_data().thickness, max(this->element_y->element_type->get_h_value(), 0.001));
			this->c_y = this->c_y / (min(this->flow_data.thickness, max(this->get_h_value(), 0.001)) + min(this->element_y->element_type->get_flow_data().thickness, max(this->element_y->element_type->get_h_value(), 0.001)));
			this->c_y= this->c_y * this->barrier_data.barrier_kf_y * 2 * (*this->x_width) / (this->flow_data.p_value * *this->area) / *this->y_width;
		}
		else {
			this->c_y = (this->T * this->element_y->element_type->get_T()) / (this->T + this->element_y->element_type->get_T()) * 2 * (*this->x_width) / (this->flow_data.p_value * *this->area) / *this->y_width;
		}
	}
	else {
		this->c_y = 0.0;
	}
	return this->c_y;
}
//Get the conveance in x-direction for boundaries
double Hyd_Element_Groundwater_Type_Standard::get_cx_bound(void) {
	this->c_x_bound = *this->y_width * (this->get_T());
	this->c_x_bound = this->c_x_bound / (this->flow_data.p_value * *this->area) / *this->x_width;
	/*if (this->get_vertex_flag() == true ) {
		this->c_x_bound = this->c_x_bound + this->get_cy_bound();
	}*/
	return this->c_x_bound;
}
//Get the conveance in y-direction
double Hyd_Element_Groundwater_Type_Standard::get_cy_bound(void) {
	this->c_y_bound = *this->x_width * (this->get_T());
	this->c_y_bound = this->c_y_bound / (this->flow_data.p_value * *this->area) / *this->y_width;
	/*if (this->get_vertex_flag() == true) {
		this->c_y_bound = this->c_y_bound + this->get_cx_bound();
	}*/
	return this->c_y_bound;
}
//Get the conveance in top-direction
double Hyd_Element_Groundwater_Type_Standard::get_ctop_bound(void) {
	return this->c_top_bound;
}
//Get the maximum of geodetic height in x-direction
double Hyd_Element_Groundwater_Type_Standard::get_smax_x(void) {
	return this->s_max_x;
}
//Get the maximum of geodetic height in y-direction
double Hyd_Element_Groundwater_Type_Standard::get_smax_y(void) {
	return this->s_max_y;
}
//Get the boundary discharge (true, return h; false, return q)
double Hyd_Element_Groundwater_Type_Standard::get_bound_discharge(bool flag) {
	bool get_h = true;
	
	double h_instat = 0.0;
	

	if (this->boundary_flag == false) {
		return 0.0;
	}
	if (this->bound_mem->boundary_data.h_boundary_flag == true && get_h == flag) {
		if (this->bound_mem->h_boundary == -1) {
			double h_boun = 0.0;
			double init_s = *this->z_value + this->flow_data.init_condition;
			if (this->element_x == NULL) {
				h_instat = h_instat + this->get_cx_bound() * (init_s - this->get_s_value());
			}
			else if (this->element_x->get_elem_type() == _hyd_gw_elem_type::NOFLOW_ELEM_GW) {
				h_instat = h_instat + this->get_cx_bound() * (init_s - this->get_s_value());
			}

			if (this->element_x_minus == NULL) {
				h_instat = h_instat + this->get_cx_bound() * (init_s - this->get_s_value());
			}
			else if (this->element_x_minus->get_elem_type() == _hyd_gw_elem_type::NOFLOW_ELEM_GW) {
				h_instat = h_instat + this->get_cx_bound() * (init_s - this->get_s_value());
			}

			if (this->element_y == NULL) {
				h_instat = h_instat + this->get_cy_bound() * (init_s - this->get_s_value());
			}
			else if (this->element_y->get_elem_type() == _hyd_gw_elem_type::NOFLOW_ELEM_GW) {
				h_instat = h_instat + this->get_cy_bound() * (init_s - this->get_s_value());
			}

			if (this->element_y_minus == NULL) {
				h_instat = h_instat + this->get_cy_bound() * (init_s - this->get_s_value());
			}
			else if (this->element_y_minus->get_elem_type() == _hyd_gw_elem_type::NOFLOW_ELEM_GW) {
				h_instat = h_instat + this->get_cy_bound() * (init_s - this->get_s_value());
			}
			//this->ds_dt_h_bound = this->ds_dt_h_bound+ h_instat;
			return h_instat;
		}

		else if (this->bound_mem->h_boundary == -2) {
			double h_boun = 0.0;
			if ((this->element_x == NULL) && (this->element_x_minus->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW)) {
				double init_s = 2 * (this->get_s_value()) - this->element_x_minus->element_type->get_s_value();
				h_instat = h_instat + this->get_cx_bound() * (init_s - this->get_s_value());
			}
			else if ((this->element_x_minus == NULL) && (this->element_x->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW)) {
				double init_s = 2 * (this->get_s_value()) - this->element_x->element_type->get_s_value();
				h_instat = h_instat + this->get_cx_bound() * (init_s - this->get_s_value());
			}
			else {
				double init_s = *this->z_value + this->flow_data.init_condition;
				h_instat = h_instat + this->get_cx_bound() * (init_s - this->get_s_value());
			}

			if ((this->element_y == NULL) && (this->element_y_minus->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW)) {
				double init_s = 2 * (this->get_s_value()) - this->element_y_minus->element_type->get_s_value();
				h_instat = h_instat + this->get_cy_bound() * (init_s - this->get_s_value());
			}
			else if ((this->element_y_minus == NULL) && (this->element_y->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW)) {
				double init_s = 2 * (this->get_s_value()) - this->element_y->element_type->get_s_value();
				h_instat = h_instat + this->get_cy_bound() * (init_s - this->get_s_value());
			}
			else {
				double init_s = *this->z_value + this->flow_data.init_condition;
				h_instat = h_instat + this->get_cy_bound() * (init_s - this->get_s_value());
			}
			//this->ds_dt_h_bound = this->ds_dt_h_bound+h_instat;
			return h_instat;
		}







		
		else {
			//this->ds_dt_h_bound = this->ds_dt_h_bound+this->get_cy_bound() * (this->bound_mem->h_boundary - this->get_s_value());
			return this->bound_mem->h_boundary;
		}
	}
	if (this->bound_mem->boundary_data.q_boundary_flag == true && get_h != flag) {
		//this->ds_dt_q_bound = this->ds_dt_q_bound+this->bound_mem->q_boundary / (this->flow_data.p_value * *this->area);
		return this->bound_mem->q_boundary;
	}
}
//Get the head flag of the h boundary
bool Hyd_Element_Groundwater_Type_Standard::get_h_flag(void) {
	if (this->bound_mem == NULL || this->bound_mem->boundary_data.h_boundary_flag == false) {
		return false;
	}
	else if (this->bound_mem->boundary_data.h_boundary_flag == true) {
		return true;
	}
}

//Get the head flag of the q boundary
bool Hyd_Element_Groundwater_Type_Standard::get_q_flag(void) {
	if (this->bound_mem == NULL || this->bound_mem->boundary_data.q_boundary_flag == false) {
		return false;
	}
	else if (this->bound_mem->boundary_data.q_boundary_flag == true) {
		return true;
	}
}


//Get the coupling discharge
double Hyd_Element_Groundwater_Type_Standard::get_coupling_discharge(void) {
	if (this->coupling_flag == false) {
		return 0.0;
	}
	this->ds_dt_coup = this->coup_mem->sum_coupling_discharge();
	return this->coup_mem->sum_coupling_discharge();
}
//Reset the discharge, which comming from coupled groundwater models (coupling_q_gw)
void Hyd_Element_Groundwater_Type_Standard::reset_coupling_discharge_gw(void) {
	if (this->coupling_flag == false) {
		return;
	}
	this->coup_mem->coupling_q_gw = 0.0;
	this->ds_dt_gw_coup = 0.0;
}
//Add coupling discharge, which coming from coupled groundwater models (coupling_q_gw) during syncronisation 
void Hyd_Element_Groundwater_Type_Standard::add_coupling_discharge_gw(const double discharge) {
	if (this->coupling_flag == false) {
		return;
	}
	this->ds_dt_gw_coup = this->ds_dt_gw_coup+discharge;
	this->coup_mem->coupling_q_gw = this->coup_mem->coupling_q_gw + discharge;
}
//Reset the discharge, which comming from coupled river models due to leakage coupling (coupling_q_rv_leakage) during syncronisation 
void Hyd_Element_Groundwater_Type_Standard::reset_coupling_discharge_rv_leakage(void) {
	if (this->coupling_flag == false) {
		return;
	}
	this->coup_mem->coupling_q_rv_leakage = 0.0;
	this->ds_dt_rv_coup = 0.0;
}
///Add boundary discharge, which comming from coupled groundwater models during syncronisation 
void Hyd_Element_Groundwater_Type_Standard::add_boundary_discharge_gw(const double discharge) {
	
	this->ds_dt_bound = this->ds_dt_bound + discharge;
	
}
///get boundary discharge from h and q boundary condition 
double Hyd_Element_Groundwater_Type_Standard::get_boundary_dsicharge_gw(void) {
	if (this->bound_mem == NULL || this->bound_mem->boundary_data.boundary_flag == false) {
		return 0.0;
	}
	else {
		return this->ds_dt_bound;
	}
}
///reset boundary discharge during synchronisation
void Hyd_Element_Groundwater_Type_Standard::reset_boundary_discharge_gw(void) {
	this->ds_dt_bound = 0.0;
	this->ds_dt_h_bound = 0.0;
	this->ds_dt_q_bound = 0.0;

}

//Add coupling discharge, which comming from coupled river models due to leakage coupling (coupling_q_rv_leakage) during syncronisation 
void Hyd_Element_Groundwater_Type_Standard::add_coupling_discharge_rv_leakage(const double discharge) {
	if (this->coupling_flag == false) {
		return;
	}

	this->ds_dt_rv_coup = this->ds_dt_rv_coup+ discharge;
	this->coup_mem->coupling_q_rv_leakage = this->coup_mem->coupling_q_rv_leakage + discharge;
}

//Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
void Hyd_Element_Groundwater_Type_Standard::set_maximum_value2querystring(ostringstream *query_string) {
	//max-values
	*query_string << this->max_h_value.maximum << " , ";
	*query_string << this->ds_dt_value * (this->flow_data.p_value * *this->area) << " , ";
	//*query_string << this->max_ds_dt.maximum << " , ";
	*query_string << this->watervolume << " , ";
	//volumes
	double boundary_in = 0.0;
	double boundary_out = 0.0;
	double coup_in = 0.0;
	double coup_out = 0.0;
	if (this->boundary_flag == true) {
		if (this->bound_mem->boundary_data.h_boundary_flag == true ) {
			boundary_in=boundary_in+this->h_boundary_volume.volume_in;
			boundary_out=boundary_out+this->h_boundary_volume.volume_out;
		}
		if (this->bound_mem->boundary_data.q_boundary_flag == true) {
			boundary_in = boundary_in + this->bound_mem->q_boundary_volume.volume_in;
			boundary_out = boundary_out + this->bound_mem->q_boundary_volume.volume_out;
		}
		*query_string << boundary_in << " , ";
		*query_string << boundary_out << " , ";

	}
	else {
		*query_string << 0.0 << " , ";
		*query_string << 0.0 << " , ";
	}
	if (this->coupling_flag == true) {
		
		//*query_string << this->coup_mem->coupling_volume_structure_rv.volume_in << " , ";
		//*query_string << this->coup_mem->coupling_volume_structure_rv.volume_out << " , ";
		//*query_string << this->coup_mem->coupling_volume_direct_rv.volume_in << " , ";
		//*query_string << this->coup_mem->coupling_volume_direct_rv.volume_out << " , ";
		
		if (this->rv_coupled == true) {
			*query_string << this->coup_mem->coupling_volume_leakage_rv.volume_in << " , ";
			*query_string << this->coup_mem->coupling_volume_leakage_rv.volume_out << " , ";
		}
		else {
			*query_string << 0.0 << " , ";
			*query_string << 0.0 << " , ";
		}
		if (this->gw_coupled == true) {
			*query_string << this->coup_mem->coupling_volume_gw.volume_in << " , ";
			*query_string << this->coup_mem->coupling_volume_gw.volume_out << " , ";
		}
		else {
			*query_string << 0.0 << " , ";
			*query_string << 0.0 << " , ";
		}

		
	}
	else {
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

		//*query_string << 0.0 << " , ";
		//*query_string << 0.0 << " , ";
		//*query_string << 0.0 << " , ";
		//*query_string << 0.0 << " , ";

		//*query_string << 0.0 << " , ";
		//*query_string << 0.0 << " , ";

	}
	*query_string << this->ds_dt_x_darcy*(this->flow_data.p_value * *this->area) << " , ";
	*query_string << this->ds_dt_y_darcy * (this->flow_data.p_value * *this->area) << " , ";
	*query_string << "true" << " , ";
	*query_string << this->max_h_value.maximum + (*this->z_value) << " , ";
}
//Set the instationary result values of an element to an query string to transfer them into a database table (Hyd_Element_Groundwater
void Hyd_Element_Groundwater_Type_Standard::set_instat_value2querystring(ostringstream *query_string) {
	//instat-values
	*query_string << this->get_h_value() << " , ";
	*query_string << this->get_s_value() << " , ";
	*query_string << this->get_ds2dt_value() << " , ";
}
//Reset the hydrological balance value and the maximum values
void Hyd_Element_Groundwater_Type_Standard::reset_hydrobalance_maxvalues(void) {

	_Hyd_Element_Groundwater_Type::reset_hydrobalance_maxvalues();

	this->s_value = (*this->z_value)+this->h_value;
	this->h_value = this->flow_data.init_condition;
	this->ds_dt_value = 0.0;
	//maximum result variables
	this->max_h_value.maximum = 0.0;
	this->max_ds_dt.maximum = 0.0;
	this->max_h_value.time_point = -1.0;
	this->max_ds_dt.time_point = -1.0;

	//coupling and boundary members
	this->reset_hydro_balance();

}
//Get the maximum change in a element 
void Hyd_Element_Groundwater_Type_Standard::get_max_changes_elements(double *max_change_h, const bool timecheck) {

	if (this->coupling_flag == true || this->boundary_flag == true) {
		this->ds_dt_coup = 0.0;
		double q_bound;
		if (this->coupling_flag == true) {
			this->ds_dt_coup = this->coup_mem->sum_coupling_discharge();
		}
		if (this->boundary_flag == true) {
			if (this->bound_mem->boundary_data.h_boundary_flag == true) {
				//REVIEW (only x_dir)
				q_bound = this->get_cx_bound() * (this->bound_mem->h_boundary-this->s_value) * ((*this->area) * this->flow_data.p_value); //m/s
				this->ds_dt_coup = this->ds_dt_coup + q_bound;
			}
			if (this->bound_mem->boundary_data.q_boundary_flag == true) {
				q_bound = this->bound_mem->q_boundary;//m^3/s
				this->ds_dt_coup = this->ds_dt_coup + q_bound;
				this->ds_dt_coup = this->ds_dt_coup;
			}



			/*if (this->bound_mem->boundary_data.head_flag == false) {
				q_bound = this->bound_mem->boundary;
				this->ds_dt_coup = this->ds_dt_coup + q_bound;
				this->ds_dt_coup = this->ds_dt_coup / ((*this->area) * this->flow_data.p_value);
			}
			else {
				q_bound = this->get_cx_bound() * (this->s_value - this->bound_mem->boundary);
				this->ds_dt_coup = this->ds_dt_coup / ((*this->area) * this->flow_data.p_value);
				this->ds_dt_coup = this->ds_dt_coup + q_bound;
			}*/
		}
		this->ds_dt_coup = this->ds_dt_coup / ((*this->area) * this->flow_data.p_value);
		*max_change_h = abs(this->ds_dt_coup_old - this->ds_dt_coup);

		if (timecheck == false) {
			this->ds_dt_coup_old = this->ds_dt_coup;
		}

	}
	else {
		*max_change_h = 0.0;
	}

}
//Get the coupling flag
bool Hyd_Element_Groundwater_Type_Standard::get_coupling_flag(void) {
	return this->coupling_flag;
}
//Get boundary condition flag
bool Hyd_Element_Groundwater_Type_Standard::get_bound_flag(void) {
	return this->boundary_flag;
}
//Delete the data of boundary condition
void Hyd_Element_Groundwater_Type_Standard::delete_bound_data(void) {
	if (this->bound_mem != NULL) {
		delete this->bound_mem;
		this->bound_mem = NULL;
		this->boundary_flag = false;
	}
}
//Set coupling data
void Hyd_Element_Groundwater_Type_Standard::set_coupling_data(void) {
	if (this->coupling_flag == true) {
		return;
	}
	this->allocate_coup_data();
}

//set rv coupled flag
void Hyd_Element_Groundwater_Type_Standard::set_rv_coupled_flag(bool flag) {
	this->rv_coupled = flag;
}

//set gw coupled flag
void Hyd_Element_Groundwater_Type_Standard::set_gw_coupled_flag(bool flag) {
	this->gw_coupled = flag;
	if (flag == true) {
		this->vertex_flag = false;
	}
}

//return rv coupled flag
bool Hyd_Element_Groundwater_Type_Standard::get_rv_coupled_flag(void) {
	return rv_coupled;
}

//return gw coupled flag
bool Hyd_Element_Groundwater_Type_Standard::get_gw_coupled_flag(void) {
	return gw_coupled;
}


//________________
//private
//Get the flow data structure
_hyd_flowdata_groundwater_elem Hyd_Element_Groundwater_Type_Standard::get_flow_data(void) {
	return this->flow_data;
}
//Get the boundary data structure
_hyd_boundarydata_groundwater_elem Hyd_Element_Groundwater_Type_Standard::get_boundary_data(void) {
	if (this->boundary_flag == true) {
		return this->bound_mem->boundary_data;
	}
	else {
		_hyd_boundarydata_groundwater_elem buffer;
		Hyd_Element_Bound_GW_Members::init_boundarydata_structures(&buffer);
		return buffer;

	}
}
//Output the setted members
void Hyd_Element_Groundwater_Type_Standard::output_setted_members(ostringstream *cout) {

	*cout << W(14) << P(2) << FORMAT_FIXED_REAL << (*this->z_value);
	//x-direction
	if (this->flow_data.no_flow_x_flag == true) {
		*cout << W(14) << " NO-flow " << W(7) << "-";
	}
	else if (this->flow_data.no_flow_x_flag == false) {
		*cout << W(14) << " DAR-flow " << W(7) << "-";
	}
	//y-direction
	if (this->flow_data.no_flow_y_flag == true) {
		*cout << W(18) << " NO-flow " << W(7) << "-";
	}

	else if (this->flow_data.no_flow_y_flag == false) {
		*cout << W(18) << " DAR-flow " << W(7) << "-";
	}
	//boundary
	if (this->boundary_flag == false) {
		*cout << W(16) << " NO-bound " << W(10) << "-";
	}
	else {
		if (this->bound_mem->boundary_data.h_boundary_flag == true && this->bound_mem->boundary_data.h_stationary_flag == true) {
			*cout << W(16) << " H-STAT " << W(10) << P(5) << FORMAT_FIXED_REAL << this->bound_mem->boundary_data.h_value;
		}
		else if (this->bound_mem->boundary_data.h_boundary_flag == true && this->bound_mem->boundary_data.h_stationary_flag == false) {
			*cout << W(16) << " H-INSTAT " << W(10) << this->bound_mem->boundary_data.h_curve_number;
		}
		if (this->bound_mem->boundary_data.q_boundary_flag == true && this->bound_mem->boundary_data.q_stationary_flag == true) {
			*cout << W(16) << " Q-STAT " << W(10) << P(5) << FORMAT_FIXED_REAL << this->bound_mem->boundary_data.q_value;
		}
		else if (this->bound_mem->boundary_data.q_boundary_flag == true && this->bound_mem->boundary_data.q_stationary_flag == false) {
			*cout << W(16) << " Q-INSTAT " << W(10) << this->bound_mem->boundary_data.q_curve_number;
		}
	}
	/*else if (this->bound_mem->boundary_data.boundary_flag == true && this->bound_mem->boundary_data.stationary_flag == false) {
		*cout << W(16) << " INSTAT " << W(10) << this->bound_mem->boundary_data.curve_number;
	}*/
	//conductivity values
	*cout << W(10) << this->flow_data.con_type << W(12) << P(5) << FORMAT_FIXED_REAL << this->flow_data.kf_value;
	//porosity values
	*cout << W(10) << this->flow_data.por_type << W(12) << P(5) << FORMAT_FIXED_REAL << this->flow_data.p_value;
	//thickness
	*cout << W(10) << FORMAT_FIXED_REAL << this->flow_data.thickness;
	//init and connection cell
	*cout << W(12) << P(2) << FORMAT_FIXED_REAL << this->flow_data.init_condition << W(10) << "-";


}
//Ouput the result members per internal timestep
void Hyd_Element_Groundwater_Type_Standard::output_result_values(ostringstream *cout) {
	if (this->h_value <= constant::dry_hyd_epsilon) {
		*cout << W(12) << functions::convert_boolean2string(false);
		*cout << W(15) << P(4) << FORMAT_FIXED_REAL << 0.0;
		*cout << W(15) << P(4) << FORMAT_FIXED_REAL << this->s_value;
		*cout << W(15) << P(4) << FORMAT_FIXED_REAL << this->ds_dt_value;
		*cout << W(17) << P(4) << FORMAT_FIXED_REAL << 0.0;
		*cout << W(15) << P(4) << FORMAT_FIXED_REAL << 0.0;
		*cout << W(15) << P(4) << FORMAT_FIXED_REAL << 0.0;
		*cout << endl;
	}
	else {
		*cout << W(15) << P(4) << FORMAT_FIXED_REAL << this->h_value;
		*cout << W(15) << P(4) << FORMAT_FIXED_REAL << this->s_value;
		*cout << W(15) << P(4) << FORMAT_FIXED_REAL << this->ds_dt_value;
		*cout << endl;
	}
}
//Ouput the maximum calculated values (not in use)
void Hyd_Element_Groundwater_Type_Standard::output_maximum_calculated_values(ostringstream *cout) {
	
	*cout << W(15) << P(4) << FORMAT_FIXED_REAL << this->max_h_value.maximum << W(12) << this->max_h_value.maximum + (*this->z_value) << W(12) << P(0) << FORMAT_FIXED_REAL << this->max_h_value.time_point;
	*cout << W(15) << P(4) << FORMAT_FIXED_REAL << this->max_ds_dt.maximum*60.0 << W(12) << P(0) << FORMAT_FIXED_REAL << this->max_ds_dt.time_point;
	*cout << P(1) << FORMAT_SCIENTIFIC_REAL;
	if (this->coupling_flag == true) {
		// REVIEW
		*cout << W(20) << this->coup_mem->coupling_volume_gw.volume_in;
		*cout << W(20) << this->coup_mem->coupling_volume_gw.volume_out;
		/*
		*cout << W(20) << this->coup_mem->coupling_volume_structure_rv.volume_in;
		*cout << W(20) << this->coup_mem->coupling_volume_structure_rv.volume_out;
		*cout << W(20) << this->coup_mem->coupling_volume_direct_rv.volume_in;
		*cout << W(20) << this->coup_mem->coupling_volume_direct_rv.volume_out;
		*cout << W(20) << this->coup_mem->coupling_volume_overflow_rv.volume_in;
		*cout << W(20) << this->coup_mem->coupling_volume_overflow_rv.volume_out;
		*cout << W(22) << this->coup_mem->coupling_volume_dikebreak_rv.volume_in;
		*cout << W(22) << this->coup_mem->coupling_volume_dikebreak_rv.volume_out;
		*cout << W(22) << this->coup_mem->coupling_volume_overflow_co.volume_in;
		*cout << W(20) << this->coup_mem->coupling_volume_overflow_co.volume_out;
		*cout << W(20) << this->coup_mem->coupling_volume_dikebreak_co.volume_in;
		*cout << W(20) << this->coup_mem->coupling_volume_dikebreak_co.volume_out;
		*/
	}
	*cout << endl;
}
//Set the data buffers of _hyd_boundarydata_groundwater_elem, _hyd_flowdata_groundwater_elem from class Hyd_Groundwater_Element
void Hyd_Element_Groundwater_Type_Standard::set_data_buffers(const _hyd_boundarydata_groundwater_elem boundary, const _hyd_flowdata_groundwater_elem flow, double *z_value) {
	this->set_boundary_condition_data(boundary);

	this->flow_data = flow;
	this->z_value = z_value;
	this->s_value = (*this->z_value)+this->h_value;
}
//Set new boundary condition data
void Hyd_Element_Groundwater_Type_Standard::set_boundary_condition_data(const _hyd_boundarydata_groundwater_elem boundary) {
	if (boundary.boundary_flag == true) {
		this->allocate_bound_data();
		this->bound_mem->boundary_data = boundary;

		if (this->bound_mem->boundary_data.h_stationary_flag == false && this->bound_mem->boundary_data.h_boundary_flag == true) {
			this->bound_mem->boundary_data.found_flag = false;
		}
		if (this->bound_mem->boundary_data.q_stationary_flag == false && this->bound_mem->boundary_data.q_boundary_flag == true) {
			this->bound_mem->boundary_data.found_flag = false;
		}
		//check the boundary type
		if (this->bound_mem->boundary_data.geom_type == _hyd_bound_geom_type::gw_length_type) {
			Error msg = this->set_error(0);
			ostringstream info;
			info << "Given type: " << Hyd_Instationary_Boundary_GW::transform_instatboundtype2txt_geom(this->bound_mem->boundary_data.geom_type) << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Set element flag for flow in x-direction
void Hyd_Element_Groundwater_Type_Standard::set_x_noflow_flag(const bool flag) {
	this->flow_data.no_flow_x_flag = flag;
}
//Set element flag for flow in y-direction
void Hyd_Element_Groundwater_Type_Standard::set_y_noflow_flag(const bool flag) {
	this->flow_data.no_flow_y_flag = flag;
}



//Calculate the maximum values to a given time point
void Hyd_Element_Groundwater_Type_Standard::calculate_maximum_values(const double time_point) {
	if (this->h_value < constant::dry_hyd_epsilon) {
		return;
	}

	//calculation of the maximum
	if (this->h_value > this->max_h_value.maximum) {
		this->max_h_value.maximum = this->h_value;
		this->max_h_value.time_point = time_point;
	}
	if (abs(this->ds_dt_value) > this->max_ds_dt.maximum) {
		this->max_ds_dt.maximum = abs(this->ds_dt_value);
		this->max_ds_dt.time_point = time_point;
	}
}

//Calculate the watervolume in the element REVIEW
void Hyd_Element_Groundwater_Type_Standard::calculate_watervolume_element(const bool infiltration) {
	this->watervolume = this->h_value*(*this->area);

}
//Check if there are unphysical waterlevel calculated by the solver REVIEW
void Hyd_Element_Groundwater_Type_Standard::check_unphysical_waterlevels(void) {
	if (abs(this->h_value) > 19882.0) {
		Error msg = this->set_error(1);
		throw msg;
	}
	if (functions::check_infinite(this->h_value) == true) {
		Error msg = this->set_error(2);
		throw msg;
	}
}
//Allocate the data of coupling
void Hyd_Element_Groundwater_Type_Standard::allocate_coup_data(void) {
	this->delete_coup_data();
	try {
		this->coup_mem = new Hyd_Element_Coup_GW_Members;
	}
	catch (bad_alloc&t) {
		Error msg = this->set_error(3);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	this->coupling_flag = true;
}
//Delete the data of coupling
void Hyd_Element_Groundwater_Type_Standard::delete_coup_data(void) {
	if (this->coup_mem != NULL) {
		delete this->coup_mem;
		this->coup_mem = NULL;
		this->coupling_flag = false;
	}
}
//Allocate the data of boundary condition
void Hyd_Element_Groundwater_Type_Standard::allocate_bound_data(void) {
	this->delete_bound_data();
	try {
		this->bound_mem = new Hyd_Element_Bound_GW_Members;
	}
	catch (bad_alloc&t) {
		Error msg = this->set_error(4);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	this->boundary_flag = true;
	this->bound_mem->boundary_data.boundary_flag = true;
}


//calculate the T over time
void Hyd_Element_Groundwater_Type_Standard::calculate_T(void) {
	//
	if (this->h_value > this->flow_data.thickness) {
		this->T = this->flow_data.thickness * this->flow_data.kf_value;
	}
	else {
		this->T =max( this->h_value,0.001) * this->flow_data.kf_value;
	}
}

//get the T
double Hyd_Element_Groundwater_Type_Standard::get_T(void) {
	return this->T;
}

//set vertex flag
void Hyd_Element_Groundwater_Type_Standard::set_vertex_flag(void) {
	if ((this->element_x == NULL && (this->element_y == NULL || this->element_y_minus == NULL)) || (this->element_x_minus == NULL && (this->element_y == NULL || this->element_y_minus == NULL))) {
		if (this->gw_coupled == false) {
			this->vertex_flag = true;
		}
		else {
			this->vertex_flag = false;
		}
		
	}
}

//get vertex flag
bool Hyd_Element_Groundwater_Type_Standard::get_vertex_flag(void) {
	return this->vertex_flag;
}

//Calculate hydrological balance for h boundary
void Hyd_Element_Groundwater_Type_Standard::calculate_h_hydro_balance(const double delta_t) {
	double q_bound;
	if (this->bound_mem->boundary_data.h_boundary_flag == true) {
		q_bound = this->get_cx_bound() * (this->s_value - this->bound_mem->h_boundary) * ((*this->area) * this->flow_data.p_value); //m^3/s
		if (this->bound_mem->h_boundary > 0.0) {
			this->h_boundary_volume.volume_in = this->h_boundary_volume.volume_in + q_bound * delta_t;
		}
		else if (this->bound_mem->h_boundary < 0.0) {
			this->h_boundary_volume.volume_out = this->h_boundary_volume.volume_out - q_bound * delta_t;
		}
		//total boundary volume
		this->h_boundary_volume.volume_total = this->h_boundary_volume.volume_in - this->h_boundary_volume.volume_out;
	}
	else {
		return;
	}
}

Error Hyd_Element_Groundwater_Type_Standard::set_error(const int err_type) {
	string place = "Hyd_Element_Groundwater_Type_Standard::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;

	switch (err_type) {
	case 0://not suitable boundary type
		place.append("set_data_buffers(const _hyd_boundarydata_groundwater_elem boundary, const _hyd_flowdata_groundwater_elem flow, const double z_value)");
		reason = "There is not a suitable boundary type given for this element";
		help = "Check the given boundary type; possible boundary type are: area and point";
		type = 18;
		break;
	case 1://emergency stop of the solver
		place.append("check_unphysical_waterlevels(void)");
		reason = "The waterlevel has no physical sense; the absolute value are > 19882.0 m (Marian Gap + Mont Everest)";
		help = "Check the model and solver settings";
		type = 17;
		break;
	case 2://the waterlevels are not defined
		place.append("check_unphysical_waterlevels(void)");
		reason = "The waterlevel is not defined";
		help = "Check the model and solver settings";
		type = 17;
		break;
	case 3://bad alloc
		place.append("allocate_coup_data(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 4://bad alloc
		place.append("allocate_bound_data(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	default:
		place.append("set_error(const int err_type)");
		reason = "Unknown flag!";
		help = "Check the flags";
		type = 6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}





///alt code, not used
//Calculate the variation of the global waterlevel (s-value) over time for flow in x-direction with the Darcy-formula 
void Hyd_Element_Groundwater_Type_Standard::calculate_ds_dt_darcy_x(void) {

	double flow_depth = 0.0;
	double flow_depth_neigh = 0.0;
	double delta_h = 0.0;
	double ds_dt_buff = 0.0;

	/*
		ostringstream cout;
		cout << "s: " << this->s_value << "; id_x: " << this->element_x->get_elem_number()  << "; s_x: " << this->element_x->element_type->get_s_value() << "; darcy_x: " << c_x * (this->element_x->element_type->get_s_value()- this->s_value) << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	*/

	ds_dt_buff = this->get_cx() * (this->element_x->element_type->get_s_value() - this->s_value);
	this->ds_dt_x_darcy = ds_dt_buff;
	/* This was taken over from the floodplain task, but does not seem to be useful here
	if (this->h_value <= constant::flow_epsilon && this->element_x->element_type->get_h_value() <= constant::flow_epsilon) {
		return;
	}

	//calculate the mid of the flow depth
	flow_depth = this->s_value - this->s_max_x;
	if (flow_depth < 0.0) {
		flow_depth = 0.0;
	}
	flow_depth_neigh = this->element_x->element_type->get_s_value() - this->s_max_x;
	if (flow_depth_neigh < 0.0) {
		flow_depth_neigh = 0.0;
	}
	//flow_depth=flow_depth*0.5+flow_depth_neigh*0.5;
	flow_depth = max(flow_depth, flow_depth_neigh);
	//set a limit to the flow depth

	if (flow_depth <= constant::flow_epsilon) {
		this->v_x = 0.0;
		ds_dt_buff = 0.0;
	}
	else {
		delta_h = this->element_x->element_type->get_s_value() - this->s_value;
		double abs_delta_h = abs(delta_h);

		if (abs_delta_h <= constant::flow_epsilon) {
			this->v_x = 0.0;
			ds_dt_buff = 0.0;
		}
		else {
			ds_dt_buff = this->c_x*pow(flow_depth, (5.0 / 3.0));
			//replace the manning strickler function by a tangens- function by a given boundary; this functions is the best fit to the square-root
			//functions between 0.001 m and 0.02 m; the boundary is set, where the functions (arctan/square root) are identically)
			if (abs_delta_h <= 0.005078) {
				ds_dt_buff = ds_dt_buff * 0.10449968880528*atan(159.877741951379*delta_h); //0.0152
			}
			//if(abs_delta_h<=0.009541142){
			//	ds_dt_buff=ds_dt_buff*0.10489968880528*atan(140.877741951379*delta_h); //0.0152
			//}
			////if(abs_delta_h<=0.000246925){
			////	ds_dt_buff=ds_dt_buff*0.02304416717797*atan(3287.68191373281*delta_h); //0.0152
			////}
			////if(abs_delta_h<=0.007581357){
			////	ds_dt_buff=ds_dt_buff*0.07370400068147*atan(321.40326964290100*delta_h); //0.0152
			////}

			//if(abs_delta_h<=0.01){
			//	ds_dt_buff=ds_dt_buff*(delta_h*1.0/(pow(0.01,0.5))); //0.0152
			//}
			else {
				ds_dt_buff = ds_dt_buff * (delta_h / pow(abs_delta_h, 0.5));
			}

			//calculate the flow-velocity
			this->v_x = -1.0*ds_dt_buff*(*this->x_width) / flow_depth;
		}
	}
	*/
}
//Calculate the variation of the global waterlevel (s-value) over time for flow in y-direction with the Darcy-formula 
void Hyd_Element_Groundwater_Type_Standard::calculate_ds_dt_darcy_y(void) {
	double flow_depth = 0.0;
	double flow_depth_neigh = 0.0;
	double delta_h = 0.0;
	double ds_dt_buff = 0.0;

	/*
		ostringstream cout;
		cout<< "s: " << this->s_value << "; id_y: " << this->element_y->get_elem_number()  << "; s_y: " << this->element_y->element_type->get_s_value() << "; darcy_y: " << c_y * (this->element_y->element_type->get_s_value()-this->s_value) << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	*/

	ds_dt_buff = this->get_cy() * (this->element_y->element_type->get_s_value() - this->s_value);
	this->ds_dt_y_darcy = ds_dt_buff;
	/* This was taken over from the floodplain task, but does not seem to be useful here
	if (this->h_value <= constant::flow_epsilon && this->element_y->element_type->get_h_value() <= constant::flow_epsilon) {
		return;
	}

	flow_depth = this->s_value - this->s_max_y;
	if (flow_depth < 0.0) {
		flow_depth = 0.0;
	}
	flow_depth_neigh = this->element_y->element_type->get_s_value() - this->s_max_y;
	if (flow_depth_neigh < 0.0) {
		flow_depth_neigh = 0.0;
	}
	//flow_depth=flow_depth*0.5+flow_depth_neigh*0.5;
	flow_depth = max(flow_depth, flow_depth_neigh);

	//set a limit to the flow depth
	if (flow_depth <= constant::flow_epsilon) {
		this->v_y = 0.0;
		ds_dt_buff = 0.0;
	}
	else {
		delta_h = this->element_y->element_type->get_s_value() - this->s_value;
		double abs_delta_h = abs(delta_h);
		if (abs_delta_h <= constant::flow_epsilon) {
			ds_dt_buff = 0.0;
			this->v_y = 0.0;
		}
		else {
			ds_dt_buff = this->c_y*pow(flow_depth, (5.0 / 3.0));
			//replace the manning strickler function by a tangens- function by a given boundary
			if (abs_delta_h <= 0.005078) {
				ds_dt_buff = ds_dt_buff * 0.10449968880528*atan(159.877741951379*delta_h); //0.0152
			}
			//if(abs_delta_h<=0.009541142){
			//	ds_dt_buff=ds_dt_buff*0.10489968880528*atan(140.877741951379*delta_h); //0.0152
			//}
			//if(abs_delta_h<=0.000246925){
			//	ds_dt_buff=ds_dt_buff*0.02304416717797*atan(3287.68191373281*delta_h); //0.0152
			//}
			//if(abs_delta_h<=0.007581357){
			//	ds_dt_buff=ds_dt_buff*0.07370400068147*atan(321.40326964290100*delta_h); //0.0152
			//}


			//if(abs_delta_h<=0.01){
			//	ds_dt_buff=ds_dt_buff*(delta_h*1.0/(pow(0.01,0.5))); //0.0152
			//}
			else {
				ds_dt_buff = ds_dt_buff * (delta_h / pow(abs_delta_h, 0.5));
			}

			//calculate the flow-velocity
			this->v_y = -1.0*ds_dt_buff*(*this->y_width) / flow_depth;
		}

	}
	*/
}
