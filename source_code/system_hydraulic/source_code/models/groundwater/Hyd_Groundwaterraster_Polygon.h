#pragma once
/**\class Hyd_Groundwaterraster_Polygon
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_GROUNDWATERRASTER_POLYGON_H
#define HYD_GROUNDWATERRASTER_POLYGON_H

//hyd-system
#include "Hyd_Element_Groundwater.h"
#include "Hyd_Groundwaterraster_Segment.h"
#include "Hyd_Groundwater_Polygon.h"

//sys-system 
//(base class)
#include "Geo_Raster_Polygon.h" 

///Geometrical polygon of a groundwater raster \ingroup hyd
/**
This polygons of the raster are always rectangular. They are not stored
in the groundwater raster. They are always generated, than the geometrical action is performed, e.g. interception
or point-inside check and than again deleted. They are generated with the help of tzhe raster segments.

\see Hyd_Groundwater_Raster, Hyd_Groundwaterraster_Segment
*/
class Hyd_Groundwaterraster_Polygon : public Geo_Raster_Polygon
{
public:
	///Default constructor
	Hyd_Groundwaterraster_Polygon(void);
	///Default destructor
	~Hyd_Groundwaterraster_Polygon(void);

	//methods
	///Set the pointer to the floodplain element
	void set_ptr2groundwater_element(Hyd_Element_Groundwater *ptr2elem);

	///Set the polygon's segments
	void set_polygon_segments(Hyd_Groundwaterraster_Segment *segment_y, Hyd_Groundwaterraster_Segment *segment_x, Hyd_Groundwaterraster_Segment *segment_minus_y, Hyd_Groundwaterraster_Segment *segment_minus_x);

	///Set the midpoint of the polygon to the given pointer
	void set_mid_point(Geo_Point *mid);

	///Check if this raster polygon is inside the given Hyd_Groundwater_Polygon and set element type of the given polygon
	void is_inside_polygon(Hyd_Groundwater_Polygon *polygon);
	void is_inside_polygon(Hyd_Floodplain_Polygon *polygon);
	///Check if this raster polygon is intercepted by the given Hyd_Groundwater_Polygon and set the given element type 
	void is_intercepted_polygon(Hyd_Groundwater_Polygon *polygon, _hyd_gw_elem_type type);
	void is_intercepted_polygon(Hyd_Floodplain_Polygon *polygon, _hyd_gw_elem_type type);

	///Calculate the interception points with a Geo_Poly_Segment and fill the Geo_Interception_Point_List 
	void calculate_polysegment_interception(Geo_Interception_Point_List *intercept_list, Geo_Polysegment *check_polysegment);

	///Output the members
	void output_members(ostringstream *cout);
	///Output the header for an output (just once)
	void output_header(ostringstream *cout);

	///Get is-outside flag
	bool get_is_outside_flag(void);

	///Set is-outside flag
	void set_is_outside_flag(const bool flag);



private:
	//members
	///Pointer to a groundwater
	Hyd_Element_Groundwater *ptr2groundwater_elem;
	///Pointer to the segments
	Hyd_Groundwaterraster_Segment *segments;

	///Is-outside flag of something, like another polygon. Use it as information-flag
	bool is_ouside;

	//method
	///Sort the points
	void sort_points(void);


};
#endif
