#pragma once
/**\class Hyd_Model_Groundwater
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_MODEL_GROUNDWATER_H
#define HYD_MODEL_GROUNDWATER_H

// system hyd libs
//Base class of hydraulic models
#include "_Hyd_Model.h"
//container class for the Groundwater parameter
#include "Hyd_Param_GW.h"
//class of 2d elemnts
#include "Hyd_Element_Groundwater.h"
//class of noflow polygons
#include "Hyd_Groundwater_Polygon.h"
//class of polysegments review
#include "Hyd_Groundwater_Polysegment.h"
//class of the groundwater raster
#include "Hyd_Groundwater_Raster.h"
//class for counting the couplings
#include "Hyd_Coupling_Model_List.h"
//info class about the hydraulic boundary szenario data
#include "Hyd_Boundary_Szenario.h"

//system_sys_ classes
#include "Geo_Point.h"
#include "Geo_Simple_Polygon.h"



///Model class for a hydraulic groundwater model \ingroup hyd
/**
The groundwater model is a raster based model.

*/
class Hyd_Model_Groundwater : public _Hyd_Model
{
	//class for displaying the data
	friend class Sys_Data_Tree_Wid;

public:

	///Default constructor
	Hyd_Model_Groundwater(void);
	///Default destructor
	~Hyd_Model_Groundwater(void);

	//members

	///Data container for the model members
	Hyd_Param_GW Param_GW;
	///The geometrical raster of the groundwater
	Hyd_Groundwater_Raster raster;
	///The groundwater elements
	Hyd_Element_Groundwater *groundwater_elems;
	///Member for storing the hydraulic boundary szenario information
	Hyd_Boundary_Szenario hyd_sz;

	///Pointer to the table for the general parameters in a database
	static Tables *general_param_table;

	//methods
	///Input the groundwater model with the index per file with parser; the general settings as well as the element data
	void input_members(const string global_file, const int index, const string global_path);
	///Set the members of the groundwater model directly
	void set_members_directly(const string name, const int index, const int no_elem_x, const int no_elem_y, const double width, const double x_coor, const double y_coor, const double noinfo);

