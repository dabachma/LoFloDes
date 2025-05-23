#include "source_code\Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_RV2GW_Merged.h"

//init static members
Tables *Hyd_Coupling_RV2GW_Merged::max_h_table = NULL;

//constructor
Hyd_Coupling_RV2GW_Merged::Hyd_Coupling_RV2GW_Merged(void) {
	this->groundwater_model = NULL;
	this->river_model = NULL;
	this->list_left.set_leftriver_bank_line(true);
	this->list_right.set_leftriver_bank_line(false);
	this->river_model_is_set = false;
	this->number_gw_models = 0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_RV2GW_Merged) - sizeof(Hyd_Coupling_Point_RV2GW_List) * 2, _sys_system_modules::HYD_SYS);

}
//destructor
Hyd_Coupling_RV2GW_Merged::~Hyd_Coupling_RV2GW_Merged(void) {
	if (this->groundwater_model != NULL) {
		delete[]this->groundwater_model;
		this->groundwater_model = NULL;
	}
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_RV2GW_Merged) - sizeof(Hyd_Coupling_Point_RV2GW_List) * 2, _sys_system_modules::HYD_SYS);
}
//___________
//public



//Switch the applied-flag in the database table for the maximum waterlevels of the coupling points for each fpl-section for the given identifier (static)
/*
void Hyd_Coupling_RV2FP_Merged::switch_applied_flag_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
	query_string << " SET ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Merged::switch_applied_flag_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
*/


//The pointer of the models for couplings are setted and the lists are merged
void Hyd_Coupling_RV2GW_Merged::set_ptr_coupling_with_merging(Hyd_Coupling_RV2GW *couplings) {
	if (couplings == NULL) {
		return;
	}
	else {
		if (this->river_model == NULL && couplings->get_is_merged() == false) {
			this->river_model_is_set = true;
			this->river_model = couplings->get_river_model();
			this->add_groundwatermodel_pointer(couplings->get_groundwater_model());
			//set the defining polysegment
			/*
			this->list_left.set_defining_polysegment_gw(couplings->list_left.get_defining_polysegment_gw());
			this->list_right.set_defining_polysegment_gw(couplings->list_right.get_defining_polysegment_gw());
			*/
			this->list_mid.set_defining_polysegment_gw(couplings->list_mid.get_defining_polysegment_gw());
			//merge the lists
			/*
			couplings->list_left.merge_coupling_list(&this->list_left);
			couplings->list_right.merge_coupling_list(&this->list_right);
			*/
			couplings->list_mid.merge_coupling_list(&this->list_mid);
			couplings->set_is_merged();

		}
		else if (this->river_model == couplings->get_river_model() && couplings->get_is_merged() == false) {
			this->add_groundwatermodel_pointer(couplings->get_groundwater_model());
			//merge the lists
			/*
			couplings->list_left.merge_coupling_list(&this->list_left);
			couplings->list_right.merge_coupling_list(&this->list_right);
			*/
			couplings->list_mid.merge_coupling_list(&this->list_mid);
			couplings->set_is_merged();
		}
	}
}

//Get the coupled river index
int Hyd_Coupling_RV2GW_Merged::get_river_index(void) {
	if (this->river_model != NULL) {
		return this->river_model->Param_RV.get_river_number();
	}
	else {
		return -1;
	}
}
//Get a pointer to the coupled river model
Hyd_Model_River* Hyd_Coupling_RV2GW_Merged::get_river_model(void) {
	return this->river_model;
}
//Get the flag if the river model is already setted
bool Hyd_Coupling_RV2GW_Merged::get_river_model_is_setted(void) {
	return this->river_model_is_set;
}

