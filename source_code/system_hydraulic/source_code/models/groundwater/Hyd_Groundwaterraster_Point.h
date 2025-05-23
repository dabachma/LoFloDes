#pragma once
/**\class Hyd_Groundwaterraster_Point
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_GROUNDWATERRASTER_POINT_H
#define HYD_GROUNDWATERRASTER_POINT_H

//sys system
#include "Geo_Point.h"

///Geometrical point of a groundwater raster \ingroup hyd
/**

\see Hyd_Groundwater_Raster, Hyd_Groundwaterraster_Segment, Hyd_Groundwaterraster_Polygon
*/
class Hyd_Groundwaterraster_Point : public Geo_Point
{
public:
	///Default constructor
	Hyd_Groundwaterraster_Point(void);
	///Copy constructor
	Hyd_Groundwaterraster_Point(const Hyd_Groundwaterraster_Point& object);
	///Default destructor
	~Hyd_Groundwaterraster_Point(void);

	///Set the point index
	void set_point_index(const int index);
	///Get the point index
	int get_point_index(void);
	///Output members
	void output_members(ostringstream *cout);
	///Output header for members output
	void output_header(ostringstream *cout);

	///Copy operator
	Hyd_Groundwaterraster_Point& operator=(const Hyd_Groundwaterraster_Point& object);



private:
	///Index of the point
	int point_index;
};
#endif
