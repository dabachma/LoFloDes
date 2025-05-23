#pragma once
/**\class Hyd_Groundwater_Polysegment
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_GROUNDWATER_POLYSEGMENT_H
#define HYD_GROUNDWATER_POLYSEGMENT_H

//system sys class
//base class
#include "Geo_Polysegment.h"
#include "Geo_Interception_Point_List.h" 
#include "Common_Const.h"
#include "_Sys_Common_System.h"

//Hyd libs
//for the input per file
#include "_Hyd_Parse_IO.h"
#include "Hyd_Element_Groundwater.h"
#include "Hyd_Groundwater_Line_Point.h"
#include "Hyd_Groundwater_Line_Point_List.h"

///Enumerator to specifies the type of the Hyd_Groundwater_Polysegment \ingroup hyd
enum _hyd_groundwater_polysegment_type {
	///Polysegment type of the river midline
	RIVERLINE_GW,
	///Polysegment type of the river bankline
	RIVERBANKLINE_GW,
	///Polysegment type of the groundwater boundary
	GW_BOUNDARYLINE,
	///general Line
	GW_LINE,
	///barrier Line
	GW_BARRIER
};


///Geometrical class for the polysegments in a groundwater model, like river bankline \ingroup hyd
/**

\see Hyd_Model_Groundwater
*/
class Hyd_Groundwater_Polysegment : public Geo_Polysegment, public _Sys_Common_System
{
public:
	///Default constructor
	Hyd_Groundwater_Polysegment(void);
	///Default destructor
	~Hyd_Groundwater_Polysegment(void);

	//members

	///A list with interception points
	Geo_Interception_Point_List intercept_list;

	///Pointer to the table for the polysegment data in a database
	static Tables *polysegment_table;
	///Pointer to the table for the polysegment point data in a database
	static Tables *polysegment_point_table;

	//methods

	///Input the members per file
	void input_members(const int index, const string filename, const double offset_x = 0.0, const double offset_y = 0.0);

	///Create the database table for the polysegment data in a database
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the polysegment data: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the polysegment data
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Delete data in the database table for the polysegment data by a given system id
	static void delete_data_in_table(QSqlDatabase *ptr_database, _sys_system_id id);
	///Close and delete the database table for the polysegment data
	static void close_table(void);
	///Select and count the number of relevant polysegments in a database table 
	static int select_relevant_polysegments_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int model_id, const bool coast_flag, const bool with_output);
	///Select and count the number of relevant polysegments in a database table (static)
	static int select_relevant_polysegments_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool coast_flag, const bool with_output);
	///Select and count the number of polysegments in a database table (static)
	static int select_polysegments_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool coast_flag, const bool with_output);


	///Switch the applied-flag for the section in the database table for a defined system state and id (static)
	static void switch_applied_flag_polysegment_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int glob_id);
	///Switch the applied-flag for the section in the database table for a defined system state and id (static)
	static void switch_applied_flag_polysegment_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);


	///Transfer the polysegment data of a file into a database table
	void transfer_polysegment2database_table(QSqlDatabase *ptr_database, const int model_id, const bool coast_flag);
	///Set the polysegment data from a database selection 
	void input_members(const QSqlQueryModel *results, QSqlDatabase *ptr_database, const int index);

	///Set number of points
	void set_number_of_points(const int number);

	///Set line polysegment by point list
	void set_line_by_list(Hyd_Groundwater_Line_Point_List *list);
	///Fill marked points with heights
	void fill_marked_points_heights(const double marker);

	///Get pointer to the line points
	Hyd_Groundwater_Line_Point* get_ptr_line_points(const int index);
	///Set the line points to Geo_Segement
	void set_linepoints_to_segment(void);

	///Set the line index 
	void set_index(const int index);
	///Get the line index 
	int get_index(void);

	///Get the name of the polysegment
	string get_name(void);

	///Set the name of the polygon
	void set_name(string nam);

	///Output the members
	void output_members(void);
	///Output the polysegment to tecplot
	void output2tecplot(ofstream *output_file);

	///Set the no_flow_flag
	void set_no_flow_flag(const bool flag);
	///Get the no_flow_flag
	bool get_no_flow_flag(void);
	///Get the height of an segment for a given index
	double get_segments_height(const int index);
	//Get the kf-factor of an segment for a given index
	double get_segments_kf(const int index);
	//Get a no flow-through flag of an segment for a given index
	bool get_segments_no_flow_through(const int index);

	///Set the type of the line (_hyd_groundwater_polysegment_type)
	void set_line_type(_hyd_groundwater_polysegment_type type);
	///Get the type of the line (_hyd_groundwater_polysegment_type)
	_hyd_groundwater_polysegment_type get_line_type(void);

	///Set the element type (_hyd_gw_elem_type) to set to elements which are intercepted by this line
	void set_element_type(_hyd_gw_elem_type type);
	void set_element_type(_hyd_elem_type type);
	///Get the element type (_hyd_gw_elem_type) to set to elements which are intercepted by this line
	_hyd_gw_elem_type get_element_type(void);


	//Set if the line is closed (true) or not (false) per string
	void set_is_closed_str(string txt);

	///Conversion function for output from enumerator _hyd_groundwater_polysegment_type to text
	static string convert_polysegment_type2txt(_hyd_groundwater_polysegment_type type);

	///Clone the groundwater-polysegment
	void clone_polysegment(Hyd_Groundwater_Polysegment *seg);
	///Clone the floodplain-polysegment
	void clone_polysegment(Hyd_Floodplain_Polysegment *seg);

	///Get the pointer to the upwards line point
	Hyd_Groundwater_Line_Point* get_ptr_upwards_point(const int segment_id);
	///Get the pointer to the downwards dikeline point
	Hyd_Groundwater_Line_Point* get_ptr_downwards_point(const int segment_id);

	///Find the pointer to the line points up and down the line to a given station
	bool find_ptr_up_down_point(const double station, Hyd_Groundwater_Line_Point **up, Hyd_Groundwater_Line_Point **down);

	///Get a polyline of the line points, which are represented by an gw-section
	void get_polyline_gw_section(Geo_Polysegment *polyline, Hyd_Groundwater_Line_Point *start, Hyd_Groundwater_Line_Point *end);

	///Get the number of involved points to a gw-section
	int get_number_involved_points(void);
	///Get a pointer to the involved points by a given index
	Hyd_Groundwater_Line_Point* get_ptr_involved_points(const int index);

	///Set distances of the line points to the beginning of the polysegement
	void set_distances2begin(void);

