#pragma once
/**\class Hyd_Element_Bound_GW_Members
	\author Bastian Winkels et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_ELEMENT_BOUND_GW_MEMBERS_H
#define HYD_ELEMENT_BOUND_GW_MEMBERS_H

//hyd system

///Data structure for storing the boundary condition relevant parameters \ingroup hyd
struct _hyd_boundarydata_groundwater_elem {
	///Flag for a boundary condition (true=> there is one; false=> there is none)
	bool boundary_flag;


	///Flag for a h boundary condition (true=> there is one; false=> there is none)
	bool h_boundary_flag;
	///Flag for the type of the h boundary condtion (true=>stationary; false=> instationary); just in connection with the boundary_flag
	bool h_stationary_flag;

	///Flag for a q boundary condition (true=> there is one; false=> there is none)
	bool q_boundary_flag;
	///Flag for the type of the q boundary condtion (true=>stationary; false=> instationary); just in connection with the boundary_flag
	bool q_stationary_flag;

	/*///Flag for distinction between q and h-boundary (true=>h-boundary, false=>q-boundary)
	bool head_flag;*/


	///Direction of the h-boundary, determines the flow area
	_hyd_bound_direction_type boundary_direction;
	///Value for a stationary h boundary condition; 
	double h_value;
	///Value for a stationary q boundary condition; 
	double q_value;


	///Number of the instationary boundary curve could be used for h boundary (unique number)
	int h_curve_number;

	///Number of the instationary boundary curve could be used for h boundary (unique number)
	int q_curve_number;

	///Flag if the instationary boundary curve could be connected
	bool found_flag;

	///Pointer to the instationary h boundary curve
	Hyd_Instationary_Boundary_GW *ptr_hboundarycurve;
	///Pointer to the instationary q boundary curve
	Hyd_Instationary_Boundary_GW* ptr_qboundarycurve;
	
	///The type of the boundary condition;
	_hyd_bound_type_gw boundary_type;
	///The type of the geometry of the boundary condition;
	_hyd_bound_geom_type geom_type;
};

///This class group the members for a boundary condition in a groundwater element \ingroup hyd
/**

*/
class Hyd_Element_Bound_GW_Members
{

	///Class where the members are required
	friend class Hyd_Element_Groundwater_Type_Standard;

public:
	///Default constructor
	Hyd_Element_Bound_GW_Members(void);
	///Default destructor
	~Hyd_Element_Bound_GW_Members(void);

	//method
	//Initialize the data structures of _hyd_boundarydata_groundwater_elem 
	static void init_boundarydata_structures(_hyd_boundarydata_groundwater_elem * data);

	///Reset volumes and discharge
	void reset_vol_q(void);

	///Calculate discharge boundary value
	void calculate_q_bound_value(const double time, const double area);
	///Calculate hydrological balance
	void calculate_q_hydro_balance(const double delta_t);

	///Calculate discharge boundary value
	void calculate_h_bound_value(const double time);
	///calculation of hydrological balance for the h boundary condition in the Type_Standard.cpp
	/*///Calculate hydrological balance
	void calculate_h_hydro_balance(const double delta_t);*/


private:
	//members
	///Data structure of the boundary data of the element (_hyd_boundarydata_groundwater_elem) [boundarycondition variables]
	_hyd_boundarydata_groundwater_elem boundary_data;
	///Value of the h boundary [boundarycondition variables]
	double h_boundary;
	///calculation of hydrological balance for the h boundary condition in the Type_Standard.cpp

	///Value of the q boundary [boundarycondition variables]
	double q_boundary;
	///Make a hydrological balance for the q boundary condition discharge [boundarycondition variables]
	_hyd_hydrological_balance q_boundary_volume;
};

#endif