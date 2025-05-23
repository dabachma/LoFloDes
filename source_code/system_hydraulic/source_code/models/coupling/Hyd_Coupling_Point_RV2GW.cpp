#include "source_code\Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_Point_RV2GW.h"

//constructor
Hyd_Coupling_Point_RV2GW::Hyd_Coupling_Point_RV2GW(void) {
	this->river_profile_index_up = -1;
	this->river_profile_up = NULL;
	this->river_profile_index_down = -1;
	this->river_profile_down = NULL;
	this->groundwater_elem_index = -1;
	this->groundwater_elem = NULL;
	this->groundwater_index = -1;
	this->file_name = "";
	//REVIEW
	this->gw_section_id = -1;

	this->first_found_elem_index = -1;
	this->mid_height = 0.0;
	this->mid_basepoint = 0.0;
	this->mid_basepoint_profile = 0.0;
	this->mid_fac_up = 0.0;
	this->mid_fac_down = 0.0;
	this->mid_waterlevel = 0.0;

	this->leakage_flag = true;
	this->leakage_flag_fixed = false;

	this->horizontal_backwater_flag = false;
	this->horizontal_backwater_flag_upstream = false;

	this->total_flow_width = 0.0;
	this->leakage_width = 0.0;
	this->max_h.maximum = 0.0;
	this->max_h.time_point = 0.0;


	this->grad_q_current = 0.0;
	this->grad_q_before = 0.0;
	this->oscilation_smoother = 1.0;
	this->number_osci_counter = 2.0;
	this->no_osci_counter = 0.0;

	this->grad_q_break_current = 0.0;
	this->grad_q_break_before = 0.0;
	this->oscilation_smoother_break = 1.0;
	this->number_osci_counter_break = 2.0;
	this->no_osci_counter_break = 0.0;

	this->predicted_h_two = 0.0;
	this->corrected_h_two = 0.0;
	this->gradient_list_h_two.clear();
	for (int i = 0; i < constant::no_stored_grad; i++) {
		this->gradient_list_h_two.append(0.0);
	}
	this->gradient_h_two = 0.0;
	this->calc_h_two = 0.0;
	this->old_calc_h_two = 0.0;

	this->store_grad_q_before = 0.0;
	this->store_oscilation_smoother = 1.0;
	this->store_number_osci_counter = 2.0;
	this->store_no_osci_counter = 0.0;

	this->groundwater_h = 0.0;
	this->river_h = 0.0;
	///final used riverbed height
	this->riverbed_h=0.0;
	///final used flow length
	this->flow_length_m=0.0;
	///final used wetted perimeter
	this->wetted_p=0.0;



	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Point_RV2GW), _sys_system_modules::HYD_SYS);

}
//Copy constructor
Hyd_Coupling_Point_RV2GW::Hyd_Coupling_Point_RV2GW(const Hyd_Coupling_Point_RV2GW& object) {
	_Hyd_Coupling_Point::operator =(object);
	this->river_profile_index_up = object.river_profile_index_up;
	this->river_profile_up = object.river_profile_up;
	this->river_profile_index_down = object.river_profile_index_down;
	this->river_profile_down = object.river_profile_down;
	this->groundwater_index = object.groundwater_index;
	this->groundwater_elem_index = object.groundwater_elem_index;
	this->first_found_elem_index = object.first_found_elem_index;
	this->groundwater_elem = object.groundwater_elem;
	this->gw_section_id = object.gw_section_id;

	this->mid_basepoint = object.mid_basepoint;
	this->mid_basepoint_profile = object.mid_basepoint_profile;
	this->mid_height = object.mid_height;
	this->mid_fac_up = object.mid_fac_up;
	this->mid_fac_down = object.mid_fac_down;
	this->mid_waterlevel = object.mid_waterlevel;

	this->leakage_flag = object.leakage_flag;
	this->leakage_flag_fixed = object.leakage_flag_fixed;

	this->horizontal_backwater_flag = object.horizontal_backwater_flag;
	this->horizontal_backwater_flag_upstream = object.horizontal_backwater_flag_upstream;


	this->total_flow_width = object.total_flow_width;
	this->leakage_width = object.leakage_width;


	this->grad_q_current = object.grad_q_current;
	this->grad_q_before = object.grad_q_before;
	this->oscilation_smoother = object.oscilation_smoother;
	this->number_osci_counter = object.number_osci_counter;
	this->no_osci_counter = object.no_osci_counter;

	this->grad_q_break_current = object.grad_q_break_current;
	this->grad_q_break_before = object.grad_q_break_before;
	this->oscilation_smoother_break = object.oscilation_smoother_break;
	this->no_osci_counter_break = object.no_osci_counter_break;
	this->number_osci_counter_break = object.number_osci_counter_break;

	this->corrected_h_two = object.corrected_h_two;
	this->predicted_h_two = object.predicted_h_two;
	this->gradient_h_two = object.gradient_h_two;
	this->calc_h_two = object.calc_h_two;
	this->old_calc_h_two = object.old_calc_h_two;
	this->gradient_list_h_two.clear();
	for (int i = 0; i < constant::no_stored_grad; i++) {
		this->gradient_list_h_two.append(0.0);
	}
	this->store_grad_q_before = object.store_grad_q_before;
	this->store_oscilation_smoother = object.store_oscilation_smoother;
	this->store_number_osci_counter = object.store_number_osci_counter;
	this->store_no_osci_counter = object.store_no_osci_counter;

	this->file_name = object.file_name;
	this->Param_RV = object.Param_RV;


	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Point_RV2GW), _sys_system_modules::HYD_SYS);

}
//destructor
Hyd_Coupling_Point_RV2GW::~Hyd_Coupling_Point_RV2GW(void) {
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_Point_RV2GW), _sys_system_modules::HYD_SYS);

}
//_________
//public
//Set the pointer/index members (without the geometrical members) from the given point
void Hyd_Coupling_Point_RV2GW::set_couplingpoint_members(Hyd_Coupling_Point_RV2GW *object) {

	//from this class
	this->river_profile_index_up = object->river_profile_index_up;
	this->river_profile_up = object->river_profile_up;
	this->river_profile_index_down = object->river_profile_index_down;
	this->river_profile_down = object->river_profile_down;
	this->groundwater_index = object->groundwater_index;
	this->groundwater_elem_index = object->groundwater_elem_index;
	this->set_pointer_groundwater_element(object->groundwater_elem);
	this->first_found_elem_index = object->first_found_elem_index;
	this->gw_section_id = object->gw_section_id;
	//this->file_name = object->file_name;
	this->Param_RV = object->Param_RV;


}
//Output the header for the setted member (static))
void Hyd_Coupling_Point_RV2GW::output_header_setted_member(ostringstream *cout) {
	*cout << W(10) << "Id_rv_up," << W(10) << "Id_rv_down," << W(7) << "Id_gw," << W(11) << "Id_gw_elem,";
	*cout << W(15) << "Coupling," << W(15) << "leakage," << W(15) << "Dist_upstr" << label::m << "," << W(15) << "Dist_downstr" << label::m << ",";
	*cout << W(15) << "abs_Height" << label::m << ",";
	*cout << W(15) << "Dist_inflow" << label::m << ",";
	*cout << W(15) << "x" << label::m << "," << W(17) << "y" << label::m << ",";
	*cout << W(17) << "Id_gw_sec" << "," << W(7) << "mid_fac_down" << "," << W(7) << "mid_fac_up";
	*cout << endl;
	Sys_Common_Output::output_hyd->output_txt(cout, true);
}
//Output the setted members
void Hyd_Coupling_Point_RV2GW::output_setted_members(ostringstream *cout) {
	*cout << W(8) << this->river_profile_index_up << "," << W(10) << this->river_profile_index_down << ",";
	*cout << W(10) << this->groundwater_index << W(10) << "," << this->groundwater_elem_index << ",";
	*cout << W(15) << functions::convert_boolean2string(this->coupling_flag) << ",";
	*cout << W(15) << functions::convert_boolean2string(this->leakage_flag) << ",";
	*cout << W(15) << P(2) << FORMAT_FIXED_REAL << this->distance_down << ",";
	*cout << W(21) << P(2) << FORMAT_FIXED_REAL << this->distance_up << ",";
	*cout << W(17) << P(2) << FORMAT_FIXED_REAL << this->mid_height << ",";
	*cout << W(17) << this->distance_along_polysegment << ",";
	*cout << W(21) << P(2) << FORMAT_FIXED_REAL << this->x_coordinate << ",";
	*cout << W(21) << P(2) << FORMAT_FIXED_REAL << this->y_coordinate << ",";
	*cout << W(21) << P(0) << FORMAT_FIXED_REAL << this->gw_section_id << ",";
	*cout << W(21) << P(10) << FORMAT_FIXED_REAL << this->mid_fac_down << ",";
	*cout << W(21) << P(10) << FORMAT_FIXED_REAL << this->mid_fac_up;

	*cout << endl;
	Sys_Common_Output::output_hyd->output_txt(cout, true);
}
//Set the indices for the upwards river profile (corresponding to the index of the polysegment of the river bank line) and the element index of the Hyd_Groundwater_Element, which is used for calculation
void Hyd_Coupling_Point_RV2GW::set_indices(const int up_river_profile, const int groundwater_model_elem) {
	this->river_profile_index_up = up_river_profile;
	this->river_profile_index_down = this->river_profile_index_up + 1;
	this->groundwater_elem_index = groundwater_model_elem;
}

