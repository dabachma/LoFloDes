#include "source_code\Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_RV2GW.h"

//constructor
Hyd_Coupling_RV2GW::Hyd_Coupling_RV2GW(void) {
	this->groundwater_model = NULL;
	this->river_model = NULL;
	this->list_mid.set_midtriver_line(true);
	this->list_left.set_midtriver_line(false);
	this->list_right.set_midtriver_line(false);
	this->list_left.set_leftriver_bank_line(true);
	this->list_right.set_leftriver_bank_line(false);
	this->is_merged_flag = false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_RV2GW) - sizeof(Hyd_Coupling_Point_RV2GW_List) * 2, _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_RV2GW::~Hyd_Coupling_RV2GW(void) {
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_RV2GW) - sizeof(Hyd_Coupling_Point_RV2GW_List) * 2, _sys_system_modules::HYD_SYS);
}
//__________________
//public
//The pointer of the models for coupling are setted
void Hyd_Coupling_RV2GW::set_ptr_coupling(Hyd_Model_Groundwater *groundwater, Hyd_Model_River *river) {
	this->groundwater_model = groundwater;
	this->river_model = river;
}
//The couplings are initialized
void Hyd_Coupling_RV2GW::init_coupling(void) {

	//set prefix for output
	ostringstream prefix;
	prefix << "RV_GW> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Coupling between groundwater model " << this->groundwater_model->Param_GW.get_groundwater_number();
	cout << " and river model " << this->river_model->Param_RV.get_river_number() << "..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	try {

		/*
		//set the river banks (left) as the relevant polysegment line
		this->list_left.set_defining_polysegment_gw(&(this->river_model->river_leftline));
		//set the river banks (right) as the relevant polysegment line
		this->list_right.set_defining_polysegment_gw(&(this->river_model->river_rightline));
		*/
		this->list_mid.set_defining_polysegment_gw(&(this->river_model->river_midline));

		//TODO add all points within the river polygon not just the banks


		//fill the list with the groundwater elements
		/*
		this->groundwater_model->raster.assign_elements2couplingpointlist(&this->list_left);
		if (this->list_left.get_number_couplings() <= 1) {
			Error msg = this->set_error(0);
			ostringstream info;
			info << "1d-river model name and id: " << this->river_model->Param_RV.get_river_name() << "   " << this->river_model->Param_RV.get_river_number() << endl;
			info << "2d-groundwater model name and id: " << this->groundwater_model->Param_GW.get_groundwater_name() << "   " << this->groundwater_model->Param_GW.get_groundwater_number() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		this->groundwater_model->raster.assign_elements2couplingpointlist(&this->list_right);
		if (this->list_right.get_number_couplings() <= 1) {
			Error msg = this->set_error(1);
			ostringstream info;
			info << "1d-river model name and id: " << this->river_model->Param_RV.get_river_name() << "   " << this->river_model->Param_RV.get_river_number() << endl;
			info << "2d-groundwater model name and id: " << this->groundwater_model->Param_GW.get_groundwater_name() << "   " << this->groundwater_model->Param_GW.get_groundwater_number() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		*/

		this->groundwater_model->raster.assign_elements2couplingpointlist(&this->list_mid);

		//allocate the couplingpoint to the river segment in which the point located in 
		this->list_mid.assign_point2polysegment();


		if (this->list_mid.get_number_couplings() <= 1) {
			Error msg = this->set_error(0);
			ostringstream info;
			info << "1d-river model name and id: " << this->river_model->Param_RV.get_river_name() << "   " << this->river_model->Param_RV.get_river_number() << endl;
			info << "2d-groundwater model name and id: " << this->groundwater_model->Param_GW.get_groundwater_name() << "   " << this->groundwater_model->Param_GW.get_groundwater_number() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		/*
		//add the relevant points of the defining polysegment
		this->list_left.add_relevant_polysegment_points(&(this->groundwater_model->raster.geometrical_bound));
		this->list_right.add_relevant_polysegment_points(&(this->groundwater_model->raster.geometrical_bound));

		//sort it along the defining line
		this->list_left.sort_distance_along_polysegment();
		this->list_right.sort_distance_along_polysegment();
		//calculate the distances
		this->list_left.calculate_alldistance_up_down();
		this->list_right.calculate_alldistance_up_down();
		//set the profile pointer
		this->list_left.convert_profiles_index2pointer(this->river_model);
		this->list_right.convert_profiles_index2pointer(this->river_model);

		//transfer the infos to the coupling points
		this->list_left.transfer_informations2points();
		this->list_right.transfer_informations2points();
		*/
		//add the relevant points of the defining polysegment
		this->list_mid.add_relevant_polysegment_points(&(this->groundwater_model->raster.geometrical_bound));

		


		//sort it along the defining line
		this->list_mid.sort_distance_along_polysegment();
		//calculate the distances
		this->list_mid.calculate_alldistance_up_down();
		//set the profile pointer
		this->list_mid.convert_profiles_index2pointer(this->river_model);
		//transfer the infos to the coupling points
		this->list_mid.transfer_informations2points();

		//this->output_result2csv_1d();
	}


	catch (Error msg) {
		ostringstream info;
		info << "Coupling between groundwater model " << this->groundwater_model->Param_GW.get_groundwater_number();
		info << "  and river model " << this->river_model->Param_RV.get_river_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if (this->list_mid.get_number_couplings() > 0) {
		ostringstream cout;
		//cout << "River to groundwater..." << endl;
		cout << "Id_RV_ds_Profile" << W(12) << "Id_GW_Element" << endl;
		for (int i = 0; i < list_mid.get_number_couplings(); i++) {
			cout << list_mid.get_couplingpoint(i).get_downstream_rvprofile()->get_profile_name() <<W(12);
			cout << list_mid.get_couplingpoint(i).get_index_groundwater_element()<<endl;
			
		}

		Sys_Common_Output::output_hyd->output_txt(&cout);
	}


	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the header for the coupled model indizes
void Hyd_Coupling_RV2GW::output_header_coupled_indices(ostringstream *cout) {
	*cout << "River to groundwater..." << endl;
	*cout << "No." << W(12) << "Id_RV" << W(12) << "Id_GW" << endl;
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the indexes of the coupled model
void Hyd_Coupling_RV2GW::output_index_coupled_models(ostringstream *cout, const int number) {
	*cout << number << W(10);
	*cout << this->river_model->Param_RV.get_river_number() << W(12);
	*cout << this->groundwater_model->Param_GW.get_groundwater_number() << endl;
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the setted coupling points in the list TODO check the lists
void Hyd_Coupling_RV2GW::output_setted_coupling_points(void) {
	ostringstream cout;
	ostringstream prefix;
	prefix << "RV_" << this->river_model->Param_RV.get_river_number() << "_GW_" << this->groundwater_model->Param_GW.get_groundwater_number() << "> ";
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
void Hyd_Coupling_RV2GW::init_output2file_csv(void) {

	// for 1d - output
	//results per timesteps

	if (this->river_model->Param_RV.get_output_couplings() == true) {
		for (int i = 0; i < this->list_mid.get_number_couplings(); i++) {


			ofstream output_csv;
			output_csv.clear();
			//open the file
			string buffer = this->river_model->Param_RV.get_crude_filename_result_1d();
			buffer += "/";
			buffer += hyd_label::paraview;
			buffer += "/";
			
			buffer += "RV2GW_";
			buffer += to_string(this->list_mid.get_ptr_couplingpoint(i)->get_index_upstream_rvprofile());
			buffer += "_";
			buffer += to_string(this->list_mid.get_ptr_couplingpoint(i)->get_index_downstream_rvprofile());
			buffer += "_";
			buffer += to_string(this->list_mid.get_ptr_couplingpoint(i)->get_groundwater_index());
			buffer += "_";
			buffer += to_string(this->list_mid.get_ptr_couplingpoint(i)->get_index_groundwater_element());

			buffer += hyd_label::csv;

			this->list_mid.get_ptr_couplingpoint(i)->set_file_name();

			//this->points[i].point_index
			output_csv.open(buffer.c_str());
			//check if it is open
			if (output_csv.is_open() == false) {
				Error msg = this->set_error(6);
				ostringstream info;
				info << "File name " << buffer<< endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			else {
				//output the file header
				output_csv << "x_coordination:" << this->list_mid.get_ptr_couplingpoint(i)->get_xcoordinate() << ",";
				output_csv << W(10) << "y_coordination:" << this->list_mid.get_ptr_couplingpoint(i)->get_ycoordinate();
				output_csv << endl;
				output_csv << "time" << label::sec << ",";
				output_csv << W(10) << " groundwater level " << label::m << "," << W(10) << " river level " << label::m << ",";
				output_csv << W(10) << " riverbed level " << label::m << ",";
				output_csv << W(10) << " section length " << label::m << ",";
				output_csv << W(10) << " wetted perimeter " << label::m << ",";
				output_csv << W(10) << " q " << label::qm_per_sec;
				output_csv << endl;

			}
			//this->list_mid.get_ptr_couplingpoint(i)->output_result_members_per_timestep2csv(&output_csv);


			output_csv.flush();
			output_csv.close();


		}
	}
	//output per profile
	


	

}
//Get a pointer to the coupled river model
Hyd_Model_River* Hyd_Coupling_RV2GW::get_river_model(void) {
	return this->river_model;
}
//Get a pointer to the coupled groundwater model
Hyd_Model_Groundwater* Hyd_Coupling_RV2GW::get_groundwater_model(void) {
	return this->groundwater_model;
}
//Get the coupled river index
int Hyd_Coupling_RV2GW::get_river_index(void) {
	if (this->river_model != NULL) {
		return this->river_model->Param_RV.get_river_number();
	}
	else {
		return -1;
	}
}
//Get the coupled groundwater index
int Hyd_Coupling_RV2GW::get_groundwater_index(void) {
	if (this->groundwater_model != NULL) {
		return this->groundwater_model->Param_GW.get_groundwater_number();
	}
	else {
		return -1;
	}
}
//Set that the couplings are already merged
void Hyd_Coupling_RV2GW::set_is_merged(void) {
	this->is_merged_flag = true;
}
//Get that the couplings are already merged
bool Hyd_Coupling_RV2GW::get_is_merged(void) {
	return this->is_merged_flag;
}
//____________
//private
//set the error
Error Hyd_Coupling_RV2GW::set_error(const int err_type) {
	string place = "Hyd_Coupling_RV2GW::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;
	switch (err_type) {
	case 0://not enough interceptions
		place.append("init_coupling(void)");
		reason = "There are no or just 1 inteception point of the left river boudnary line with the 2d-raster elements";
		help = "The river should have at least 2 interceptions with the 2d-raster elements; extend the river model or reduce the 2d-element size";
		type = 16;
		break;
	case 1://not enough interceptions
		place.append("init_coupling(void)");
		reason = "There are no or just 1 inteception point of the right river boudnary line with the 2d-raster elements";
		help = "The river should have at least 2 interceptions with the 2d-raster elements; extend the river model or reduce the 2d-element size";
		type = 16;
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
