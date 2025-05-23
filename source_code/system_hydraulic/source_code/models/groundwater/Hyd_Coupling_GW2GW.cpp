#include "source_code\Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_GW2GW.h"

//constructor
Hyd_Coupling_GW2GW::Hyd_Coupling_GW2GW(void) {
	this->groundwater_model_1 = NULL;
	this->groundwater_model_2 = NULL;
	this->defining_polysegment.set_line_type(_hyd_groundwater_polysegment_type::GW_BOUNDARYLINE);

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_GW2GW) - sizeof(Hyd_Coupling_Point_GW2GW_List), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_GW2GW::~Hyd_Coupling_GW2GW(void) {

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_GW2GW) - sizeof(Hyd_Coupling_Point_GW2GW_List), _sys_system_modules::HYD_SYS);
}
//__________________
//public
//The pointer of the models for coupling are setted
void Hyd_Coupling_GW2GW::set_ptr_coupling(Hyd_Model_Groundwater *groundwater_1, Hyd_Model_Groundwater *groundwater_2) {
	this->groundwater_model_1 = groundwater_1;
	this->groundwater_model_2 = groundwater_2;
}
//The couplings are initialized
void Hyd_Coupling_GW2GW::init_coupling(void) {
	//set prefix for output
	ostringstream prefix;
	prefix << "GW_GW> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Coupling between groundwater model " << this->groundwater_model_1->Param_GW.get_groundwater_number();
	cout << " and groundwater model " << this->groundwater_model_2->Param_GW.get_groundwater_number() << "..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	try {
		/*this->groundwater_model_1->raster.output_geometrical_boundary();
		this->groundwater_model_2->raster.output_geometrical_boundary();*/


		this->set_defining_polysegment();

		//first groundwater model
		this->groundwater_model_1->raster.assign_elements2couplingpointlist(&this->list, true);

		//second groundwater model
		this->groundwater_model_2->raster.assign_elements2couplingpointlist(&this->list, false);

		//sort it along the defining line
		this->list.sort_distance_along_polysegment();
		//calculate the distances
		this->list.calculate_alldistance_up_down();

		//complete the points in the list
		this->list.complete_couplingpoints(this->groundwater_model_1, this->groundwater_model_2);

		//transfer the infos to the coupling points
		this->list.transfer_informations2points();

		////output for development
		ostringstream cout;
		this->list.output_setted_members(&cout);

	}
	catch (Error msg) {
		ostringstream info;
		info << "Coupling between groundwater model " << this->groundwater_model_1->Param_GW.get_groundwater_number();
		info << " and groundwater model " << this->groundwater_model_2->Param_GW.get_groundwater_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Synchronise the models
void Hyd_Coupling_GW2GW::synchronise_models(const double timepoint, const double delta_t, const bool time_check, const int internal_counter) {

	try {
		this->list.syncronisation_models_bylistpoints(timepoint, delta_t, time_check, internal_counter);
	}
	catch (Error msg) {
		ostringstream info;
		info << "Synchronisation  between groundwater model " << this->groundwater_model_1->Param_GW.get_groundwater_number();
		info << " and groundwater model " << this->groundwater_model_2->Param_GW.get_groundwater_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Output the header for the coupled model indizes (static)
void Hyd_Coupling_GW2GW::output_header_coupled_indices(ostringstream *cout) {
	*cout << "Groundwater to groundwater..." << endl;
	*cout << "No." << W(12) << "Id_GW_small" << W(12) << "Id_GW_big" << endl;
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the indices of the coupled model
void Hyd_Coupling_GW2GW::output_index_coupled_models(ostringstream *cout, const int number) {
	*cout << number << W(10);
	*cout << this->groundwater_model_1->Param_GW.get_groundwater_number() << W(12);
	*cout << this->groundwater_model_2->Param_GW.get_groundwater_number() << endl;
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the setted coupling points in the list
void Hyd_Coupling_GW2GW::output_setted_coupling_points(void) {
	ostringstream cout;
	ostringstream prefix;
	prefix << "GW_" << this->groundwater_model_1->Param_GW.get_groundwater_number() << "_GW_" << this->groundwater_model_2->Param_GW.get_groundwater_number() << " > ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "List of coupling points" << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	this->list.output_setted_members(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Clone the groundwater to groundwater couplings
void Hyd_Coupling_GW2GW::clone_couplings(Hyd_Coupling_GW2GW *coupling, Hyd_Hydraulic_System *system) {
	if (coupling->groundwater_model_1 != NULL) {
		this->groundwater_model_1 = system->get_ptr_groundwater_model(coupling->groundwater_model_1->Param_GW.get_groundwater_number());
	}
	if (coupling->groundwater_model_2 != NULL) {
		this->groundwater_model_2 = system->get_ptr_groundwater_model(coupling->groundwater_model_2->Param_GW.get_groundwater_number());
	}

	this->defining_polysegment.clone_polysegment(&coupling->defining_polysegment);

	this->list.set_defining_polysegment_gw(&this->defining_polysegment);
	this->list.clone_list(&coupling->list, this->groundwater_model_1, this->groundwater_model_2);

}
//______________
//private
//Set the defining polysegment with the boundary of the first groundwater model
void Hyd_Coupling_GW2GW::set_defining_polysegment(void) {

	//set the line type of the defining polysegment
	this->defining_polysegment.set_line_type(_hyd_groundwater_polysegment_type::GW_BOUNDARYLINE);

	//add additional points (corner points)
	//check if the first groundwater is inside the second one
	if (this->groundwater_model_2->raster.geometrical_bound.check_polygon_inside(&(this->groundwater_model_1->raster.geometrical_bound)) == true) {
		this->defining_polysegment.set_polysegment(&(this->groundwater_model_1->raster.geometrical_bound));
	}
	else {
		Hyd_Groundwater_Polysegment polysegment_buffer;
		_geo_multisegment_interception point_buffer;
		point_buffer.index_intercepts = -1;
		point_buffer.index_is_intercept = -1;
		point_buffer.index_point_is_intercepted = -1;
		int old_number_interception = -1;
		polysegment_buffer.set_line_type(_hyd_groundwater_polysegment_type::GW_BOUNDARYLINE);
		polysegment_buffer.set_polysegment(&(this->groundwater_model_1->raster.geometrical_bound));
		//fill the list with interception point (interception boundary gw1 with boundary gw2)
		this->groundwater_model_2->raster.geometrical_bound.calculate_polysegment_interception(&(polysegment_buffer.intercept_list), &polysegment_buffer);
		old_number_interception = polysegment_buffer.intercept_list.get_number_points();
		for (int i = 0; i < old_number_interception; i++) {
			//first point
			point_buffer.distance = polysegment_buffer.get_distance_along_polysegment(&(polysegment_buffer.my_segment[polysegment_buffer.intercept_list.get_point(i).index_intercepts].point1));
			point_buffer.line_interception.interception_point = polysegment_buffer.my_segment[polysegment_buffer.intercept_list.get_point(i).index_intercepts].point1;
			polysegment_buffer.intercept_list.set_new_point(&point_buffer);
			//second point
			point_buffer.distance = polysegment_buffer.get_distance_along_polysegment(&(polysegment_buffer.my_segment[polysegment_buffer.intercept_list.get_point(i).index_intercepts].point2));
			if (polysegment_buffer.intercept_list.get_point(i).index_intercepts == 3 && point_buffer.distance < constant::zero_epsilon) {
				point_buffer.distance = polysegment_buffer.get_total_distance();
			}
			point_buffer.line_interception.interception_point = polysegment_buffer.my_segment[polysegment_buffer.intercept_list.get_point(i).index_intercepts].point2;
			polysegment_buffer.intercept_list.set_new_point(&point_buffer);

		}
		this->defining_polysegment.set_is_closed(false);



		polysegment_buffer.intercept_list.sort_distance();
		if (this->groundwater_model_2->raster.geometrical_bound.check_point_inside_atboundary(polysegment_buffer.get_first_point()) == true) {
			this->defining_polysegment.set_is_closed(true);
		}

		//convert the interception list into a normal Geo_Point_List
		Geo_Point_List buffer_points;
		Geo_Point buffer_point;
		for (int i = 0; i < polysegment_buffer.intercept_list.get_number_points(); i++) {
			buffer_point.set_point(polysegment_buffer.intercept_list.get_point(i).line_interception.interception_point);
			buffer_points.set_new_point(&buffer_point);
		}

		//set the geo_point_list to the defining polysegment
		this->defining_polysegment.set_number_of_points(buffer_points.get_number_points());
		//check
		ostringstream cout;
		buffer_points.output_members(&cout);
		Sys_Common_Output::output_hyd->output_txt(&cout);

		this->defining_polysegment.set_points(buffer_points.get_total_list());
	}
	//set the defining polysegment to the list
	this->list.set_defining_polysegment_gw(&this->defining_polysegment);

	//check for development
	//this->defining_polysegment.output_members();

}