	///Transfer the groundwater model data to a database; the general settings as well as the element data
	void transfer_input_members2database(QSqlDatabase *ptr_database);
	///Transfer a hydraulic boundary szenario from file to a database
	void transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database);
	///Input the relevant groundwater model with the index from database selection; the general settings as well as the element data
	void input_members(const int index, const QSqlTableModel *query_result, QSqlDatabase *ptr_database, const bool just_elems, const bool relevant, const bool output_flag = true);

	///Create the database table for the general parameters of the groundwater model
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the general parameters of the groundwater model: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the groundwater model parameters as well as the groundwater elements data
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the general parameters of the groundwater model
	static void close_table(void);
	///Select and count the number of relevant groundwater models in a database table
	static int select_relevant_model_database(QSqlTableModel *results, const bool with_output = true);
	///Select and count the number of relevant groundwater models in a database table with a given groundwater number
	static int select_relevant_model_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int fp_no);
	///Check the number of relevant groundwater models in a database table
	static int check_relevant_model_database(QSqlDatabase *ptr_database);

	///Clone the members of the groundwater model
	void clone_model(Hyd_Model_Groundwater *groundwater, Hyd_Param_Conductivity *conductivity_table, Hyd_Param_Porosity *porosity_table);

	///Initialize the groundwater elements
	void init_groundwater_model(Hyd_Param_Conductivity *conductivity_table, Hyd_Param_Porosity *porosity_table);

	///Compare the equality of two groundwater models in terms of geometrical settings and the number of elements
	void compare_models(Hyd_Model_Groundwater *compare_model);

	///Set a new hydraulic boundary scenario id to the groundwater models
	void set_new_hyd_bound_sz_id(Hyd_Boundary_Szenario new_sz);

	///Intercept the model with other models (Hyd_Model_Groundwater)
	bool intercept_model(Hyd_Model_Groundwater *other_model, _hyd_model_coupling *indexes);
	///Intercept the model with other models (Hyd_Model_River)
	bool intercept_model(Hyd_Model_River *river_model);

	///Get a pointer to the polylines of a model and the number
	Hyd_Groundwater_Polysegment *get_polylines(int *number);
	///Get a pointer to the noflow polygons of a model and the number
	//Only tested for coupling yet
	Hyd_Groundwater_Polygon *get_noflow_polygons(int *number);

	///Get the number of elements
	int get_number_elements(void);

	///Clear all not needed data from this model such as geometry etc.
	void clear_model(void);

	///Reset the model to the state before the calculation after the preproccesing
	void reset_model(Hyd_Param_Global *global_params);

	///Initialize the solver with the given parameters
	void init_solver(Hyd_Param_Global *global_params);
	///Reinitialize the solver
	void reinit_solver(Hyd_Param_Global *global_params);

	///Clear the boundary condition, which are set; boundary conditions due to interception are not cleared
	void clear_boundary_condition(const bool with_output = true);
	///Set new boundary condition per database for an new hydraulic boundary scenario; the new boundary scenario has to be specified before 
	void set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database);

	///Make the syncronisation between the models and the boundaries
	void make_syncronisation(const double time_point);
	///Get the maximum change in a element
	void get_max_changes_elements(double *max_change_h, const bool timecheck);
	///Calculate the hydrological balance and the maximum values of the model
	void make_hyd_balance_max(const double time_point);

	///Reset the solver of the model
	void reset_solver(void);
	///Solve the model
	void solve_model(const double next_time_point, const string system_id);

	///Output the given members
	void output_members(void);
	///Output the setted and calculated members
	void output_setted_members(void);
	///Output the geometrie to tecplot
	//We only work with Paraview for now
	void output_geometrie2tecplot(void);
	///Output the geometrie to BlueKenue file
	//We only work with Paraview for now
	void output_geometrie2bluekenue(void);
	///Output the geometrie to Paraview file
	void output_geometrie2paraview(void);

	///Output the result members per timestep
	void output_result_members_per_timestep(void);
	///Output the result members per timestep to tecplot
	// We only work with Paraview for now
	void output_result2tecplot(const double timepoint, const int timestep_number,string unit);
	///Output the result members per timestep to r2c file used in BlueKenue
	// We only work with Paraview for now
	void output_result2bluekenue(const double timepoint, const int timestep_number, const double start_time);
	///Output the result members per timestep to Paraview
	void output_result2paraview(const double timepoint, const int timestep_number,string unit);

	///Output the result members per timestep to database
	void output_result2database(QSqlDatabase *ptr_database, const double timepoint, const int timestep_number, const string time);

	///Output solver errors for one solver step
	void output_solver_errors(const double time_point, const int step_counter, const string timestring, const string realtime, const double diff_time, const int total_internal, const int internal_steps);
	///Output final statistics of the groundwater model
	void output_final(void);
	///Output the maximum result members to tecplot
	// We only work with Paraview for now
	void output_result_max2tecplot(void);
	///Output the maximum result members to bluekenue
	// We only work with Paraview for now
	void output_result_max2bluekenue(void);
	///Output the maximum result members to paraview
	void output_result_max2paraview(void);
	///Output the maximum result members to a database table
	void output_result_max2database(QSqlDatabase *ptr_database, bool *was_output, const bool must_output);

	///Set the folder for the file-output in case of an database based calculation
	void set_output_folder(const string folder);
	///Get a pointer to a groundwater element by a given index
	Hyd_Element_Groundwater* get_ptr_groundwater_elem(const int index);

	///Set the indices of a element a the 8 surrounding elements (array of int with 9 elements);
	void set_relevant_elem_indices(const int current_id, int *elem_index);

	///Input elements per database
	void input_elems_database(QSqlDatabase *ptr_database, QSqlQueryModel *bound_result, const int number_bound, const bool output_flag, const bool just_elems);

	///Delete emlements and raster geomtry
	void delete_elems_raster_geo(void);

	///Compare the equality of two hydraulic systems in terms of model numbers; further the models are compared
	void transfer_glob_elem_id_gw(Hyd_Model_Groundwater *to_gw);