//Get the number of coupled groundwaters
int Hyd_Coupling_RV2GW_Merged::get_number_coupled_gw(void) {
	return this->number_gw_models;
}
//Get the index of a coupled groundwater model by the given index (number of coupled groundwater models)
int Hyd_Coupling_RV2GW_Merged::get_index_coupled_gw_models(const int index) {
	int index_gw = -1;
	if (index < 0 || index >= this->number_gw_models) {
		return index_gw = -1;
	}
	else {
		index_gw = this->groundwater_model[index]->Param_GW.get_groundwater_number();
	}

	return index_gw;
}
//Synchronise the models
void Hyd_Coupling_RV2GW_Merged::synchronise_models(const double timepoint, const double delta_t, const bool time_check, const int internal_counter) {
	try {
		/*
		this->list_left.syncronisation_models_bylistpoints(timepoint, delta_t, time_check, internal_counter);
		this->list_right.syncronisation_models_bylistpoints(timepoint, delta_t, time_check, internal_counter);
		*/
		this->list_mid.syncronisation_models_bylistpoints(timepoint, delta_t, time_check, internal_counter);
	}
	catch (Error msg) {
		ostringstream info;
		info << "Synchronisation between groundwaters ";
		for (int i = 0; i < this->number_gw_models; i++) {
			info << this->groundwater_model[i]->Param_GW.get_groundwater_number() << ", ";
		}
		info << " and river model " << this->river_model->Param_RV.get_river_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get the maximum waterlevel gradient 
double Hyd_Coupling_RV2GW_Merged::get_maximum_h_grad(void) {
	double buffer = 0.0;
	double buffer_max = 0.0;

	/*
	buffer = this->list_left.get_maximum_h_grad();
	if (buffer_max < buffer) {
		buffer_max = buffer;
	}

	buffer = this->list_right.get_maximum_h_grad();
	if (buffer_max < buffer) {
		buffer_max = buffer;
	}
	*/
	buffer = this->list_mid.get_maximum_h_grad();
	if (buffer_max < buffer) {
		buffer_max = buffer;
	}
	return buffer_max;
}
//Reset the coupling discharges
void  Hyd_Coupling_RV2GW_Merged::reset_coupling_discharges(void) {
	/*
	this->list_left.reset_coupling_discharge();
	this->list_right.reset_coupling_discharge();
	*/
	this->list_mid.reset_coupling_discharge();
}
//Output the header for the coupled model indizes
void  Hyd_Coupling_RV2GW_Merged::output_header_coupled_indices(ostringstream *cout) {
	*cout << "River to multiple groundwaters..." << endl;
	*cout << "No." << W(12) << "Id_RV" << W(12) << "No_GWs" << W(12) << "Id_GWs" << endl;
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the indexes of the coupled model
void  Hyd_Coupling_RV2GW_Merged::output_index_coupled_models(ostringstream *cout, const int number) {
	*cout << number << W(10);
	*cout << this->river_model->Param_RV.get_river_number() << W(12);
	*cout << this->number_gw_models << W(12);
	for (int i = 0; i < this->number_gw_models; i++) {
		*cout << this->groundwater_model[i]->Param_GW.get_groundwater_number() << "  ";
	}
	*cout << endl;

	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the setted coupling points in the list
void  Hyd_Coupling_RV2GW_Merged::output_setted_coupling_points(void) {
	ostringstream cout;
	ostringstream prefix;
	prefix << "RV_" << this->river_model->Param_RV.get_river_number() << "_GW_MERG> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	/*
	cout << "List of coupling points (left river bank)" << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	this->list_left.output_setted_members(&cout);
	cout << "List of coupling points (right river bank)" << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	this->list_right.output_setted_members(&cout);
	*/
	cout << "List of coupling points (midline)" << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	this->list_mid.output_setted_members(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Calculate and output final results of the maximum waterlevel of the coupling points to database REVIEW
/*
void Hyd_Coupling_RV2GW_Merged::calc_output_final_results(QSqlDatabase *ptr_database, const _sys_system_id id, const string id_break, const int bound_sc) {

	int sec_id = 0;
	int number_sec = 0;
	_hyd_max_values h_max;
	h_max.maximum = 0.0;
	h_max.time_point = 0.0;
	int point_id_h_max = 0;
	//left list
	number_sec = this->list_left.get_ptr_fpl_section_ids()->count();
	for (int i = 0; i < number_sec; i++) {
		sec_id = this->list_left.get_ptr_fpl_section_ids()->at(i);
		this->list_left.get_max_h2fpl(sec_id, &h_max, &point_id_h_max);
		this->list_left.get_max_h2break(sec_id, &h_max2break, &point_id_h_max2break);
		Hyd_Coupling_RV2FP_Merged::output_final_results(ptr_database, id, id_break, bound_sc, sec_id, h_max, point_id_h_max, h_max2break, point_id_h_max2break);
	}
	//right list
	number_sec = this->list_right.get_ptr_fpl_section_ids()->count();
	for (int i = 0; i < number_sec; i++) {
		sec_id = this->list_right.get_ptr_fpl_section_ids()->at(i);
		this->list_right.get_max_h2fpl(sec_id, &h_max, &point_id_h_max);
		this->list_right.get_max_h2break(sec_id, &h_max2break, &point_id_h_max2break);
		Hyd_Coupling_RV2FP_Merged::output_final_results(ptr_database, id, id_break, bound_sc, sec_id, h_max, point_id_h_max, h_max2break, point_id_h_max2break);
	}


}
*/

//Check coupling from one bank line to the other side of the river; the coupling is than not possible
void Hyd_Coupling_RV2GW_Merged::check_coupling2_other_river_side(void) {
	if (this->river_model != NULL) {
		/*
		this->list_left.check_intercepting_riverbankline2connecting_segment(&this->river_model->river_rightline);
		this->list_right.check_intercepting_riverbankline2connecting_segment(&this->river_model->river_leftline);
		*/
		this->list_mid.check_intercepting_riverbankline2connecting_segment(&this->river_model->river_midline);
	}
}
//Check coupling from one bank line to the other side of other rivers; the coupling is than not possible
void Hyd_Coupling_RV2GW_Merged::check_coupling2_other_river_side(Hyd_Coupling_RV2GW_Merged *other_river) {
	if (other_river != NULL) {
		bool same_flag = false;
		//check if there are same ´groundwaters
		for (int i = 0; i < this->number_gw_models; i++) {
			for (int j = 0; j < this->get_number_coupled_gw(); j++) {
				if (this->get_index_coupled_gw_models(i) == other_river->get_index_coupled_gw_models(j)) {
					same_flag = true;
					break;
				}
			}
		}
		//check the intercepting
		if (same_flag == true) {
			/*
			this->list_left.check_intercepting_riverbankline2connecting_segment(&other_river->get_river_model()->river_rightline);
			this->list_right.check_intercepting_riverbankline2connecting_segment(&other_river->get_river_model()->river_rightline);
			this->list_left.check_intercepting_riverbankline2connecting_segment(&other_river->get_river_model()->river_leftline);
			this->list_right.check_intercepting_riverbankline2connecting_segment(&other_river->get_river_model()->river_leftline);
			*/
		}
	}
}
//Clone the river to groundwater merged coupling
void Hyd_Coupling_RV2GW_Merged::clone_couplings(Hyd_Coupling_RV2GW_Merged *coupling, Hyd_Hydraulic_System *system) {
	this->river_model_is_set = coupling->river_model_is_set;

	if (coupling->river_model != NULL) {
		this->river_model = system->get_ptr_river_model(coupling->river_model->Param_RV.get_river_number());
	}
	//set the gw-models
	for (int i = 0; i < coupling->number_gw_models; i++) {
		Hyd_Model_Groundwater *buffer;
		buffer = system->get_ptr_groundwater_model(coupling->groundwater_model[i]->Param_GW.get_groundwater_number());
		this->add_groundwatermodel_pointer(buffer);
	}
	//the lists
	/*
	this->list_left.clone_list(&coupling->list_left, system, this->river_model->Param_RV.get_river_number());
	this->list_right.clone_list(&coupling->list_right, system, this->river_model->Param_RV.get_river_number());
	*/
	this->list_mid.clone_list(&coupling->list_mid, system, this->river_model->Param_RV.get_river_number());
}

//__________
//private
//Add a pointer of additional groundwater models
void Hyd_Coupling_RV2GW_Merged::add_groundwatermodel_pointer(Hyd_Model_Groundwater *pointer) {

	if (pointer == NULL) {
		return;
	}

	//check if it is the same
	for (int i = 0; i < this->number_gw_models; i++) {
		if (this->groundwater_model[i] == pointer) {
			return;
		}
	}

	//allocate a buffer
	Hyd_Model_Groundwater **buffer = NULL;
	try {
		(buffer) = new Hyd_Model_Groundwater*[this->number_gw_models];
	}
	catch (bad_alloc &t) {
		Error msg = this->set_error(2);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//copy it
	for (int i = 0; i < this->number_gw_models; i++) {
		buffer[i] = this->groundwater_model[i];
	}
	//delete the pointer
	if (this->groundwater_model != NULL) {
		delete[]this->groundwater_model;
		this->groundwater_model = NULL;
	}
	//count up
	this->number_gw_models++;
	//allocate new
	try {
		(this->groundwater_model) = new Hyd_Model_Groundwater*[this->number_gw_models];
	}
	catch (bad_alloc &t) {
		Error msg = this->set_error(2);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//recopy it
	for (int i = 0; i < this->number_gw_models - 1; i++) {
		this->groundwater_model[i] = buffer[i];
	}
	//set the last pointer
	this->groundwater_model[this->number_gw_models - 1] = pointer;

	//delete the buffer
	if (buffer != NULL) {
		delete[]buffer;
		buffer = NULL;
	}
}


//set the error
Error Hyd_Coupling_RV2GW_Merged::set_error(const int err_type) {
	string place = "Hyd_Coupling_RV2GW_Merged::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;
	switch (err_type) {
	case 2://bad alloc
		place.append("add_groundwatermodel_pointer(Hyd_Model_Groundwater *pointer)");
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
//Set warning(s)
Warning Hyd_Coupling_RV2GW_Merged::set_warning(const int warn_type) {
	string place = "Hyd_Coupling_RV2GW_Merged::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
	default:
		place.append("set_warning(const int warn_type)");
		reason = "Unknown flag!";
		help = "Check the flags";
		type = 5;
	}
	msg.set_msg(place, reason, help, reaction, type);
	msg.make_second_info(info.str());
	return msg;
};