//Set the indices for the element index of the Hyd_Groundwater_Element, which is used for calculation
void Hyd_Coupling_Point_RV2GW::set_gw_indices(const int groundwater_model_elem) {
	this->groundwater_elem_index = groundwater_model_elem;
}

//Set the indices for the upwards river profile (corresponding to the index of the polysegment of the river bank line) 
void Hyd_Coupling_Point_RV2GW::set_rv_indices(const int up_river_profile) {
	this->river_profile_index_up = up_river_profile;
	this->river_profile_index_down = this->river_profile_index_up + 1;
}

//Set the pointers to the river profiles with help of the indices
void Hyd_Coupling_Point_RV2GW::set_pointer_river_profiles(Hyd_Model_River *river_model) {

	if (this->river_profile_index_up<0 || this->river_profile_index_up>river_model->Param_RV.get_number_profiles()) {
		this->river_profile_up = NULL;
		this->river_profile_down = NULL; 
		this->Param_RV = NULL;
	}
	//inflow profile upwards
	else if (this->river_profile_index_up == 0) {
		this->river_profile_up = &(river_model->inflow_river_profile);
		this->river_profile_down = &(river_model->river_profiles[this->river_profile_index_down - 1]);
		this->Param_RV = &(river_model->Param_RV);

	}
	//outflow profile downwards
	else if (this->river_profile_index_up == river_model->Param_RV.get_number_profiles() - 2) {
		this->river_profile_up = &(river_model->river_profiles[this->river_profile_index_up - 1]);
		this->river_profile_down = &(river_model->outflow_river_profile);
		this->Param_RV = &(river_model->Param_RV);
	
	}
	//just inbetween profiles
	else {
		this->river_profile_up = &(river_model->river_profiles[this->river_profile_index_up - 1]);
		this->river_profile_down = &(river_model->river_profiles[this->river_profile_index_down - 1]);
		this->Param_RV = &(river_model->Param_RV);

	}

}
//Set the indexof the first found Hyd_Groundwater_Element; it is not necessarily used for calculation (e.g. river elements)
void Hyd_Coupling_Point_RV2GW::set_first_found_elem_index(const int index) {
	this->first_found_elem_index = index;
}
//Set the pointers to the groundwater elements
void Hyd_Coupling_Point_RV2GW::set_pointer_groundwater_element(Hyd_Element_Groundwater *groundwater) {
	this->groundwater_elem = groundwater;
	if (this->groundwater_elem != NULL && this->groundwater_elem_index >= 0) {
		this->groundwater_elem->element_type->set_coupling_data();
		this->groundwater_elem->element_type->set_rv_coupled_flag(true);
	}
}
//Set the index of the groundwater element
void Hyd_Coupling_Point_RV2GW::set_index_groundwater_element(const int index) {
	this->groundwater_elem_index = index;
}
//Get the index of the groundwater element
int Hyd_Coupling_Point_RV2GW::get_index_groundwater_element(void) {
	return this->groundwater_elem_index;
}
//Set the index of the coupled groundwater
void Hyd_Coupling_Point_RV2GW::set_groundwater_index(const int index) {
	this->groundwater_index = index;
}
//Get the index of the coupled groundwater
int Hyd_Coupling_Point_RV2GW::get_groundwater_index(void) {
	return this->groundwater_index;
}
//Get the index of the coupled fpl-section
int Hyd_Coupling_Point_RV2GW::get_gw_section_index(void) {
	return this->gw_section_id;
}
//Get the mid of waterlevel of the two profiles in the river
double Hyd_Coupling_Point_RV2GW::get_mid_waterlevel(void) {
	return this->mid_waterlevel;
}
//Calculate the current mid-waterlevel via the factors in the river
void Hyd_Coupling_Point_RV2GW::calculate_mid_waterlevel(void) {
	if (this->coupling_flag == true) {
		this->horizontal_backwater_flag = false;
		this->horizontal_backwater_flag_upstream = false;
		//set the mid_waterlevel via the factors
		if (this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h() > constant::dry_hyd_epsilon && this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h() > constant::dry_hyd_epsilon) {
			this->mid_waterlevel = this->mid_fac_down*this->river_profile_down->get_actual_global_waterlevel() + this->mid_fac_up*this->river_profile_up->get_actual_global_waterlevel();
		}
		else if (this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h() <= constant::dry_hyd_epsilon&& this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h() > constant::dry_hyd_epsilon) {
			//this->mid_waterlevel=this->river_profile_up->get_actual_global_waterlevel();
			this->mid_waterlevel = this->mid_fac_down*this->river_profile_down->get_actual_global_waterlevel() + this->mid_fac_up*this->river_profile_up->get_actual_global_waterlevel();

			this->horizontal_backwater_flag_upstream = true;
		}
		else if (this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h() > constant::dry_hyd_epsilon&& this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h() <= constant::dry_hyd_epsilon) {
			//this->mid_waterlevel=this->river_profile_down->get_actual_global_waterlevel();
			this->mid_waterlevel = this->mid_fac_down*this->river_profile_down->get_actual_global_waterlevel() + this->mid_fac_up*this->river_profile_up->get_actual_global_waterlevel();

			this->horizontal_backwater_flag = true;
		}
		else if (this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h() <= constant::dry_hyd_epsilon && this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h() <= constant::dry_hyd_epsilon) {
			this->mid_waterlevel = this->mid_fac_down*this->river_profile_down->typ_of_profile->get_global_z_min() + this->mid_fac_up*this->river_profile_up->typ_of_profile->get_global_z_min();
		}

	}
}

//Get the mid height for the leakage-coupling (global)
double Hyd_Coupling_Point_RV2GW::get_mid_height(void) {
	return this->mid_height;
}

