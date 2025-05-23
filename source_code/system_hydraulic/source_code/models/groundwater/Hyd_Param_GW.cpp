#include "Hyd_Param_GW.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
Hyd_Param_GW::Hyd_Param_GW(void) :default_rel_tol(1.0e-8), default_abs_tol(1.0e-5) {

	this->GWName = label::not_set;
	this->GWNumber = -1;
	this->GWNofX = 0;
	this->GWNofY = 0;
	this->width_x = 0.0;
	this->width_y = 0.0;
	this->area = 0.0;
	this->angle = 0.0;
	this->GWLowLeftX = 0.0;
	this->GWLowLeftY = 0.0;
	this->groundwater_file = label::not_set;

	this->number_instat_boundary = 0;
	this->inst_boundary_file = label::not_set;

	this->number_polylines = 0;
	this->line_file = label::not_set;

	this->noinfo_value = -9999.0;

	this->number_noflow_polys = 0;
	this->noflow_file = label::not_set;

	this->tecplot_outfile_name = label::not_set;

	this->abs_tolerance = this->default_abs_tol;
	this->rel_tolerance = this->default_rel_tol;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Param_GW), _sys_system_modules::HYD_SYS);
}
//copy constructor
Hyd_Param_GW::Hyd_Param_GW(const Hyd_Param_GW& par) :default_rel_tol(1.0e-8), default_abs_tol(1.0e-5) {
	this->GWName = par.GWName;
	this->GWNumber = par.GWNumber;
	this->GWNofX = par.GWNofX;
	this->GWNofY = par.GWNofY;
	this->width_x = par.width_x;
	this->width_y = par.width_y;
	this->area = par.area;
	this->angle = par.angle;
	this->GWLowLeftX = par.GWLowLeftX;
	this->GWLowLeftY = par.GWLowLeftY;
	this->groundwater_file = par.groundwater_file;

	this->number_instat_boundary = par.number_instat_boundary;
	this->inst_boundary_file = par.inst_boundary_file;

	this->number_polylines = par.number_polylines;
	this->line_file = par.line_file;

	this->noinfo_value = par.noinfo_value;

	this->number_noflow_polys = par.number_noflow_polys;
	this->noflow_file = par.noflow_file;

	this->tecplot_outfile_name = par.tecplot_outfile_name;

	this->abs_tolerance = par.abs_tolerance;
	this->rel_tolerance = par.rel_tolerance;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Param_GW), _sys_system_modules::HYD_SYS);


}
//destructor
Hyd_Param_GW::~Hyd_Param_GW(void) {
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Param_GW), _sys_system_modules::HYD_SYS);
}
//__________________
//public
//Get the groundwater number
int Hyd_Param_GW::get_groundwater_number(void) {
	return this->GWNumber;
}
//Get the groundwater name
string Hyd_Param_GW::get_groundwater_name(void) {
	return this->GWName;
}
//Set the floodplain model parameters per parser from file 
void Hyd_Param_GW::parameter_per_parser(const string file, const int index, const string path) {
	Hyd_Parse_GW gw_parser;
	ostringstream cout;
	cout << "Read the global GW parameters from file " << file << "..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//set the file name 
	gw_parser.set_input_filename(file);


	try {
		//open file
		gw_parser.open_input_file();
		//read in the parameters
		gw_parser.parse_groundwatermodel_params(index);
	}
	catch (Error msg) {
		gw_parser.close_input_file();
		throw msg;
	}

	//close the file
	gw_parser.close_input_file();

	*this = gw_parser.get_groundwatermodel_params();
	this->calculate_area();

	this->complete_filenames_with_path(path);
}
//check the members
void Hyd_Param_GW::check_members(void) {
	//errors
	if (this->width_x <= 0.0) {
		Error msg = this->set_error(0);
		throw msg;
	}
	if (this->width_y <= 0.0) {
		Error msg = this->set_error(1);
		throw msg;
	}
	//relation of the width
	if (this->width_x > this->width_y) {
		double rel = this->width_y / this->width_x;
		if (rel <= 0.25) {
			Warning msg = this->set_warning(3);
			ostringstream info;
			info << "Relation y/x -width" << rel << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}
	if (this->width_y > this->width_x) {
		double rel = this->width_x / this->width_y;
		if (rel <= 0.25) {
			Warning msg = this->set_warning(3);
			ostringstream info;
			info << "Relation x/y -width" << rel << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}
	if (this->GWNofX <= 0) {
		Error msg = this->set_error(2);
		throw msg;
	}

	if (this->GWNofY <= 0) {
		Error msg = this->set_error(3);
		throw msg;
	}


	//warnings
	if (this->abs_tolerance >= 1.0e-2 || this->abs_tolerance < 1.0e-6) {
		Warning msg = this->set_warning(0);
		stringstream info;
		info << "Default value: " << this->default_abs_tol << endl;
		msg.make_second_info(info.str());
		//reaction
		this->abs_tolerance = this->default_abs_tol;
		msg.output_msg(2);

	}
	if (this->rel_tolerance >= 1.0e-2 || this->rel_tolerance < 1.0e-10) {
		Warning msg = this->set_warning(1);
		stringstream info;
		info << "Default value: " << this->default_rel_tol << endl;
		msg.make_second_info(info.str());
		//reaction
		this->rel_tolerance = this->default_rel_tol;
		msg.output_msg(2);
	}
}
//Get number of elements in x-direction
int Hyd_Param_GW::get_no_elems_x(void) {
	return this->GWNofX;
}
///Get number of elements in y-direction
int Hyd_Param_GW::get_no_elems_y(void) {
	return this->GWNofY;
}
///Get the geometrical information of the groundwater
_hyd_groundwater_geo_info Hyd_Param_GW::get_geometrical_info(void) {
	_hyd_groundwater_geo_info buffer;
	buffer.angle = this->angle;
	buffer.number_x = this->GWNofX;
	buffer.number_y = this->GWNofY;
	buffer.origin_global_x = this->GWLowLeftX;
	buffer.origin_global_y = this->GWLowLeftY;
	buffer.width_x = this->width_x;
	buffer.width_y = this->width_y;
	return buffer;
}
//Get a pointer to the absolute tolerance for the solver
double* Hyd_Param_GW::get_absolute_solver_tolerance(void) {
	return &this->abs_tolerance;
}
//Get the relative tolerance for the solver
double Hyd_Param_GW::get_relative_solver_tolerance(void) {
	return this->rel_tolerance;
}
//Output the members of the groundwater model
void Hyd_Param_GW::output_members(void) {
	//set prefix for output REVIEW
	ostringstream prefix;
	prefix << "GEN> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	//output
	ostringstream cout;
	// Put out specific information for the model
	cout << "GENERAL   " << endl;
	cout << " Number                       : " << W(7) << this->GWNumber << endl;
	cout << " Name                         : " << W(7) << this->GWName << endl;
	cout << " Approximate Workspace        : " << W(7) << functions::convert_byte2string(this->calculate_approx_workspace());
	cout << "ELEMENTS   " << endl;
	cout << " Number elements x            : " << W(7) << this->GWNofX << endl;
	cout << " Number elements y            : " << W(7) << this->GWNofY << endl;
	cout << " Elementwidth x               : " << W(7) << P(2) << FORMAT_FIXED_REAL << this->width_x << label::m << endl;
	cout << " Elementwidth y               : " << W(7) << P(2) << FORMAT_FIXED_REAL << this->width_y << label::m << endl;
	cout << " Angle to normal x-direction  : " << W(7) << P(2) << FORMAT_FIXED_REAL << this->angle << label::degree << endl;
	cout << " LowLeftx                     : " << W(7) << P(2) << FORMAT_FIXED_REAL << this->GWLowLeftX << label::m << endl;
	cout << " LowLefty                     : " << W(7) << P(2) << FORMAT_FIXED_REAL << this->GWLowLeftY << label::m << endl;
	cout << " Groundwater file              : " << W(7) << Hyd_Param_Global::get_print_filename(37, this->groundwater_file.c_str()) << endl;
	cout << " Noinfo value                 : " << W(7) << P(2) << FORMAT_FIXED_REAL << this->noinfo_value << endl;
	if (this->number_instat_boundary > 0) {
		cout << "INSTAT-BOUNDARYCONDITIONS  " << endl;
		cout << " Instationary Boundaryfile    : " << W(7) << Hyd_Param_Global::get_print_filename(37, this->inst_boundary_file.c_str()) << endl;
		cout << " Number of curves             : " << W(7) << this->number_instat_boundary << endl;
	}
	if (this->number_noflow_polys > 0) {
		cout << "NOFLOW-POLYGONS  " << endl;
		cout << " Noflow-polygons file         : " << W(7) << Hyd_Param_Global::get_print_filename(37, this->noflow_file.c_str()) << endl;
		cout << " Number of polygons           : " << W(7) << this->number_noflow_polys << endl;
	}
	if (this->number_polylines > 0) {
		cout << "POLYLINES  " << endl;
		cout << " Polylines file          : " << W(7) << Hyd_Param_Global::get_print_filename(37, this->line_file.c_str()) << endl;
		cout << " Number of polylines          : " << W(7) << this->number_polylines << endl;
	}
	cout << "SOLVER/CALCULATION" << endl;
	cout << " Absolute solver tolerance    : " << W(7) << P(2) << FORMAT_SCIENTIFIC_REAL << this->abs_tolerance << endl;
	cout << " Relative solver tolerance    : " << W(7) << P(2) << FORMAT_SCIENTIFIC_REAL << this->rel_tolerance << endl;
	if (this->tecplot_outfile_name != label::not_set) {
		cout << "2D-RESULTFILE     " << endl;
		cout << " 2d-geometry in folder          : " << W(7) << Hyd_Param_Global::get_print_filename(60, this->tecplot_outfile_name.c_str()) << endl;
		cout << " 2d-results in folder           : " << W(7) << Hyd_Param_Global::get_print_filename(60, this->tecplot_outfile_name.c_str()) << endl;
		cout << " 2d-maximum results in folder   : " << W(7) << Hyd_Param_Global::get_print_filename(60, this->tecplot_outfile_name.c_str()) << endl;
	}

	Sys_Common_Output::output_hyd->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Get the filename for the groundwater geometry file
string Hyd_Param_GW::get_filename_geometrie2file(const string type) {
	string buffer;
	buffer = this->tecplot_outfile_name;

	if (buffer != label::not_set) {
		stringstream suffix;
		suffix << "GW_GEO_" << this->GWNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;

}
//Get the filename for the floodplain geometrie file without suffix
string Hyd_Param_GW::get_filename_geometrie(const string type) {
	string buffer;
	buffer = this->tecplot_outfile_name;

	if (buffer != label::not_set) {
		stringstream suffix;
		suffix << "GW_GEO_" << this->GWNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;

}
//Get the filename for the groundwater result file 
string Hyd_Param_GW::get_filename_result2file(const string type, const double timepoint) {
	string buffer;
	buffer = this->tecplot_outfile_name;

	if (buffer != label::not_set) {
		stringstream suffix;
		suffix << "GW_RES_" << this->GWNumber;
		if (timepoint >= 0.0) {
			
			suffix << "_" << timepoint;
		}
		else {
			suffix << "_time";
		}
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;

}
//Get the filename for the groundwater result file without suffix
string Hyd_Param_GW::get_filename_result(const string type) {
	string buffer;
	buffer = this->tecplot_outfile_name;

	if (buffer != label::not_set) {
		stringstream suffix;
		suffix << "GW_RES_" << this->GWNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;

}
//Get the filename for the maximum groundwater result file 
string Hyd_Param_GW::get_filename_result_max2file(const string type) {
	string buffer;
	buffer = this->tecplot_outfile_name;

	if (buffer != label::not_set) {
		stringstream suffix;
		suffix << "GW_RESMAX_" << this->GWNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Get the filename for the groundwater observation points to file 
string Hyd_Param_GW::get_filename_obs_point2file(const string type) {
	string buffer;
	buffer = this->tecplot_outfile_name;

	if (buffer != label::not_set) {
		stringstream suffix;
		suffix << "GW_OBS_POINT";
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Calculate the approximate workspace requirement
long long int Hyd_Param_GW::calculate_approx_workspace(void) {
	long long int mem = 0.0;
	mem = sizeof(Hyd_Model_Groundwater) + this->GWNofX*this->GWNofY*(sizeof(Hyd_Element_Groundwater) + sizeof(Hyd_Element_Groundwater_Type_Standard));
	//solver
	long long int number_int = 50 + (constant::krylow_subspace + 5) + this->GWNofX*this->GWNofY;
	long long int number_real = this->GWNofX*this->GWNofY*(constant::krylow_subspace + 5) + (constant::krylow_subspace + 5)*(constant::krylow_subspace + 4) +
		this->GWNofX*this->GWNofY*(10 + 20 + 5);
	mem = mem + sizeof(int)*number_int + sizeof(realtype)*number_real + this->GWNofX*this->GWNofY * sizeof(Hyd_Element_Groundwater_Type_Standard);
	return mem;
}
//Get pointer to the element width in x-direction
double *Hyd_Param_GW::get_ptr_width_x(void) {
	return &this->width_x;
}
//Get pointer to the element width in y-direction
double *Hyd_Param_GW::get_ptr_width_y(void) {
	return &this->width_y;
}
//Get pointer to the element area
double *Hyd_Param_GW::get_ptr_elem_area(void) {
	return &this->area;
}
//copy operator
Hyd_Param_GW& Hyd_Param_GW::operator= (const Hyd_Param_GW& par) {

	this->GWName = par.GWName;
	this->GWNumber = par.GWNumber;
	this->GWNofX = par.GWNofX;
	this->GWNofY = par.GWNofY;
	this->width_x = par.width_x;
	this->width_y = par.width_y;
	this->area = par.area;
	this->angle = par.angle;
	this->GWLowLeftX = par.GWLowLeftX;
	this->GWLowLeftY = par.GWLowLeftY;
	this->groundwater_file = par.groundwater_file;

	this->number_instat_boundary = par.number_instat_boundary;
	this->inst_boundary_file = par.inst_boundary_file;

	this->noinfo_value = par.noinfo_value;

	this->number_noflow_polys = par.number_noflow_polys;
	this->noflow_file = par.noflow_file;

	this->number_polylines = par.number_polylines;
	this->line_file = par.line_file;

	this->tecplot_outfile_name = par.tecplot_outfile_name;

	this->abs_tolerance = par.abs_tolerance;
	this->rel_tolerance = par.rel_tolerance;


	return *this;
}
//__________________
//private
//Complete the filenames with the path
void Hyd_Param_GW::complete_filenames_with_path(string global_path) {
	ostringstream buff;
	buff << global_path << this->groundwater_file;
	this->groundwater_file = _Hyd_Parse_IO::insert_linux_slash2string(buff.str());
	buff.str("");
	buff.clear();

	if (this->tecplot_outfile_name != label::not_set) {
		buff << global_path << this->tecplot_outfile_name;
		this->tecplot_outfile_name = _Hyd_Parse_IO::insert_linux_slash2string(buff.str());
		buff.str("");
		buff.clear();

	}
	if (this->inst_boundary_file != label::not_set) {
		buff << global_path << this->inst_boundary_file;
		this->inst_boundary_file = buff.str();
		buff.str("");
		buff.clear();

	}
	if (this->noflow_file != label::not_set) {
		buff << global_path << this->noflow_file;
		this->noflow_file = _Hyd_Parse_IO::insert_linux_slash2string(buff.str());
		buff.str("");
		buff.clear();

	}
	if (this->line_file != label::not_set) {
		buff << global_path << this->line_file;
		this->line_file = _Hyd_Parse_IO::insert_linux_slash2string(buff.str());
		buff.str("");
		buff.clear();

	}

}
//Calculate the element area
void Hyd_Param_GW::calculate_area(void) {
	this->area = this->width_x*this->width_y;
}
//Set the warning
Warning Hyd_Param_GW::set_warning(const int warn_type) {
	string place = "Hyd_Param_GW::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
	case 0://absolute solver tolerance
		place.append("check_members(void)");
		reason = "The absolute solver tolerance have to be: 1e-6<= abs_tol <=1e-2";
		reaction = "The default value is taken";
		help = "Check the given absolute solver tolerance";
		info << "Groundwater Model number : " << this->GWNumber << endl;
		type = 3;
		break;
	case 1://relative solver tolerance
		place.append("check_members(void)");
		reason = "The relative solver tolerance have to be: 1e-10<= abs_tol <=1e-2";
		reaction = "The default value is taken";
		help = "Check the given relative solver tolerance";
		info << "Groundwater Model number : " << this->GWNumber << endl;
		type = 3;
		break;
	case 2://boundary for taken ds_dt into account
		place.append("check_members(void)");
		reason = "The boundary for which ds to dt (wet flag) is taken into account can not be <=0";
		reaction = "The default value is used";
		help = "Check the given ds to dt boundary";
		info << "Groundwater Model number : " << this->GWNumber << endl;
		type = 3;
		break;
	case 3://relation between y- and x-width
		place.append("check_members(void)");
		reason = "The relation between the element widths x/y are <0.25; there can be numerical problems";
		reaction = "No reaction";
		help = "Check the given x- and y-width";
		info << "Groundwater Model number : " << this->GWNumber << endl;
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
}
//set the error
Error Hyd_Param_GW::set_error(const int err_type) {
	string place = "Hyd_Param_Global::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;

	switch (err_type) {
	case 0://x-width is <=0.0
		place.append("check_members(void)");
		reason = "Element width in x-direction can not be <=0.0";
		help = "Check the groundwater parameters";
		type = 3;
		break;
	case 1://y-width <=0.0
		place.append("check_members(void)");
		reason = "Element width in y-direction can not be <=0.0";
		help = "Check the groundwater parameters";
		type = 3;
		break;
	case 2://number of x-elements is <=0
		place.append("check_members(void)");
		reason = "Number of elements in x-direction can not be <=0";
		help = "Check the number of internal timesteps";
		type = 3;
		break;
	case 3://number of x-elements is <=0
		place.append("check_members(void)");
		reason = "Number of elements in y-direction can not be <=0";
		help = "Check the number of internal timesteps";
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