private:

	//members


	///Name of the polysegment
	string name;

	///Index of the polysegment
	int index;
	///The line points 
	Hyd_Groundwater_Line_Point *line_points;
	///Flag to set noflow boundaries; if this flag is set, the height is not more important
	bool no_flow_flag;
	///Specifies the type of the line (_hyd_groundwater_polysegment_type)
	_hyd_groundwater_polysegment_type line_type;
	///Specifies the element type (_hyd_elem_type) which are intercepted by this line
	_hyd_gw_elem_type intercepted_elem_type;


	///A list of pointer to a part of the line points
	Hyd_Groundwater_Line_Point **list_part_points;
	///Number of the points stored in the list list_part_points
	int number_part_points;
	///kf_id for barrier line
	int barrier_kf_id;



	//methods

	///Allocate the points of the line
	void allocate_line_points(void);
	///Delete the points of the line
	void delete_line_points(void);



	///Allocate the list of the pointer of a part of the line points
	void allocate_part_point_list(void);
	///Delete the list of pointer of a part of the line points
	void delete_part_point_list(void);
	///Set the list of the pointer of a part of the line points by two given points
	void set_part_point_list(Hyd_Groundwater_Line_Point *up, Hyd_Groundwater_Line_Point *down);

	///Create the database table for the polysegment point data in a database
	static void create_point_table(QSqlDatabase *ptr_database);
	///Set the database table for the polysegment point data: it sets the table name and the name of the columns and allocate them 
	static void set_point_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the polysegment point data 
	static void delete_point_data_in_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the polysegment point data 
	static void delete_point_data_in_table(QSqlDatabase *ptr_database, const int glob_id);
	///Transfer the polysegment point data of a file into a database table
	void transfer_polysegment_point2database_table(QSqlDatabase *ptr_database, const int poly_id);
	///Close and delete the database table for the polysegment point data 
	static void close_point_table(void);
	///Select and count the number of relevant polysegment points in a database table
	static int select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int global_segment_id);

	///Check the line points
	void check_line_points(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};

#endif

