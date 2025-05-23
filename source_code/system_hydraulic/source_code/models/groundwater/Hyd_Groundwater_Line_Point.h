#pragma once
/**\class Hyd_Groundwater_Line_Point
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_GROUNDWATER_LINE_POINT_H
#define HYD_GROUNDWATER_LINE_POINT_H

//system sys
#include "Geo_Point.h"
#include "_Hyd_River_Profile.h"

struct _hyd_break_parameters;
struct _hyd_idealisation_parameters;


///Geometrical class for the point of a lin within groundwater \ingroup hyd
/**
Here all the relevant information are stored, like the relative height [m],
the break parameters etc.

\see Hyd_Groundwater_Polysegment

*/
class Hyd_Groundwater_Line_Point : public Geo_Point, public _Sys_Common_System
{
public:
	///Default constructor
	Hyd_Groundwater_Line_Point(void);
	///Copy constructor
	Hyd_Groundwater_Line_Point(const Hyd_Groundwater_Line_Point& object);
	///Default destructor
	~Hyd_Groundwater_Line_Point(void);

	//method
	///Set the absolute height of the point (crest)
	void set_abs_height(const double h);
	///Get the absolute height of the point (crest)
	double get_abs_height(void);
	///Set the absolute height of the point (base)
	void set_base_height(const double h);
	///Get the absolute base height of the point (base)
	double get_base_height(void);

	///Set the poleni factor of the point
	void set_kf_fac(const int kf);
	///Get the poleni factor of the point
	int get_kf_fac(void);
	///Set the overflow flag of the point
	void set_flow_through_flag(const bool flag);
	///Get the overflow flag of the point
	bool get_flow_through_flag(void);

	///Check the line point
	void check_line_point(void);

	///Set the distance to the beginning of a polysegment from the line point
	void set_distance2begin(const double distance);
	///Get the distance to the beginning of a polysegment from the line point
	double get_distance2begin(void);

	///Get the gw-section id (if existing )of the line point
	int get_gw_section_id(void);

	//Idealise the line points with given parameters of gw-section
	//void idealise_points_gw(_hyd_idealisation_parameters ideal_param, const int section_id, const bool last_flag);

	///Output the header for the setted member (static)
	static void output_header_setted_member(ostringstream *cout);
	///Output the setted members
	void output_setted_members(ostringstream *cout);

	void clone_fp_point(Hyd_Floodplain_Dikeline_Point point);
	///Copy operator
	Hyd_Groundwater_Line_Point& operator=(const Hyd_Groundwater_Line_Point& object);






private:
	//members
	///Absolute height of the point (crest)
	double abs_height;
	///Absolute height of the point (base)
	double base_height;

	///conductivity (kf) factor
	int kf_fac;

	///Flow-through flag
	bool flow_through_flag;

	///Distance to the begin of a line
	double distance2begin;

	//Flag if the point is idealised by a GW-section
	//bool point2gw;

	///Section id of a connected GW-section
	int gw_sec_id;


	//method
	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};
#endif

