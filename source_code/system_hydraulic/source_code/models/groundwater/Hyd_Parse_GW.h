#pragma once
/**\class Hyd_Parse_GW
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_PARSE_GW_H
#define HYD_PARSE_GW_H


//hyd_system class
#include "_Hyd_Parse_IO.h"
#include "Hyd_Param_GW.h"
///Class for parsing the general input parameter in a file for a groundwater model \ingroup hyd
/**
The general parameter are stored in Hyd_Param_GW.

\see Hyd_Model_Groundwater, Hyd_Param_GW
*/
class Hyd_Parse_GW : public _Hyd_Parse_IO
{
public:
	///Default constructor
	Hyd_Parse_GW(void);
	///Default destructor
	~Hyd_Parse_GW(void);

	//members

	//method
	///Parse for the global groundwater model keywords in the groundwater model parameter file and read in the parameters for the groundwater model with the given index
	void parse_groundwatermodel_params(const int gw_index);
	///Get the globals groundwater model parameters, which are read in per file parser
	Hyd_Param_GW get_groundwatermodel_params(void);


private:
	//members
	///Container for all global groundwatermodel parameters; it is filled per fileparser
	Hyd_Param_GW gw_params;

	///Foundflag for the groundwater file name
	bool found_groundwaterfile_name;
	///Foundflag for number of elements in x-direction
	bool found_no_elem_x;
	///Foundflag for number of elements in y-direction
	bool found_no_elem_y;
	///Foundflag for the element-width in x-direction
	bool found_elem_width_x;
	///Foundflag for the element-width in y-direction
	bool found_elem_width_y;
	///Foundflag for the angle to the normal x-direction
	bool found_angle;

	//methods
	///Parse for general settings
	void parse_general(_hyd_keyword_file Key, word Command);
	///Parse for the name of the groundwater file
	void parse_groundwater_filename(_hyd_keyword_file Key, word Command);
	///Parse for the instationary_boundary_file name and number of curves
	void parse_instat_boundary(_hyd_keyword_file Key, word Command);
	///Parse for the file and the number of noflow polygons (enclosing noflow cells)
	void parse_no_flow(_hyd_keyword_file Key, word Command);
	//Parse for the file and the number of lines 
	void parse_lines(_hyd_keyword_file Key, word Command);
	///Parse for the calculation limits
	void parse_calculation_limits(_hyd_keyword_file Key, word Command);
	///Parse for the 2d_result_file name for tecplot
	void parse_2d_result_file(_hyd_keyword_file Key, word Command);

	///Check if the requiered paramaters were found
	void check_parameters_found(void);


	///Set the warning
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);


};
#endif