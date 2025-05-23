#pragma once
/**\class Hyd_Param_Conductivity
	\author Bastian Winkels
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.10.0
	\date 2022
*/
#ifndef HYD_PARAM_CONDUCTIVITY_H
#define HYD_PARAM_CONDUCTIVITY_H

//system_sys_ classes
#include "_Sys_Common_System.h"

//enum
///Enumerator for the conductivity types (Kf, ...) \ingroup hyd
enum _hyd_con_coefficient_types {
	///Darcy K value for hydraulic conductivity [
	Kf_coefficient,
	
	
};

///Data container for one conductivity parameter: the value, the type (_hyd_con_coefficient_types) and a identifying number are stored \ingroup hyd
struct _hyd_conductivity_params {
	///Value of the conductivity parameter 
	double value;
	///Identifying number
	int type_number;
	//Type of the conductivity parameter (_hyd_con_coefficient_types)
	_hyd_con_coefficient_types type;
};
///Container class for the conductivity parameters of the hydraulic system (Hyd_System_Hydraulic) \ingroup hyd
/**
	This is a data container for the material members. This parameters are global. They are relevant for the elements
	of a groundwater model (Hyd_Groundwater_Element).
	Besides data storing it handles:
		- the input per file parser,
		- the input per database,
		- the output of the members,
		- transfering the material type id to the material value, connected with a check of the material value

	\see Hyd_Groundwater_Element
*/
class Hyd_Param_Conductivity
{
	///Part of the members, which are stored in Hyd_Param_Global are used in _Hyd_Model, like the integration settings or  preconditioner settings for the cvode-solver
	friend class _Hyd_Model;
	///Main part of the members, which are stored in Hyd_Param_Global are used in Hyd_Hydraulic_System
	friend class Hyd_Hydraulic_System;
public:
	///Default constructor
	Hyd_Param_Conductivity(void);
	///Copy constructor
	Hyd_Param_Conductivity(Hyd_Param_Conductivity &par);
	///Default destructor
	~Hyd_Param_Conductivity(void);

	//members


	//methods

	///Set the filename, where the conductivity parameters are stored
	void set_filename(const string filename);
	///Get the conductivity parameter corresponding to conductivity type and the identifying number
	double get_con_value(const int con_number, _hyd_con_coefficient_types);
	///Read the conductivity parameters from file
	void read_values_per_file(void);

	///Create the database table for the conductivity data of the hydraulic system
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the conductivity parameters: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the conductivity parameters
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the conductivity parameters
	static void close_table(void);

	///Transfer the conductivity parameters of a file into a database table
	void transfer_conductivityparams_file2database_table(QSqlDatabase *ptr_database, const string con_file);
	///Transfer just one dummy conductivity parameter to database 
	void transfer_dummy_params2database_table(QSqlDatabase *ptr_database);

	///Set the conductivity parameters per database
	void conparams_per_database(QSqlDatabase *ptr_database, const bool with_output = true);

	///Output the members
	void output_members(void);

	///Total reset of the conductivity parameters
	void total_reset(void);

	///Copy operator
	Hyd_Param_Conductivity& operator= (const Hyd_Param_Conductivity& par);


private:
	//members

	///Pointer to the table for the conductivity parameters in a database
	static Tables *con_param_table;

	///Number of the global conductivity parameters
	int no_values;
	///Filename where the conductivity parameters are stored
	string filename;
	///Pointer to the array with the conductivity parameters
	_hyd_conductivity_params *conductivity_params;

	///Default value for the kf coefficient
	const double default_kf;

	//methods

	///Allocate the conductivity parameters for the number of values:
	void alloc_con_array(void);
	///Convert a string to an enumerator con_coefficient_types
	_hyd_con_coefficient_types convert_txt2coefficienttype(const string txt);
	///Convert an enumerator con_coefficient_types to a string
	string convert_coefficienttype2txt(_hyd_con_coefficient_types type);

	///Transfer the conductivity parameters to a database table
	void conductivity_params2database(QSqlDatabase *ptr_database);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
