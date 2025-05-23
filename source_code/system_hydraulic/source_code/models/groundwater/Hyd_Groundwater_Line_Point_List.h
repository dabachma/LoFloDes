#pragma once
/**\class Hyd_Groundwater_Line_Point_List
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_GROUNDWATER_LINE_POINT_LIST_H
#define HYD_GROUNDWATER_LINE_POINT_LIST_H

//system_hyd
#include "Hyd_Groundwater_Line_Point.h"


///List-class where the groundwater line points are stored and managed \ingroup hyd
/**

\see Hyd_Groundwater_Line_Point
*/
class Hyd_Groundwater_Line_Point_List
{
public:
	///Default constructor
	Hyd_Groundwater_Line_Point_List(void);
	///Default destructor
	~Hyd_Groundwater_Line_Point_List(void);

	//methods
	///Get the number of points
	int get_number_points(void);
	///Get the total list as a pointer
	Hyd_Groundwater_Line_Point_List* get_total_list(void);
	///Get a specific  points with the given index
	Hyd_Groundwater_Line_Point get_point(const int index);
	///Get a specific point pointer with the given index
	Hyd_Groundwater_Line_Point* get_ptr_point(const int index);


	///Set a new points
	void set_new_point(Hyd_Groundwater_Line_Point *new_point);

	///Delete the list
	void delete_list(void);

	///Output the setted members
	void output_setted_members(ostringstream *cout);

	///Use offset of coordinates
	void use_offset_coordinates(const double x_off, const double y_off);

private:

	//members
	///Coupling points in list
	Hyd_Groundwater_Line_Point *points;

	///Number of list-elements in a block for allocation
	const int block_elems;
	///Number of points in list
	int number;


	//methods
	///Delete the allocated points
	void delete_points(void);



	///Set error(s)
	Error set_error(const int err_type);

};
#endif
