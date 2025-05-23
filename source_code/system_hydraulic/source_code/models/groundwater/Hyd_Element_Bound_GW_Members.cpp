#include "source_code\Hyd_Headers_Precompiled.h"
//#include "Hyd_Element_Bound_GW_Members.h"

//constructor
Hyd_Element_Bound_GW_Members::Hyd_Element_Bound_GW_Members(void) {

	this->reset_vol_q();
	this->init_boundarydata_structures(&this->boundary_data);

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Element_Bound_GW_Members), _sys_system_modules::HYD_SYS);
}

//destructor
Hyd_Element_Bound_GW_Members::~Hyd_Element_Bound_GW_Members(void) {


	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Element_Bound_GW_Members), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//Initialize the data structures of _hyd_boundarydata_groundwater_elem (static
void Hyd_Element_Bound_GW_Members::init_boundarydata_structures(_hyd_boundarydata_groundwater_elem * data) {
	data->boundary_flag = false;

	data->h_boundary_flag = false;
	data->q_boundary_flag = false;

	data->h_curve_number = -1;
	data->q_curve_number = -1;
	data->found_flag = true;
	data->h_stationary_flag = false;
	data->h_value = 0.0;

	/*data->q_curve_number = -1;*/
	//data->found_flag = true;
	data->q_stationary_flag = false;
	
	data->q_value = 0.0;

	data->ptr_hboundarycurve = NULL;
	data->ptr_qboundarycurve = NULL;
	/*data->boundary_type = _hyd_bound_type_gw::discharge_boundary;
	data->head_flag = false;*/
	//boundary direction may be used later to account for anisotropy. At the moment it is not relevant though
	data->boundary_direction = _hyd_bound_direction_type::x_dir;
	data->geom_type = _hyd_bound_geom_type::gw_point_type;
}
//Reset volumes and discharge
void Hyd_Element_Bound_GW_Members::reset_vol_q(void) {
	this->h_boundary = 0.0;


	this->q_boundary = 0.0;
	this->q_boundary_volume.volume_in = 0.0;
	this->q_boundary_volume.volume_out = 0.0;
	this->q_boundary_volume.volume_total = 0.0;
}
//Calculate boundary value in m^3/s
void Hyd_Element_Bound_GW_Members::calculate_q_bound_value(const double time, const double area) {
	//head bondary would be calculated in the Hyd_Element_Groundwater_Type_Standard
	//if (this->boundary_data.head_flag == true) {
	//	return;
	//}
	//
	//if (this->boundary_data.head_flag == false) {
	//	if (this->boundary_data.stationary_flag == true) {
	//		this->boundary = this->boundary_data.value;
	//		//for area boundary type 
	//		if (this->boundary_data.geom_type == _hyd_bound_geom_type::gw_area_type) {
	//			this->boundary = this->boundary * (area);
	//		}
	//	}
	//	else if (this->boundary_data.stationary_flag == false) {
	//		this->boundary = this->boundary_data.ptr_boundarycurve->calculate_actuel_boundary_value(time);
	//		if (this->boundary_data.geom_type == _hyd_bound_geom_type::gw_area_type) {
	//			this->boundary = this->boundary * (area);
	//		}
	//	}
	//
	//}
	//if (this->boundary_data.stationary_flag == true) {
	//	if (this->boundary_data.head_flag == false) {
	//		this->boundary = this->boundary_data.value;
	//		//for area boundary type 
	//		if (this->boundary_data.geom_type == _hyd_bound_geom_type::gw_area_type) {
	//			this->boundary = this->boundary * (area);
	//		}
	//	}
	//	else {
	//		this->boundary = this->boundary_data.value;
	//	}
	//	//head boundaries are handled in the Hyd_Model_Groundwater class because Bound..Members can not access element s and c_x value

	//}
	////instat
	//else if (this->boundary_data.stationary_flag == false) {
	//	if (this->boundary_data.head_flag == false) {
	//		this->boundary = this->boundary_data.ptr_boundarycurve->calculate_actuel_boundary_value(time);
	//		if (this->boundary_data.geom_type == _hyd_bound_geom_type::gw_area_type) {
	//			this->boundary = this->boundary * (area);
	//		}
	//	}
	//	else {
	//		this->boundary = this->boundary_data.ptr_boundarycurve->calculate_actuel_boundary_value(time);
	//	}

	//}
	//stationar
	if (this->boundary_data.q_stationary_flag == true) {
		this->q_boundary = this->boundary_data.q_value;
		//for area boundary type 
		if (this->boundary_data.geom_type == _hyd_bound_geom_type::gw_area_type) {
			this->q_boundary = this->q_boundary*(area);
		}
	}
	//instat
	else if (this->boundary_data.q_stationary_flag == false) {
		this->q_boundary = this->boundary_data.ptr_qboundarycurve->calculate_actuel_boundary_value(time);
		if (this->boundary_data.geom_type == _hyd_bound_geom_type::gw_area_type) {
			this->q_boundary = this->q_boundary*(area);
		}

		
	}
	
}


//Calculate h boundary value in m^3/s
void Hyd_Element_Bound_GW_Members::calculate_h_bound_value(const double time) {

	if (this->boundary_data.h_stationary_flag == true) {
		
		this->h_boundary = this->boundary_data.h_value;
	}
	//instat
	else if (this->boundary_data.h_stationary_flag == false) {
		this->h_boundary = this->boundary_data.ptr_hboundarycurve->calculate_actuel_boundary_value(time);

	}

}
//Calculate hydrological balance
void Hyd_Element_Bound_GW_Members::calculate_q_hydro_balance(const double delta_t) {
	if (this->q_boundary > 0.0) {
		this->q_boundary_volume.volume_in = this->q_boundary_volume.volume_in + this->q_boundary * delta_t;
	}
	else if (this->q_boundary < 0.0) {
		this->q_boundary_volume.volume_out = this->q_boundary_volume.volume_out - this->q_boundary * delta_t;
	}
	//total boundary volume
	this->q_boundary_volume.volume_total = this->q_boundary_volume.volume_in - this->q_boundary_volume.volume_out;
 
//	if (this->boundary_data.head_flag == false) {
//		//boundary volume
//		if (this->boundary > 0.0) {
//			this->boundary_volume.volume_in = this->boundary_volume.volume_in + this->boundary * delta_t;
//		}
//		else if (this->boundary < 0.0) {
//			this->boundary_volume.volume_out = this->boundary_volume.volume_out - this->boundary * delta_t;
//		}
//		//total boundary volume
//		this->boundary_volume.volume_total = this->boundary_volume.volume_in - this->boundary_volume.volume_out;
//
//	}
//	else {
//		//boundary volume
//		if (this->boundary > 0.0) {
//			this->boundary_volume.volume_in = this->boundary_volume.volume_in + this->boundary * delta_t;
//		}
//		else if (this->boundary < 0.0) {
//			this->boundary_volume.volume_out = this->boundary_volume.volume_out - this->boundary * delta_t;
//		}
//		//total boundary volume
//		this->boundary_volume.volume_total = this->boundary_volume.volume_in - this->boundary_volume.volume_out;
//	}
//
//
}