//Get the pointer to the downstream river profile
_Hyd_River_Profile* Hyd_Coupling_Point_RV2GW::get_downstream_rvprofile(void) {
	return this->river_profile_down;
}
//Get the pointer to the upstream river profile
_Hyd_River_Profile* Hyd_Coupling_Point_RV2GW::get_upstream_rvprofile(void) {
	return this->river_profile_up;
}
//Get the index of the downstream river profile
int Hyd_Coupling_Point_RV2GW::get_index_downstream_rvprofile(void) {
	if (this->river_profile_down == NULL) {
		return -1;
	}
	else {
		return this->river_profile_down->get_profile_number();
	}
}
//Get the index to the upstream river profile
int Hyd_Coupling_Point_RV2GW::get_index_upstream_rvprofile(void) {
	if (this->river_profile_up == NULL) {
		return -1;
	}
	else {
		return this->river_profile_up->get_profile_number();
	}
}
//Get the pointer to the groundwater element
Hyd_Element_Groundwater* Hyd_Coupling_Point_RV2GW::get_groundwater_element(void) {
	return this->groundwater_elem;
}
//Get the upwards factor for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
double Hyd_Coupling_Point_RV2GW::get_upwards_mid_factor(void) {
	return this->mid_fac_up;
}
//Get the downwards factor for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
double Hyd_Coupling_Point_RV2GW::get_downwards_mid_factor(void) {
	return this->mid_fac_down;
}
//Get the distance from the coupling point to the element mid point
double Hyd_Coupling_Point_RV2GW::get_distancecoupling2elem_mid(void) {
	double distance = -1.0;
	if (this->coupling_flag == false || this->groundwater_elem == NULL) {
		return distance;
	}
	else {
		distance = this->distance(this->groundwater_elem->get_mid_point());
	}

	return distance;
}
//Transfer the coupling characteristics of the coupled elements
void Hyd_Coupling_Point_RV2GW::transfer_coupling_characteristics(const bool left_river_flag) {


	if (this->river_profile_index_up < 0 || this->groundwater_elem_index < 0 || this->river_profile_index_down < 0) {
		this->coupling_flag = false;
		return;
	}
	if (this->groundwater_elem->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW){// || this->groundwater_elem->get_elem_type() == _hyd_gw_elem_type::LINE_ELEM_GW) {
		this->coupling_flag = true;
		//left river bank
		if (left_river_flag == true) {
			this->transfer_coupling_characteristics_leftbank();
		}
		//right river bank
		else {
			this->transfer_coupling_characteristics_rightbank();
		}
		//Daniel ok? If this is used no direct in coupling is possible
		//if(abs(this->distance_down)<=constant::meter_epsilon){
			//this->coupling_flag=false;
		//}
	}
	else {
		this->coupling_flag = false;
		return;
	}
}


//Reset the current coupling discharge of the points and the coupled element
void Hyd_Coupling_Point_RV2GW::reset_coupling_discharge(const bool left_river_flag) {
	_Hyd_Coupling_Point::reset_coupling_discharge();

	if (this->coupling_flag == true) {
		if (this->leakage_flag == true) {
			ostringstream cout;
			cout << "before resetting coup discharge: " << this->groundwater_elem->element_type->get_coupling_discharge() << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout, true);
			this->groundwater_elem->element_type->reset_coupling_discharge_rv_leakage();
			cout << "after resetting coup discharge: " << this->groundwater_elem->element_type->get_coupling_discharge() << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout, true);
			this->river_profile_down->reset_leakage_coupling_discharge();
			if (left_river_flag == true) {
				this->river_profile_up->reset_coupling_discharge_left_bank();
				this->river_profile_down->reset_coupling_discharge_left_bank();
			}
			else {
				this->river_profile_up->reset_coupling_discharge_right_bank();
				this->river_profile_down->reset_coupling_discharge_right_bank();
			}
		}
	}
}
//Syncronisation of the coupled models with the couplingspoint
void Hyd_Coupling_Point_RV2GW::syncronisation_coupled_models(const double timepoint, const double delta_t, const bool left_river_flag, const bool time_check, const int internal_counter) {

	_Hyd_Coupling_Point::syncronisation_coupled_models();
	this->delta_t = delta_t;
	this->time_check = time_check;


	if (this->mid_fac_down < 0 || this->mid_fac_up < 0) {
		this->leakage_flag = false;
	}

	if (this->coupling_flag == false) {
		return;
	}

	else {
		//calculate the current midwaterlevel in the river
		this->calculate_mid_waterlevel();


		double h_one_buff = 0.0;
		double h_two_buff = 0.0;

		this->predict_values(internal_counter);
		h_one_buff = 0.5*(this->predicted_h_one + this->calc_h_one);
		h_two_buff = 0.5*(this->predicted_h_two + this->calc_h_two);

		if (this->time_check == true) {
			this->store_grad_q_before = this->grad_q_before;
			this->store_oscilation_smoother = this->oscilation_smoother;
			this->store_number_osci_counter = this->number_osci_counter;
			this->store_no_osci_counter = this->no_osci_counter;
			this->store_old_q = this->old_q;

		}
		else {
			this->grad_q_before = this->store_grad_q_before;
			this->oscilation_smoother = this->store_oscilation_smoother;
			this->number_osci_counter = this->store_number_osci_counter;
			this->no_osci_counter = this->store_no_osci_counter;
			this->old_q = this->store_old_q;

			/*
			//left river bank
			//if (left_river_flag == true) {
				if (this->leakage_flag == true) {
					this->syncronisation_coupled_models_leakage(timepoint,  h_one_buff, h_two_buff);
					


					if (this->horizontal_backwater_flag == false && this->horizontal_backwater_flag_upstream == false) {
						//discharge to river profile segment: division to the profiles weighted with the factors 
						this->river_profile_up->add_coupling_discharge_left_bank(this->current_q*this->mid_fac_up);
						this->river_profile_down->add_coupling_discharge_left_bank(this->current_q*this->mid_fac_down);
					}
					else if (this->horizontal_backwater_flag == true) {
						//discharge to river profile segment: just to/out of the downstream profile 
						this->river_profile_down->add_coupling_discharge_left_bank(this->current_q);
					}
					else if (this->horizontal_backwater_flag_upstream == true) {
						//discharge to river profile segment: just to/out of the upstream profile 
						this->river_profile_up->add_coupling_discharge_left_bank(this->current_q);
					}
				}
			//}
			*/
			//right river bank
			/*
			else {
					
				if (this->leakage_flag == true) {
					this->syncronisation_coupled_models_leakage(timepoint, h_one_buff, h_two_buff);

					//if (this->floodplain_elem_index == 285692) {
					//	//	
					//	ostringstream cout;
					//	cout << this->current_q << " mid_w " << this->mid_waterlevel << " pre " << this->predicted_h_two << " aver  " << h_two_buff << " mid_h " << this->mid_height << endl;
					//	Sys_Common_Output::output_hyd->output_txt(&cout, true);


					//}

					//discharge to river profile segment: division to the profiles weighted with the factors 
					if (this->horizontal_backwater_flag == false && this->horizontal_backwater_flag_upstream == false) {
						this->river_profile_up->add_coupling_discharge_right_bank(this->current_q*this->mid_fac_up);
						this->river_profile_down->add_coupling_discharge_right_bank(this->current_q*this->mid_fac_down);
					}
					else if (this->horizontal_backwater_flag == true) {
						//discharge to river profile segment: just to/out of the downstream profile 
						this->river_profile_down->add_coupling_discharge_right_bank(this->current_q);
					}
					else if (this->horizontal_backwater_flag_upstream == true) {
						//discharge to river profile segment: just to/out of the upstream profile 
						this->river_profile_up->add_coupling_discharge_right_bank(this->current_q);
					}
				}
			}
			*/
			if (this->leakage_flag == true) {
				if (this->Param_RV->get_gw2rv_coupling_calculation_type() == _hyd_gw2rv_calculation_type::Darcy) {
					this->syncronisation_coupled_models_Darcy(timepoint, h_one_buff, h_two_buff);
				}
				else if (this->Param_RV->get_gw2rv_coupling_calculation_type() == _hyd_gw2rv_calculation_type::Rushton) {
					this->syncronisation_coupled_models_Rushton(timepoint, h_one_buff, h_two_buff);
				}
				


				/*
				if (this->horizontal_backwater_flag == false && this->horizontal_backwater_flag_upstream == false) {
					//discharge to river profile segment: division to the profiles weighted with the factors 
					this->river_profile_up->add_coupling_discharge_left_bank(this->current_q*this->mid_fac_up);
					this->river_profile_down->add_coupling_discharge_left_bank(this->current_q*this->mid_fac_down);
				}
				else if (this->horizontal_backwater_flag == true) {
					//discharge to river profile segment: just to/out of the downstream profile 
					this->river_profile_down->add_coupling_discharge_left_bank(this->current_q);
				}
				else if (this->horizontal_backwater_flag_upstream == true) {
					//discharge to river profile segment: just to/out of the upstream profile 
					this->river_profile_up->add_coupling_discharge_left_bank(this->current_q);
				}
				*/
			}
		}
	}
}
//Set the flag if a coupling due to a leakage is applicable
void Hyd_Coupling_Point_RV2GW::set_leakage_flag(const bool flag) {
	this->leakage_flag = flag;
}
//Set the fixed leakage flag to true; it is not more dependent of the profiles
void Hyd_Coupling_Point_RV2GW::set_fixed_leakage_flag(void) {
	this->leakage_flag_fixed = true;
}

