#include "source_code\Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_Point_GW2GW.h"

//constructor
Hyd_Coupling_Point_GW2GW::Hyd_Coupling_Point_GW2GW(void) {
	this->gw1_elem_index = -1;
	this->gw1_elem_is_setted = false;
	this->gw1_elem = NULL;
	this->gw2_elem_index = -1;
	this->gw2_elem_is_setted = false;
	this->gw2_elem = NULL;
	this->x_width_flag = true;
	this->flow_distance = 0.0;
	this->max_z = 0.0;
	this->mid_T = 0.0;
	this->c_flow = 0.0;
	this->grad_q_current = 0.0;
	this->grad_q_before = 0.0;
	this->oscilation_smoother = 1.0;
	this->no_osci_counter = 0.0;
	this->number_osci_counter = 1.0;
	this->store_grad_q_before = 0.0;
	this->store_oscilation_smoother = 1.0;
	this->store_no_osci_counter = 0.0;
	this->store_number_osci_counter = 1.0;
	//this->q_list.relevant=false;

	this->predicted_h_two = 0.0;
	this->corrected_h_two = 0.0;
	this->gradient_list_h_two.clear();
	for (int i = 0; i < constant::no_stored_grad; i++) {
		this->gradient_list_h_two.append(0.0);
	}
	this->gradient_h_two = 0.0;
	this->calc_h_two = 0.0;
	this->old_calc_h_two = 0.0;


	this->grad_delta_h = 0.0;
	this->store_grad_delta_h = 0.0;
	this->old_delta_h = 0.0;
	this->store_old_delta_h = 0.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Point_GW2GW), _sys_system_modules::HYD_SYS);
}
//Copy constructor
Hyd_Coupling_Point_GW2GW::Hyd_Coupling_Point_GW2GW(const Hyd_Coupling_Point_GW2GW& object) {
	_Hyd_Coupling_Point::operator =(object);
	this->gw1_elem_index = object.gw1_elem_index;
	this->gw1_elem = object.gw1_elem;
	this->gw2_elem_index = object.gw2_elem_index;
	this->gw2_elem = object.gw2_elem;
	this->x_width_flag = object.x_width_flag;
	this->gw2_elem_is_setted = object.gw2_elem_is_setted;
	this->gw1_elem_is_setted = object.gw1_elem_is_setted;
	this->flow_distance = object.flow_distance;
	this->max_z = object.max_z;
	this->mid_T = object.mid_T;
	this->c_flow = object.c_flow;
	this->grad_q_current = object.grad_q_current;
	this->grad_q_before = object.grad_q_before;
	this->oscilation_smoother = object.oscilation_smoother;
	this->no_osci_counter = object.no_osci_counter;
	this->number_osci_counter = object.number_osci_counter;
	this->store_grad_q_before = object.store_grad_q_before;
	this->store_oscilation_smoother = object.store_oscilation_smoother;
	this->store_no_osci_counter = object.store_no_osci_counter;
	this->store_number_osci_counter = object.store_number_osci_counter;

	//this->q_list.relevant=object.q_list.relevant;

	this->corrected_h_two = object.corrected_h_two;
	this->predicted_h_two = object.predicted_h_two;
	this->gradient_h_two = object.gradient_h_two;
	this->calc_h_two = object.calc_h_two;
	this->old_calc_h_two = object.old_calc_h_two;
	this->gradient_list_h_two.clear();
	for (int i = 0; i < constant::no_stored_grad; i++) {
		this->gradient_list_h_two.append(0.0);
	}

	this->grad_delta_h = object.grad_delta_h;
	this->store_grad_delta_h = object.store_grad_delta_h;
	this->old_delta_h = object.old_delta_h;
	this->store_old_delta_h = object.store_old_delta_h;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Point_GW2GW), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_Point_GW2GW::~Hyd_Coupling_Point_GW2GW(void) {
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_Point_GW2GW), _sys_system_modules::HYD_SYS);
}
//_________
//public
//Output the header for the setted member (static)
void Hyd_Coupling_Point_GW2GW::output_header_setted_member(ostringstream *cout) {
	*cout << W(10) << "I_gw_small" << W(10) << "I_gw_big" << W(15) << "Coupling " << W(15) << "Dist_up " << label::m << W(15) << "Dist_down" << label::m;
	*cout << W(18) << "Dir_x-flow" << W(18) << "Flow-distance" << label::m;
	*cout << W(15) << "x" << label::m << W(17) << "y" << label::m;
	*cout << endl;
	Sys_Common_Output::output_hyd->output_txt(cout, true);
}
//Output the setted members
void Hyd_Coupling_Point_GW2GW::output_setted_members(ostringstream *cout) {
	*cout << W(10) << this->gw1_elem_index << W(10) << this->gw2_elem_index;
	*cout << W(15) << functions::convert_boolean2string(this->coupling_flag);
	*cout << W(15) << P(2) << FORMAT_FIXED_REAL << this->distance_up;
	*cout << W(17) << P(2) << FORMAT_FIXED_REAL << this->distance_down;
	*cout << W(19) << functions::convert_boolean2string(this->x_width_flag);
	*cout << W(17) << P(2) << FORMAT_FIXED_REAL << this->flow_distance;
	*cout << W(21) << P(2) << FORMAT_FIXED_REAL << this->x_coordinate;
	*cout << W(21) << P(2) << FORMAT_FIXED_REAL << this->y_coordinate;
	*cout << endl;
	Sys_Common_Output::output_hyd->output_txt(cout, true);
}
//Set the index of the elements for the first groundwater model
void Hyd_Coupling_Point_GW2GW::set_first_gwelem_index(const int first_model_elem) {
	this->gw1_elem_index = first_model_elem;
	this->gw1_elem_is_setted = true;
}
//Set the index of the elements for the second groundwater model
void Hyd_Coupling_Point_GW2GW::set_second_gwelem_index(const int second_model_elem) {
	this->gw2_elem_index = second_model_elem;
	this->gw2_elem_is_setted = true;
}
//Set the pointer to the elements of the first groundwater model
void Hyd_Coupling_Point_GW2GW::set_first_gwelem_pointer(Hyd_Element_Groundwater *gw1_elem) {
	this->gw1_elem = gw1_elem;
	if (this->gw1_elem != NULL && this->gw1_elem_index >= 0) {
		this->gw1_elem->element_type->set_coupling_data();
		this->gw1_elem->element_type->set_gw_coupled_flag(true);
	}
}
//Set the pointer to the elements of the second groundwater model
void Hyd_Coupling_Point_GW2GW::set_second_gwelem_pointer(Hyd_Element_Groundwater *gw2_elem) {
	this->gw2_elem = gw2_elem;
	if (this->gw2_elem != NULL && this->gw2_elem_index >= 0) {
		this->gw2_elem->element_type->set_coupling_data();
		this->gw2_elem->element_type->set_gw_coupled_flag(true);
	}
}
//Set the direction of coupling
void Hyd_Coupling_Point_GW2GW::set_direction_coupling(const bool x_direction) {
	this->x_width_flag = x_direction;
}
//Set the distance for the Darcy-flow calculation from the first groundwater (smaller size of elements)
//Same method as for floodplain. Could be changed to calculate from both groundwater characteristics
void Hyd_Coupling_Point_GW2GW::set_flow_distance(Hyd_Model_Groundwater *gw1) {
	//x-direction
	if (this->x_width_flag == true) {
		this->flow_distance = gw1->Param_GW.get_geometrical_info().width_x;
		this->flow_width = gw1->Param_GW.get_geometrical_info().width_y;
	}
	//y-direction
	else {
		this->flow_distance = gw1->Param_GW.get_geometrical_info().width_y;
		this->flow_width = gw1->Param_GW.get_geometrical_info().width_x;
	}

}
//Transfer the coupling characteristics of the coupled elements
void Hyd_Coupling_Point_GW2GW::transfer_coupling_characteristics(void) {
	//check if the characteristics are already tranfered	



	if (this->gw1_elem_index < 0 || this->gw2_elem_index < 0) {
		this->coupling_flag = false;
		return;
	}
	if ((this->gw1_elem->get_elem_type() == _hyd_elem_type::STANDARD_ELEM) &&
		(this->gw2_elem->get_elem_type() == _hyd_elem_type::STANDARD_ELEM)) {
		if (this->gw1_elem->element_type->get_boundary_data().h_value == (-1 ||-2) || this->gw2_elem->element_type->get_boundary_data().h_value == (-1 || -2)) {
			this->coupling_flag = false;
			return;
		}
		/*if (this->gw1_elem->element_type->get_flow_data().no_flow_y_flag == false || this->gw2_elem->element_type->get_flow_data().no_flow_y_flag == false) {
			this->coupling_flag = false;
			return;
		}
		if (this->gw1_elem->element_type->get_flow_data().no_flow_x_flag == false || this->gw2_elem->element_type->get_flow_data().no_flow_x_flag == false) {
			this->coupling_flag = false;
			return;
		}*/

		this->flow_width = this->get_distance_down();
		this->flow_distance = this->gw2_elem->get_mid_point()->distance(this->gw1_elem->get_mid_point());


		this->coupling_flag = true;
		//mid darcy-value; REVIEW
		this->mid_T = 2 * ((this->gw1_elem->get_flow_data().kf_value * min((this->gw1_elem->element_type->get_h_value()), this->gw1_elem->get_flow_data().thickness)) * (this->gw2_elem->get_flow_data().kf_value * min((this->gw2_elem->element_type->get_h_value()), this->gw2_elem->get_flow_data().thickness)));
		this->mid_T = this->mid_T / (this->gw1_elem->get_flow_data().kf_value * min(this->gw1_elem->element_type->get_h_value(), this->gw1_elem->get_flow_data().thickness) + this->gw2_elem->get_flow_data().kf_value * min(this->gw2_elem->element_type->get_h_value(), this->gw2_elem->get_flow_data().thickness));
		//max geodetic height
		this->max_z = max(this->gw1_elem->get_z_value(), this->gw2_elem->get_z_value()) /*+ constant::sill_heightcoupling_gw*/;
		//calculate c_flow REVIEW
		//T is currently not updated between calculations
		this->c_flow = this->mid_T * this->flow_width / (this->gw1_elem->get_flow_data().p_value * this->gw1_elem->element_type->get_area()) / this->flow_distance;
		//ostringstream cout;
		//cout << "c_flow: " << this->c_flow << " ;flow_distance: " << this->flow_distance<< " ;flow_width: " << this->flow_width << endl;
		//Sys_Common_Output::output_hyd->output_txt(&cout);
		//set minimal area
		this->min_area = min(this->gw1_elem->element_type->get_relevant_area(), this->gw2_elem->element_type->get_relevant_area());
	}
	else {
		this->coupling_flag = false;
		return;
	}
}
//Reset the current coupling discharge of the points and the coupled element
void Hyd_Coupling_Point_GW2GW::reset_coupling_discharge(void) {
	_Hyd_Coupling_Point::reset_coupling_discharge();
	if (this->coupling_flag == true) {
		this->gw1_elem->element_type->reset_coupling_discharge_gw();
		this->gw2_elem->element_type->reset_coupling_discharge_gw();
	}
}
//Syncronisation of the coupled models with the couplingspoint
void Hyd_Coupling_Point_GW2GW::syncronisation_coupled_models(const double timepoint, const double delta_t, const bool time_check, const int internal_counter) {

	_Hyd_Coupling_Point::syncronisation_coupled_models();
	this->delta_t = delta_t;
	this->time_check = time_check;

	if (this->coupling_flag == false) {
		return;
	}
	else {
		double h_one_buff = 0.0;
		double h_two_buff = 0.0;

		this->predict_values(internal_counter);
		h_one_buff = 0.5*(this->predicted_h_one + this->calc_h_one);
		h_two_buff = 0.5*(this->predicted_h_two + this->calc_h_two);
		/*if(h_one_buff>0.0 || h_two_buff>0.0){
			double test;
			test=0.0;
		}*/


		if (this->time_check == true) {
			this->store_grad_q_before = this->grad_q_before;
			this->store_oscilation_smoother = this->oscilation_smoother;
			this->store_no_osci_counter = this->no_osci_counter;
			this->store_old_q = this->old_q;
			this->store_number_osci_counter = this->number_osci_counter;
			this->store_grad_delta_h = this->grad_delta_h;
			this->store_old_delta_h = this->old_delta_h;
		}
		else {
			this->grad_q_before = this->store_grad_q_before;
			this->oscilation_smoother = this->store_oscilation_smoother;
			this->no_osci_counter = this->store_no_osci_counter;
			this->old_q = this->store_old_q;
			this->number_osci_counter = this->store_number_osci_counter;
			this->grad_delta_h = this->store_grad_delta_h;
			this->old_delta_h = this->store_old_delta_h;
		}


		double flow_depth_neigh = 0.0;
		double flow_depth = 0.0;
		double delta_h = 0.0;
		double q_buff = 0.0;

		//calculate the mid of the flow depth
		flow_depth = (this->gw1_elem->get_z_value() + h_one_buff) - this->max_z;
		if (flow_depth < 0.0) {
			flow_depth = 0.0;
		}
		flow_depth_neigh = (this->gw2_elem->get_z_value() + h_two_buff) - this->max_z;
		if (flow_depth_neigh < 0.0) {
			flow_depth_neigh = 0.0;
		}
		//flow_depth=flow_depth*0.5+flow_depth_neigh*0.5;
		flow_depth = max(flow_depth, flow_depth_neigh);

		//set a limit to the calculation
		if (abs(flow_depth) <= constant::dry_hyd_epsilon) {
			q_buff = 0.0;
			this->smooth_coupling_discharge(0.0, &this->old_q);
		}
		else {

			delta_h = (this->gw1_elem->get_z_value() + h_one_buff) - (this->gw2_elem->get_z_value() + h_two_buff);

			double abs_delta_h = abs(delta_h);
			//no slope
			if (abs_delta_h <= constant::flow_epsilon) {
				this->coupling_v = 0.0;
				q_buff = 0.0;
			}
			else {
				q_buff = this->c_flow * delta_h;
				/*
				ostringstream cout;
				cout << "delta_h: " << delta_h << " ;q_buff: " << q_buff <<endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				*/
			}
		}


		//set a boundary to the coupling discharge
		if (abs(q_buff) <= this->discharge_boundary) {
			q_buff = 0.0;
		}

		//just for testing
		//if(q_buff!=0.0){
		//	this->q_list.relevant=true;
		//}
		//if(this->q_list.relevant==true){
		//	this->q_list.timepoint.append(timepoint);
		//	this->q_list.current_discharge.append(q_buff);
		//}

		//smooth it for numerical reasons
		this->current_q = this->smooth_coupling_discharge(q_buff, &this->old_q);

		if (this->delta_t > 0.0) {
			if (abs(this->old_delta_h) > constant::flow_epsilon && this->oscilation_smoother <= 3.0001) {
				this->grad_delta_h = delta_h / (this->delta_t*this->old_delta_h) - 1.0 / this->delta_t;

			}
			else {
				this->grad_delta_h = 0.0;
			}
			this->old_delta_h = delta_h;
		}

		//if(this->q_list.relevant==true){
		//	this->q_list.smooth_discharge.append(this->current_q);
		//}
		/*if (this->gw1_elem->element_type->get_flow_data().no_flow_y_flag == false || this->gw2_elem->element_type->get_flow_data().no_flow_y_flag == false) {
			this->coupling_flag = false;
			return;
		}
		if (this->gw1_elem->element_type->get_flow_data().no_flow_x_flag == false || this->gw2_elem->element_type->get_flow_data().no_flow_x_flag == false) {
			this->coupling_flag = false;
			return;
		}*/

		//REVIEW
		this->calculate_maximum_values(timepoint, this->current_q, &this->max_coupling_v);
		this->calculate_hydrological_balance(this->current_q, &this->coupling_volume);
		//add the coupling_q to the elements
		this->gw1_elem->element_type->add_coupling_discharge_gw(-1.0*this->current_q);
		/*
		ostringstream cout;
		cout << this->current_q << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		cout << -1.0*this->current_q << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		*/
		this->gw2_elem->element_type->add_coupling_discharge_gw(this->current_q);

	}
}
//Reset the smoothing calculation members
void Hyd_Coupling_Point_GW2GW::reset_smoothing(void) {
	_Hyd_Coupling_Point::reset_smoothing();

	this->grad_q_current = 0.0;
	this->grad_q_before = 0.0;
	this->oscilation_smoother = 1.0;
	this->no_osci_counter = 0.0;
	this->number_osci_counter = 2.0;
	this->store_grad_q_before = 0.0;
	this->store_oscilation_smoother = 1.0;
	this->store_number_osci_counter = 2.0;
	this->store_no_osci_counter = 0.0;

	//this->q_list.relevant=false;

	this->predicted_h_two = 0.0;
	this->corrected_h_two = 0.0;
	this->gradient_list_h_two.clear();
	for (int i = 0; i < constant::no_stored_grad; i++) {
		this->gradient_list_h_two.append(0.0);
	}
	this->gradient_h_two = 0.0;
	this->calc_h_two = 0.0;
	this->old_calc_h_two = 0.0;

	this->grad_delta_h = 0.0;
	this->store_grad_delta_h = 0.0;
	this->old_delta_h = 0.0;
	this->store_old_delta_h = 0.0;
}
//Output the discharges  to file
//void Hyd_Coupling_Point_GW2GW::output_discharge_list(void){
//
//	if(this->q_list.relevant==false){
//		return;
//	}
//	ostringstream cout;
//	//set prefix for output
//	ostringstream prefix;
//	
//	prefix << "DB_GW2GW_"<<this->point_index<<"> ";
//	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
//	cout<<"  Id  "<<"   time  "<< "    q_current  "<< "   q_smooth   "<<endl;
//	for(int i=0; i< this->q_list.timepoint.count(); i++){
//		cout << "  " <<i<<"   " << this->q_list.timepoint.at(i)<< "  "<<this->q_list.current_discharge.at(i);
//		cout << "   "<< this->q_list.smooth_discharge.at(i)<<endl;
//
//	}
//
//	Sys_Common_Output::output_hyd->output_txt(&cout,true);
//	Sys_Common_Output::output_hyd->rewind_userprefix();
//
//}
//Copy operator
Hyd_Coupling_Point_GW2GW& Hyd_Coupling_Point_GW2GW::operator=(const Hyd_Coupling_Point_GW2GW& object) {
	_Hyd_Coupling_Point::operator =(object);
	this->gw1_elem_index = object.gw1_elem_index;
	this->gw1_elem = object.gw1_elem;
	this->gw2_elem_index = object.gw2_elem_index;
	this->gw2_elem = object.gw2_elem;
	this->x_width_flag = object.x_width_flag;
	this->gw2_elem_is_setted = object.gw2_elem_is_setted;
	this->gw1_elem_is_setted = object.gw1_elem_is_setted;
	this->flow_distance = object.flow_distance;
	this->max_z = object.max_z;
	this->mid_T = object.mid_T;
	this->c_flow = object.c_flow;
	this->grad_q_current = object.grad_q_current;
	this->grad_q_before = object.grad_q_before;
	this->oscilation_smoother = object.oscilation_smoother;
	this->no_osci_counter = object.no_osci_counter;
	this->number_osci_counter = object.number_osci_counter;
	this->store_grad_q_before = object.store_grad_q_before;
	this->store_oscilation_smoother = object.store_oscilation_smoother;
	this->store_no_osci_counter = object.store_no_osci_counter;
	this->store_number_osci_counter = object.store_number_osci_counter;

	//this->q_list.relevant=object.q_list.relevant;

	this->corrected_h_two = object.corrected_h_two;
	this->predicted_h_two = object.predicted_h_two;
	this->gradient_h_two = object.gradient_h_two;
	this->calc_h_two = object.calc_h_two;
	this->old_calc_h_two = object.old_calc_h_two;

	this->grad_delta_h = object.grad_delta_h;
	this->store_grad_delta_h = object.store_grad_delta_h;
	this->old_delta_h = object.old_delta_h;
	this->store_old_delta_h = object.store_old_delta_h;

	return *this;
}
//____________
//private
//Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
double Hyd_Coupling_Point_GW2GW::smooth_coupling_discharge(const double q_current, double *old_q) {
	double smooth_q = 0.0;



	if (this->delta_t > 0.0) {
		this->grad_q_current = (q_current - *old_q) / (this->delta_t);
		if ((this->grad_q_before > 0.0 && this->grad_q_current < 0.0) || (this->grad_q_before<0.0 && this->grad_q_current>0.0)) {
			this->oscilation_smoother = this->oscilation_smoother*2.0;
			//this->oscilation_smoother=this->oscilation_smoother+this->number_osci_counter;
			//this->no_osci_counter=0.0;
			//this->number_osci_counter=1.0;
			if (this->oscilation_smoother >= 100.0) {
				this->oscilation_smoother = 100.0;
			}

		}
		else {
			this->oscilation_smoother = this->oscilation_smoother / 2.0;
			//this->oscilation_smoother=this->oscilation_smoother-pow(this->no_osci_counter,0.75);
			//this->no_osci_counter=this->no_osci_counter+1.0;
			//this->number_osci_counter=2.0;
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
void Hyd_Coupling_Point_GW2GW::predict_values(const int int_counter) {
	if (this->delta_t > 0.0) {
		//one

		this->calc_h_one = this->gw1_elem->element_type->get_h_value();

		this->gradient_h_one = (this->calc_h_one - this->old_calc_h_one) / this->delta_t;
		if (this->time_check == false) {
			this->old_calc_h_one = this->calc_h_one;
			this->swap_grad_list(&this->gradient_list_h_one, this->gradient_h_one);
		}
		else {
			this->replace_first_grad_list(&this->gradient_list_h_one, this->gradient_h_one);
		}

		//this->predicted_h_one=this->calc_h_one+(1.0/12.0)*(23.0*this->gradient_list_h_one.at(0)-16.0*this->gradient_list_h_one.at(1)+5.0*this->gradient_list_h_one.at(2))*this->delta_t;
		//this->predicted_h_one=this->calc_h_one+(0.5)*(3.0*this->gradient_list_h_one.at(0)-1.0*this->gradient_list_h_one.at(1))*this->delta_t;

		//two

		this->calc_h_two = this->gw2_elem->element_type->get_h_value();

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
			//explicit
			this->predicted_h_two = this->calc_h_two;
			this->predicted_h_one = this->calc_h_one;

			if (this->predicted_h_two < 0.0) {
				this->predicted_h_two = 0.0;
			}
			this->predicted_h_one = this->calc_h_one + (this->gradient_list_h_one.at(0))*this->delta_t;
			if (this->predicted_h_one < 0.0) {
				this->predicted_h_one = 0.0;
			}
		}
		else {
			this->predicted_h_two = this->calc_h_two;
			this->predicted_h_one = this->calc_h_one;
		}


	}

}