#pragma once
/**\class Hyd_Element_Groundwater_Type_Line
	\author You WU et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_ELEMENT_GROUNDWATER_TYPE_LINE_H
#define HYD_ELEMENT_GROUNDWATER_TYPE_LINE_H

//hyd system
//base class
#include "_Hyd_Element_Groundwater_Type.h"

///Class for the element type of the groundwater-model, which is a line \ingroup hyd
/**
The element is determined as the barrier line which intercept the standard groundwater elements
\see Hyd_Groundwater_Polygon
*/
class Hyd_Element_Groundwater_Type_Line : public _Hyd_Element_Groundwater_Type
{
	///Class where the element type is decided and which work with the element type
	friend class Hyd_Element_Groundwater;

public:
	///Get the global waterlevel (s_value)
	double get_s_value(void);
	///Get the local waterlevel (h_value)
	double get_h_value(void);

	///Default constructor
	Hyd_Element_Groundwater_Type_Line(void);
	///Default destructor
	~Hyd_Element_Groundwater_Type_Line(void);

	//methods
	///Initialize the element (here it is not needed)
	void init_element(_hyd_neighbouring_gw_elems neigh_elem, double* width_x, double* width_y, double* area, Hyd_Param_Conductivity* con_table, Hyd_Param_Porosity* por_table, const bool clone);

private:
	//members
	///Gloabl waterlevel:= geodetic height (z) + waterlevel (h); it is also the result of the solver [result variables]
	double s_value;
	///Local waterlevel:= waterlevel (h) [result variables]
	double h_value;

	///Output the setted members
	void output_setted_members(ostringstream* cout);

	///Ouput the result members per internal timestep
	void output_result_values(ostringstream* cout);
	///Ouput the maximum calculated values
	void output_maximum_calculated_values(ostringstream* cout);
};

#endif
