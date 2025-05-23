#pragma once
/**\class Hyd_Instationary_Boundary
	\author Bastian Winkels et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_INSTATIONARY_BOUNDARY_GW_H
#define HYD_INSTATIONARY_BOUNDARY_GW_H

//system_sys_ classes
#include "_Sys_Common_System.h"
//system hyd class
//info class about the hydraulic boundary szenario data
#include "Hyd_Boundary_Szenario.h"


///This structure combine the values of the instationary boundary curve: value (Q(point, length, area), h(point)) and the corresponding point of time \ingroup hyd
struct _hyd_bound_point_gw {
	///Value of the instationary boundary curve point (Q(point, length, area), h(point))
	double value;
	///Corresponing time point [h] of the instationary boundary value
	double hour;
};
///This type specifies for the output the type of geometry of instationary boundary values (Q/h related to point m³/s, lenght (m³/s)/m, area (m³/s)/m²; or waterlevel (m)) \ingroup hyd
enum _hyd_bound_geom_type {
	///Discharge value related to a point [m³/s]; used for _Hyd_River_Profile or Hyd_Element_Floodplain
	gw_point_type,
	///Discharge value related to an area [(m³/s)/m²]; used for Hyd_Element_Floodplain
	gw_area_type,
	///Discharge value related to a length [(m³/s)/m]; used for _Hyd_River_Profile
	gw_length_type,
};
///Specifies whether the boundary is an h or Q-boundary
enum _hyd_bound_type_gw {
	/// Specified head_boundary [m]
	head_boundary,
	///Specified discharge boundary (m³/s)
	discharge_boundary,
};

///Specifies the direction of the h-boundary condition
enum _hyd_bound_direction_type {
	x_dir,
	y_dir,
	top_dir,
};


///Enumerator for the different types of hydraulic models \ingroup hyd
enum _hyd_model_type_gw{
	///Hydraulic model: river model, 1d (Hyd_Model_River)
	GW_RIVER_MODEL,
	///Hydraulic model: floodplain model, 2d (Hyd_Model_Floodplain)
	GW_FLOODPLAIN_MODEL,
	///Hydraulic model: groundwater model, 2d (Hyd_Model_Groundwater)
	GW_GROUNDWATER_MODEL
};

///Class for the instationary boundary curves of the hydraulic system (Hyd_System_Hydraulic) \ingroup hyd
/**

*/
class Hyd_Instationary_Boundary_GW : public _Sys_Common_System
{
public:
	///Default constructor
	Hyd_Instationary_Boundary_GW(void);
	///Default destructor
	~Hyd_Instationary_Boundary_GW(void);

	//members

	///Member for storing the hydraulic boundary szenario information
	Hyd_Boundary_Szenario hyd_sz;

	//method

	///Read in the boundary values (value and hour h) and the point in time from a file 
	void read_value(const string file_name, const int index);

	///Create the database table for the instationary boundary curves
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the instationary boundary curves: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the instationary boundary curves
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the instationary boundary curves
	static void close_table(void);
	///Delete the boundary curve data in the database table for a given boundary scenario
	static void delete_boundary_curve_by_scenario(QSqlDatabase *ptr_database, const int sc_id);


	///Create the database point table for the instationary boundary curves
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database point table for the instationary boundary curves: it sets the table name and the name of the columns and allocate them
	static void set_point_table(QSqlDatabase *ptr_database);
	///Delete all point data in the database table for the instationary boundary curves
	static void delete_point_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database point table for the instationary boundary curves
	static void close_point_table(void);

	///Select and count the number of relevant instationary boundary curves in a database table 
	static int select_relevant_curves_database(QSqlTableModel *results, const int model_id, _hyd_model_type_gw model_type, const bool with_output, Hyd_Boundary_Szenario *ptr_hyd_sz);

	///Transfer the instationary boundary curves of a file into a database table
	void transfer_instat_curve2database_table(QSqlDatabase *ptr_database, _hyd_model_type_gw model_type, const int model_id);
	///Set the instationary boundary curves from a given selection of a database table
	void read_value(QSqlTableModel *results, const int index);

	///Get the curve number
	int get_curve_number(void);

	///Clone the instationary boundary curve
	void clone_curve(Hyd_Instationary_Boundary_GW *curve);

	///Set the type of the instationary boundary curve (e.g. discharge or head);
	void set_type(_hyd_bound_type_gw bound_type);