//Get maximum waterlevel at this coupling point related to the mid base points of the profiles 
_hyd_max_values Hyd_Coupling_Point_RV2GW::get_max_h(void) {
	return this->max_h;
}

//Reset the hydrological balance values and the maximum calculated values
void Hyd_Coupling_Point_RV2GW::reset_hydro_balance_max_values(void) {
	_Hyd_Coupling_Point::reset_hydro_balance_max_values();	
	this->max_h.maximum = 0.0;
	this->max_h.time_point = 0.0;

}
//Reset the smoothing calculation members
void Hyd_Coupling_Point_RV2GW::reset_smoothing(void) {
	_Hyd_Coupling_Point::reset_smoothing();

	this->grad_q_current = 0.0;
	this->grad_q_before = 0.0;
	this->oscilation_smoother = 1.0;
	this->number_osci_counter = 2.0;
	this->no_osci_counter = 0.0;

	this->grad_q_break_current = 0.0;
	this->grad_q_break_before = 0.0;
	this->oscilation_smoother_break = 1.0;
	this->number_osci_counter_break = 2.0;
	this->no_osci_counter_break = 0.0;

	this->predicted_h_two = 0.0;
	this->corrected_h_two = 0.0;
	this->gradient_list_h_two.clear();
	for (int i = 0; i < constant::no_stored_grad; i++) {
		this->gradient_list_h_two.append(0.0);
	}
	this->gradient_h_two = 0.0;
	this->calc_h_two = 0.0;
	this->old_calc_h_two = 0.0;

	this->store_grad_q_before = 0.0;
	this->store_oscilation_smoother = 1.0;
	this->store_number_osci_counter = 2.0;
	this->store_no_osci_counter = 0.0;

}
//Get the maximum waterlevel gradient 
double Hyd_Coupling_Point_RV2GW::get_maximum_h_grad(void) {
	double buffer = 0.0;
	double buffer_max = 0.0;

	buffer = abs(this->grad_q_current / this->min_area);
	if (buffer_max < buffer) {
		buffer_max = buffer;
	}


	return buffer_max;
}
//Copy operator
Hyd_Coupling_Point_RV2GW& Hyd_Coupling_Point_RV2GW::operator=(const Hyd_Coupling_Point_RV2GW& object) {
	_Hyd_Coupling_Point::operator =(object);
	this->river_profile_index_up = object.river_profile_index_up;
	this->river_profile_up = object.river_profile_up;
	this->river_profile_index_down = object.river_profile_index_down;
	this->river_profile_down = object.river_profile_down;
	this->groundwater_index = object.groundwater_index;
	this->gw_section_id = object.gw_section_id;
	this->groundwater_elem_index = object.groundwater_elem_index;
	this->first_found_elem_index = object.first_found_elem_index;
	this->groundwater_elem = object.groundwater_elem;
	this->mid_height = object.mid_height;
	this->mid_basepoint = object.mid_basepoint;
	this->mid_basepoint_profile = object.mid_basepoint_profile;
	this->mid_fac_up = object.mid_fac_up;
	this->mid_fac_down = object.mid_fac_down;
	this->mid_waterlevel = object.mid_waterlevel;


	this->leakage_flag = object.leakage_flag;
	this->leakage_flag_fixed = object.leakage_flag_fixed;
	this->total_flow_width = object.total_flow_width;
	this->leakage_width = object.leakage_width;
	this->max_h = object.max_h;

	this->horizontal_backwater_flag = object.horizontal_backwater_flag;
	this->horizontal_backwater_flag_upstream = object.horizontal_backwater_flag_upstream;
	this->grad_q_current = object.grad_q_current;
	this->grad_q_before = object.grad_q_before;
	this->oscilation_smoother = object.oscilation_smoother;
	this->number_osci_counter = object.number_osci_counter;
	this->no_osci_counter = object.no_osci_counter;

	this->grad_q_break_current = object.grad_q_break_current;
	this->grad_q_break_before = object.grad_q_break_before;
	this->oscilation_smoother_break = object.oscilation_smoother_break;
	this->no_osci_counter_break = object.no_osci_counter_break;
	this->number_osci_counter_break = object.number_osci_counter_break;

	this->corrected_h_two = object.corrected_h_two;
	this->predicted_h_two = object.predicted_h_two;
	this->gradient_h_two = object.gradient_h_two;
	this->calc_h_two = object.calc_h_two;
	this->old_calc_h_two = object.old_calc_h_two;

	this->store_grad_q_before = object.store_grad_q_before;
	this->store_oscilation_smoother = object.store_oscilation_smoother;
	this->store_number_osci_counter = object.store_number_osci_counter;
	this->store_no_osci_counter = object.store_no_osci_counter;

	this->file_name = object.file_name;
	this->Param_RV = object.Param_RV;



	return *this;
}

void Hyd_Coupling_Point_RV2GW::output_result_members_per_timestep2csv(ofstream* file) {
	if (this->leakage_flag == true && this->coupling_flag == true)
	{
		/*ofstream strings;
		strings 
		*file <<this->output_string;*/
		//*file << W(12) << P(2) << FORMAT_FIXED_REAL << this->x_coordinate << ",";
		//*file << W(12) << P(2) << FORMAT_FIXED_REAL << this->y_coordinate << ",";
		//*file << W(12) << P(2) << FORMAT_FIXED_REAL << this->get_index_groundwater_element() << ",";
		//*file << W(12) << P(2) << FORMAT_FIXED_REAL << this->get_index_downstream_rvprofile() << ",";
		//*file << W(12) << P(2) << FORMAT_FIXED_REAL << this->groundwater_elem->element_type->get_s_value() << ",";
		//*file << W(12) << P(2) << FORMAT_FIXED_REAL << this->river_profile_up->get_actual_global_waterlevel() << ",";
		//*file << W(12) << P(2) << FORMAT_FIXED_REAL << this->mid_height << ",";
		//*file << W(12) << P(2) << FORMAT_FIXED_REAL << this->get_distance_up() << ",";
		//*file << W(12) << P(2) << FORMAT_FIXED_REAL << this->mid_fac_down * this->river_profile_down->typ_of_profile->get_wetted_perimeter() + this->mid_fac_up * this->river_profile_up->typ_of_profile->get_wetted_perimeter() << ",";
		//*file << W(12) << P(2) << FORMAT_SCIENTIFIC_REAL<< this->get_current_q() << "," << endl;
	}

}

