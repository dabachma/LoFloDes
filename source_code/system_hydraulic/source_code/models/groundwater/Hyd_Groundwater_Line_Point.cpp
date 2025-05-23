#include "source_code\Hyd_Headers_Precompiled.h"
//#include "Hyd_Groundwater_Line_Point.h"


//Default constructor
Hyd_Groundwater_Line_Point::Hyd_Groundwater_Line_Point(void) {

	this->abs_height = 0.0;
	this->base_height = 0.0;
	this->set_point_name(hyd_label::line_point);
	this->kf_fac = 0;
	this->flow_through_flag = true;
	this->distance2begin = -1.0;
	//this->point2gw = false;
	this->gw_sec_id = -1;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Groundwater_Line_Point), _sys_system_modules::HYD_SYS);
}
//copy constructor
Hyd_Groundwater_Line_Point::Hyd_Groundwater_Line_Point(const Hyd_Groundwater_Line_Point& object) {

	this->abs_height = object.abs_height;
	this->base_height = object.base_height;
	this->kf_fac = object.kf_fac;
	this->flow_through_flag = object.flow_through_flag;
	this->distance2begin = object.distance2begin;
	//this->point2gw = object.point2gw;
	this->system_id = object.system_id;
	this->gw_sec_id = object.gw_sec_id;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Groundwater_Line_Point), _sys_system_modules::HYD_SYS);

}
//Default destructor
Hyd_Groundwater_Line_Point::~Hyd_Groundwater_Line_Point(void) {
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Groundwater_Line_Point), _sys_system_modules::HYD_SYS);
}
//____________
//public
//Set the absolute height of the point (crest)
void Hyd_Groundwater_Line_Point::set_abs_height(const double h) {
	this->abs_height = h;
}
//Get the absolute height of the point (crest)
double Hyd_Groundwater_Line_Point::get_abs_height(void) {
	return this->abs_height;
}
//Set the absolute height of the point (base)
void Hyd_Groundwater_Line_Point::set_base_height(const double h) {
	this->base_height = h;
}
//Get the absolute base height of the point (base)
double Hyd_Groundwater_Line_Point::get_base_height(void) {
	return this->base_height;
}
//Set the kf factor of the point
void Hyd_Groundwater_Line_Point::set_kf_fac(const int pol) {
	this->kf_fac = pol;
}
//Get the kf factor of the point
int Hyd_Groundwater_Line_Point::get_kf_fac(void) {
	return this->kf_fac;
}
//Set the flow-through flag of the point
void Hyd_Groundwater_Line_Point::set_flow_through_flag(const bool flag) {
	this->flow_through_flag = flag;
}
//Get the flow-through flag of the point
bool Hyd_Groundwater_Line_Point::get_flow_through_flag(void) {
	return this->flow_through_flag;
}
//Check the line point
void Hyd_Groundwater_Line_Point::check_line_point(void) {
	//check kf
	/*if (this->kf_fac<0.35 || this->kf_fac>1.0) {
		Warning msg = this->set_warning(0);
		ostringstream info;
		info << "Given kf factor : " << this->kf_fac << label::kf_unit << endl;
		info << "New kf factor   : " << constant::kf_const << label::kf_unit << endl;
		info << "Point id            : " << this->number << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
		this->kf_fac = constant::kf_const;
	}*/
	//edited until here
	if (this->base_height > this->abs_height) {
		Error msg = this->set_error(2);
		ostringstream info;
		info << "Absolute base point height : " << this->base_height << label::m << endl;
		info << "Absolute crest height      : " << this->abs_height << label::m << endl;
		info << "Point id                   : " << this->number << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	if (this->distance2begin < 0.0) {
		Warning msg = this->set_warning(1);
		ostringstream info;
		info << "Distance to begin   : " << this->distance2begin << label::m << endl;
		info << "Point id            : " << this->number << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
		this->distance2begin = 0.0;
	}
}
//Set the distance to the beginning of a polysegment from the line point
void Hyd_Groundwater_Line_Point::set_distance2begin(const double distance) {
	this->distance2begin = distance;
}
//Get the distance to the beginning of a polysegment from the line point
double Hyd_Groundwater_Line_Point::get_distance2begin(void) {
	return this->distance2begin;
}
//Get the gw-section id (if existing )of the ine point
int Hyd_Groundwater_Line_Point::get_gw_section_id(void) {
	return this->gw_sec_id;
}
//Idealise the line points with given parameters of a gw-section
/*
void Hyd_Groundwater_Line_Point::idealise_points_gw(_hyd_idealisation_parameters ideal_param, const int section_id, const bool last_flag) {

	//ouput to the fpl-display
	ostringstream cout;
	cout << "Idealise dikeline point " << this->number << "..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	//check if it is already idealised
	if (this->point2fpl == true) {
		Error msg = this->set_error(3);
		ostringstream info;
		info << "Point index              : " << this->number << endl;
		info << "Station                  : " << this->distance2begin << label::m << endl;
		info << "FPL-section index        : " << ideal_param.index << endl;
		info << "FPL-section name         : " << ideal_param.name << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	if (this->break_flag == false) {
		this->break_flag = true;
	}
	if (this->overflow_flag == false) {
		this->overflow_flag = true;
	}

	//check the heights
	if ((abs(ideal_param.struct_height - (this->abs_height - this->base_height)) / ideal_param.struct_height) > 0.5 && this->system_id.area_state == 0
		&& this->system_id.measure_nr == 0) {
		Warning msg = this->set_warning(2);
		ostringstream info;
		info << "Point index              : " << this->number << endl;
		info << "Station                  : " << this->distance2begin << label::m << endl;
		info << "Relative height          : " << this->abs_height - this->base_height << label::m << endl;
		info << "FPL-section index        : " << ideal_param.index << endl;
		info << "FPL-section name         : " << ideal_param.name << endl;
		info << "Structure height         : " << ideal_param.struct_height << label::m << endl;
		msg.make_second_info(info.str());
		//warning to fpl!!
		msg.output_msg(1);
	}
	//set the ideal height
	this->abs_height = this->base_height + ideal_param.struct_height;

	//check the breach type
	if (this->break_param.abrupt_fails_flag != ideal_param.break_params.abrupt_fails_flag && this->system_id.area_state == 0
		&& this->system_id.measure_nr == 0) {
		Warning msg = this->set_warning(3);
		ostringstream info;
		info << "Point index              : " << this->number << endl;
		info << "Station                  : " << this->distance2begin << label::m << endl;
		info << "Abrupt breach profile    : " << functions::convert_boolean2string(this->break_param.abrupt_fails_flag) << endl;
		info << "FPL-section index        : " << ideal_param.index << endl;
		info << "FPL-section name         : " << ideal_param.name << endl;
		info << "Abrupt breach section    : " << functions::convert_boolean2string(ideal_param.break_params.abrupt_fails_flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	//set the parameters
	this->break_param = ideal_param.break_params;

	this->point2fpl = true;
	if (last_flag == false) {
		this->fpl_sec_id = section_id;
	}
}
*/
//Output the header for the setted member (static)
void Hyd_Groundwater_Line_Point::output_header_setted_member(ostringstream *cout) {
	*cout << W(15) << "Abs. height " << label::m << W(15) << "Base height " << label::m << W(15) << "Kf Factor " << label::kf_unit << W(15) << "Flow-through " << W(15) << "Break ";
	*cout << W(15) << "x" << label::m << W(17) << "y" << label::m;
	*cout << endl;
	Sys_Common_Output::output_hyd->output_txt(cout, true);
}
//Output the setted members
void Hyd_Groundwater_Line_Point::output_setted_members(ostringstream *cout) {



	this->flow_through_flag = true;

	*cout << W(15) << P(5) << this->abs_height << W(15) << this->base_height;
	*cout << W(15) << this->kf_fac;
	*cout << W(15) << functions::convert_boolean2string(this->flow_through_flag);
	*cout << W(21) << P(2) << FORMAT_FIXED_REAL << this->x_coordinate;
	*cout << W(21) << P(2) << FORMAT_FIXED_REAL << this->y_coordinate;
	*cout << endl;
	Sys_Common_Output::output_hyd->output_txt(cout, true);
}

//clone a floddplaine point; used for coupling 
//TODO see what should be done with other members
void Hyd_Groundwater_Line_Point::clone_fp_point(Hyd_Floodplain_Dikeline_Point point) {
	Geo_Point::operator =(point);
	this->abs_height = point.abs_height;
	this->base_height = point.base_height;
	this->distance2begin = point.distance2begin;
	this->system_id = point.system_id;
}
//copy operator
Hyd_Groundwater_Line_Point& Hyd_Groundwater_Line_Point::operator=(const Hyd_Groundwater_Line_Point& object) {
	Geo_Point::operator =(object);
	this->abs_height = object.abs_height;
	this->base_height = object.base_height;
	this->kf_fac = object.kf_fac;
	this->flow_through_flag = object.flow_through_flag;
	this->distance2begin = object.distance2begin;
	this->system_id = object.system_id;
	this->gw_sec_id = object.gw_sec_id;

	return *this;
}
//___________
//private
//Set warning(s)
Warning Hyd_Groundwater_Line_Point::set_warning(const int warn_type) {
	string place = "Hyd_Groundwater_Line_Point::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
	case 0://poleni
		place.append("check_line_point(void)");
		reason = "The kf factor is not in a physical interval (0.35-1.0)"; //REVIEW
		help = "Check the kf factor";
		reaction = "The factor is set to the default factor";
		type = 3;
		break;
	case 1://distance
		place.append("check_line_point(void)");
		reason = "The distance to the begin of the line is smaller than zero";
		help = "Check the distance calculation";
		reaction = "The distance is set to zero";
		type = 3;
		break;
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
//Set error(s)
Error Hyd_Groundwater_Line_Point::set_error(const int err_type) {
	string place = "Hyd_Groundwater_Line_Point::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;

	switch (err_type) {
	case 2://base height higher than crest height
		place.append("check_line_point(void)");
		reason = "The base height is above the absolute crest height";
		help = "Check the geometric parameters of the line point";
		type = 3;
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