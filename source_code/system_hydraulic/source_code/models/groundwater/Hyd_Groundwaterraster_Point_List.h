#pragma once
/**\class Hyd_Groundwaterraster_Point_List
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_GROUNDWATERRASTER_POINT_LIST_H
#define HYD_GROUNDWATERRASTER_POINT_LIST_H

///Datstructure to store the index of a point, a given value to the point and a given flag to the point \ingroup hyd REVIEW	
struct _hyd_gw_value2pointindex {
	///Value as a charcteristic of a point as absolute height [m]
	double value_h;
	///Index of the point
	int index;
	///Flag as a charcteristic of a point
	bool flag;
};
///List-class which gathers point of a groundwater raster in list \ingroup hyd
/**

\see Hyd_Groundwater_Raster, Hyd_Groundwaterraster_Point
*/
class Hyd_Groundwaterraster_Point_List
{
public:
	///Default constructor
	Hyd_Groundwaterraster_Point_List(void);
	///Default destructor
	~Hyd_Groundwaterraster_Point_List(void);

	//methods
	///Set a new point in the list
	void set_new_point(_hyd_value2pointindex *new_point);
	///Get the total point list as a pointer
	_hyd_value2pointindex* get_total_list(void);

	///Get a point with a given index
	_hyd_value2pointindex get_point(const int index);
	///Get the number of points stored in the list
	int get_number_points(void);

	///Delete the list
	void delete_list(void);

	///Output members
	void output_members(void);


private:
	//members
	///Number of list-elements in a block for allocation
	const int block_elems;
	///Number of points
	int number_points;
	///Pointer 
	_hyd_value2pointindex *points;

	///Delete the points
	void delete_points(void);

	///Allocate the points
	void allocate_points(void);

	///Set error(s)
	Error set_error(const int err_type);

};
#endif