	///Set the geometry type of the instationary boundary curve;
	void set_geom(_hyd_bound_geom_type bound_type);

	///Set the direction type of the instationary boundary curve;
	void set_dir(_hyd_bound_direction_type dir_type);

	///Set the specific value (e.g. a length or an area); This value is multiplied with the discharge to get always the unit m³/s
	void set_specific_value(const double specific_multiplicator);

	///Calculate the boundary condition at a given point in time; it is returned
	double calculate_actuel_boundary_value(const double time_point);

	///Check the duration of the boundary curve
	void check_boundary_curve_duration(const double needed_duration);
	///Check if it is start by a given time
	void check_starting_time(const double time_sec);
	///Check if there are negative values in the boundary curve
	void check_boundary_curve_neg_null_values(void);

	///Output the curve
	void output_member(void);

	///Transform text to enum _hyd_bound_type_gw
	static _hyd_bound_type_gw transform_txt2_instatboundtype(string txt);
	///Transform enum _hyd_bound_type_gw to text
	static string transform_instatboundtype2txt(const _hyd_bound_type_gw type);

	///Transform text to enum _hyd_bound_geom_type
	static _hyd_bound_geom_type transform_txt2_instatboundtype_geom(string txt);
	///Transform enum _hyd_bound_geom_type to text
	static string transform_instatboundtype2txt_geom(const _hyd_bound_geom_type type);

	///Transform text to enum _hyd_bound_direction_type
	static _hyd_bound_direction_type transform_txt2_instatboundtype_dir(string txt);
	///Transform enum _hyd_bound_direction_type to text
	static string transform_instatboundtype2txt_dir(const _hyd_bound_direction_type type);

	///Clear the instationary boundary curve, delete the points
	void clear_boundary_curve(void);

private:

	//members

	///Pointer to the table for the instationary boundary curves in a database
	static Tables *instat_bound_table;
	///Pointer to the table for the points of the instationary boundary curves in a database
	static Tables *instat_bound_point_table;

	///Number of the curve
	int number;
	///Number of boundary points representing the curve
	int number_of_points;
	///Pointer to the structure _hyd_bound_point_gw
	_hyd_bound_point_gw *boundary_points;
	///Store an index for a faster interpolation scheme
	int index_left;

	///Specific value for lateral boundary condition or area related boundary conditions; it is multiplied with the discharge to get always the unit m³/s
	double specific_multiplicator;

	///Specifies the setted geom type of the instationary boundary curve (enum _hyd_bound_geom_type)
	_hyd_bound_geom_type setted_geom_type;
	///Specifies the given geom type of the instationary boundary curve (enum _hyd_bound_geom_type)
	_hyd_bound_geom_type given_geom_type;
	

	///Specifies the setted type of the instationary boundary curve (enum _hyd_bound_type)
	_hyd_bound_type_gw setted_bound_type;
	///Specifies the given type of the instationary boundary curve (enum _hyd_bound_type)
	_hyd_bound_type_gw given_bound_type;
	
	///Specifies the given direction type of the instationary boundary curve (enum _hyd_bound_direction_type)
	_hyd_bound_direction_type given_dir_type;
	///Specifies the setted direction type of the instationary boundary curve (enum _hyd_bound_dir_type)
	_hyd_bound_direction_type setted_dir_type;

	///The actuel value, which is calculated by calculate_actuel_discharge(const double time_point) and returned
	double value_actuel;

	//methods

	///Check if the time values are sequentiell increasing and that there are no negative time values
	void check_time_values(void);
	///Allocate the necessary boundary points, consisting of time and value (structure _hyd_bound_point_gw) of the curve
	void alloc_points(void);
	///Delete the boundary points
	void delete_points(void);

	///Transfer the instationary boundary curves of a file into a database table
	void transfer_instat_points2database_table(QSqlDatabase *ptr_database, const int curve_glob_id);

	///Select and count the number of relevant instationary boundary curve points in a database table 
	static int select_relevant_points_database(QSqlTableModel *results, const int global_curve_id);
	///Delete the boundary curve point data in the database table for a given curve id
	static void delete_boundary_curve_points_by_curve_id(QSqlDatabase *ptr_database, const int curve_id);

	///Read in the points of the boundary curve from a given selection of a database table
	void read_points(const QSqlTableModel *results, const int index);


	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);


};
#endif