///Set file name
void Hyd_Coupling_Point_RV2GW::set_file_name(void) {
	if (this->coupling_flag == true && this->leakage_flag == true) {
		string buffer = this->Param_RV->get_crude_filename_result_1d();
		buffer += "/";
		buffer += hyd_label::paraview;
		buffer += "/";
		buffer += "RV2GW_";
		buffer += to_string(this->get_index_downstream_rvprofile());
		buffer += "_";
		buffer += to_string(this->get_groundwater_index());
		buffer += "_";
		buffer += to_string(this->get_index_groundwater_element());

		buffer += hyd_label::csv;
		this->file_name = buffer;
		
	}
}
//______________
//private
//transfer the coupling characteristics of the coupled elements for the mid river line
void Hyd_Coupling_Point_RV2GW::transfer_coupling_characteristics_mid(void) {
	if (this->river_profile_index_up < 0 || this->groundwater_elem_index < 0 || this->river_profile_index_down < 0) {
		this->coupling_flag = false;
		return;
	}
	if (this->groundwater_elem->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW ) {//|| this->groundwater_elem->get_elem_type() == _hyd_gw_elem_type::LINE_ELEM_GW
		this->coupling_flag = true;
	}
	else {
		this->coupling_flag = false;
		return;
	}
	//check if a leakage coupling is given by the profiles
	if (this->leakage_flag_fixed == false) {
		if (this->river_profile_up->get_leakage_flag_left() == false) {
			this->leakage_flag = false;
		}
		else {
			this->leakage_flag = true;
		}
	}

	//set the fpl index, if available REVIEW
	//this->gw_section_id = this->river_profile_up->get_fpl_section_id_left();


	//this->coupling_flag=true;
	//set the factors for the mid-value calculation depending on the distances
	double total_prof_distance = 0.0;
	double distance_prof_up = 0.0;
	double distance_prof_down = 0.0;
	//calculate the distances to the profiles for a point between the coupling points => hmid
	total_prof_distance = abs(this->river_profile_down->typ_of_profile->mid_point_global_x_y.distance(&(this->river_profile_up->typ_of_profile->mid_point_global_x_y)));
	distance_prof_up = abs(this->distance(&(this->river_profile_up->typ_of_profile->mid_point_global_x_y)));
	distance_prof_down = abs(this->distance(&(this->river_profile_down->typ_of_profile->mid_point_global_x_y)));

	//set the factors via the distances
	this->mid_fac_down = (1.0 - distance_prof_down / total_prof_distance);
	this->mid_fac_up = (1.0 - distance_prof_up / total_prof_distance);



	
	//check for a weir profile
	if (this->river_profile_up->get_profile_type() == _hyd_profile_types::WEIR_TYPE) {
		//full weight to the downstream profile
		this->mid_fac_down = 1.0;
		this->mid_fac_up = 0.0;
	}
	if (this->river_profile_down->get_profile_type() == _hyd_profile_types::WEIR_TYPE) {
		//full weight to the upstream profile
		this->mid_fac_down = 0.0;
		this->mid_fac_up = 1.0;
	}
	//set the mid_height via the factors
	//this->mid_height=this->mid_fac_down*this->river_profile_down->typ_of_profile->get_first_point()->get_global_z_coordinate()+this->mid_fac_up*this->river_profile_up->typ_of_profile->get_first_point()->get_global_z_coordinate();
	this->mid_height = this->mid_fac_down * this->river_profile_down->typ_of_profile->get_global_z_min() + this->mid_fac_up * this->river_profile_up->typ_of_profile->get_global_z_min();
	//if the height of the element is above the river bank line set the element height REVIEW
	//this->mid_height = max(this->mid_height, this->groundwater_elem->get_z_value());


	//set the width
	this->total_flow_width = this->distance_down;
	this->leakage_width = this->total_flow_width;

	//set minimal area
	this->min_area = this->groundwater_elem->element_type->get_relevant_area();
}

