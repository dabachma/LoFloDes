//#include "Hyd_Parse_GW.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
Hyd_Parse_GW::Hyd_Parse_GW(void) {
	this->found_groundwaterfile_name = false;
	this->found_no_elem_x = false;
	this->found_no_elem_y = false;
	this->found_elem_width_x = false;
	this->found_elem_width_y = false;
	this->found_angle = false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Parse_GW), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Parse_GW::~Hyd_Parse_GW(void) {
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Parse_GW), _sys_system_modules::HYD_SYS);
}
//_________________
//public
//Parse for the global groundwatermodel keywords in the groundwatermodel parameter file and read in the parameters for the groundwater model with the given index. Look in the ilm-file
void Hyd_Parse_GW::parse_groundwatermodel_params(const int gw_index) {
	_hyd_keyword_file Key;
	word Command;
	bool found_flag = false;
	bool model_read_finished_flag = false;

	//find the begin of groundwater model with the given index
	while (this->GetLine(Command)) {
		//look for a keyword
		Key = this->ParseNextKeyword(Command);
		stringstream buffer;
		buffer << Command;
		//if the keyword is found, read the first line
		if (Key == eBEGINGWMODEL) {
			int gw_number = -1;
			buffer >> gw_number;
			if (buffer.fail() == true) {
				ostringstream info;
				info << "Wrong input sequenze " << buffer.str() << endl;			//gw_number needs to be first always, therefore we check for it here
				Error msg = this->set_error(10);
				msg.make_second_info(info.str());
				throw msg;
			}
			//check if the index and the model number is the same
			if (gw_number == gw_index) {
				gw_params.GWNumber = gw_number;
				found_flag = true;
				// There could be some spaces in the model text
				char buffer[512];
				RemoveDelimiters(Command, buffer);
				gw_params.GWName = buffer;
				break;
			}
		}
		else if (Key == eENDDESCRIPTION) {
			break;
		}
	}
	if (found_flag == false) {
		Error msg = this->set_error(1);
		ostringstream info;
		info << "Given groundwaternumber: " << gw_index << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	//search for the specific input 
	try {
		while (this->GetLine(Command) && model_read_finished_flag == false) {
			Key = ParseNextKeyword(Command);
			//check the keywords, order is not relevant
			switch (Key) {
			case eGENERAL:
				this->parse_general(Key, Command);
				break;
			case eGROUNDWATERFILE:
				this->parse_groundwater_filename(Key, Command);
				break;
			case eINSTATBOUNDFILE:
				this->parse_instat_boundary(Key, Command);
				break;
			case eNOFLOWPOLYFILE:
				this->parse_no_flow(Key, Command);
				break;
			case eLINEFILE:
				this->parse_lines(Key, Command);
			case eLIMITS:
				this->parse_calculation_limits(Key, Command);
				break;
			case e2DOUTPUT:
				this->parse_2d_result_file(Key, Command);
				break;
			case eENDGWMODEL:
				model_read_finished_flag = true;
				break;
			default:
				break;
			}
		}
		//check if the last keyword ENDGWMODEL was found
		if (model_read_finished_flag == false) {
			Error msg = this->set_error(2);
			throw msg;
		}

		//check if all necessary parameters were found
		this->check_parameters_found();

	}
	catch (Error msg) {
		stringstream info;
		info << "Groundwaternumber: " << gw_index << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
}
//get the parsed groundwater parameter
Hyd_Param_GW Hyd_Parse_GW::get_groundwatermodel_params(void) {
	return this->gw_params;
}
//_______________
//private
//Parse for general settings (mostly geometry)
void Hyd_Parse_GW::parse_general(_hyd_keyword_file Key, word Command) {
	// Next keyword must be SET
	do {
		if (Key == eFAIL) {
			if (!this->GetLine(Command)) {
				Error msg = this->set_error(3);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	} while (Key != eSET);

	// Get general model information ...
	do {
		if (Key == eFAIL) {
			if (!this->GetLine(Command)) {
				Error msg = this->set_error(3);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
		stringstream buffer;
		buffer << Command;
		if (Key == eNX) {
			buffer >> this->gw_params.GWNofX;
			this->found_no_elem_x = true;
		}
		else if (Key == eNY) {
			buffer >> this->gw_params.GWNofY;
			this->found_no_elem_y = true;
		}
		else if (Key == eELEMENTWIDTH_X) {
			buffer >> this->gw_params.width_x;
			this->found_elem_width_x = true;
		}
		else if (Key == eELEMENTWIDTH_Y) {
			buffer >> this->gw_params.width_y;
			this->found_elem_width_y = true;
		}
		else if (Key == eANGLE) {
			buffer >> this->gw_params.angle;
			this->found_angle = true;
		}
		else if (Key == eLOWLEFTX) {
			buffer >> this->gw_params.GWLowLeftX;
		}
		else if (Key == eLOWLEFTY) {
			buffer >> this->gw_params.GWLowLeftY;
		}
		else if (Key == eNOINFOVALUE) {
			buffer >> this->gw_params.noinfo_value;
		}
		if (buffer.fail() == true) {
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			Error msg = this->set_error(11);
			msg.make_second_info(info.str());
			throw msg;
		}

	} while (Key != eSET); // End of specifications are marked by </Set>
}
//Parse for the name of the groundwater file
void Hyd_Parse_GW::parse_groundwater_filename(_hyd_keyword_file Key, word Command) {
	// There is not a following keyword.
	if (strlen(Command) <= 0) {
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	if (Key == eFAIL) {
		char buffer[512];
		RemoveDelimiters(Command, buffer);
		UseLinuxSlash(buffer);
		gw_params.groundwater_file = buffer;
		this->found_groundwaterfile_name = true;
	}
}
//Parse for the instationary_boundary_file name and number of curves
void Hyd_Parse_GW::parse_instat_boundary(_hyd_keyword_file Key, word Command) {
	int must_found = 0;
	// Next keyword must be SET
	do {
		if (Key == eFAIL) {
			if (!this->GetLine(Command)) {
				Error msg = this->set_error(0);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	} while (Key != eSET);

	do {
		if (Key == eFAIL) {
			if (!this->GetLine(Command)) {
				Error msg = this->set_error(0);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
		stringstream buffer;
		buffer << Command;
		if (Key == eFILENAME) {
			char buffer[512];
			RemoveDelimiters(Command, buffer);
			UseLinuxSlash(buffer);
			this->gw_params.inst_boundary_file = buffer;
			must_found++;

		}
		else if (Key == eNUMBER_CURVE) {
			buffer >> this->gw_params.number_instat_boundary;
			must_found++;
		}
		if (buffer.fail() == true) {
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			Error msg = this->set_error(12);
			msg.make_second_info(info.str());
			throw msg;
		}

	} while (Key != eSET); // End of specifications are marked by </Set>

	if (must_found != 2) {
		ostringstream info;
		info << "Not all needed infos are found " << endl;
		Error msg = this->set_error(12);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Parse for the no flow cell polygon file value
void Hyd_Parse_GW::parse_no_flow(_hyd_keyword_file Key, word Command) {
	int must_found = 0;
	// Next keyword must be SET
	do {
		if (Key == eFAIL) {
			if (!this->GetLine(Command)) {
				Error msg = this->set_error(0);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	} while (Key != eSET);

	do {
		if (Key == eFAIL) {
			if (!this->GetLine(Command)) {
				Error msg = this->set_error(0);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
		stringstream buffer;
		buffer << Command;
		if (Key == eFILENAME) {
			char buffer[512];
			RemoveDelimiters(Command, buffer);
			UseLinuxSlash(buffer);
			this->gw_params.noflow_file = buffer;
			must_found++;

		}
		else if (Key == eNO_NOFLOWPOLYS) {
			buffer >> this->gw_params.number_noflow_polys;
			must_found++;
		}
		if (buffer.fail() == true) {
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			Error msg = this->set_error(13);
			msg.make_second_info(info.str());
			throw msg;
		}

	} while (Key != eSET); // End of specifications are marked by </Set>

	if (must_found != 2) {
		ostringstream info;
		info << "Not all needed infos are found " << endl;
		Error msg = this->set_error(13);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Parse for the barrier line file value
void Hyd_Parse_GW::parse_lines(_hyd_keyword_file Key, word Command) {
	int must_found = 0;
	// Next keyword must be SET
	do {
		if (Key == eFAIL) {
			if (!this->GetLine(Command)) {
				Error msg = this->set_error(0);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	} while (Key != eSET);

	do {
		if (Key == eFAIL) {
			if (!this->GetLine(Command)) {
				Error msg = this->set_error(0);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
		stringstream buffer;
		buffer << Command;
		if (Key == eFILENAME) {
			char buffer[512];
			RemoveDelimiters(Command, buffer);
			UseLinuxSlash(buffer);
			this->gw_params.line_file = buffer;
			must_found++;

		}
		else if (Key == eNODIKEPOLYLINES) {
			buffer >> this->gw_params.number_polylines;
			must_found++;
		}
		if (buffer.fail() == true) {
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			Error msg = this->set_error(13);
			msg.make_second_info(info.str());
			throw msg;
		}

	} while (Key != eSET); // End of specifications are marked by </Set>

	if (must_found != 2) {
		ostringstream info;
		info << "Not all needed infos are found " << endl;
		Error msg = this->set_error(13);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Parse for the calculation limits
void Hyd_Parse_GW::parse_calculation_limits(_hyd_keyword_file Key, word Command) {
	// Next keyword is STANDARD or SET
	do {
		if (Key == eFAIL) {
			if (!this->GetLine(Command)) {
				Error msg = this->set_error(4);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	} while ((Key != eSTANDARD) && (Key != eSET));

	if (Key == eSTANDARD) {
		return;
	}
	else if (Key == eSET) {
		do {
			if (Key == eFAIL) {
				if (!this->GetLine(Command)) {
					Error msg = this->set_error(4);
					throw msg;
				}
			}
			Key = ParseNextKeyword(Command);
			stringstream buffer;
			buffer << Command;
			if (Key == eATOL) {
				buffer >> this->gw_params.abs_tolerance;
			}
			else if (Key == eRTOL) {
				buffer >> this->gw_params.rel_tolerance;
			}
			if (buffer.fail() == true) {
				ostringstream info;
				info << "Wrong input sequenze " << buffer.str() << endl;
				Error msg = this->set_error(14);
				msg.make_second_info(info.str());
				throw msg;
			}

		} while (Key != eSET); // End of Init specifications are marked by </SET>
	}
}
//Parse for the 2d_result_file name for tecplot
void Hyd_Parse_GW::parse_2d_result_file(_hyd_keyword_file Key, word Command) {
	// There must not be a following keyword.
	if (strlen(Command) <= 0) {
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	if (Key == eFAIL) {
		char buffer[512];
		RemoveDelimiters(Command, buffer);
		UseLinuxSlash(buffer);
		string buff2 = buffer;
		_Hyd_Parse_IO::erase_leading_whitespace_tabs(&buff2);
		_Hyd_Parse_IO::erase_end_whitespace_tabs(&buff2);
		ostringstream info;
		info << "See  !2DOUTPUT keyword:" << buffer << " in the GW-section" << endl;
		Warning msg = this->set_warning(0);
		msg.make_second_info(info.str());
		msg.output_msg(2);




	}
}
//Check if the requiered paramaters were found
void Hyd_Parse_GW::check_parameters_found(void) {
	if (this->found_groundwaterfile_name == false) {
		Error msg = this->set_error(7);
		throw msg;
	}
	if (this->found_no_elem_x == false) {
		Error msg = this->set_error(8);
		throw msg;
	}
	if (this->found_no_elem_y == false) {
		Error msg = this->set_error(9);
		throw msg;
	}
	if (this->found_elem_width_x == false) {
		Error msg = this->set_error(15);
		throw msg;
	}
	if (this->found_elem_width_y == false) {
		Error msg = this->set_error(16);
		throw msg;
	}
	if (this->found_angle == false) {
		Error msg = this->set_error(6);
		throw msg;
	}

}
//Set the warning
Warning Hyd_Parse_GW::set_warning(const int warn_type) {
	string place = "Hyd_Parse_GW::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
	case 0://absolute solver tolerance
		place.append("parse_2d_result_file(_hyd_keyword_file Key, word Command)");
		reason = "Keyword is not more supported; the output-settings are done now in the global parameter section";
		reaction = "No reaction";
		help = "Just erase the following line in the .ilm-file";
		type = 1;
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
Error Hyd_Parse_GW::set_error(const int err_type) {
	string place = "Hyd_Parse_GW::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;

	switch (err_type) {
	case 0://problem with instationary boundary settings
		place.append("parse_instat_boundary(_hyd_keyword_file Key, word Command)");
		reason = "Can not read in properly the instationary boundary block";
		help = "Check the instationary boundary block in the inputfile";
		info << "Filename: " << this->input_file_name << endl;
		info << "Error occurs near line: " << this->line_counter << endl;
		type = 1;
		break;
	case 1://no groundwater model is found to the given index
		place.append("parse_groundwatermodel_params(const int gw_index)");
		reason = "There are no data corresponding to the given number of groundwatermodel";
		help = "The groundwatermodel numbers have to be sequentially; starting by 0; check the file";
		info << "Filename: " << this->input_file_name << endl;
		type = 1;
		break;
	case 2://no end is found
		place.append("parse_groundwatermodel_params(const int gw_index)");
		reason = "Do not find the end of the description of the groundwatermodel parameters; it has to end with !$ENDFPMODEL";
		help = "Check the inputfile";
		info << "Filename: " << this->input_file_name << endl;
		type = 1;
		break;
	case 3://problems with the general settings
		place.append("parse_general(_hyd_keyword_file Key, word Command)");
		reason = "Can not read in properly the general setting block";
		help = "Check the general setting of the groundwatermodel in the inputfile";
		info << "Filename: " << this->input_file_name << endl;
		info << "Error occurs near line: " << this->line_counter << endl;
		type = 1;
		break;
	case 4://problems with the calculation limits settings
		place.append("parse_calculation_limits(_hyd_keyword_file Key, word Command)");
		reason = "Can not read in properly the calculation limits setting block";
		help = "Check the calculation limits setting of the groundwatermodel in the inputfile";
		info << "Filename: " << this->input_file_name << endl;
		info << "Error occurs near line: " << this->line_counter << endl;
		type = 1;
		break;
	case 5://problems with the calculation limits settings
		place.append("parse_no_flow_value(_hyd_keyword_file Key, word Command)");
		reason = "Can not read in properly the noflow setting block";
		help = "Check the noflow setting of the groundwatermodel in the inputfile";
		info << "Filename: " << this->input_file_name << endl;
		info << "Error occurs near line: " << this->line_counter << endl;
		type = 1;
		break;
	case 6://no angle is found
		place.append("check_parameters_found(void)");
		reason = "Did not find the angle to normal x-direction (--> x-normal; positive angle=> clockwise)";
		help = "Check the element angle in the groundwatermodel description";
		info << "Filename: " << this->input_file_name << endl;
		type = 1;
		break;
	case 7://groundwaterfile name is not found
		place.append("check_parameters_found(void)");
		reason = "Did not find the floodplain filename";
		help = "Check the groundwater filename in the groundwatermodel description";
		info << "Filename: " << this->input_file_name << endl;
		type = 1;
		break;
	case 8://number of elements in x-direction is not found
		place.append("check_parameters_found(void)");
		reason = "Did not find the number of elements in x-direction";
		help = "Check the number of elements in x-direction in the groundwatermodel description";
		info << "Filename: " << this->input_file_name << endl;
		type = 1;
		break;
	case 9://number of elements in y-direction is not found
		place.append("check_parameters_found(void)");
		reason = "Did not find the number of elements in y-direction";
		help = "Check the number of elements in y-direction in the groundwatermodel description";
		info << "Filename: " << this->input_file_name << endl;
		type = 1;
		break;
	case 10://wrong input
		place.append("parse_groundwatermodel_params(const int gw_index)");
		reason = "There is a problem with the input; wrong sign is read in";
		help = "Check the groundwatermodel number in the global groundwatermodel parameters in file";
		info << "Filename: " << this->input_file_name << endl;
		info << "Error occurs near line: " << this->line_counter << endl;
		type = 1;
		break;
	case 11://wrong input
		place.append("parse_general(_hyd_keyword_file Key, word Command)");
		reason = "There is a problem with the input; wrong sign is read in";
		help = "Check the general settings in the global groundwatermodel parameters in file";
		info << "Filename: " << this->input_file_name << endl;
		info << "Error occurs near line: " << this->line_counter << endl;
		type = 1;
		break;
	case 12://wrong input
		place.append("parse_instat_boundary(_hyd_keyword_file Key, word Command)");
		reason = "There is a problem with the input";
		help = "Check the number of instationary boundary curves in the global groundwatermodel parameters in file";
		info << "Filename: " << this->input_file_name << endl;
		info << "Error occurs near line: " << this->line_counter << endl;
		type = 1;
		break;
	case 13://wrong input
		place.append("parse_no_flow(_hyd_keyword_file Key, word Command)");
		reason = "There is a problem with the input";
		help = "Check the noflow-polygon settings in the global groundwatermodel parameters in file";
		info << "Filename: " << this->input_file_name << endl;
		info << "Error occurs near line: " << this->line_counter << endl;
		type = 1;
		break;
	case 14://wrong input
		place.append("parse_calculation_limits(_hyd_keyword_file Key, word Command)");
		reason = "There is a problem with the input; wrong sign is read in";
		help = "Check the calculation-limits settings in the global groundwatermodel parameters in file";
		info << "Filename: " << this->input_file_name << endl;
		info << "Error occurs near line: " << this->line_counter << endl;
		type = 1;
		break;
	case 15://element width in x is not found
		place.append("check_parameters_found(void)");
		reason = "Did not find the element width in x-direction";
		help = "Check the element width in x-direction in the groundwatermodel description";
		info << "Filename: " << this->input_file_name << endl;
		type = 1;
		break;
	case 16://element width in y is not found
		place.append("check_parameters_found(void)");
		reason = "Did not find the element width in y-direction";
		help = "Check the element width in y-direction in the groundwatermodel description";
		info << "Filename: " << this->input_file_name << endl;
		type = 1;
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