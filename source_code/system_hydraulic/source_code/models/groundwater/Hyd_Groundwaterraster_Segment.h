#pragma once
/**\class Hyd_Groundwaterraster_Segment
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_GROUNDWATERRASTER_SEGMENT_H
#define HYD_GROUNDWATERRASTER_SEGMENT_H

//system classes
#include "Geo_Raster_Segment.h"
//hyd-system
#include "Hyd_Element_Groundwater.h"

///Enumerator for the groundwater segment raster type \ingroup hyd
enum _hyd_groundwater_raster_segment_type {
	///Groundwater raster segment is at the border of the groundwater raster
	BORDER_GW,
	///Groundwater raster segment is in x-flow direction (positiv x)
	XFLOW_GW,
	///Groundwater raster segment is in y-flow direction (positiv y)
	YFLOW_GW
};

///Geometrical segment of a groundwater raster \ingroup hyd
/**
The segments are generated with the help of the raster points. With the help of the
segment each polygon of the groundwater raster can be generated.

\see Hyd_Groundwater_Raster, Hyd_Groundwaterraster_Point, Hyd_Groundwaterraster_Polygon
*/
class Hyd_Groundwaterraster_Segment : public Geo_Raster_Segment
{
public:
	///Default constructor
	Hyd_Groundwaterraster_Segment(void);
	///Default destructor
	~Hyd_Groundwaterraster_Segment(void);


	//methods

	///Set the type of the segment
	void set_segmenttype(_hyd_groundwater_raster_segment_type type);
	///Get the type of the segment
	_hyd_groundwater_raster_segment_type get_segmenttype(void);
	///Set a pointer to Hyd_Element_Groundwater
	void set_element_ptr(Hyd_Element_Groundwater *element);

	///Set the index in the raster of the segment
	void set_segment_index(const int index);
	///Get the index in the raster of the segment
	int get_segment_index(void);

	///Set the index in the raster of the first point
	void set_firstpoint_index(const int index);
	///Get the index in the raster of the first point
	int get_firstpoint_index(void);

	///Set the index in the raster of the second point
	void set_secondpoint_index(const int index);
	///Get the index in the raster of the second point
	int get_secondpoint_index(void);

	///Output the header for the members
	void output_header(ostringstream *cout);
	///Output members
	void output_members(ostringstream *cout);

	///Assign a border flag to the Hyd_Element_Groundwater pointer
	void assign_value_flag2groundwater_element(const bool flag);

	///Switch the points
	void switch_points(void);

	///Copy operator
	Hyd_Groundwaterraster_Segment& operator=(const Hyd_Groundwaterraster_Segment& object);



private:

	//members
	///Index in the raster of the segment
	int segment_index;
	///Index in the raster of the first point
	int index_point_1;
	///Index in the raster of the second point
	int index_point_2;

	///Type of the segment
	_hyd_groundwater_raster_segment_type type;

	///Pointer to the groundwater element
	Hyd_Element_Groundwater *ptr_groundwater_elem;

	//methods

	///Conversion function for output (Enumerator _hyd_groundwater_raster_segment_type to text)
	string convert_segment_type2txt(_hyd_groundwater_raster_segment_type type);

};
#endif