//Transfer the coupling characteristics of the coupled elements for a left river bank (in flow direction)
void Hyd_Coupling_Point_RV2GW::transfer_coupling_characteristics_leftbank(void) {
	//check if a leakage coupling is given by the profiles
	if (this->leakage_flag_fixed == false) {
		if (this->river_profile_up->get_leakage_flag_left() == false) {
			this->leakage_flag = false;
		}
		else {
			this->leakage_flag = true;
		}
	}
	
	//set the fpl index, if available REVIEW
	//this->gw_section_id = this->river_profile_up->get_fpl_section_id_left();


	//this->coupling_flag=true;
	//set the factors for the mid-value calculation depending on the distances
	double total_prof_distance = 0.0;
	double distance_prof_up = 0.0;
	double distance_prof_down = 0.0;
	//calculate the distances to the profiles for a point between the coupling points => hmid
	total_prof_distance = abs(this->river_profile_down->typ_of_profile->get_first_point()->get_global_x_y_coordinates().distance(&(this->river_profile_up->typ_of_profile->get_first_point()->get_global_x_y_coordinates())));
	distance_prof_up = abs(this->distance(&(this->river_profile_up->typ_of_profile->get_first_point()->get_global_x_y_coordinates())));
	distance_prof_up = distance_prof_up - this->distance_down*0.5;
	distance_prof_down = abs(this->distance(&(this->river_profile_down->typ_of_profile->get_first_point()->get_global_x_y_coordinates())));

	distance_prof_down = distance_prof_down + this->distance_down*0.5;
	//set the factors via the distances
	this->mid_fac_down = (1.0 - distance_prof_down / total_prof_distance);
	this->mid_fac_up = (1.0 - distance_prof_up / total_prof_distance);

	//check for a weir profile
	if (this->river_profile_up->get_profile_type() == _hyd_profile_types::WEIR_TYPE) {
		//full weight to the downstream profile
		this->mid_fac_down = 1.0;
		this->mid_fac_up = 0.0;
	}
	if (this->river_profile_down->get_profile_type() == _hyd_profile_types::WEIR_TYPE) {
		//full weight to the upstream profile
		this->mid_fac_down = 0.0;
		this->mid_fac_up = 1.0;
	}
	//set the mid_height via the factors
	//this->mid_height=this->mid_fac_down*this->river_profile_down->typ_of_profile->get_first_point()->get_global_z_coordinate()+this->mid_fac_up*this->river_profile_up->typ_of_profile->get_first_point()->get_global_z_coordinate();
	this->mid_height = this->mid_fac_down*this->river_profile_down->get_height_left_bank_abs() + this->mid_fac_up*this->river_profile_up->get_height_left_bank_abs();
	//if the height of the element is above the river bank line set the element height REVIEW
	//this->mid_height = max(this->mid_height, this->groundwater_elem->get_z_value());


	//set the width
	this->total_flow_width = this->distance_down;
	this->leakage_width = this->total_flow_width;

	//set minimal area
	this->min_area = this->groundwater_elem->element_type->get_relevant_area();

}
//Transfer the coupling characteristics of the coupled elements for a right river bank (in flow direction)
void Hyd_Coupling_Point_RV2GW::transfer_coupling_characteristics_rightbank(void) {
	//check if a leakage coupling is given by the profiles
	if (this->leakage_flag_fixed == false) {
		if (this->river_profile_up->get_leakage_flag_right() == false) {
			this->leakage_flag = false;
		}
		else {
			this->leakage_flag = true;
		}
	}

	//this->coupling_flag=true;
	//set the factors for the mid-value calculation depending on the distances
	double total_prof_distance = 0.0;
	double distance_prof_up = 0.0;
	double distance_prof_down = 0.0;
	//calculate the distances to the profiles
	total_prof_distance = abs(this->river_profile_down->typ_of_profile->get_last_point()->get_global_x_y_coordinates().distance(&(this->river_profile_up->typ_of_profile->get_last_point()->get_global_x_y_coordinates())));
	distance_prof_up = abs(this->distance(&(this->river_profile_up->typ_of_profile->get_last_point()->get_global_x_y_coordinates())));
	distance_prof_up = distance_prof_up - 0.5*this->distance_down;
	distance_prof_down = abs(this->distance(&(this->river_profile_down->typ_of_profile->get_last_point()->get_global_x_y_coordinates())));
	
	distance_prof_down = distance_prof_down + 0.5*this->distance_down;
	//set the factors via the distances
	this->mid_fac_down = (1.0 - distance_prof_down / total_prof_distance);
	this->mid_fac_up = (1.0 - distance_prof_up / total_prof_distance);
	//set the mid_height via the factors
	//this->mid_height=this->mid_fac_down*this->river_profile_down->typ_of_profile->get_last_point()->get_global_z_coordinate()+this->mid_fac_up*this->river_profile_up->typ_of_profile->get_last_point()->get_global_z_coordinate();
	this->mid_height = this->mid_fac_down*this->river_profile_down->get_height_right_bank_abs() + this->mid_fac_up*this->river_profile_up->get_height_right_bank_abs();
	

	//check for a weir profile
	if (this->river_profile_up->get_profile_type() == _hyd_profile_types::WEIR_TYPE || this->river_profile_up->get_profile_type() == _hyd_profile_types::BRIDGE_TYPE) {
		//full weight to the downstream profile
		this->mid_fac_down = 1.0;
		this->mid_fac_up = 0.0;
	}
	if (this->river_profile_down->get_profile_type() == _hyd_profile_types::WEIR_TYPE || this->river_profile_down->get_profile_type() == _hyd_profile_types::BRIDGE_TYPE) {
		//full weight to the upstream profile
		this->mid_fac_down = 0.0;
		this->mid_fac_up = 1.0;
	}

	//set the width
	this->total_flow_width = this->distance_down;
	this->leakage_width = this->total_flow_width;

	//set minimal area
	this->min_area = this->groundwater_elem->element_type->get_relevant_area();

}
//Syncronisation of the coupled models with the couplingspoint for leakage Darcy
//mid height is wrong, need to correct
void Hyd_Coupling_Point_RV2GW::syncronisation_coupled_models_Darcy(const double timepoint, const double h_one, const double h_two) {
	
	double delta_h_river = 0.0;
	double delta_h_elem = 0.0;
	double h_elem = 0.0;
	//double h_rv_bed = 0.0;
	double q_buff = 0.0;
	//flow length for leakage is selected as the minimum of river_segment length and gw-element length
	double flow_length = 0.0;
	//flow width for leakage is selected as the minimum of wetted perimeter and gw-element width
	double flow_width = 0.0;
	//kf gets calculated from up and downstream profile
	double kf_riverbed = 0.0;
	//mean of upstream and downstream wetted perimeter
	double mid_wetted_perimeter = 0.0;
	//mean of upstream and downstream thickness
	double mid_thickness = 0.0;

	double mid_waterlevel=0.0;

	h_elem = (this->groundwater_elem->get_z_value() + h_one);
	//h_rv_bed= (this->river_profile_down->typ_of_profile->get_global_z_min() + this->river_profile_up->typ_of_profile->get_global_z_min()) / 2;
	mid_wetted_perimeter = this->mid_fac_down*this->river_profile_down->typ_of_profile->get_wetted_perimeter() + this->mid_fac_up*this->river_profile_up->typ_of_profile->get_wetted_perimeter();
	kf_riverbed = 2 * (this->river_profile_down->get_conductivity_value()*this->river_profile_up->get_conductivity_value()) / (this->river_profile_down->get_conductivity_value() + this->river_profile_up->get_conductivity_value());
	//considering the up distance as the flow_length
	flow_length = this->get_distance_up();
	
	////this could be improved by getting the actual contact length between groundwater and river. However a corresponding geometrical method might be very complicated 
	//flow_length = min(this->groundwater_elem->element_type->get_width_x(), this->groundwater_elem->element_type->get_width_y());
	//flow_length = min(flow_length, this->river_profile_down->get_distance2upstream());

	//again this could be further refined. However this should always be fairly accurate
	flow_width = min(this->groundwater_elem->element_type->get_width_x(), this->groundwater_elem->element_type->get_width_y());
	flow_width = min(flow_width, mid_wetted_perimeter);

	mid_thickness = this->mid_fac_down*this->river_profile_down->get_thickness() + this->mid_fac_up*this->river_profile_up->get_thickness();
	//mid_waterlevel = (river_profile_down->get_actual_global_waterlevel()+this->river_profile_up->get_actual_global_waterlevel())/2;

	delta_h_river = h_two - this->mid_height;
	//if(delta_h_river>0.0){
	//	cout <<"Test"<<endl;
	//}
	//delta_h_elem = (this->groundwater_elem->get_z_value() + h_one) - mid_waterlevel;
	delta_h_elem = (this->groundwater_elem->get_z_value() + h_one) - h_two;
	this->groundwater_h = (this->groundwater_elem->get_z_value() + h_one);
	this->river_h = h_two;
	///final used riverbed height
	this->riverbed_h=this->mid_height;
	///final used flow length
	this->flow_length_m=flow_length;
	///final used wetted perimeter
	this->wetted_p=mid_wetted_perimeter;

	//no width
	/*
	if (abs(this->leakage_width) < constant::flow_epsilon) {
		this->coupling_v = 0.0;
		q_buff = 0.0;
	}
	*/
	//same waterlevels=> no flow
	if (abs(delta_h_elem) < constant::dry_hyd_epsilon) {
		this->coupling_v = 0.0;
		q_buff = 0.0;
	}
	
	//both waterlevel are under the weirsill=> no flow
	else if (delta_h_river < constant::dry_hyd_epsilon && h_one < constant::dry_hyd_epsilon) {
		this->coupling_v = 0.0;
		q_buff = 0.0;
	}
	//flow out of the river model into the groundwater


	//linear relationship
	else {
		if (h_elem >= this->mid_height) {
			q_buff = kf_riverbed * flow_length * mid_wetted_perimeter / mid_thickness * delta_h_elem;
		}
		else if (h_elem < this->mid_height) {
			q_buff = kf_riverbed * flow_length * mid_wetted_perimeter / mid_thickness * (h_two -this->mid_height)*(-1);
		}
		
		//q_buff = 0.001;
	}
	 
	

	//none linear relationship
	//else {
	//	double k1 = 0.18693;
	//	double k2 = 9.76534;
	//	double k3 = 0.5164;
	//	if (delta_h_elem >= 0) {
	//		q_buff = (k2 * (1 - exp(-k3 * delta_h_elem))) * flow_length / 86400;//unit in m³/d ->m³/s
	//	}
	//	else {
	//		q_buff = k1 * (exp(k3 * delta_h_elem) - 1) * flow_length / 86400;
	//	}
	//}


	//ostringstream cout;
	//cout << "Q_buff: " << q_buff << " ,delta_h: " << delta_h_elem<< endl;
	//Sys_Common_Output::output_hyd->output_txt(&cout, true);
	
	//set a boundary to the discharge
	if (abs(q_buff) <= this->discharge_boundary) {
		q_buff = 0.0;
		this->coupling_v = 0.0;
	}
	
	ostringstream cout;
	cout << "gw_element: " << this->groundwater_elem_index << " ,river_down: " << this->river_profile_down->get_profile_number() << " ,river_level: " << h_two << " ,gw_level: " << this->groundwater_elem->get_z_value() + h_one << " ,Q_buff: " << q_buff << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);

	//smooth it for numerical reasons
	q_buff = this->smooth_coupling_discharge(q_buff, &this->old_q);
	this->current_q = q_buff;
	cout << "smoothed: " << this->current_q << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	this->calculate_maximum_values(timepoint, this->current_q, &this->max_coupling_v);
	this->calculate_hydrological_balance(this->current_q, &this->coupling_volume);

	

	//discharge to element
	this->groundwater_elem->element_type->add_coupling_discharge_rv_leakage((-1.0) * this->current_q / (this->groundwater_elem->element_type->get_area()*this->groundwater_elem->element_type->get_flow_data().p_value));
	this->river_profile_down->add_leakage_coupling_discharge(this->current_q);
	

	////for TEST
	//ostringstream info;
	////cout << "Id_RV_ds_Profile" << W(12) << "Id_GW_Element" << endl;
	//info << this->get_number() << W(12);
	//info << this->get_downstream_rvprofile()->get_profile_name() << W(12);
	//info << this->get_index_groundwater_element()<<W(12);
	//info << this->mid_height<<W(12);
	//info << h_two << W(12);
	//info << this->groundwater_elem->get_z_value() + h_one << W(12);
	//info << this->current_q << W(12);
	//info << kf_riverbed << W(12);
	//info << flow_length << W(12);
	//info << mid_wetted_perimeter << W(12);
	//info << mid_thickness;

	//this->output_string << info.str()<<endl;

	//Sys_Common_Output::output_hyd->output_txt(&info);
	//if (this->Param_RV.get_filename_result2file_1d(hyd_label::paraview) != label::not_set) {
	//int count = 0;
	//	
	if (this->Param_RV->get_output_couplings() == true) {
		ofstream output_csv;
		//
		//open the file
		//open the file
		string buffer = this->Param_RV->get_crude_filename_result_1d();
		buffer += "/";
		buffer += hyd_label::paraview;
		buffer += "/";
		
		buffer += "RV2GW_";
		buffer += to_string(this->get_index_upstream_rvprofile());
		buffer += "_";
		buffer += to_string(this->get_index_downstream_rvprofile());
		buffer += "_";
		buffer += to_string(this->get_groundwater_index());
		buffer += "_";
		buffer += to_string(this->get_index_groundwater_element());

		buffer += hyd_label::csv;
		//this->file_name = buffer;
		//this->points[i].point_index
		output_csv.open(buffer, ios::app);

		output_csv << timepoint << ",";
		output_csv << W(10) << this->groundwater_elem->get_z_value() + h_one << "," << W(10) << h_two << ",";
		output_csv << W(10) << this->mid_height << ",";
		output_csv << W(10) << flow_length << ",";
		output_csv << W(10) << mid_wetted_perimeter << ",";
		output_csv << W(10) << P(5) << FORMAT_SCIENTIFIC_REAL << this->current_q;
		output_csv << endl;

		output_csv.flush();
		output_csv.close();
	}
	
	//check if it is open
	/*if (output_csv.is_open() == false) {
		Error msg = this->set_error(6);
		ostringstream info;
		info << "File name " << this->Param_RV.get_filename_rv2gwcoupling_file_1d(hyd_label::paraview) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}*/
	//


	//	
	//output_csv.close();
		
		


	//}
}
//Syncronisation of the coupled models with the couplingspoint for leakage Rushton
//mid height is wrong, need to correct
void Hyd_Coupling_Point_RV2GW::syncronisation_coupled_models_Rushton(const double timepoint, const double h_one, const double h_two) {

	double delta_h_river = 0.0;
	double delta_h_elem = 0.0;
	double h_elem = 0.0;
	//double h_rv_bed = 0.0;
	double q_buff = 0.0;
	//flow length for leakage is selected as the minimum of river_segment length and gw-element length
	double flow_length = 0.0;
	//flow width for leakage is selected as the minimum of wetted perimeter and gw-element width
	double flow_width = 0.0;
	//kf gets calculated from up and downstream profile
	double kf_riverbed = 0.0;
	//mean of upstream and downstream wetted perimeter
	double mid_wetted_perimeter = 0.0;
	//mean of upstream and downstream thickness
	double mid_thickness = 0.0;
	double mid_waterlevel = 0.0;

	double k1 = this->river_profile_down->get_leakage_parameter()->K1_value;
	double k2 = this->river_profile_down->get_leakage_parameter()->K2_value;
	double k3 = this->river_profile_down->get_leakage_parameter()->K3_value;


	h_elem = (this->groundwater_elem->get_z_value() + h_one);
	//h_rv_bed= (this->river_profile_down->typ_of_profile->get_global_z_min() + this->river_profile_up->typ_of_profile->get_global_z_min()) / 2;
	mid_wetted_perimeter = this->mid_fac_down * this->river_profile_down->typ_of_profile->get_wetted_perimeter() + this->mid_fac_up * this->river_profile_up->typ_of_profile->get_wetted_perimeter();
	kf_riverbed = 2 * (this->river_profile_down->get_conductivity_value() * this->river_profile_up->get_conductivity_value()) / (this->river_profile_down->get_conductivity_value() + this->river_profile_up->get_conductivity_value());
	
	//considering the up distance as the flow_length
	flow_length = this->get_distance_up();

	////this could be improved by getting the actual contact length between groundwater and river. However a corresponding geometrical method might be very complicated 
	//flow_length = min(this->groundwater_elem->element_type->get_width_x(), this->groundwater_elem->element_type->get_width_y());
	//flow_length = min(flow_length, this->river_profile_down->get_distance2upstream());

	//again this could be further refined. However this should always be fairly accurate
	flow_width = min(this->groundwater_elem->element_type->get_width_x(), this->groundwater_elem->element_type->get_width_y());
	flow_width = min(flow_width, mid_wetted_perimeter);

	mid_thickness = this->mid_fac_down * this->river_profile_down->get_thickness() + this->mid_fac_up * this->river_profile_up->get_thickness();
	//mid_waterlevel = (river_profile_down->get_actual_global_waterlevel()+this->river_profile_up->get_actual_global_waterlevel())/2;

	delta_h_river = h_two - this->mid_height;
	//if(delta_h_river>0.0){
	//	cout <<"Test"<<endl;
	//}
	//delta_h_elem = (this->groundwater_elem->get_z_value() + h_one) - mid_waterlevel;
	delta_h_elem = (this->groundwater_elem->get_z_value() + h_one) - h_two;
	this->groundwater_h = (this->groundwater_elem->get_z_value() + h_one);
	this->river_h = h_two;
	///final used riverbed height
	this->riverbed_h = this->mid_height;
	///final used flow length
	this->flow_length_m = flow_length;
	///final used wetted perimeter
	this->wetted_p = mid_wetted_perimeter;

	//no width
	/*
	if (abs(this->leakage_width) < constant::flow_epsilon) {
		this->coupling_v = 0.0;
		q_buff = 0.0;
	}
	*/
	//same waterlevels=> no flow
	if (abs(delta_h_elem) < constant::dry_hyd_epsilon) {
		this->coupling_v = 0.0;
		q_buff = 0.0;
	}

	//both waterlevel are under the weirsill=> no flow
	else if (delta_h_river < constant::dry_hyd_epsilon && h_one < constant::dry_hyd_epsilon) {
		this->coupling_v = 0.0;
		q_buff = 0.0;
	}
	//flow out of the river model into the groundwater


	//linear relationship
	//else {
	//	if (h_elem >= this->mid_height) {
	//		q_buff = kf_riverbed * flow_length * mid_wetted_perimeter / mid_thickness * delta_h_elem;
	//	}
	//	else if (h_elem < this->mid_height) {
	//		q_buff = kf_riverbed * flow_length * mid_wetted_perimeter / mid_thickness * (h_two -this->mid_height)*(-1);
	//	}
	//	
	//	//q_buff = 0.001;
	//}



	//none linear relationship
	else {
		if (delta_h_elem >= 0) {
			q_buff = (k2 * (1 - exp(-k3 * delta_h_elem))) * flow_length / 86400;//unit in m³/d ->m³/s
		}
		else {
			q_buff = k1 * (exp(k3 * delta_h_elem) - 1) * flow_length / 86400;
		}
	}


	//ostringstream cout;
	//cout << "Q_buff: " << q_buff << " ,delta_h: " << delta_h_elem<< endl;
	//Sys_Common_Output::output_hyd->output_txt(&cout, true);

	//set a boundary to the discharge
	if (abs(q_buff) <= this->discharge_boundary) {
		q_buff = 0.0;
		this->coupling_v = 0.0;
	}

	ostringstream cout;
	cout << "gw_element: " << this->groundwater_elem_index << " ,river_down: " << this->river_profile_down->get_profile_number() << " ,river_level: " << h_two << " ,gw_level: " << this->groundwater_elem->get_z_value() + h_one << " ,Q_buff: " << q_buff << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);

	//smooth it for numerical reasons
	q_buff = this->smooth_coupling_discharge(q_buff, &this->old_q);
	this->current_q = q_buff;
	cout << "smoothed: " << this->current_q << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	this->calculate_maximum_values(timepoint, this->current_q, &this->max_coupling_v);
	this->calculate_hydrological_balance(this->current_q, &this->coupling_volume);



	//discharge to element
	this->groundwater_elem->element_type->add_coupling_discharge_rv_leakage((-1.0) * this->current_q / (this->groundwater_elem->element_type->get_area() * this->groundwater_elem->element_type->get_flow_data().p_value));
	this->river_profile_down->add_leakage_coupling_discharge(this->current_q);


	////for TEST
	//ostringstream info;
	////cout << "Id_RV_ds_Profile" << W(12) << "Id_GW_Element" << endl;
	//info << this->get_number() << W(12);
	//info << this->get_downstream_rvprofile()->get_profile_name() << W(12);
	//info << this->get_index_groundwater_element()<<W(12);
	//info << this->mid_height<<W(12);
	//info << h_two << W(12);
	//info << this->groundwater_elem->get_z_value() + h_one << W(12);
	//info << this->current_q << W(12);
	//info << kf_riverbed << W(12);
	//info << flow_length << W(12);
	//info << mid_wetted_perimeter << W(12);
	//info << mid_thickness;

	//this->output_string << info.str()<<endl;

	//Sys_Common_Output::output_hyd->output_txt(&info);
	//if (this->Param_RV.get_filename_result2file_1d(hyd_label::paraview) != label::not_set) {
	//int count = 0;
	//	
	if (this->Param_RV->get_output_couplings() == true) {
		ofstream output_csv;
		//
		//open the file
		//open the file
		string buffer = this->Param_RV->get_crude_filename_result_1d();
		buffer += "/";
		buffer += hyd_label::paraview;
		buffer += "/";
		
		buffer += "RV2GW_";
		buffer += to_string(this->get_index_upstream_rvprofile());
		buffer += "_";
		buffer += to_string(this->get_index_downstream_rvprofile());
		buffer += "_";
		buffer += to_string(this->get_groundwater_index());
		buffer += "_";
		buffer += to_string(this->get_index_groundwater_element());

		buffer += hyd_label::csv;
		//this->file_name = buffer;
		//this->points[i].point_index
		output_csv.open(buffer, ios::app);

		output_csv << timepoint << ",";
		output_csv << W(10) << this->groundwater_elem->get_z_value() + h_one << "," << W(10) << h_two << ",";
		output_csv << W(10) << this->mid_height << ",";
		output_csv << W(10) << flow_length << ",";
		output_csv << W(10) << mid_wetted_perimeter << ",";
		output_csv << W(10) << P(5) << FORMAT_SCIENTIFIC_REAL << this->current_q;
		output_csv << endl;

		output_csv.flush();
		output_csv.close();
	}
	//check if it is open
	/*if (output_csv.is_open() == false) {
		Error msg = this->set_error(6);
		ostringstream info;
		info << "File name " << this->Param_RV.get_filename_rv2gwcoupling_file_1d(hyd_label::paraview) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}*/
	//


	//	
	//output_csv.close();




	//}
}
//Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
double Hyd_Coupling_Point_RV2GW::smooth_coupling_discharge(const double q_current, double *old_q) {
	double smooth_q = 0.0;

	if (this->delta_t > 0.0) {
		this->grad_q_current = (q_current - *old_q) / (this->delta_t);
		if ((this->grad_q_before >= 0.0 && this->grad_q_current < 0.0) || (this->grad_q_before <= 0.0 && this->grad_q_current > 0.0)) {
			this->oscilation_smoother = this->oscilation_smoother + this->number_osci_counter;
			//this->oscilation_smoother=this->oscilation_smoother+1.0;
			this->no_osci_counter = 0.0;
			this->number_osci_counter = 2.0;
			if (this->oscilation_smoother >= 100.0) {
				this->oscilation_smoother = 100.0;
			}

		}
		else {
			this->oscilation_smoother = this->oscilation_smoother - pow(this->no_osci_counter, 0.75);
			//this->oscilation_smoother=this->oscilation_smoother-pow(this->no_osci_counter,1.5);
			this->no_osci_counter++;
			this->number_osci_counter = 2.0;
			if (this->oscilation_smoother < 1.0) {
				this->oscilation_smoother = 1.0;
			}

		}
		this->grad_q_before = this->grad_q_current;
	}


	smooth_q = q_current * (1.0 / this->oscilation_smoother) + (*old_q)*(1.0 - (1.0 / this->oscilation_smoother));
	if (this->delta_t > 0.0) {
		this->grad_q_current = (smooth_q - *old_q) / (this->delta_t);
		this->grad_q_before = this->grad_q_current;
	}


	*old_q = smooth_q;
	return smooth_q;
}

