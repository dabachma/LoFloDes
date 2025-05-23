#pragma once
/**\class Hyd_Param_Porosity
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_PARAM_POROSITY_H
#define HYD_PARAM_POROSITY_H

//system_sys_ classes
#include "_Sys_Common_System.h"

//enum
///Enumerator for the porosity types \ingroup hyd
enum _hyd_por_coefficient_types {
	///effective porosity [
	effective_porosity,

};

///Data container for one porosity parameter: the value, the type (_hyd_por_coefficient_types) and a identifying number are stored \ingroup hyd
struct _hyd_porosity_params {
	///Value of the porosity parameter 
	double value;
	///Identifying number
	int type_number;
	//Type of the porosity parameter (_hyd_por_coefficient_types)
	_hyd_por_coefficient_types type;
};
///Container class for the porosity parameters of the hydraulic system (Hyd_System_Hydraulic) \ingroup hyd
/**
	This is a data container for the porosity members. This parameters are global. They are relevant for the elements
	of a groundwater model (Hyd_Groundwater_Element).
	Besides data storing it handles:
		- the input per file parser,
		- the input per database,
		- the output of the members,
		- transfering the porosity type id to the porosity value, connected with a check of the material value

	\see Hyd_Groundwater_Element
*/
class Hyd_Param_Porosity
{
	///Part of the members, which are stored in Hyd_Param_Global are used in _Hyd_Model, like the integration settings or  preconditioner settings for the cvode-solver
	friend class _Hyd_Model;
	///Main part of the members, which are stored in Hyd_Param_Global are used in Hyd_Hydraulic_System
	friend class Hyd_Hydraulic_System;
public:
	///Default constructor
	Hyd_Param_Porosity(void);
	///Copy constructor
	Hyd_Param_Porosity(Hyd_Param_Porosity &par);
	///Default destructor
	~Hyd_Param_Porosity(void);

	//members


	//methods

	///Set the filename, where the porosity parameters are stored
	void set_filename(const string filename);
	///Get the porosity parameter corresponding to porosity type and the identifying number
	double get_por_value(const int por_number, _hyd_por_coefficient_types);
	///Read the porosity parameters from file
	void read_values_per_file(void);

	///Create the database table for the porosity data of the hydraulic system
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the porosity parameters: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the porosity parameters
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the porosity parameters
	static void close_table(void);

	///Transfer the porosity parameters of a file into a database table
	void transfer_porosityparams_file2database_table(QSqlDatabase *ptr_database, const string con_file);
	///Transfer just one dummy porosity parameter to database 
	void transfer_dummy_params2database_table(QSqlDatabase *ptr_database);

	///Set the porosity parameters per database
	void porparams_per_database(QSqlDatabase *ptr_database, const bool with_output = true);

	///Output the members
	void output_members(void);

	///Total reset of the porosity parameters
	void total_reset(void);

	///Copy operator
	Hyd_Param_Porosity& operator= (const Hyd_Param_Porosity& par);


private:
	//members

	///Pointer to the table for the porosity parameters in a database
	static Tables *por_param_table;

	///Number of the global porosity parameters
	int no_values;
	///Filename where the porosity parameters are stored
	string filename;
	///Pointer to the array with the porosity parameters
	_hyd_porosity_params *porosity_params;

	///Default value for the effective porosity	
	const double default_p;

	//methods

	///Allocate the porosity parameters for the number of values:
	void alloc_por_array(void);
	///Convert a string to an enumerator con_coefficient_types
	_hyd_por_coefficient_types convert_txt2coefficienttype(const string txt);
	///Convert an enumerator con_coefficient_types to a string
	string convert_coefficienttype2txt(_hyd_por_coefficient_types type);

	///Transfer the porosity parameters to a database table
	void porosity_params2database(QSqlDatabase *ptr_database);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
