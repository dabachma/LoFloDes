#pragma once
/**\class Hyd_Param_GW
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_PARAM_GW_H
#define HYD_PARAM_GW_H

//system_sys_ classes
#include "_Sys_Common_System.h"
#include "Common_Const.h"

///Structure where the geometrical information of the groundwater are stored \ingroup hyd
struct _hyd_groundwater_geo_info {
	///Element width in x-direction
	double width_x;
	///Element width in y-direction
	double width_y;
	///Angle of the x-axis to the normal x-direction (--> x-normal; positive angle=> turning of the coodinate system (global) in clockwise direction (local) )
	double angle;
	///Number of elements in x
	int number_x;
	///Number of elements in y
	int number_y;
	///Global x-coordinate of the origin
	double origin_global_x;
	///Global y-coordinate of the origin
	double origin_global_y;
};

///Container class, where the members of the groundwater model is stored (Hyd_Model_Groundwater) \ingroup hyd
/**
	This is a data container for the groundwater model members. Besides data storing it handles:
		- the input per file parser,
		- the output of the members,
		- checking the members for application

	\see Hyd_Model_Groundwater
*/

class Hyd_Param_GW
{
	///File parser for the members of Hyd_Param_GW
	friend class Hyd_Parse_GW;
	///The members, which are stored in Hyd_Param_GW are used in Hyd_Model_Groundwater
	friend class Hyd_Model_Groundwater;

public:
	///Default constructor
	Hyd_Param_GW(void);
	///Copy constructor
	Hyd_Param_GW(const Hyd_Param_GW& par);
	///Default destructor
	~Hyd_Param_GW(void);

	//members				

	//methods
	///Set the groundwater model parameters per parser from file
	void parameter_per_parser(const string file, const int index, const string path);
	///Get the groundwater number
	int get_groundwater_number(void);
	///Get the groundwater name
	string get_groundwater_name(void);
	///Check the members for impossible values
	void check_members(void);
	///Get number of elements in x-direction
	int get_no_elems_x(void);
	///Get number of elements in y-direction
	int get_no_elems_y(void);
	///Get the geometrical information of the groundwater
	_hyd_groundwater_geo_info get_geometrical_info(void);
	///Get a pointer to the absolute tolerance for the solver
	double* get_absolute_solver_tolerance(void);
	///Get the relative tolerance for the solver
	double get_relative_solver_tolerance(void);
	///Output the members of the groundwater model
	void output_members(void);
	///Get the filename for the groundwater geometry file
	string get_filename_geometrie2file(const string type);
	///Get the filename for the groundwater geometry file without suffix
	string get_filename_geometrie(const string type);
	///Get the filename for the groundwater result file 
	string get_filename_result2file(const string type, const double timepoint);
	///Get the filename for the groundwater result file without suffix
	string get_filename_result(const string type);
	///Get the filename for the groundwater result file 
	string get_filename_result_max2file(const string type);
	///Get the filename for the maximum groundwater observation points to file 
	string get_filename_obs_point2file(const string type);
	///Calculate the approximate workspace requirement
	long long int calculate_approx_workspace(void);

	///Get pointer to the element width in x-direction
	double *get_ptr_width_x(void);
	///Get pointer to the element width in y-direction
	double *get_ptr_width_y(void);
	///Get pointer to the element area
	double *get_ptr_elem_area(void);

	///Copy operator
	Hyd_Param_GW& operator= (const Hyd_Param_GW& par);

private:
	///Name of the groundwater model
	string GWName;
	///Index of the groundwater model
	int GWNumber;
	///Number of elements in x
	int GWNofX;
	///Number of elements in y
	int GWNofY;

	///Element width in x-direction
	double width_x;
	///Element width in y-direction
	double width_y;
	///Area of element
	double area;
	///Angle of the x-axis to the normal x-direction (--> x-normal; positive angle=> anti-clockwise)
	double angle;

	///Global x-coordinate of the origin
	double GWLowLeftX;
	///Global y-coordinate of the origin
	double GWLowLeftY;
	///Name of the element file
	string groundwater_file;

	///Number of instationary boundary curves used
	int number_instat_boundary;
	///Filename where the instationary boundary curves are given
	string inst_boundary_file;

	///Value which marks a element, where no information is available
	double noinfo_value;

	///Number of Polygons which enclose noflow elements
	int number_noflow_polys;
	///Filename where the Polygons enclosing noflow elements are stated
	string noflow_file;

	///Number of polylines 
	int number_polylines;
	///Filename where the polylines are stated
	string line_file;

	///Name of the result file
	string tecplot_outfile_name;

	///Absolute tolerance of the solver for each model
	double abs_tolerance;
	///Default absolute tolerance of the solver for each model
	const double default_abs_tol;
	///Relative tolerance of the solver for each model
	double rel_tolerance;
	///Default relative tolerance of the solver for each model
	const double default_rel_tol;

	//methods
	///Complete the filenames with the path
	void complete_filenames_with_path(string global_path);

	///Calculate the element area
	void calculate_area(void);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};


#endif