private:
	//members

	///Noflow-polygons
	Hyd_Groundwater_Polygon *noflow_polygons;

	///Line-polysegments
	Hyd_Groundwater_Polysegment *line_polysegments;

	//Optimizing the cache
	///Waterlevel
	double **opt_h;
	///Change of waterlevel
	double **opt_dsdt;
	///Geodetic height
	double **opt_z;
	///Waterlevel + Geodetic height
	double **opt_s;
	double *opt_s_linear;
	//Conveancy in x-direction
	double **opt_cx;
	//Conveancy in y-direction
	double **opt_cy;
	///Maximum heigt in x-direction
	double **opt_zxmax;
	///Maximum heigt in y-direction
	double **opt_zymax;

	///Index in y-direction reduced to the equation to solve
	int *id_y;

	///Number of applied h boundary condition
	int number_hbound_cond;
	///Number of applied q boundary condition
	int number_qbound_cond;
	///Number of applied coupling condition
	int number_coup_cond;
	///Index of element where a h boundary condition is applied
	int *hbound_cond_id;
	///Index of element where a q boundary condition is applied
	int *qbound_cond_id;
	///Index of element where a coupling condition is applied
	int *coup_cond_id;

	///Index of element where a h boundary condition is applied reduced to the equation to solve
	int *hbound_cond_rid;
	///Index of element where a q boundary condition is applied reduced to the equation to solve
	int *qbound_cond_rid;
	///Index of element where a coupling condition is applied reduced to the equation to solve
	int *coup_cond_rid;

	///ds to dt due to h boundary condition
	double *hbound_cond_dsdt;
	///ds to dt due to q boundary condition
	double *qbound_cond_dsdt;
	///ds to dt due to coupling condition
	double *coup_cond_dsdt;

	///Flag if the element is a flow element
	bool **flow_elem;

	///Flag if the element has flow in x-direction
	bool **noflow_x;
	///Flag if the element has flow in y-direction
	bool **noflow_y;

	///Area of the element
	double elem_area;




	///The time point the syncronisation step before
	double old_time_point_syncron;

	///Number of noinfo elements
	int number_noinfo_elems;
	///Number of noflow elements
	int number_noflow_elems;
	///Number of groundwater intercepted elements
	int number_gwinside_elems;
	///Number of river elements
	int number_river_elems;
	///Number of line elements
	int number_line_elems;
	///Number of standard elements
	int number_standard_elems;

	///Total hydrological balance of the boundary discharges to the elements
	_hyd_hydrological_balance total_boundary_volume;
	///Total hydrological balance of the coupling discharges to the elements due to a coupling between two groundwater models
	_hyd_hydrological_balance total_coupling_gw_volume;
	///Total hydrological balance of the coupling discharges to the elements due to river coupling via a hydraulic structure REVIEW
	_hyd_hydrological_balance total_structure_coupling_rv_volume;
	///Total hydrological balance of the coupling discharges to the elements due to a direct river outflow REVIEW
	_hyd_hydrological_balance total_direct_coupling_rv_volume;
	///Total hydrological balance of the coupling discharges to the elements due to a leakage
	_hyd_hydrological_balance total_leakage_coupling_rv_volume;
	///Total volume error due to outflow of an element, if there is no more watervolume left in the element
	double total_volume_error_zero_outflow;

	///The error in the hydrological balance due to a setted outflow but no watervolume is left in the element
	double error_zero_outflow_volume;
	///Error discharge due to a setted outflow but no watervolume is left in the element
	double q_zero_outflow_error;
	///Error discharge due to a setted outflow but no watervolume is left in the element
	double h_zero_outflow_error;


	///The old timepoint (one solver step before) for the error volume calculation due to a setted outflow but no watervolume is left in the element
	double old_timepoint_error;
	///The delta time to the old time step (t-1) for the error volume calculation due to a setted outflow but no watervolume is left in the element
	double old_delta_t_error;
	///The discharge of the old time step (t-1) for the error volume calculation due to a setted outflow but no watervolume is left in the element
	double old_discharge_error;
	///The discharge of the old time step (t-2) for the error volume calculation due to a setted outflow but no watervolume is left in the element
	double old_discharge_2_error;

	// methods
	///Generate the geometrical boundary of the raster polygon
	void generate_geo_bound_raster_polygon(void);

	///Allocate the elements
	void allocate_elems(void);
	///Delete the elements
	void delete_elems(void);

	///Allocate the optimized data
	void allocate_opt_data(void);
	///Delete the optimized data
	void delete_opt_data(void);

	///Allocate the optimized data for coupling condition
	void allocate_opt_data_coup(void);
	///Delete the optimized data for coupling condition
	void delete_opt_data_coup(void);
	///Allocate the optimized data for boundary condition
	void allocate_opt_data_bound(bool flag);
	///Delete the optimized data for boundary condition
	void delete_opt_data_bound(bool flag);

	///Allocate the reduced optimized data
	void allocate_opt_data_reduced(void);
	///Delete the reduced optimized data
	void delete_opt_data_reduced(void);

	///Initialize the optimized data
	void init_opt_data(void);
	///Initialize the optimized data for boundary and coupling
	void init_opt_data_bound_coup(void);
	///Update the optimized data by the elements per syncronization step
	void update_opt_data_by_elems(void);
	///Update the elements by the optimized data  per syncronization step
	void update_elems_by_opt_data(void);

	///Initialize the reduced id
	void init_reduced_id(void);
	///Calculate the reduced id by a given id
	int calc_reduced_id(const int id);

	///Calculate and set maximum step size
	void calc_set_max_step_size(const double next_time_point);

	///Read in the groundwater elements: nr, z-value, con-type, por-type, init condition
	void read_elems(void);
	///Transfer the groundwater element data to a database
	void transfer_element_members2database(QSqlDatabase *ptr_database);
	///Transfer a hydraulic boundary szenario from file to a database per element
	void transfer_hydraulic_boundary_sz2database_per_elem(QSqlDatabase *ptr_database);

	///Set the geometry of the elements (area and mid point)
	void set_elem_geometry(void);

	///Initialize the elements
	void init_elements( Hyd_Param_Conductivity *conductivity_table, Hyd_Param_Porosity *porosity_table, const bool clone);

	///Set the init condition to the result vector
	void set_initcond2resultvector(void);

	///Allocate the instationary boundary curve
	void allocate_instat_boundarycurves(void);
	///Read the instationary boundary curves per file
	void read_instationary_boundary_file(void);
	///Read in the instationary boundary curves from a database
	void read_instat_boundary_curves(const bool output_flag, QSqlDatabase *ptr_database);
	///Output the instationary boundary curves
	void output_instationary_boundary_curves(void);
	///Connect the elements to the instationary boundary curves
	void connect_elems2instat_boundarycurves(void);

	///Allocate the noflow polygon
	void allocate_noflow_polygons(void);
	///Read the noflow polygons per file
	void read_noflow_polygons_file(void);
	///Read in the noflow polygons from a database
	void read_noflow_polygons_database(const bool output_flag, QSqlDatabase *ptr_database);
	///Output the noflow polygons
	void output_noflow_polygons(void);
	///Delete the noflow polygons
	void delete_noflow_polygons(void);
	///Connect the elements with the noflow polygons
	void connect_elems2noflowpolygons(void);

	///Allocate the line polysegments
	void allocate_line_polysegments(void);
	///Read the line polysegments per file
	void read_line_polysegments_file(void);
	///Read in the line polysegments from a database
	void read_line_polysegments_database(const bool output_flag, QSqlDatabase *ptr_database);
	///Output the line polysegments
	void output_line_polysegments(void);
	///Delete the line polysegments
	void delete_line_polysegments(void);
	///Connect the elements with the line polysegments
	void connect_elems2line_polysegments(Hyd_Param_Conductivity* conductivity_table);
	///Set the neighbouring elements with a given element index
	Hyd_Element_Groundwater * set_neighbouring_elements(const int index, _hyd_neighbouring_direction_gw dir);

	///Set function to solve to the solver
	void set_function2solver(void);
	///Output the statistics of the solver
	void output_solver_statistics(void);
	///Output the maximum values of the elements
	void output_maximum_values(void);

	///Count element types
	void count_element_types(void);

	///Output the hydrological balance of the groundwater model
	void output_hydrological_balance(void);
	///Calculate the hydrological balance of the groundwater model
	void calculate_total_hydrological_balance(void);
	///Reset the total hydrological balance values
	void reset_total_hydrological_balance(void);

	///Get the model description
	string get_model_description(void);

	/*
	//Get a noflow line from mid up to down of the geometrical boundary (protection by fp interception) REVIEW
	//To my knowledge these are not used anywhere and i do not see what their purpose is
	void get_mid_up2down_noflowline_geo_bound(Hyd_Groundwater_Polysegment *polyline);
	///Set a noflow line from mid left to right of the geometrical boundary (protection by fp interception) REVIEW
	void get_mid_left2right_noflowline_geo_bound(Hyd_Groundwater_Polysegment *polyline);
	*/

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

	///This equation is the differential equation which is to solve
	friend int __cdecl f2D_equation2solve_gw(realtype time, N_Vector results, N_Vector ds_dt, void *groundwater_data);
	//static int __cdecl f2D_equation2solve(realtype time, N_Vector results, N_Vector ds_dt, void *groundwater_data);
	//friend static CVRhsFn f2D_equation2solve(realtype time, N_Vector results, N_Vector ds_dt, void *groundwater_data);
};

int __cdecl f2D_equation2solve_gw(realtype time, N_Vector results, N_Vector ds_dt, void *groundwater_data);
#endif