//Predict the values
void Hyd_Coupling_Point_RV2GW::predict_values(const int int_counter) {
	if (this->delta_t > 0.0) {
		//one
		this->calc_h_one = this->groundwater_elem->element_type->get_h_value();

		this->gradient_h_one = (this->calc_h_one - this->old_calc_h_one) / this->delta_t;
		if (this->time_check == false) {
			this->old_calc_h_one = this->calc_h_one;
			this->replace_first_grad_list(&this->gradient_list_h_one, this->gradient_h_one);

		}
		else {
			this->swap_grad_list(&this->gradient_list_h_one, this->gradient_h_one);
		}

		//this->predicted_h_one=this->calc_h_one+(1.0/12.0)*(23.0*this->gradient_list_h_one.at(0)-16.0*this->gradient_list_h_one.at(1)+5.0*this->gradient_list_h_one.at(2))*this->delta_t;
		//this->predicted_h_one=this->calc_h_one+(0.5)*(3.0*this->gradient_list_h_one.at(0)-1.0*this->gradient_list_h_one.at(1))*this->delta_t;
		//this->predicted_h_one=this->calc_h_one+(this->gradient_list_h_one.at(0))*this->delta_t;

		//two
		this->calc_h_two = this->mid_waterlevel;
		//this->calc_h_two = this->mid_fac_down * this->river_profile_down->typ_of_profile->get_actual_global_waterlevel() + this->mid_fac_up * this->river_profile_up->typ_of_profile->get_actual_global_waterlevel();


		this->gradient_h_two = (this->calc_h_two - this->old_calc_h_two) / this->delta_t;
		if (this->time_check == false) {
			this->old_calc_h_two = this->calc_h_two;
			this->replace_first_grad_list(&this->gradient_list_h_two, this->gradient_h_two);
		}
		else {
			this->swap_grad_list(&this->gradient_list_h_two, this->gradient_h_two);

		}
		if (int_counter > 3) {
			//this->predicted_h_two=this->calc_h_two+(1.0/12.0)*(23.0*this->gradient_list_h_two.at(0)-16.0*this->gradient_list_h_two.at(1)+5.0*this->gradient_list_h_two.at(2))*this->delta_t;
			//this->predicted_h_two=this->calc_h_two+(0.5)*(3.0*this->gradient_list_h_two.at(0)-1.0*this->gradient_list_h_two.at(1))*this->delta_t;
			//this->predicted_h_two=this->calc_h_two+(this->gradient_list_h_two.at(0))*this->delta_t;
			//this->predicted_h_one=this->calc_h_one+(this->gradient_list_h_one.at(0))*this->delta_t;
			//changed to explicit!!!
			this->predicted_h_two = this->calc_h_two;
			this->predicted_h_one = this->calc_h_one;
		}
		else {
			this->predicted_h_two = this->calc_h_two;
			this->predicted_h_one = this->calc_h_one;
		}

	}
}