//#include "Hyd_Model_Groundwater.h"
#include "Hyd_Headers_Precompiled.h"
#include <omp.h>
//static double my_time = 0;
//static int my_c = 0;

//init static members
Tables *Hyd_Model_Groundwater::general_param_table = NULL;

//constructor
Hyd_Model_Groundwater::Hyd_Model_Groundwater(void) {


	this->groundwater_elems = NULL;
	this->noflow_polygons = NULL;
	this->line_polysegments = NULL;

	//different element types (not all in use)
	this->number_noinfo_elems = 0;
	this->number_noflow_elems = 0;
	this->number_line_elems = 0;
	this->number_river_elems = 0;
	this->number_standard_elems = 0;
	this->number_gwinside_elems = 0;

	this->reset_total_hydrological_balance();
	this->error_zero_outflow_volume = 0.0;

	//data from calculations
	this->opt_h = NULL;
	this->opt_dsdt = NULL;
	this->opt_z = NULL;
	this->opt_s = NULL;
	this->opt_s_linear = NULL;
	this->opt_cx = NULL;
	this->opt_cy = NULL;
	this->opt_zxmax = NULL;
	this->opt_zymax = NULL;

	//is an element a flow element, are the neighbours flow elements (looking in positive x and y index direction)
	this->flow_elem = NULL;
	//x+1 element
	this->noflow_x = NULL;
	//y+1 element
	this->noflow_y = NULL;

	//boundaries and couplings
	this->number_hbound_cond = 0;
	this->number_hbound_cond = 0;
	this->number_coup_cond = 0;
	
	this->hbound_cond_id = NULL;
	this->qbound_cond_id = NULL;
	this->coup_cond_id = NULL;
	
	this->hbound_cond_dsdt = NULL;
	this->qbound_cond_dsdt = NULL;
	this->coup_cond_dsdt = NULL;

	this->id_y = NULL;

	this->hbound_cond_rid = NULL;
	this->qbound_cond_rid = NULL;
	this->coup_cond_rid = NULL;

	this->elem_area = 0.0;

	this->old_time_point_syncron = 0.0;

	this->error_zero_outflow_volume = 0.0;
	this->q_zero_outflow_error = 0.0;
	this->old_timepoint_error = 0.0;
	this->old_delta_t_error = 0.0;
	this->old_discharge_error = 0.0;
	this->old_discharge_2_error = 0.0;
	//default 4
	this->ilu_number = 4;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Model_Groundwater) - sizeof(Hyd_Groundwater_Raster) - sizeof(Hyd_Param_GW), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Model_Groundwater::~Hyd_Model_Groundwater(void) {
	this->delete_elems();
	this->delete_opt_data();
	this->delete_opt_data_reduced();
	this->delete_opt_data_bound(true);
	this->delete_opt_data_bound(false);
	this->delete_opt_data_coup();
	this->delete_noflow_polygons();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Model_Groundwater) - sizeof(Hyd_Groundwater_Raster) - sizeof(Hyd_Param_GW), _sys_system_modules::HYD_SYS);
}
//_________________________________
//public
//input all members groundwater global and elems specific from files
void Hyd_Model_Groundwater::input_members(const string global_file, const int index, const string global_path) {
	//set prefix for output
	ostringstream cout;
	ostringstream prefix;
	prefix << "GW_" << index << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try {
		//read in the global gw params
		this->Param_GW.parameter_per_parser(global_file, index, global_path);
		//check it
		this->Param_GW.check_members();

		//read in the element members
		cout << "Read in " << this->Param_GW.GWNofX*this->Param_GW.GWNofY << " elements of the Groundwatermodel from file " << this->Param_GW.groundwater_file << " ..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		this->generate_geo_bound_raster_polygon();
		//allocate and init the data for the groundwater
		this->allocate_elems();
		//read in the element data into a data buffer
		this->read_elems();
		//set the geometry of the elements
		this->set_elem_geometry();

		//set the groundwater elements to the raster
		raster.set_groundwater_elem_pointer(this->groundwater_elems);
		//set the points of the groundwater raster
		this->raster.generate_alloc_points_segments();

		//noflow polygons
		this->read_noflow_polygons_file();

		//instationary boundary 
		this->read_instationary_boundary_file();

		//read lines
		this->read_line_polysegments_file();
	}
	catch (Error msg) {
		ostringstream info;
		info << "Groundwatermodelnumber: " << this->Param_GW.GWNumber << endl;
		info << "Groundwatermodelname  : " << this->Param_GW.GWName << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Set the members of the groundwater model directly
void Hyd_Model_Groundwater::set_members_directly(const string name, const int index, const int no_elem_x, const int no_elem_y, const double width, const double x_coor, const double y_coor, const double noinfo) {
	//set the global groundwater parameters
	this->Param_GW.GWName = name;
	this->Param_GW.GWNumber = index;
	this->Param_GW.GWNofX = no_elem_x;
	this->Param_GW.GWNofY = no_elem_y;
	this->Param_GW.width_x = width;
	this->Param_GW.width_y = width;
	this->Param_GW.GWLowLeftX = x_coor;
	this->Param_GW.GWLowLeftY = y_coor;
	this->Param_GW.noinfo_value = noinfo;
	this->generate_geo_bound_raster_polygon();
	//allocate and init the data for the groundwater
	this->allocate_elems();
}
//Transfer the groundwater model data to a database; the general settings as well as the element data
void Hyd_Model_Groundwater::transfer_input_members2database(QSqlDatabase *ptr_database) {
	//set prefix for output
	ostringstream prefix;
	prefix << "GW_" << this->Param_GW.GWNumber << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;

	cout << "Transfer general parameters of the groundwater model..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);


	//mysql query with the table_model
	QSqlTableModel model(0, *ptr_database);
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Model_Groundwater::set_table(ptr_database);
	}
	catch (Error msg) {
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}

	//give the table name to the query model
	model.setTable((Hyd_Model_Groundwater::general_param_table->get_table_name()).c_str());

	//insert row
	model.insertRows(0, 1);

	//insert data
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::genmod_id)).c_str())), this->Param_GW.GWNumber);
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::atol)).c_str())), this->Param_GW.abs_tolerance);
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::rtol)).c_str())), this->Param_GW.rel_tolerance);
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::d2output)).c_str())), this->folder_output_db.c_str());
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::genmod_name)).c_str())), this->Param_GW.GWName.c_str());
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::nx)).c_str())), this->Param_GW.GWNofX);
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::ny)).c_str())), this->Param_GW.GWNofY);
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::elemwidth_x)).c_str())), this->Param_GW.width_x);
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::elemwidth_y)).c_str())), this->Param_GW.width_y);
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::angle)).c_str())), this->Param_GW.angle);
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::lowleftx)).c_str())), this->Param_GW.GWLowLeftX);
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::lowlefty)).c_str())), this->Param_GW.GWLowLeftY);
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::noinfovalue)).c_str())), this->Param_GW.noinfo_value);
	model.setData(model.index(0, model.record().indexOf((Hyd_Model_Groundwater::general_param_table->get_column_name(label::applied_flag)).c_str())), true);
	//submit it to the database
	Data_Base::database_submit(&model);
	if (model.lastError().isValid()) {
		Warning msg = this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Hyd_Model_Groundwater::general_param_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Model groundwater number   : " << this->Param_GW.GWNumber << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}

	//transfer groundwater elements to database
	this->transfer_element_members2database(ptr_database);
	this->transfer_hydraulic_boundary_sz2database_per_elem(ptr_database);

	//transfer noflow polygons
	for (int i = 0; i < this->Param_GW.number_noflow_polys; i++) {
		this->noflow_polygons[i].transfer_polygons2database_table(ptr_database, this->Param_GW.get_groundwater_number());
	}

	//transfer instationary boundary curves
	for (int i = 0; i < this->Param_GW.number_instat_boundary; i++) {
		this->instat_boundary_curves_gw[i].transfer_instat_curve2database_table(ptr_database, _hyd_model_type_gw::GW_GROUNDWATER_MODEL, this->Param_GW.get_groundwater_number());
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Transfer a hydraulic boundary szenario from file to a database
void Hyd_Model_Groundwater::transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database) {
	/**
	This transfer includes just the boundary conditions of the elements (Hyd_Element_Groundwater)
	and the instationary boundary curves (Hyd_Instationary_Boundary). The id of the hyraulic boundary
	scenario has to be set before.
	*/

	//set prefix for output
	ostringstream prefix;
	prefix << "GW_" << this->Param_GW.GWNumber << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Transfer hydraulic boundary scenario of groundwater model " << this->Param_GW.GWNumber << " ..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	try {
		//for(int i=0; i< this->Param_GW.GWNofY*this->Param_GW.GWNofX; i++){
			//Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			//this->groundwater_elems[i].transfer_hydraulic_boundary_sz2database(ptr_database, this->Param_GW.GWNumber);
		//}
		this->transfer_hydraulic_boundary_sz2database_per_elem(ptr_database);


		//transfer instationary boundary curves
		for (int i = 0; i < this->Param_GW.number_instat_boundary; i++) {
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->instat_boundary_curves_gw[i].transfer_instat_curve2database_table(ptr_database, _hyd_model_type_gw::GW_GROUNDWATER_MODEL, this->Param_GW.GWNumber);
		}
	}
	catch (Error msg) {
		ostringstream info;
		info << "Groundwater model number        : " << this->Param_GW.GWNumber << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Input the relevant groundwater model with the index from database selection; the general settings as well as the element data
void Hyd_Model_Groundwater::input_members(const int index, const QSqlTableModel *query_result, QSqlDatabase *ptr_database, const bool just_elems, const bool, const bool output_flag) {
	//set the groundwater number
	this->Param_GW.GWNumber = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::genmod_id)).c_str()).toInt();

	if (output_flag == true) {
		ostringstream cout;
		cout << "Read in groundwater model " << this->Param_GW.GWNumber << " from database..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//set prefix for output
		ostringstream prefix;
		prefix << "GW_" << this->Param_GW.GWNumber << "> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	}

	//read out the results
	//solver tolerances
	this->Param_GW.abs_tolerance = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::atol)).c_str()).toDouble();
	this->Param_GW.rel_tolerance = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::rtol)).c_str()).toDouble();
	//name
	this->Param_GW.GWName = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::genmod_name)).c_str()).toString().toStdString();
	//gemetrical settings
	this->Param_GW.GWNofX = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::nx)).c_str()).toInt();
	this->Param_GW.GWNofY = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::ny)).c_str()).toInt();
	this->Param_GW.width_x = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::elemwidth_x)).c_str()).toDouble();
	this->Param_GW.width_y = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::elemwidth_y)).c_str()).toDouble();
	this->Param_GW.GWLowLeftX = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::lowleftx)).c_str()).toDouble();
	this->Param_GW.GWLowLeftY = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::lowlefty)).c_str()).toDouble();
	this->Param_GW.angle = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::angle)).c_str()).toDouble();
	this->Param_GW.noinfo_value = query_result->record(index).value((Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::noinfovalue)).c_str()).toDouble();
	this->Param_GW.calculate_area();

	//read in the elements
	try {
		//check the general gw-members
		this->Param_GW.check_members();
		this->generate_geo_bound_raster_polygon();

		if (output_flag == true && just_elems == false) {
			ostringstream cout;
			cout << "Read in " << this->Param_GW.get_no_elems_x()*this->Param_GW.get_no_elems_y() << " elements of the groundwater model from database..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}

		QSqlQueryModel bound_result;
		int number_bound = 0;
		if (just_elems == false) {
			number_bound = Hyd_Element_Groundwater::select_relevant_boundary_cond_database(&bound_result, ptr_database, this->system_id, this->hyd_sz.get_id());



			this->input_elems_database(ptr_database, &bound_result, number_bound, output_flag, just_elems);

			//read them in
			//instationary boundary
			this->read_instat_boundary_curves(output_flag, ptr_database);
			//noflow polygons
			this->read_noflow_polygons_database(output_flag, ptr_database);
		}
	}
	catch (Error msg) {
		ostringstream info;
		info << "Groundwater number: " << this->Param_GW.GWNumber << endl;
		info << "Groundwater name  : " << this->Param_GW.GWName << endl;
		msg.make_second_info(info.str());
		if (output_flag == true) {
			//rewind the prefix
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		throw msg;
	}
	if (output_flag == true) {
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Create the database table for the general parameters of the groundwater model (static)
void Hyd_Model_Groundwater::create_table(QSqlDatabase *ptr_database) {
	if (Hyd_Model_Groundwater::general_param_table == NULL) {
		ostringstream cout;
		cout << "Create general groundwater model data database table..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name = hyd_label::tab_gw_gen;
		const int num_col = 15;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for (int i = 0; i < num_col; i++) {
			tab_col[i].key_flag = false;
			tab_col[i].unsigned_flag = false;
			tab_col[i].primary_key_flag = false;
		}

		tab_col[0].name = hyd_label::genmod_id;
		tab_col[0].type = sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag = true;
		tab_col[0].primary_key_flag = true;

		tab_col[1].name = hyd_label::genmod_name;
		tab_col[1].type = sys_label::tab_col_type_string;

		tab_col[2].name = label::applied_flag;
		tab_col[2].type = sys_label::tab_col_type_bool;
		tab_col[2].default_value = "true";

		tab_col[3].name = hyd_label::atol;
		tab_col[3].type = sys_label::tab_col_type_double;
		tab_col[3].unsigned_flag = true;
		tab_col[3].default_value = "5e-7";

		tab_col[4].name = hyd_label::rtol;
		tab_col[4].type = sys_label::tab_col_type_double;
		tab_col[4].unsigned_flag = true;
		tab_col[4].default_value = "5e-7";

		tab_col[5].name = hyd_label::d2output;
		tab_col[5].type = sys_label::tab_col_type_string;

		tab_col[6].name = hyd_label::nx;
		tab_col[6].type = sys_label::tab_col_type_int;
		tab_col[6].unsigned_flag = true;

		tab_col[7].name = hyd_label::ny;
		tab_col[7].type = sys_label::tab_col_type_int;
		tab_col[7].unsigned_flag = true;

		tab_col[8].name = hyd_label::elemwidth_x;
		tab_col[8].type = sys_label::tab_col_type_double;
		tab_col[8].unsigned_flag = true;

		tab_col[9].name = hyd_label::elemwidth_y;
		tab_col[9].type = sys_label::tab_col_type_double;
		tab_col[9].unsigned_flag = true;

		tab_col[10].name = hyd_label::angle;
		tab_col[10].type = sys_label::tab_col_type_double;

		tab_col[11].name = hyd_label::lowleftx;
		tab_col[11].type = sys_label::tab_col_type_double;

		tab_col[12].name = hyd_label::lowlefty;
		tab_col[12].type = sys_label::tab_col_type_double;

		tab_col[13].name = hyd_label::noinfovalue;
		tab_col[13].type = sys_label::tab_col_type_double;
		tab_col[13].default_value = "-9999.0";

		tab_col[14].name = label::description;
		tab_col[14].type = sys_label::tab_col_type_string;

		try {
			Hyd_Model_Groundwater::general_param_table = new Tables();
			if (Hyd_Model_Groundwater::general_param_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::hyd) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Hyd_Model_Groundwater::create_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Hyd_Model_Groundwater::close_table();;
			throw msg;
		}

		Hyd_Model_Groundwater::close_table();
	}
}
//Set the database table for the general parameters of the groundwater model: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Model_Groundwater::set_table(QSqlDatabase *ptr_database) {
	if (Hyd_Model_Groundwater::general_param_table == NULL) {
		//make specific input for this class
		const string tab_id_name = hyd_label::tab_gw_gen;
		string tab_id_col[15];
		tab_id_col[0] = hyd_label::genmod_id;
		tab_id_col[1] = hyd_label::atol;
		tab_id_col[2] = hyd_label::rtol;
		tab_id_col[3] = hyd_label::d2output;
		tab_id_col[4] = hyd_label::genmod_name;
		tab_id_col[5] = hyd_label::nx;
		tab_id_col[6] = hyd_label::ny;
		tab_id_col[7] = hyd_label::elemwidth_x;
		tab_id_col[8] = hyd_label::elemwidth_y;
		tab_id_col[9] = hyd_label::angle;
		tab_id_col[10] = hyd_label::lowleftx;
		tab_id_col[11] = hyd_label::lowlefty;
		tab_id_col[12] = hyd_label::noinfovalue;
		tab_id_col[13] = label::description;
		tab_id_col[14] = label::applied_flag;

		try {
			Hyd_Model_Groundwater::general_param_table = new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col) / sizeof(tab_id_col[0]));
			Hyd_Model_Groundwater::general_param_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Hyd_Model_Groundwater::set_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Hyd_Model_Groundwater::close_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the groundwater model parameters as well as the groundwater elements data (static)
void Hyd_Model_Groundwater::delete_data_in_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Model_Groundwater::set_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	Hyd_Model_Groundwater::general_param_table->delete_data_in_table(ptr_database);

	//delete data of appending tables
	Hyd_Element_Groundwater::delete_data_in_table(ptr_database);
}
//Close and delete the database table for the general parameters of the groundwater model (static)
void Hyd_Model_Groundwater::close_table(void) {
	if (Hyd_Model_Groundwater::general_param_table != NULL) {
		delete Hyd_Model_Groundwater::general_param_table;
		Hyd_Model_Groundwater::general_param_table = NULL;
	}
	//close appending tables
	Hyd_Element_Groundwater::close_table();
}
//Select and count the number of relevant groundwater models in a database table (static)
int Hyd_Model_Groundwater::select_relevant_model_database(QSqlTableModel *results, const bool with_output) {
	int number = 0;
	try {
		Hyd_Model_Groundwater::set_table(&results->database());
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream prefix;
		prefix << "GW> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

		ostringstream cout;
		cout << "Search for relevant groundwater models in database ..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	//give the table name to the query model
	results->setTable((Hyd_Model_Groundwater::general_param_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Hyd_Model_Groundwater::general_param_table->get_column_name(label::applied_flag) << " = true";
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Hyd_Model_Groundwater::select_relevant_model_database(QSqlTableModel *results, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Model_Groundwater::general_param_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::genmod_id).c_str()), Qt::AscendingOrder);

	number = results->rowCount();

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant groundwater model(s) are found in database" << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}

	return number;
}

//Check the number of relevant groundwater models in a database table (static)
int Hyd_Model_Groundwater::check_relevant_model_database(QSqlDatabase *ptr_database) {
	QSqlQueryModel results;
	try {
		Hyd_Model_Groundwater::set_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	int number = 0;

	ostringstream test_filter;
	test_filter << "Select * ";
	test_filter << " from " << Hyd_Model_Groundwater::general_param_table->get_table_name();

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Warning msg;
		msg.set_msg("Hyd_Model_Groundwater::select_relevant_model_database(QSqlDatabase *ptr_database)", "Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Hyd_Model_Groundwater::general_param_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	number = results.rowCount();

	return number;
}
//Select and count the number of relevant groundwater models in a database table with a given groundwater number (static)
int Hyd_Model_Groundwater::select_relevant_model_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int fp_no) {
	try {
		Hyd_Model_Groundwater::set_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	int number = 0;

	ostringstream test_filter;
	test_filter << "Select * ";
	test_filter << " from " << Hyd_Model_Groundwater::general_param_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Model_Groundwater::general_param_table->get_column_name(hyd_label::genmod_id) << " = " << fp_no;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Warning msg;
		msg.set_msg("Hyd_Model_Groundwater::select_relevant_model_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int fp_no)", "Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Hyd_Model_Groundwater::general_param_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	number = results->rowCount();

	return number;
}
//Copy the members of the groundwater model
void Hyd_Model_Groundwater::clone_model(Hyd_Model_Groundwater *groundwater, Hyd_Param_Conductivity *conductivity_table, Hyd_Param_Porosity *porosity_table) {
	ostringstream cout;

	this->system_id = groundwater->get_sys_system_id();
	this->hyd_sz = groundwater->hyd_sz;
	this->Param_GW = groundwater->Param_GW;
	this->elem_area = groundwater->elem_area;
	this->raster.set_geometric_info(this->Param_GW.get_geometrical_info());
	this->raster.clone_raster_points(&groundwater->raster);

	_Hyd_Model::clone_model(this);

	this->number_noinfo_elems = groundwater->number_noinfo_elems;
	this->number_noflow_elems = groundwater->number_noflow_elems;
	this->number_line_elems = groundwater->number_line_elems;
	this->number_gwinside_elems = groundwater->number_gwinside_elems;
	this->number_river_elems = groundwater->number_river_elems;
	this->number_standard_elems = groundwater->number_standard_elems;

	cout << "Clone groundwater model elements..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	this->generate_geo_bound_raster_polygon();
	//allocate the elements
	this->allocate_elems();
	for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].clone_element(&groundwater->groundwater_elems[i]);
	}
	this->init_elements(conductivity_table,  porosity_table, true);
	cout << "Clone groundwater model boundary curves..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//boundary curves
	this->allocate_instat_boundarycurves();
	for (int i = 0; i < this->Param_GW.number_instat_boundary; i++) {
		this->instat_boundary_curves_gw[i].clone_curve(&groundwater->instat_boundary_curves_gw[i]);
	}
	//connect them to the instationary bounadry curves to the elems
	this->connect_elems2instat_boundarycurves();

	cout << "Cloning of the groundwater model is finished" << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
}
//Initialize the groundwater elements
void Hyd_Model_Groundwater::init_groundwater_model(Hyd_Param_Conductivity *conductivity_table, Hyd_Param_Porosity *porosity_table) {
	try {
		this->elem_area = this->Param_GW.get_geometrical_info().width_x*this->Param_GW.get_geometrical_info().width_y;
		//intercept them with the raster
		this->connect_elems2noflowpolygons();
		//initialize the elements
		this->init_elements(conductivity_table, porosity_table, false);
		//connect them to the instationary bounadry curves to the elems REVIEW
		this->connect_elems2instat_boundarycurves();
		//count the element types
		this->count_element_types();
		//connet barrier lines to the elements
		this->connect_elems2line_polysegments(conductivity_table);
	}
	catch (Error msg) {
		ostringstream info;
		info << "Groundwatermodelnumber: " << this->Param_GW.GWNumber << endl;
		info << "Groundwatermodelname  : " << this->Param_GW.GWName << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Compare the equality of two groundwater models in terms of geometrical settings and the number of elements
void Hyd_Model_Groundwater::compare_models(Hyd_Model_Groundwater *compare_model) {
	/**This comparison is primarily important for adding new hydraulic boundary scenarios. It ensures, that
	the new boundary conditions are set properly to the groundwater model and his elements.
	*/

	if (this->Param_GW.get_geometrical_info().number_x != compare_model->Param_GW.get_geometrical_info().number_x) {
		Error msg = this->set_error(13);
		ostringstream info;
		info << "Nx in model " << this->Param_GW.GWNumber << ": " << this->Param_GW.get_geometrical_info().number_x << endl;
		info << "Nx in model " << compare_model->Param_GW.GWNumber << ": " << compare_model->Param_GW.get_geometrical_info().number_x << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if (this->Param_GW.get_geometrical_info().number_y != compare_model->Param_GW.get_geometrical_info().number_y) {
		Error msg = this->set_error(13);
		ostringstream info;
		info << "Ny in model " << this->Param_GW.GWNumber << ": " << this->Param_GW.get_geometrical_info().number_y << endl;
		info << "Ny in model " << compare_model->Param_GW.GWNumber << ": " << compare_model->Param_GW.get_geometrical_info().number_y << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if (abs(this->Param_GW.get_geometrical_info().angle - compare_model->Param_GW.get_geometrical_info().angle) > constant::angle_epsilon) {
		Error msg = this->set_error(13);
		ostringstream info;
		info << "Angle in model " << FORMAT_FIXED_REAL << P(5) << this->Param_GW.GWNumber << ": " << this->Param_GW.get_geometrical_info().angle << label::degree << endl;
		info << "Angle in model " << compare_model->Param_GW.GWNumber << ": " << compare_model->Param_GW.get_geometrical_info().angle << label::degree << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if (abs(this->Param_GW.get_geometrical_info().width_x - compare_model->Param_GW.get_geometrical_info().width_x) > constant::meter_epsilon) {
		Error msg = this->set_error(13);
		ostringstream info;
		info << "Width x in model " << FORMAT_FIXED_REAL << P(5) << this->Param_GW.GWNumber << ": " << this->Param_GW.get_geometrical_info().width_x << label::m << endl;
		info << "Width x in model " << compare_model->Param_GW.GWNumber << ": " << compare_model->Param_GW.get_geometrical_info().width_x << label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if (abs(this->Param_GW.get_geometrical_info().width_y - compare_model->Param_GW.get_geometrical_info().width_y) > constant::meter_epsilon) {
		Error msg = this->set_error(13);
		ostringstream info;
		info << "Width y in model " << FORMAT_FIXED_REAL << P(5) << this->Param_GW.GWNumber << ": " << this->Param_GW.get_geometrical_info().width_y << label::m << endl;
		info << "Width y in model " << compare_model->Param_GW.GWNumber << ": " << compare_model->Param_GW.get_geometrical_info().width_y << label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if (abs(this->Param_GW.get_geometrical_info().origin_global_x - compare_model->Param_GW.get_geometrical_info().origin_global_x) > constant::meter_epsilon) {
		Error msg = this->set_error(13);
		ostringstream info;
		info << "Origin x in model " << FORMAT_FIXED_REAL << P(5) << this->Param_GW.GWNumber << ": " << this->Param_GW.get_geometrical_info().origin_global_x << label::m << endl;
		info << "Origin x in model " << compare_model->Param_GW.GWNumber << ": " << compare_model->Param_GW.get_geometrical_info().origin_global_x << label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if (abs(this->Param_GW.get_geometrical_info().origin_global_y - compare_model->Param_GW.get_geometrical_info().origin_global_y) > constant::meter_epsilon) {
		Error msg = this->set_error(13);
		ostringstream info;
		info << "Origin y in model " << FORMAT_FIXED_REAL << P(5) << this->Param_GW.GWNumber << ": " << this->Param_GW.get_geometrical_info().origin_global_y << label::m << endl;
		info << "Origin y in model " << compare_model->Param_GW.GWNumber << ": " << compare_model->Param_GW.get_geometrical_info().origin_global_y << label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if (this->Param_GW.number_noflow_polys != compare_model->Param_GW.number_noflow_polys) {
		Error msg = this->set_error(13);
		ostringstream info;
		info << "No noflow-polygon(s) in model " << this->Param_GW.GWNumber << ": " << this->Param_GW.number_noflow_polys << endl;
		info << "No noflow-polygon(s) in model " << compare_model->Param_GW.GWNumber << ": " << compare_model->Param_GW.number_noflow_polys << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set a new hydraulic boundary scenario id to the groundwater models
void Hyd_Model_Groundwater::set_new_hyd_bound_sz_id(Hyd_Boundary_Szenario new_sz) {
	this->hyd_sz = new_sz;
	//set the boundary szenario data to the elements
	for (int j = 0; j < this->Param_GW.GWNofY*this->Param_GW.GWNofX; j++) {
		this->groundwater_elems[j].hyd_sz = &this->hyd_sz;
	}
	//set the boundary szenario data to the instationary boundary curves
	for (int i = 0; i < this->Param_GW.number_instat_boundary; i++) {
		this->instat_boundary_curves_gw[i].hyd_sz.set_members(&new_sz);
	}
}
//Intercept the model with other models (Hyd_Model_Groundwater) for coupling
bool Hyd_Model_Groundwater::intercept_model(Hyd_Model_Groundwater *other_model, _hyd_model_coupling *indexes) {
	bool intercept_flag = false;
	ostringstream cout;
	cout << "Intercept GW-model " << this->Param_GW.GWNumber << " with GW-model " << other_model->Param_GW.GWNumber << "..." << endl;

	_polygon_intercept intercept_type;
	intercept_type = this->raster.geometrical_bound.check_polygon_interception(&(other_model->raster.geometrical_bound));
	cout << "It is " << _Geo_Polygon::convert_ploygon_intercept2txt(intercept_type) << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	if (intercept_type != _polygon_intercept::complete_outside && intercept_type != _polygon_intercept::outside_with_boundary) {
		//which model has the bigger raster size
		double size_this = this->Param_GW.width_x*this->Param_GW.width_y;
		double size_other = other_model->Param_GW.width_x*other_model->Param_GW.width_y;
		if (size_this <= size_other) {
			other_model->raster.set_rasterpolygon_inside(&(this->raster.geometrical_bound));
			other_model->raster.set_raster_segments2noflow(&(this->raster.geometrical_bound));
			//set the noflow polygons of the bigger model to the smaller one
			int number_noflow_poly = 0;
			Hyd_Groundwater_Polygon *noflow_polygons = other_model->get_noflow_polygons(&number_noflow_poly);
			for (int i = 0; i < number_noflow_poly; i++) {
				this->raster.set_rasterpolygon_inside(&(noflow_polygons[i]));
			}
			//set the indices
			indexes->index_first_model = this->Param_GW.get_groundwater_number();
			indexes->index_second_model = other_model->Param_GW.get_groundwater_number();
		}
		else {
			this->raster.set_rasterpolygon_inside(&(other_model->raster.geometrical_bound));
			this->raster.set_raster_segments2noflow(&(other_model->raster.geometrical_bound));
			//set the noflow polygons of the bigger model to the smaller one
			int number_noflow_poly = 0;
			Hyd_Groundwater_Polygon *noflow_polygons = this->get_noflow_polygons(&number_noflow_poly);
			for (int i = 0; i < number_noflow_poly; i++) {
				other_model->raster.set_rasterpolygon_inside(&(noflow_polygons[i]));
			}
			//set the indices
			indexes->index_first_model = other_model->Param_GW.get_groundwater_number();
			indexes->index_second_model = this->Param_GW.get_groundwater_number();
		}
		return intercept_flag = true;
	}
	return intercept_flag;
}

//Intercept the model with other models (Hyd_Model_River) REVIEW
bool Hyd_Model_Groundwater::intercept_model(Hyd_Model_River *river_model) {
	
	bool intercept_flag = false;
	ostringstream cout;
	cout << "Intercept GW-model " << this->Param_GW.GWNumber << " with RV-model " << river_model->Param_RV.get_river_number() << "..." << endl;

	_polygon_intercept intercept_type;
	intercept_type = this->raster.geometrical_bound.check_polygon_interception(&(river_model->river_polygon));
	cout << "It is " << _Geo_Polygon::convert_ploygon_intercept2txt(intercept_type) << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	if (intercept_type != _polygon_intercept::complete_outside && intercept_type != _polygon_intercept::outside_with_boundary) {
		cout << "Set elements inside the river.." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//set the groundwater elements as a rivertype if they are completely in the river polygon
		//Commenting this out for now since we do not work with river type at the moment
		//this->raster.set_rasterpolygon_inside(&(river_model->river_polygon));
		cout << "Set dividing river segments.." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		double diagonal = 0.0;
		diagonal = pow(pow(*this->Param_GW.get_ptr_width_x(), 2) + pow(*this->Param_GW.get_ptr_width_y(), 2), 0.5);
		if (river_model->get_min_width() < 2.0*diagonal) {
			this->raster.interception_polysegments2raster(&(river_model->river_midline));
		}
		else {
			cout << "No dividing river segments are required (min 1d-river width > 2 x 2d-Cell-diagonal)!" << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);

		}
		//set the element, where the inflow and outflow profile is to river elements
		//These were in the floodplain model but should be irrelevant for groundwater. We do not work with river type at the moment
		int index = this->raster.find_elem_index_by_point(river_model->river_midline.get_first_point());
		if (index >= 0) {
			//this->groundwater_elems[index].set_element_type(river_model->river_midline.get_element_type());
		}
		//outflow
		index = this->raster.find_elem_index_by_point(river_model->river_midline.get_last_point());
		if (index >= 0) {
			//this->groundwater_elems[index].set_element_type(river_model->river_midline.get_element_type());
		}
		return intercept_flag = true;
	}
	return intercept_flag;
	
	return false;
}

//Get a pointer to the line polylines of a model and the number
Hyd_Groundwater_Polysegment *Hyd_Model_Groundwater::get_polylines(int *number) {
	*number = this->Param_GW.number_polylines;
	return this->line_polysegments;
}
//Get a pointer to the noflow polygons of a model and the number. Noflow polys for groundwater have only been tested for coupling yet.
Hyd_Groundwater_Polygon *Hyd_Model_Groundwater::get_noflow_polygons(int *number) {
	*number = this->Param_GW.number_noflow_polys;
	return this->noflow_polygons;
}
//Get the number of elements
int Hyd_Model_Groundwater::get_number_elements(void) {
	return this->NEQ;
}
//Clear all not needed data from this model such as geometry etc.
void Hyd_Model_Groundwater::clear_model(void) {
	this->Param_GW.number_noflow_polys = 0;
	this->Param_GW.number_polylines = 0;
	this->delete_line_polysegments();
	this->delete_noflow_polygons();
	this->raster.delete_raster_segments();
}
//Reset the model to the state before the calculation
void Hyd_Model_Groundwater::reset_model(Hyd_Param_Global *global_params) {
	ostringstream prefix;
	prefix << "GW_" << this->Param_GW.get_groundwater_number() << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Reset groundwater model and reinit solver..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	this->reset_total_hydrological_balance();
	this->error_zero_outflow_volume = 0.0;
	this->warn_counter = 0;
	this->tol_reduce_counter = 0;
	for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].element_type->reset_hydrobalance_maxvalues();
	}
	this->change_abs_tol = this->setted_abs_tol;
	this->change_rel_tol = this->setted_rel_tol;

	this->reinit_solver(global_params);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Initialize the solver with the given parameters
void Hyd_Model_Groundwater::init_solver(Hyd_Param_Global *global_params) {
	//test opti
	this->allocate_opt_data();
	this->init_opt_data();
	this->init_opt_data_bound_coup();
	this->allocate_opt_data_reduced();
	this->init_reduced_id();

	_Hyd_Model::init_solver(global_params);
}
//Reinitialize the solver
void Hyd_Model_Groundwater::reinit_solver(Hyd_Param_Global *global_params) {

	//test opti

	this->allocate_opt_data();
	this->init_opt_data();
	this->allocate_opt_data_reduced();
	this->init_reduced_id();
	//this->NEQ_real=this->NEQ;
	_Hyd_Model::reinit_solver(global_params);

}
//Clear the boundary condition, which are set; boundary conditions due to interception are not cleared
void Hyd_Model_Groundwater::clear_boundary_condition(const bool with_output) {
	ostringstream prefix;
	prefix << "GW_" << this->Param_GW.get_groundwater_number() << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	if (with_output == true) {
		cout << "Clear groundwater model boundary condition..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	this->delete_instationary_boundary_curve();
	this->Param_GW.number_instat_boundary = 0;
	for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].reset_boundary_data();
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Set new boundary condition per database for an new hydraulic boundary scenario; the new boundary scenario has to be specified before
void Hyd_Model_Groundwater::set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database) {
	ostringstream prefix;
	prefix << "GW_" << this->Param_GW.get_groundwater_number() << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try {
		//instationary boundary
		this->read_instat_boundary_curves(output_flag, ptr_database);

		QSqlQueryModel prof_query_result;
		//get the number of elements in the table
		int number_elems = 0;
		number_elems = Hyd_Element_Groundwater::select_relevant_elements_database(&prof_query_result, ptr_database, this->system_id, this->Param_GW.GWNumber, output_flag);
		//check the number of found elements
		if (number_elems != this->NEQ) {
			Error msg = this->set_error(14);
			ostringstream info;
			info << "Table name               : " << Hyd_Model_Groundwater::general_param_table->get_table_name() << endl;
			info << "Number found elements    : " << number_elems << endl;
			info << "Number required elements : " << this->NEQ << endl;
			msg.make_second_info(info.str());
			//rewind the prefix
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}
		int glob_id = -1;

		QSqlQueryModel bound_result;
		int number_bound;
		number_bound = Hyd_Element_Groundwater::select_relevant_boundary_cond_database(&bound_result, ptr_database, this->system_id, this->hyd_sz.get_id());
		//read in the element members
		//read in the element members
		int last_index = 0;
		for (int i = 0; i < this->NEQ; i++) {
			glob_id = -1;
			glob_id = prof_query_result.record(i).value((Hyd_Element_Groundwater::elem_table->get_column_name(hyd_label::elemdata_glob_id)).c_str()).toInt();
			this->groundwater_elems[i].hyd_sz = &this->hyd_sz;
			this->groundwater_elems[i].input_element_boundarydata2database(&bound_result, number_bound, glob_id, &last_index);
			this->groundwater_elems[i].set_boundary_data_buffer2type();
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}

		//make the connection between the istationary curves and the elements
		this->connect_elems2instat_boundarycurves();

		this->init_opt_data_bound_coup();

	}
	catch (Error msg) {
		ostringstream info;
		info << "Groundwaternumber: " << this->Param_GW.GWNumber << endl;
		info << "Groundwatername  : " << this->Param_GW.GWName << endl;
		msg.make_second_info(info.str());
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//make the syncronisation between the models and the boundaries
void Hyd_Model_Groundwater::make_syncronisation(const double time_point) {
	//the instationary boundary
	for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].element_type->calculate_boundary_value(time_point);
	}
}
//Get the maximum change in a element
void Hyd_Model_Groundwater::get_max_changes_elements(double *max_change_h, const bool timecheck) {
	double buff_h = 0.0;
	for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].element_type->get_max_changes_elements(&buff_h, timecheck);
		if (buff_h > *max_change_h) {
			*max_change_h = buff_h;
		}
	}
}
//Calculate the hydrological balance and the maximum/minimum values of the model REVIEW

void Hyd_Model_Groundwater::make_hyd_balance_max(const double time_point) {
	
	double delta_t = time_point - this->old_time_point_syncron;
	this->old_time_point_syncron = time_point;
	//int number_bound_cond;

	for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].element_type->calc_max_values(time_point);
		//this->groundwater_elems[i].element_type->calc_min_values(time_point);
	}
	
	//boundary condition
	for (int i = 0; i < this->number_hbound_cond; i++) {
		this->groundwater_elems[this->hbound_cond_id[i]].element_type->calculate_hydrolocigal_balance_boundary(delta_t, true);
	}
	for (int i = 0; i < this->number_qbound_cond; i++) {
		this->groundwater_elems[this->qbound_cond_id[i]].element_type->calculate_hydrolocigal_balance_boundary(delta_t, false);
	}
	//coupling condition
	for (int i = 0; i < this->number_coup_cond; i++) {
		this->groundwater_elems[this->coup_cond_id[i]].element_type->calculate_hydrolocigal_balance_coupling(delta_t);
	}
	
}

//Reset the solver of the model
void Hyd_Model_Groundwater::reset_solver(void) {
	this->set_solver_statistics();
	CVodeReInit(this->cvode_mem, this->old_time_point_syncron, this->results);
	this->no_reset++;
}
//solve_model REVIEW

void Hyd_Model_Groundwater::solve_model(const double next_time_point, const string system_id) {
	
	try {

		this->update_opt_data_by_elems();
		this->calc_set_max_step_size(next_time_point);

		this->run_solver(next_time_point, system_id);

		long int counter = 0;
		//long int counter_wet = 0;
		for (int i = 0; i < this->NEQ; i++) {
			//because we only use Standard and Noflow elems, this basically means if there is flow
			if (this->groundwater_elems[i].get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW) {
				this->groundwater_elems[i].element_type->set_solver_result_value(this->results_real[counter]);
				counter++;
			}

		}
		//Update the optimalized data
		this->update_elems_by_opt_data();
		double new_abs_tol = 0.0;
		//new_abs_tol = (double(counter_wet) / double(counter))*this->setted_abs_tol;
		if (new_abs_tol < 1e-5) {

			new_abs_tol = 1e-5;
		}
		//CVode_change_SStolerances(this->cvode_mem, this->setted_rel_tol,new_abs_tol);
	}
	catch (Error msg) {
		ostringstream info;
		info << "Time point       :" << next_time_point << endl;
		info << "Groundwaternumber :" << this->Param_GW.GWNumber << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
}

//output the members
void Hyd_Model_Groundwater::output_members(void) {
	//set prefix for output
	ostringstream prefix;
	prefix << "GW_" << this->Param_GW.GWNumber << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Output the general parameters..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//out put the global groundwater parameters
	this->Param_GW.output_members();

	cout << "Output the groundwater elements (" << this->Param_GW.GWNofX*this->Param_GW.GWNofY << ")..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//output the elemnt parameters (just for debugging purpose
//	ostringstream prefix1;
//	prefix1 << "ELEM> ";
//	Sys_Common_Output::output_hyd->set_userprefix(prefix1.str());
//	if(this->Param_FP.FPNofX*this->Param_FP.FPNofY>0){
//		Hyd_Element_Floodplain::output_member_header(&cout);
//		Sys_Common_Output::output_hyd->output_txt(&cout,true);
//		for (int i=0 ; i< this->NEQ; i++){
//			this->floodplain_elems[i].output_members(&cout);
//			Sys_Common_Output::output_hyd->output_txt(&cout,true);
//		}
//	}

//	//rewind the prefix
//	Sys_Common_Output::output_hyd->rewind_userprefix();

	//output the noflow
	this->output_noflow_polygons();
	//output the lines
	this->output_line_polysegments();
	//output the instationary boundary curves
	this->output_instationary_boundary_curves();

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//output the setted members
void Hyd_Model_Groundwater::output_setted_members(void) {
	//set prefix for output
	ostringstream prefix;
	prefix << "GW_" << this->Param_GW.GWNumber << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "GENERAL" << endl;
	cout << " Area " << W(13) << P(5) << this->raster.geometrical_bound.calculate_area() << label::squaremeter << endl;
	cout << " Perimeter " << W(8) << P(5) << this->raster.geometrical_bound.calculate_perimeter() << label::m << endl;
	cout << "ELEMENTTYPES" << endl;
	cout << " No. of standard elements   : " << W(15) << this->number_standard_elems << endl;
	cout << " No. of line elements   : " << W(15) << this->number_line_elems << endl;
	cout << " No. of river elements      : " << W(15) << this->number_river_elems << endl;
	cout << " No. of gw inside elements  : " << W(15) << this->number_gwinside_elems << endl;
	cout << " No. of noflow elements     : " << W(15) << this->number_noflow_elems << endl;
	cout << " No. of noinfo elements     : " << W(15) << this->number_noinfo_elems << endl;

	Sys_Common_Output::output_hyd->output_txt(&cout);
	this->raster.output_geometrical_boundary();
	//output the elements just for debugging purpose!

//	ostringstream prefix1;
//	prefix1 << "ELEM> ";
//	Sys_Common_Output::output_hyd->set_userprefix(prefix1.str());
//	Hyd_Element_Floodplain::output_setted_header(&cout);
//	Sys_Common_Output::output_hyd->output_txt(&cout,true);
//	for(int i =0; i< this->NEQ;i++){
//		this->floodplain_elems[i].output_setted_members(&cout);
//		Sys_Common_Output::output_hyd->output_txt(&cout,true);
//	}

//	//rewind the prefix
//	Sys_Common_Output::output_hyd->rewind_userprefix();

	//output the raster (for development)
	//this->raster.output_members();

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the geometrie to tecplot, currently tecplot is not in use
void Hyd_Model_Groundwater::output_geometrie2tecplot(void) {
	//get the file name
	string filename = this->Param_GW.get_filename_geometrie2file(hyd_label::tecplot);
	//Add file type
	filename += hyd_label::dat;
	if (filename == label::not_set) {
		return;
	}
	//open the file
	this->tecplot_output.open(filename.c_str());
	if (this->tecplot_output.is_open() == false) {
		Error msg = this->set_error(6);
		ostringstream info;
		info << "Filename " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//fileheader
	//output the file header
	this->tecplot_output << "TITLE = " << "\" " << this->Param_GW.GWName << " No. " << this->Param_GW.GWNumber << " \"" << endl;
	this->tecplot_output << "VARIABLES = " << endl;
	this->tecplot_output << "\" x " << label::m << "\" " << endl;
	this->tecplot_output << "\" y " << label::m << "\" " << endl;
	this->tecplot_output << "\" Elem.No. " << label::no_unit << "\" " << endl;
	this->tecplot_output << "\" z " << label::m << "\" " << endl;
	this->tecplot_output << "\" type " << label::no_unit << "\" " << endl;
	this->tecplot_output << "\" h init " << label::m << "\" " << endl;
	this->tecplot_output << "\" con-type " << label::no_unit << "\" " << endl;
	this->tecplot_output << "\" kf-value " << label::kf_unit << "\" " << endl << endl;
	this->tecplot_output << "\" por-type " << label::no_unit << "\" " << endl;
	this->tecplot_output << "\" p-value " << label::no_unit << "\" " << endl << endl;
	this->tecplot_output << "#Type expression: 0 STANDARD_ELEM; 1 NOFLOW_ELEM; 2 NOINFO_ELEM; 3 RIVER_ELEM; 4 OTHER_GW_ELEM; 5 LINE_ELEM" << endl << endl;
	//output zone header
	this->tecplot_output << "ZONE T= " << " \" Geometrie " << this->Param_GW.GWName << " No. " << this->Param_GW.GWNumber << "\" " << endl;
	this->tecplot_output << "I=" << this->Param_GW.GWNofX + 1 << ", J=" << this->Param_GW.GWNofY + 1 << ", K=1" << endl;
	this->tecplot_output << "ZONETYPE=ORDERED" << endl;
	this->tecplot_output << "DATAPACKING=BLOCK" << endl;
	this->tecplot_output << "VARLOCATION=([1-2]=NODAL, [3-8]=CELLCENTERED)" << endl;
	this->tecplot_output << "DT=(SINGLE SINGLE LONGINT SINGLE SHORTINT SINGLE LONGINT SINGLE)" << endl;
	this->tecplot_output << endl;
	int counter_endl = 0;
	//x-coordinate
	//this->tecplot_output << "#x " << label::m << " NODAL No. " << this->raster.get_number_raster_points() << endl;
	//for (int i = 0; i < this->raster.get_number_raster_points(); i++) {
	//	if (counter_endl == Param_GW.get_no_elems_x()) {
	//		counter_endl = 0;
	//		this->tecplot_output << endl;
	//	}
	//	this->tecplot_output << P(2) << FORMAT_FIXED_REAL << this->raster.get_raster_point(i)->get_xcoordinate() << "  ";
	//	counter_endl++;
	//}
	//counter_endl = 0;
	//this->tecplot_output << endl << endl;
	//this->tecplot_output << "#y " << label::m << " NODAL No. " << this->raster.get_number_raster_points() << endl;
	////y-coordinate
	//for (int i = 0; i < this->raster.get_number_raster_points(); i++) {
	//	if (counter_endl == Param_GW.get_no_elems_x()) {
	//		counter_endl = 0;
	//		this->tecplot_output << endl;
	//	}
	//	this->tecplot_output << P(2) << FORMAT_FIXED_REAL << this->raster.get_raster_point(i)->get_ycoordinate() << "  ";
	//	counter_endl++;
	//}
	//counter_endl = 0;
	//this->tecplot_output << endl << endl;
	//this->tecplot_output << "#Elem.No. " << label::no_unit << " CELLCENTERED No. " << this->NEQ << endl;
	//x-coordinate
	this->tecplot_output << "#x " << label::m << " CELLCENTERED No. " << this->NEQ << endl;
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << P(2) << FORMAT_FIXED_REAL << this->groundwater_elems[i].get_mid_point()->get_xcoordinate() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#y " << label::m << " CELLCENTERED No. " << this->NEQ << endl;
	//y-coordinate
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << P(2) << FORMAT_FIXED_REAL << this->groundwater_elems[i].get_mid_point()->get_ycoordinate() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Elem.No. " << label::no_unit << " CELLCENTERED No. " << this->NEQ << endl;
	//elem_number
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].get_elem_number() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#z " << label::m << " CELLCENTERED No. " << this->NEQ << endl;
	//global height
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].get_z_value() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#type " << label::no_unit << " CELLCENTERED No. " << this->NEQ << endl;
	//type
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].get_element_type_id() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#h init " << label::m << " CELLCENTERED No. " << this->NEQ << endl;
	//h init
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].get_flow_data().init_condition << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#con-type " << label::no_unit << " CELLCENTERED No. " << this->NEQ << endl;
	//con-type
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].get_flow_data().con_type << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#kf-value " << label::kf_unit << " CELLCENTERED No. " << this->NEQ << endl;
	//n-value
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << P(2) << FORMAT_SCIENTIFIC_REAL<< this->groundwater_elems[i].get_flow_data().kf_value << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#por-type " << label::no_unit << " CELLCENTERED No. " << this->NEQ << endl;
	//por-type
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].get_flow_data().por_type << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#p-value " << label::n_unit << " CELLCENTERED No. " << this->NEQ << endl;
	//n-value
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].get_flow_data().kf_value << "  ";
		counter_endl++;
	}

	//output geometry
	this->tecplot_output << endl << endl;
	this->tecplot_output << "########################################################" << endl;
	this->tecplot_output << "#Geometrical output" << endl;
	//geometry of the boundary
	this->tecplot_output << "#Groundwater boundary" << endl;
	this->raster.output_geometrical_boundary2tecplot(&this->tecplot_output);
	//output the boundary geometrie of the elements
	this->tecplot_output << "#######################" << endl;
	this->tecplot_output << "#Groundwater elements" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].output_boundaries2tecplot(&this->Param_GW.get_geometrical_info(), &this->tecplot_output);
	}
	//output the geometry of the lines
	for (int i = 0; i < this->Param_GW.number_polylines; i++) {
		this->tecplot_output << "#######################" << endl;
		this->line_polysegments[i].output2tecplot(&this->tecplot_output);
	}
	//output the geometry of the noflow polygons
	for (int i = 0; i < this->Param_GW.number_noflow_polys; i++) {
		this->tecplot_output << "#######################" << endl;
		this->noflow_polygons[i].output2tecplot(&this->tecplot_output);
	}
	//close the file
	this->tecplot_output.close();
	this->tecplot_output.clear();
}
//Output the geometrie to BlueKenue file, currently BlueKenue is not in use
void Hyd_Model_Groundwater::output_geometrie2bluekenue(void) {

	//get the file name
	string filename = this->Param_GW.get_filename_geometrie2file(hyd_label::bluekenue);
	//Add file type
	filename += hyd_label::r2s;

	if (this->Param_GW.get_geometrical_info().number_x == 0) {
		return;
	}



	//open the file
	this->bluekenue_output.open(filename.c_str());
	if (this->bluekenue_output.is_open() == false) {
		Error msg = this->set_error(18);
		ostringstream info;
		info << "Filename " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//output geometrie header
	this->bluekenue_output << endl << endl;
	this->bluekenue_output << "####################################################################### " << endl;
	this->bluekenue_output << ":FileType r2s ASCII EnSim 1.0  " << endl;
	this->bluekenue_output << "# Canadian Hydraulics Centre/National Research Council (c) 1998-2005  " << endl;
	this->bluekenue_output << "# DataType 2D Rect Scalar   (Element midpoints are displayed!) " << endl;
	this->bluekenue_output << ":Application BlueKenue      " << endl;
	this->bluekenue_output << ":Version 3.3.4      " << endl;
	this->bluekenue_output << ":WrittenBy       ProMaIDes HS-M" << endl;
	this->bluekenue_output << ":CreationDate " << Sys_Output_Division::set_time() << endl;
	this->bluekenue_output << "# Element-type expression: 0 STANDARD_ELEM; 1 NOFLOW_ELEM; 2 NOINFO_ELEM; 3 RIVER_ELEM; 4 OTHER_GW_ELEM; 5 LINE_ELEM" << endl << endl;
	this->bluekenue_output << "#------------------------------------------------------------------------" << endl << endl;
	this->bluekenue_output << ":Name Geometry of groundwater " << this->Param_GW.get_groundwater_name() << endl;
	this->bluekenue_output << ":Title " << this->Param_GW.get_groundwater_name() << endl << endl;
	this->bluekenue_output << ":Projection Cartesian" << endl;
	this->bluekenue_output << ":Ellipsoid Unknown " << endl << endl;
	this->bluekenue_output << ":xOrigin " << P(5) << FORMAT_FIXED_REAL << this->groundwater_elems[0].get_mid_point()->get_xcoordinate() << endl;
	this->bluekenue_output << ":yOrigin " << P(5) << FORMAT_FIXED_REAL << this->groundwater_elems[0].get_mid_point()->get_ycoordinate() << endl;
	this->bluekenue_output << ":xCount " << this->Param_GW.GWNofX << endl;
	this->bluekenue_output << ":yCount " << this->Param_GW.GWNofY << endl;
	this->bluekenue_output << ":xDelta " << this->Param_GW.width_x << endl;
	this->bluekenue_output << ":yDelta " << this->Param_GW.width_y << endl;
	this->bluekenue_output << ":Angle " << this->Param_GW.angle << endl;
	//output geometrie attribute

	this->bluekenue_output << ":AttributeName  1 Elem-No." << endl;
	this->bluekenue_output << ":AttributeUnits 1 " << label::no_unit << endl;
	this->bluekenue_output << ":AttributeName  2 z" << endl;
	this->bluekenue_output << ":AttributeUnits 2 " << label::m << endl;
	this->bluekenue_output << ":AttributeName  3 type" << endl;
	this->bluekenue_output << ":AttributeUnits 3 " << label::no_unit << endl;
	this->bluekenue_output << ":AttributeName  4 h_init" << endl;
	this->bluekenue_output << ":AttributeUnits 4 " << label::m << endl;
	this->bluekenue_output << ":AttributeName  5 con_type" << endl;
	this->bluekenue_output << ":AttributeUnits 5 " << label::no_unit << endl;
	this->bluekenue_output << ":AttributeName  6 kf_value" << endl;
	this->bluekenue_output << ":AttributeUnits 6 " << label::kf_unit << endl;
	this->bluekenue_output << ":AttributeName  7 por_type" << endl;
	this->bluekenue_output << ":AttributeUnits 7 " << label::no_unit << endl;
	this->bluekenue_output << ":AttributeName  8 p_value" << endl;
	this->bluekenue_output << ":AttributeUnits 8 " << label::p_unit << endl;

	this->bluekenue_output << ":EndHeader " << endl << endl;

	int counter_endl = 0;


	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].get_elem_number() << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}
	counter_endl = 0;
	this->bluekenue_output << endl << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].get_z_value() << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}
	counter_endl = 0;
	this->bluekenue_output << endl << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].get_element_type_id() << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}
	counter_endl = 0;
	this->bluekenue_output << endl << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].get_flow_data().init_condition << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}
	counter_endl = 0;
	this->bluekenue_output << endl << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].get_flow_data().con_type << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}
	counter_endl = 0;
	this->bluekenue_output << endl << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].get_flow_data().kf_value << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}
	counter_endl = 0;
	this->bluekenue_output << endl << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].get_flow_data().por_type << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}
	counter_endl = 0;
	this->bluekenue_output << endl << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].get_flow_data().p_value << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}
	this->bluekenue_output << endl;

	//close the file
	this->bluekenue_output.close();
	this->bluekenue_output.clear();


}
//Output the geometrie to Paraview file
void Hyd_Model_Groundwater::output_geometrie2paraview(void) {
	//get the file name
	string filename = this->Param_GW.get_filename_geometrie2file(hyd_label::paraview);
	//Add file type
	filename += hyd_label::vtk;

	if (filename == label::not_set) {
		return;
	}
	//open the file
	this->tecplot_output.open(filename.c_str());
	if (this->tecplot_output.is_open() == false) {
		Error msg = this->set_error(7);
		ostringstream info;
		info << "Filename " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//fileheader
	//output the file header
	this->tecplot_output << "# vtk DataFile Version 3.0" << endl;
	this->tecplot_output << "Geometry of groundwater " << this->Param_GW.GWNumber << " with groundwater name " << this->Param_GW.GWName << endl;
	//this->tecplot_output << "  (Element-Type expression: 0 STANDARD_ELEM; 1 NOFLOW_ELEM; 2 NOINFO_ELEM; 3 RIVER_ELEM; 4 OTHER_GW_ELEM; 5 LINE_ELEM)" << endl;
	this->tecplot_output << "ASCII" << endl;
	this->tecplot_output << "DATASET STRUCTURED_GRID" << endl;
	this->tecplot_output << "DIMENSIONS " << this->Param_GW.GWNofX + 1 << " " << this->Param_GW.GWNofY + 1 << " 1" << endl;
	this->tecplot_output << "POINTS " << (this->Param_GW.GWNofX + 1)*(this->Param_GW.GWNofY + 1) << " double" << endl;
	this->tecplot_output << FORMAT_FIXED_REAL << setprecision(2);

	//output raster
	for (int i = 0; i < this->raster.get_number_raster_points(); i++) {
		this->tecplot_output << this->raster.get_raster_point(i)->get_xcoordinate() << " " << this->raster.get_raster_point(i)->get_ycoordinate() << "  0" << endl;
	}

	this->tecplot_output << endl << endl;

	//output data
	string buff_unit;
	this->tecplot_output << "CELL_DATA " << this->NEQ << endl;
	buff_unit = " Elem_No_";
	buff_unit += label::no_unit;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " unsigned_int" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].get_elem_number() << "  ";
	}

	this->tecplot_output << endl << endl;
	buff_unit = " z_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].get_z_value() << "  ";
	}

	this->tecplot_output << endl << endl;
	buff_unit = " Elem_Type_";
	buff_unit += label::no_unit;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " unsigned_int" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].get_element_type_id() << "  ";
	}

	this->tecplot_output << endl << endl;
	buff_unit = " Init_cond_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].get_flow_data().init_condition << "  ";
	}

	this->tecplot_output << endl << endl;
	buff_unit = " Con_Type_";
	buff_unit += label::no_unit;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " unsigned_int" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].get_flow_data().con_type << "  ";
	}

	this->tecplot_output << endl << endl;
	buff_unit = " kf_value_";
	buff_unit += label::n_unit;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].get_flow_data().kf_value << "  ";
	}
	this->tecplot_output << endl;

	this->tecplot_output << endl << endl;
	buff_unit = " Por_Type_";
	buff_unit += label::no_unit;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " unsigned_int" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].get_flow_data().por_type << "  ";
	}

	this->tecplot_output << endl << endl;
	buff_unit = " p_value_";
	buff_unit += label::n_unit;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].get_flow_data().p_value << "  ";
	}
	this->tecplot_output << endl;


	//close the file
	this->tecplot_output.close();
	this->tecplot_output.clear();
}

//Output the result members per timestep
void Hyd_Model_Groundwater::output_result_members_per_timestep(void) {
	//set prefix for output
	ostringstream prefix;
	prefix << "GW_" << this->Param_GW.GWNumber << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	Hyd_Element_Groundwater::output_header_result_values(&cout);
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].output_result_values(&cout);
		Sys_Common_Output::output_hyd->output_txt(&cout, true);
	}

	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the result members per timestep to tecplot
void Hyd_Model_Groundwater::output_result2tecplot(const double timepoint, const int timestep_number,string unit) {
	int time=0;
	if (unit == "s") {
		time = int(timepoint);
	}
	else if (unit == "h") {
		time = int(timepoint / 3600);
	}
	else if (unit == "d") {
		time = int(timepoint / 86400);
	}
	//get the file name
	string filename = this->Param_GW.get_filename_result2file(hyd_label::tecplot, time);
	//Add file type
	filename += hyd_label::dat;

	if (filename == label::not_set) {
		return;
	}
	//open the file
	this->tecplot_output.open(filename.c_str());
	if (this->tecplot_output.is_open() == false) {
		Error msg = this->set_error(7);
		ostringstream info;
		info << "Filename " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//output the file header
	this->tecplot_output << "TITLE = " << "\" " << this->Param_GW.GWName << " GWNo. " << this->Param_GW.GWNumber << " \"" << endl;
	this->tecplot_output << "VARIABLES = " << endl;
	this->tecplot_output << "\" x " << label::m << "\" " << endl;
	this->tecplot_output << "\" y " << label::m << "\" " << endl;
	this->tecplot_output << "\" z " << label::m << "\" " << endl;
	this->tecplot_output << "\" h " << label::m << "\" " << endl;
	this->tecplot_output << "\" s " << label::m << "\" " << endl;
	this->tecplot_output << "\" ds_dt " << label::m_per_min << "\" " << endl;
	this->tecplot_output << "\" ds_dt_x_darcy " << label::m_per_min << "\"" << endl;
	this->tecplot_output << "\" ds_dt_y_darcy " << label::m_per_min << "\"" << endl;
	this->tecplot_output << "\" ds_dt_rv_coulping " << label::m_per_min << "\"" << endl;
	this->tecplot_output << "\" ds_dt_gw_coupling " << label::m_per_min << "\"" << endl;
	this->tecplot_output << "\" ds_dt_h_boundary " << label::m_per_min << "\"" << endl;
	this->tecplot_output << "\" ds_dt_q_boundary " << label::m_per_min << "\"" << endl;
	//output zone header
	this->tecplot_output << "ZONE T= " << " \" 2d-Results Stepno. " << timestep_number << "; Time " << timepoint << label::sec << " " << this->Param_GW.GWName << " No. " << this->Param_GW.GWNumber << "\" " << endl;
	this->tecplot_output << "I=" << this->Param_GW.GWNofX + 1 << ", J=" << this->Param_GW.GWNofY + 1 << ", K=1" << endl;
	this->tecplot_output << "ZONETYPE=ORDERED" << endl;
	this->tecplot_output << "DATAPACKING=BLOCK" << endl;
	this->tecplot_output << "SOLUTIONTIME=" << timepoint << endl;
	this->tecplot_output << "STRANDID=" << 1 << endl;
	this->tecplot_output << "VARLOCATION=([1-2]=NODAL, [3-10]=CELLCENTERED)" << endl;
	this->tecplot_output << "DT=(SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SHORTINT)" << endl;
	this->tecplot_output << endl;
	this->tecplot_output << P(4) << FORMAT_FIXED_REAL;
	int counter_endl = 0;
	//x-coordinate
	this->tecplot_output << "#x " << label::m << " NODAL No. " << this->raster.get_number_raster_points() << endl;
	for (int i = 0; i < this->raster.get_number_raster_points(); i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->raster.get_raster_point(i)->get_xcoordinate() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#y " << label::m << " NODAL No. " << this->raster.get_number_raster_points() << endl;
	//y-coordinate
	for (int i = 0; i < this->raster.get_number_raster_points(); i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->raster.get_raster_point(i)->get_ycoordinate() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#z " << label::m << " CELLCENTERED No. " << this->NEQ << endl;
	//z-coordinate
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].get_z_value() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#h " << label::m << " CELLCENTERED No. " << this->NEQ << endl;
	//h
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].element_type->get_h_value() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#s " << label::m << " CELLCENTERED No. " << this->NEQ << endl;
	//s
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].element_type->get_s_value() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#ds_dt " << label::m_per_min << " CELLCENTERED No. " << this->NEQ << endl;
	//ds_dt
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << P(2) << FORMAT_SCIENTIFIC_REAL<< this->groundwater_elems[i].element_type->get_ds2dt_value()*(double)constant::minute_second << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#ds_dt_x_darcy " << label::m_per_min << " CELLCENTERED No. " << this->NEQ << endl;
	//ds_dt_x_dir
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << P(2) << FORMAT_SCIENTIFIC_REAL << this->groundwater_elems[i].element_type->get_ds2dt_per_type(0) * (double)constant::minute_second << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#ds_dt_y_darcy " << label::m_per_min << " CELLCENTERED No. " << this->NEQ << endl;
	//ds_dt_y_dir
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << P(2) << FORMAT_SCIENTIFIC_REAL << this->groundwater_elems[i].element_type->get_ds2dt_per_type(1) * (double)constant::minute_second << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#ds_dt_rv_coupling " << label::m_per_min << " CELLCENTERED No. " << this->NEQ << endl;
	//ds_dt_rv_coupling
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << P(2) << FORMAT_SCIENTIFIC_REAL << this->groundwater_elems[i].element_type->get_ds2dt_per_type(4) * (double)constant::minute_second << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#ds_dt_gw_coupling " << label::m_per_min << " CELLCENTERED No. " << this->NEQ << endl;
	//ds_dt_gw_coupling
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << P(2) << FORMAT_SCIENTIFIC_REAL << this->groundwater_elems[i].element_type->get_ds2dt_per_type(5) * (double)constant::minute_second << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#ds_dt_h_boundary " << label::m_per_min << " CELLCENTERED No. " << this->NEQ << endl;
	//ds_dt_h_boundary
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << P(2) << FORMAT_SCIENTIFIC_REAL << this->groundwater_elems[i].element_type->get_ds2dt_per_type(2) * (double)constant::minute_second << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#ds_dt_q_boundary " << label::m_per_min << " CELLCENTERED No. " << this->NEQ << endl;
	//ds_dt_q_boundary
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << P(2) << FORMAT_SCIENTIFIC_REAL << this->groundwater_elems[i].element_type->get_ds2dt_per_type(3) * (double)constant::minute_second << "  ";
		counter_endl++;
	}


	counter_endl = 0;
	this->tecplot_output << endl << endl;
	//close the file
	this->tecplot_output.close();
	this->tecplot_output.clear();
}
//Output the result members per timestep to r2s file used in BlueKenue
void Hyd_Model_Groundwater::output_result2bluekenue(const double timepoint, const int timestep_number, const double start_time) {

	//get the file name
	string filename = this->Param_GW.get_filename_result(hyd_label::bluekenue);
	//Add file type
	filename += hyd_label::r2s;

	if (this->Param_GW.get_geometrical_info().number_x == 0) {
		return;
	}


	//open the file h
	if (timestep_number == 0) {
		this->bluekenue_output1.setFileName(filename.c_str());
		this->bluekenue_output1.open(QIODevice::WriteOnly);
	}
	else {
		this->bluekenue_output1.setFileName(filename.c_str());
		this->bluekenue_output1.open(QIODevice::Append);
	}
	if (this->bluekenue_output1.isOpen() == false) {
		Error msg = this->set_error(19);
		ostringstream info;
		info << "Filename " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//header h
	if (timestep_number == 0) {
		QTextStream txt(&this->bluekenue_output1);
		txt << endl << endl;
		txt << "####################################################################### " << endl;
		txt << ":FileType r2s BINARY EnSim 1.0  " << endl;
		txt << "# Canadian Hydraulics Centre/National Research Council (c) 1998-2005  " << endl;
		txt << "# DataType 2D Rect Scalar     (Element midpoints are displayed!) " << endl;
		txt << ":Application BlueKenue      " << endl;
		txt << ":Version 3.3.4      " << endl;
		txt << ":WrittenBy       ProMaIDes HS-M" << endl;
		txt << ":CreationDate " << Sys_Output_Division::set_time().c_str() << endl;
		txt << "#------------------------------------------------------------------------" << endl << endl;
		txt << ":Name Results of groundwater " << this->Param_GW.get_groundwater_name().c_str() << endl;
		txt << ":Title " << this->Param_GW.get_groundwater_name().c_str() << endl << endl;
		txt << ":Projection Cartesian" << endl;
		txt << ":Ellipsoid Unknown " << endl << endl;
		txt.setRealNumberNotation(QTextStream::FixedNotation);
		txt.setRealNumberPrecision(5);
		txt << ":xOrigin " << this->groundwater_elems[0].get_mid_point()->get_xcoordinate() << endl;
		txt << ":yOrigin " << this->groundwater_elems[0].get_mid_point()->get_ycoordinate() << endl;
		txt.setRealNumberNotation(QTextStream::SmartNotation);
		txt << ":xCount " << this->Param_GW.GWNofX << endl;
		txt << ":yCount " << this->Param_GW.GWNofY << endl;
		txt << ":xDelta " << this->Param_GW.width_x << endl;
		txt << ":yDelta " << this->Param_GW.width_y << endl;
		txt << ":Angle " << this->Param_GW.angle << endl;
		//output attributes

		txt << ":AttributeName  1 h" << endl;
		txt << ":AttributeUnits 1 " << (label::m).c_str() << endl;
		txt << ":AttributeName  3 s" << endl;
		txt << ":AttributeUnits 3 " << label::m.c_str() << endl;
		txt << ":EndHeader \n";
	}

	// we will serialize the data into the file
	QDataStream bin(&this->bluekenue_output1);

	//output timestamp
	tm t_info;
	functions::convert_seconds2datestruct(timepoint + start_time, &t_info);
	int it = timestep_number + 1;


	int num = t_info.tm_year;
	num = num + 1900;
	bin.writeRawData(reinterpret_cast<const char *>(&it), 4);
	bin.writeRawData(reinterpret_cast<const char *>(&it), 4);
	bin.writeRawData(reinterpret_cast<const char *>(&num), 4);
	num = t_info.tm_mon;
	num++;
	bin.writeRawData(reinterpret_cast<const char *>(&num), 4);
	num = t_info.tm_mday;
	bin.writeRawData(reinterpret_cast<const char *>(&num), 4);
	num = t_info.tm_hour;
	bin.writeRawData(reinterpret_cast<const char *>(&num), 4);
	num = t_info.tm_min;
	bin.writeRawData(reinterpret_cast<const char *>(&num), 4);
	num = t_info.tm_sec;
	bin.writeRawData(reinterpret_cast<const char *>(&num), 4);
	num = 0;
	bin.writeRawData(reinterpret_cast<const char *>(&num), 4);

	//output data
	float h;
	for (int i = 0; i < this->NEQ; i++) {
		h = (float)this->groundwater_elems[i].element_type->get_h_value();
		bin.writeRawData(reinterpret_cast<const char *>(&h), 4);
	}
	for (int i = 0; i < this->NEQ; i++) {
		h = (float)this->groundwater_elems[i].element_type->get_s_value();
		bin.writeRawData(reinterpret_cast<const char *>(&h), 4);
	}



	//close the file h
	this->bluekenue_output1.close();




}
//Output the result members per timestep to Paraview
void Hyd_Model_Groundwater::output_result2paraview(const double timepoint, const int timestep_number, string unit) {
	double time=0;
	if (unit == "s") {
		time = int(timepoint);
	}
	else if (unit == "h") {
		time = int(timepoint / 3600);
	}
	else if (unit == "d") {
		time = int(timepoint / 86400);
	}
	//get the file name
	string filename = this->Param_GW.get_filename_result2file(hyd_label::paraview, time);
	//Add file type
	filename += unit;
	filename += hyd_label::vtk;

	if (filename == label::not_set) {
		return;
	}
	//open the file
	QFile output;
	output.setFileName(filename.c_str());
	output.open(QIODevice::WriteOnly);
	if (output.isOpen() == false) {
		Error msg = this->set_error(7);
		ostringstream info;
		info << "Filename " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	// we will serialize the data into the file
	QDataStream bin(&output);

	//fileheader
	QTextStream txt(&output);
	//output the file header
	txt << "# vtk DataFile Version 3.0" << endl;
	txt << "Result of groundwater " << this->Param_GW.GWNumber << " with groundwater name " << this->Param_GW.GWName.c_str() << " for time " << timepoint << endl;
	txt << "BINARY" << endl;
	txt << "DATASET STRUCTURED_GRID" << endl;
	txt << "DIMENSIONS " << this->Param_GW.GWNofX + 1 << " " << this->Param_GW.GWNofY + 1 << " 1" << endl;
	txt << "POINTS " << (this->Param_GW.GWNofX + 1)*(this->Param_GW.GWNofY + 1) << " float" << endl;

	//output raster
	float h;
	for (int i = 0; i < this->raster.get_number_raster_points(); i++) {
		h = this->raster.get_raster_point(i)->get_xcoordinate();
		functions::SwapEnd(h);
		bin.writeRawData(reinterpret_cast<const char *>(&h), sizeof(float));
		h = this->raster.get_raster_point(i)->get_ycoordinate();
		functions::SwapEnd(h);
		bin.writeRawData(reinterpret_cast<const char *>(&h), sizeof(float));
		h = 0;
		functions::SwapEnd(h);
		bin.writeRawData(reinterpret_cast<const char *>(&h), sizeof(float));
	}




	//output data
	string buff_unit;
	txt << "CELL_DATA " << this->NEQ << endl;
	buff_unit = " h_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	txt << "SCALARS" << "  " << buff_unit.c_str() << " float" << endl;
	txt << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		h = this->groundwater_elems[i].element_type->get_h_value();
		functions::SwapEnd(h);
		bin.writeRawData(reinterpret_cast<const char *>(&h), sizeof(float));
	}
	txt << endl;


	buff_unit = " s_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	txt << "SCALARS" << "  " << buff_unit.c_str() << " float" << endl;
	txt << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		h = this->groundwater_elems[i].element_type->get_s_value();
		functions::SwapEnd(h);
		bin.writeRawData(reinterpret_cast<const char *>(&h), sizeof(float));
	}

	//close the file
	output.close();

}
//Output the result members per timestep to database
void Hyd_Model_Groundwater::output_result2database(QSqlDatabase *ptr_database, const double timepoint, const int timestep_number, const string time) {


	//evaluate the global identifier
	int id_glob = Hyd_Element_Groundwater::get_max_glob_id_erg_instat_table(ptr_database) + 1;

	//Set the query
	QSqlQuery query_buff(*ptr_database);



	//get the header for the query
	string query_header;
	query_header = Hyd_Element_Groundwater::get_insert_header_erg_instat_data_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;

	//time string
   //string time;
   //stringstream buff_t;
   //tm time_struct;

   //functions::convert_seconds2datestruct(timepoint, &time_struct);
   //buff_t << "'19" << time_struct.tm_year << "-" << setw(2) << setfill('0') << time_struct.tm_mon + 1;
   //buff_t << "-" << setw(2) << setfill('0') << time_struct.tm_mday << " ";
   //buff_t << setw(2) << setfill('0') << time_struct.tm_hour << ":";
   //buff_t << setw(2) << setfill('0') << time_struct.tm_min << ":" << setw(2) << setfill('0')<< time_struct.tm_sec << "'";
   //time = buff_t.str();




	for (int i = 0; i < this->NEQ; i++) {


		buffer_data = this->groundwater_elems[i].get_datastring_erg_instat_2database(i, this->Param_GW.get_groundwater_number(), id_glob, time, this->Param_GW.get_geometrical_info());
		if (buffer_data != label::not_set) {
			query_data << buffer_data << " ,";
			//count the global index
			id_glob++;
			counter++;
		}

		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);

			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(3);
				ostringstream info;
				info << "Table Name                : " << Hyd_Element_Groundwater::erg_instat_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				info << "Data string               : " << query_data.str() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(2);
			}
			//delete them
			query_total.str("");
			query_data.str("");
			query_buff.clear();
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(4);
			ostringstream info;
			info << "Table Name                : " << Hyd_Element_Groundwater::erg_instat_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}

}
//output solver errors
void Hyd_Model_Groundwater::output_solver_errors(const double time_point, const int step_counter, const string timestring, const string realtime, const double diff_time, const int total_internal, const int internal_steps) {
	//set prefix for output
	ostringstream prefix;
	prefix << "FP_" << this->Param_GW.GWNumber << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << W(2) << step_counter;
	cout << W(15) << P(1) << FORMAT_FIXED_REAL << time_point;
	cout << W(15) << timestring;
	cout << W(15) << realtime;
	cout << P(1) << FORMAT_FIXED_REAL << "  (" << diff_time << ")  ";
	cout << W(15) << this->get_number_solversteps();
	cout << "  (" << this->get_number_solversteps() - this->diff_solver_steps << ")   ";
	cout << W(12) << P(1) << FORMAT_SCIENTIFIC_REAL << this->norm_estim_error;
	cout << W(12) << P(1) << FORMAT_SCIENTIFIC_REAL << this->max_estim_error;
	cout << W(12) << total_internal;
	cout << "  (" << internal_steps << ")";
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	this->diff_solver_steps = this->get_number_solversteps();

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//out put final statistics for the groundwater
void Hyd_Model_Groundwater::output_final(void) {
	//set prefix for output
	ostringstream prefix;
	prefix << "GW_" << this->Param_GW.GWNumber << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	this->output_solver_statistics();
	//hydrological balance
	this->calculate_total_hydrological_balance();
	this->output_hydrological_balance();
	//output the maximum values (just for debugging)
	//this->output_maximum_values();
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the maximum result members to tecplot
void Hyd_Model_Groundwater::output_result_max2tecplot(void) {
	//get the file name
	string filename = this->Param_GW.get_filename_result_max2file(hyd_label::tecplot);
	//Add file type
	filename += hyd_label::dat;

	if (filename == label::not_set) {
		return;
	}
	//open the file
	this->tecplot_output.open(filename.c_str());
	if (this->tecplot_output.is_open() == false) {
		Error msg = this->set_error(7);
		ostringstream info;
		info << "Filename " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//fileheader
	//output the file header
	this->tecplot_output << "TITLE = " << "\" " << this->Param_GW.GWName << " No. " << this->Param_GW.GWNumber << " \"" << endl;
	this->tecplot_output << "VARIABLES = " << endl;
	this->tecplot_output << "\" x " << label::m << "\" " << endl;
	this->tecplot_output << "\" y " << label::m << "\" " << endl;
	this->tecplot_output << "\" h_max " << label::m << "\" " << endl;
	this->tecplot_output << "\" t(h_max) " << label::sec << "\" " << endl;
	this->tecplot_output << "\" ds_dt_max " << label::m_per_min << "\" " << endl;
	this->tecplot_output << "\" t(ds_dt_max) " << label::sec << "\" " << endl;

	// REVIEW
	this->tecplot_output << "\" CVol_GW_in " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_GW_out " << label::cubicmeter << "\" " << endl;
	/*
	this->tecplot_output << "\" CVol_RV_Sc_in " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_RV_Sc_out " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_RV_Di_in " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_RV_Di_out " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_RV_OV_in " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_RV_OV_out " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_RV_DB_in " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_RV_DB_out " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_CO_OV_in " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_CO_OV_out " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_CO_DB_in " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_CO_DB_out " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_sum_in " << label::cubicmeter << "\" " << endl;
	this->tecplot_output << "\" CVol_sum_out " << label::cubicmeter << "\" " << endl;
	*/

	//output zone header
	this->tecplot_output << "ZONE T= " << " \" 2d-Results " << this->Param_GW.GWName << " No. " << this->Param_GW.GWNumber << "\" " << endl;
	this->tecplot_output << "I=" << this->Param_GW.GWNofX + 1 << ", J=" << this->Param_GW.GWNofY + 1 << ", K=1" << endl;
	this->tecplot_output << "ZONETYPE=ORDERED" << endl;
	this->tecplot_output << "DATAPACKING=BLOCK" << endl;
	this->tecplot_output << "VARLOCATION=([1-2]=NODAL, [3-33]=CELLCENTERED)" << endl;
	this->tecplot_output << "DT=(SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SHORTINT SINGLE SINGLE";
	this->tecplot_output << " SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SINGLE SHORTINT SINGLE SINGLE	SINGLE)";
	this->tecplot_output << endl;
	int counter_endl = 0;
	//x-coordinate
	this->tecplot_output << "#x " << label::m << " NODAL No. " << this->raster.get_number_raster_points() << endl;
	for (int i = 0; i < this->raster.get_number_raster_points(); i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->raster.get_raster_point(i)->get_xcoordinate() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#y " << label::m << " NODAL No. " << this->raster.get_number_raster_points() << endl;
	//y-coordinate
	for (int i = 0; i < this->raster.get_number_raster_points(); i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->raster.get_raster_point(i)->get_ycoordinate() << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#max_h " << label::m << " CELLCENTERED No. " << this->NEQ << endl;
	//max_h_value
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].element_type->get_max_h_value().maximum << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#max_h_time " << label::sec << " CELLCENTERED No. " << this->NEQ << endl;
	//max_h_time
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].element_type->get_max_h_value().time_point << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#max_ds_dt " << label::m_per_min << " CELLCENTERED No. " << this->NEQ << endl;
	//max_ds2dt_value
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].element_type->get_max_ds2dt().maximum*60.0 << "  ";
		counter_endl++;
	}
	counter_endl = 0;
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#max_ds_dt_time " << label::sec << " CELLCENTERED No. " << this->NEQ << endl;
	//max_ds2dt_time
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		this->tecplot_output << this->groundwater_elems[i].element_type->get_max_ds2dt().time_point << "  ";
		counter_endl++;
	}
	// REVIEW
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume in from GW " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling inflow gw
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->groundwater_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->groundwater_elems[i].element_type->get_hydro_balance_coupling_gw().volume_in << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume out to a GW " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling outflow gw
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == Param_GW.get_no_elems_x()) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->groundwater_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->groundwater_elems[i].element_type->get_hydro_balance_coupling_gw().volume_out << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;
	/*
	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume in from RV by structure " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling inflow structure
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_structure_coupling_rv().volume_in << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume out to RV by structure " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling outflow structure
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_structure_coupling_rv().volume_out << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume in direct from RV " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling inflow direct by RV
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_direct_coupling_rv().volume_in << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume out direct to RV " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling outlow direct by RV
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_direct_coupling_rv().volume_out << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume in from RV overflow " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling inflow overflow by RV
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_overflow_coupling_rv().volume_in << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume out to RV overflow " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling outflow overflow by RV
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_overflow_coupling_rv().volume_out << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume in from RV dikebreak " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling inflow dikebreak by RV
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_dikebreak_coupling_rv().volume_in << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume out to RV dikebreak " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling outflow dikebreak by RV
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_dikebreak_coupling_rv().volume_out << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume in from CO overflow " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling inflow overflow by CO
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_overflow_coupling_co().volume_in << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume out to CO overflow " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling outflow overflow by CO
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_overflow_coupling_co().volume_out << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume in from CO dikebreak " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling inflow dikebreak by RV
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_dikebreak_coupling_co().volume_in << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume out to CO dikebreak " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling outflow dikebreak by RV
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_hydro_balance_dikebreak_coupling_co().volume_out << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume total in " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling inflow dikebreak by RV
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_total_coupling_inflow_volume() << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	this->tecplot_output << "#Couplingvolume total out " << label::cubicmeter << " CELLCENTERED No. " << this->NEQ << endl;
	//coupling outflow dikebreak by RV
	for (int i = 0; i < this->NEQ; i++) {
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output << endl;
		}
		if (this->floodplain_elems[i].element_type->get_coupling_flag() == true) {
			this->tecplot_output << this->floodplain_elems[i].element_type->get_total_coupling_outflow_volume() << "  ";
		}
		else {
			this->tecplot_output << 0.0 << "  ";
		}
		counter_endl++;
	}
	counter_endl = 0;

	this->tecplot_output << endl << endl;
	//close the file
	this->tecplot_output.close();
	this->tecplot_output.clear();
	*/
}

//Output the maximum result members to bluekenue REVIEW
void Hyd_Model_Groundwater::output_result_max2bluekenue(void) {

	//get the file name
	string filename = this->Param_GW.get_filename_result_max2file(hyd_label::bluekenue);
	//Add file type
	filename += hyd_label::r2s;

	if (filename == label::not_set) {
		return;
	}
	//open the file
	this->bluekenue_output.open(filename.c_str());
	if (this->bluekenue_output.is_open() == false) {
		Error msg = this->set_error(7);
		ostringstream info;
		info << "Filename " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//fileheader
	//output the file header
	//output geometrie header
	this->bluekenue_output << endl << endl;
	this->bluekenue_output << "####################################################################### " << endl;
	this->bluekenue_output << ":FileType r2s ASCII EnSim 1.0  " << endl;
	this->bluekenue_output << "# Canadian Hydraulics Centre/National Research Council (c) 1998-2005  " << endl;
	this->bluekenue_output << "# DataType 2D Rect Scalar   (Element midpoints are displayed!) " << endl;
	this->bluekenue_output << ":Application BlueKenue      " << endl;
	this->bluekenue_output << ":Version 3.3.4      " << endl;
	this->bluekenue_output << ":WrittenBy       ProMaIDes HS-M" << endl;
	this->bluekenue_output << ":CreationDate " << Sys_Output_Division::set_time() << endl;
	this->bluekenue_output << "#------------------------------------------------------------------------" << endl << endl;
	this->bluekenue_output << ":Name Max values of groundwater " << this->Param_GW.get_groundwater_name() << endl;
	this->bluekenue_output << ":Title " << this->Param_GW.get_groundwater_name() << endl << endl;
	this->bluekenue_output << ":Projection Cartesian" << endl;
	this->bluekenue_output << ":Ellipsoid Unknown " << endl << endl;
	this->bluekenue_output << ":xOrigin " << P(5) << FORMAT_FIXED_REAL << this->groundwater_elems[0].get_mid_point()->get_xcoordinate() << endl;
	this->bluekenue_output << ":yOrigin " << P(5) << FORMAT_FIXED_REAL << this->groundwater_elems[0].get_mid_point()->get_ycoordinate() << endl;
	this->bluekenue_output << ":xCount " << this->Param_GW.GWNofX << endl;
	this->bluekenue_output << ":yCount " << this->Param_GW.GWNofY << endl;
	this->bluekenue_output << ":xDelta " << this->Param_GW.width_x << endl;
	this->bluekenue_output << ":yDelta " << this->Param_GW.width_y << endl;
	this->bluekenue_output << ":Angle " << this->Param_GW.angle << endl;
	//output geometrie attribute

	this->bluekenue_output << ":AttributeName  1 h_max" << endl;
	this->bluekenue_output << ":AttributeUnits 1 " << label::m << endl;
	this->bluekenue_output << ":AttributeName  2 t(h_max)" << endl;
	this->bluekenue_output << ":AttributeUnits 2 " << label::sec << endl;
	this->bluekenue_output << ":AttributeName  3 ds_dt_max" << endl;
	this->bluekenue_output << ":AttributeUnits 3 " << label::m_per_min << endl;
	this->bluekenue_output << ":AttributeName  4 t(ds_dt_max)" << endl;
	this->bluekenue_output << ":AttributeUnits 4 " << label::sec<< endl;


	this->bluekenue_output << ":EndHeader " << endl << endl;

	int counter_endl = 0;

	//required?
	//this->tecplot_output << "\" CVol_FP_in " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_FP_out " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_RV_Sc_in " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_RV_Sc_out " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_RV_Di_in " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_RV_Di_out " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_RV_OV_in " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_RV_OV_out " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_RV_DB_in " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_RV_DB_out " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_CO_OV_in " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_CO_OV_out " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_CO_DB_in " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_CO_DB_out " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_sum_in " << label::cubicmeter << "\" " << endl;
	//this->tecplot_output << "\" CVol_sum_out " << label::cubicmeter << "\" " << endl;

	
	counter_endl = 0;
	this->bluekenue_output << endl << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].element_type->get_max_h_value().maximum << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}
	counter_endl = 0;
	this->bluekenue_output << endl << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].element_type->get_max_h_value().time_point << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}
	counter_endl = 0;
	this->bluekenue_output << endl << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].element_type->get_max_ds2dt().maximum*60.0 << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}
	counter_endl = 0;
	this->bluekenue_output << endl << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->bluekenue_output << this->groundwater_elems[i].element_type->get_max_ds2dt().time_point << "  ";
		counter_endl++;
		if (counter_endl == this->Param_GW.GWNofX) {
			counter_endl = 0;
			this->bluekenue_output << endl;
		}
	}


	//close the file
	this->bluekenue_output.close();
	this->bluekenue_output.clear();

}
//Output the maximum result members to paraview
void Hyd_Model_Groundwater::output_result_max2paraview(void) {

	//get the file name
	string filename = this->Param_GW.get_filename_result_max2file(hyd_label::paraview);
	//Add file type
	filename += hyd_label::vtk;

	if (filename == label::not_set) {
		return;
	}
	//open the file
	this->tecplot_output.open(filename.c_str());
	if (this->tecplot_output.is_open() == false) {
		Error msg = this->set_error(7);
		ostringstream info;
		info << "Filename " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//fileheader
	//output the file header
	this->tecplot_output << "# vtk DataFile Version 3.0"<< endl;
	this->tecplot_output << "Maximum results of groundwater " << this->Param_GW.GWNumber <<" with groundwater name "<< this->Param_GW.GWName << endl;
	this->tecplot_output << "ASCII" << endl;
	this->tecplot_output << "DATASET STRUCTURED_GRID" << endl;
	this->tecplot_output << "DIMENSIONS " << this->Param_GW.GWNofX+1<< " " << this->Param_GW.GWNofY +1<< " 1"<< endl;
	this->tecplot_output << "POINTS " <<(this->Param_GW.GWNofX + 1)*(this->Param_GW.GWNofY + 1) << " double" <<endl;

	//output raster
	for (int i = 0; i < this->raster.get_number_raster_points(); i++) {
		this->tecplot_output << this->raster.get_raster_point(i)->get_xcoordinate() << " " << this->raster.get_raster_point(i)->get_ycoordinate() << "  0" << endl;
	}

	this->tecplot_output << endl << endl;
	string buff_unit;
	//output data
	this->tecplot_output << "CELL_DATA " << this->NEQ << endl;
	this->tecplot_output << endl << endl;
	buff_unit = " h_max_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  "<<buff_unit<< " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].element_type->get_max_h_value().maximum << "  ";
	}
	
	this->tecplot_output << endl << endl;
	buff_unit = " t(h_max)_";
	buff_unit += label::sec;
	buff_unit=functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  "<< buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].element_type->get_max_h_value().time_point << "  ";
	}

	this->tecplot_output << endl << endl;
	buff_unit = " ds_dt_max_";
	buff_unit += label::m_per_min;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].element_type->get_max_ds2dt().maximum*60.0 << "  ";
	}
	this->tecplot_output << endl << endl;
	buff_unit = " t(ds_dt_max)_";
	buff_unit += label::sec;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->NEQ; i++) {
		this->tecplot_output << this->groundwater_elems[i].element_type->get_max_ds2dt().time_point << "  ";
	}


		//close the file
	this->tecplot_output.close();
	this->tecplot_output.clear();
}

//Output the maximum result members to a database table
void Hyd_Model_Groundwater::output_result_max2database(QSqlDatabase *ptr_database, bool *was_output, const bool must_output){
	//mysql query with the query-class
	//QSqlQuery model(*ptr_database);

	ostringstream cout;
	cout <<"Transfer hydraulic results of the Groundwatermodel " << this->Param_GW.GWNumber <<" to database ..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//evaluate the global identifier
	int id_glob=Hyd_Element_Groundwater::get_max_glob_id_erg_table(ptr_database)+1;

	//Set the query
	QSqlQuery query_buff(*ptr_database);

	//for(int i=0; i<this->NEQ;i++){
	//	this->floodplain_elems[i].output_maximum_calculated_values(&id_glob, &model, ptr_database, this->Param_FP.FPNumber, this->Param_FP.get_geometrical_info(), break_sz);
	//}

	//get the header for the query
	string query_header;
	query_header=Hyd_Element_Groundwater::get_insert_header_erg_data_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter=0;
	string buffer_data;
	bool must_output2=false;
	for(int i=0; i<this->NEQ; i++){
		if(i==this->NEQ-1 && must_output==true && *was_output==false){
			must_output2=true;
		}
		if(i%10000==0 && i>0){
			cout << i <<" ("<<this->get_number_elements()<<") result data of HYD raster elements are transfered to database..."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}

		buffer_data = this->groundwater_elems[i].get_datastring_erg2database(id_glob, this->Param_GW.get_groundwater_number(), this->Param_GW.get_geometrical_info(), must_output2);
		if(buffer_data!=label::not_set){
			query_data <<  buffer_data << " ,";
			//count the global index
			id_glob++;
			counter++;
			*was_output=true;
		}

		//send packages of 100
		if(counter==100){
			query_total<< query_header << query_data.str();
			//delete last komma
			string buff=query_total.str();
			buff.erase(buff.length()-1);
			Data_Base::database_request(&query_buff, buff, ptr_database);

			counter=0;
			if(query_buff.lastError().isValid()){
				Warning msg=this->set_warning(3);
				ostringstream info;
				info << "Table Name                : " << Hyd_Element_Groundwater::erg_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				info << "Data string               : " << query_data.str()<<endl;
				msg.make_second_info(info.str());
				msg.output_msg(2);
			}
			//delete them
			query_total.str("");
			query_data.str("");
			query_buff.clear();
		}
	}
	//send the rest
	if(counter!=0){
		query_total<< query_header << query_data.str();
		//delete last komma
		string buff=query_total.str();
		buff.erase(buff.length()-1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter=0;
		if(query_buff.lastError().isValid()){
			Warning msg=this->set_warning(3);
			ostringstream info;
			info << "Table Name                : " << Hyd_Element_Groundwater::erg_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}
}

//Set the folder for the file-output in case of an database based calculation
void Hyd_Model_Groundwater::set_output_folder(const string folder){
	this->Param_GW.tecplot_outfile_name=folder;
	this->folder_output_db=folder;
}
//Get a pointer to a groundwater element by a given index
Hyd_Element_Groundwater* Hyd_Model_Groundwater::get_ptr_groundwater_elem(const int index){
	if(index <0 || index >= this->NEQ){
		return NULL;
	}
	else{
		return &this->groundwater_elems[index];
	}
}
//Set the indices of a element a the 8 surrounding elements (array of int with 9 elements);
void Hyd_Model_Groundwater::set_relevant_elem_indices(const int current_id, int *elem_index){
	for(int i=0; i<9; i++){
		elem_index[i]=-1;
	}
	if(current_id<0 || current_id>this->NEQ-1){
		return;
	}

	int i_1=1;
	int i_2=2;
	int i_3=3;
	int i_4=4;
	int i_5=5;
	int i_6=6;
	int i_7=7;
	int i_8=8;
	double angle_buff=_Geo_Geometrie::angle_to_360(this->Param_GW.angle);

	//set order
	if(angle_buff<=45.0 || (angle_buff>315.0 && angle_buff<=360.0)){
		i_1=1;
		i_5=2;
		i_6=3;
		i_2=4;
		i_3=5;
		i_4=6;
		i_7=7;
		i_8=8;
	}
	else if(angle_buff>45.0 && angle_buff<=135.0){
		i_4=1;
		i_5=2;
		i_7=3;
		i_1=4;
		i_2=5;
		i_3=6;
		i_6=7;
		i_8=8;
	}
	else if(angle_buff>135.0 && angle_buff<=225.0){
		i_2=1;
		i_7=2;
		i_8=3;
		i_1=4;
		i_3=5;
		i_4=6;
		i_5=7;
		i_6=8;
	}
	else if(angle_buff>225.0 && angle_buff<=315.0){
		i_3=1;
		i_6=2;
		i_8=3;
		i_1=4;
		i_2=5;
		i_4=6;
		i_5=7;
		i_7=8;
	}

	elem_index[0]=current_id;

	elem_index[i_1]=current_id+1;
	if(elem_index[i_1]>this->NEQ-1){
		elem_index[i_1]=-1;
	}

	elem_index[i_2]=current_id-1;
	if(elem_index[i_2]<0){
		elem_index[i_2]=-1;
	}

	elem_index[i_3]=current_id-this->Param_GW.GWNofX;
	if(elem_index[i_3]<0){
		elem_index[i_3]=current_id+this->Param_GW.GWNofX*(this->Param_GW.GWNofY-1);
		if(elem_index[i_3]>this->NEQ-1){
			elem_index[i_3]=-1;
		}
	}

	elem_index[i_4]=current_id+this->Param_GW.GWNofX;
	if(elem_index[i_4]>this->NEQ-1){
		elem_index[i_4]=current_id-this->Param_GW.GWNofX*(this->Param_GW.GWNofY-1);
		if(elem_index[i_4]<0){
			elem_index[i_4]=-1;
		}
	}

	elem_index[i_5]=current_id+1+this->Param_GW.GWNofX;
	if(elem_index[i_5]>this->NEQ-1){
		elem_index[i_5]=(current_id+1)-this->Param_GW.GWNofX*(this->Param_GW.GWNofY-1);
		if(elem_index[i_5]<0){
			elem_index[i_5]=-1;
		}
	}

	elem_index[i_6]=current_id+1-this->Param_GW.GWNofX;
	if(elem_index[i_6]<0){
		elem_index[i_6]=(current_id+1)+this->Param_GW.GWNofX*(this->Param_GW.GWNofY-1);
		if(elem_index[i_6]>this->NEQ-1){
			elem_index[i_6]=-1;
		}
	}

	elem_index[i_7]=current_id-1+this->Param_GW.GWNofX;
	if(elem_index[i_7]>this->NEQ-1){
		elem_index[i_7]=(current_id-1)-this->Param_GW.GWNofX*(this->Param_GW.GWNofY-1);
		if(elem_index[i_7]<0){
			elem_index[i_7]=-1;
		}
	}

	elem_index[i_8]=current_id-1-this->Param_GW.GWNofX;
	if(elem_index[i_8]<0){
		elem_index[i_8]=(current_id-1)+this->Param_GW.GWNofX*(this->Param_GW.GWNofY-1);
		if(elem_index[i_8]>this->NEQ-1){
			elem_index[i_8]=-1;
		}
	}
}
//Input elements oer database
void Hyd_Model_Groundwater::input_elems_database(QSqlDatabase *ptr_database, QSqlQueryModel *bound_result, const int number_bound, const bool output_flag, const bool just_elems) {
	QSqlQueryModel prof_query_result;
	//get the number of elements in the table
	int number_elems = 0;

	number_elems = Hyd_Element_Groundwater::count_relevant_elements_database(&prof_query_result, ptr_database, this->system_id, this->Param_GW.GWNumber, output_flag);
	prof_query_result.clear();
	//check the number of found elements
	if (number_elems != this->Param_GW.GWNofX*Param_GW.GWNofY) {
		Error msg = this->set_error(12);
		ostringstream info;
		info << "Table name               : " << Hyd_Model_Groundwater::general_param_table->get_table_name() << endl;
		info << "Number found elements    : " << number_elems << endl;
		info << "Number required elements : " << this->Param_GW.GWNofX*Param_GW.GWNofY << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//allocate the groundwater elements
	this->allocate_elems();
	int counter = 0;
	int counter2 = 0;
	//read in the element members
	int last_index = 0;

	for (int i = 0; i < this->NEQ; i++) {
		if (i == counter * constant::max_rows) {
			prof_query_result.clear();
			Hyd_Element_Groundwater::select_relevant_elements_database(&prof_query_result, ptr_database, this->system_id, this->Param_GW.GWNumber, i, constant::max_rows, output_flag);
			counter++;
			counter2 = 0;
		}
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->groundwater_elems[i].input_members(counter2, &prof_query_result, bound_result, number_bound, just_elems, &last_index);
		counter2++;
	}


	//set the geometry of the elements
	this->set_elem_geometry();

	//set the groundwater elements to the raster
	this->raster.set_groundwater_elem_pointer(this->groundwater_elems);

	//set the points of the groundwater raster
	this->raster.generate_alloc_points_segments();
	prof_query_result.clear();

	ptr_database->close();
	ptr_database->open();
}
//Delete emlements and raster geomtry
void Hyd_Model_Groundwater::delete_elems_raster_geo(void) {
	if (this->groundwater_elems != NULL) {
		delete[] this->groundwater_elems;
		this->groundwater_elems = NULL;
	}

	this->raster.delete_points_segments();
}
//Compare the equality of two hydraulic systems in terms of model numbers; further the models are compared
void Hyd_Model_Groundwater::transfer_glob_elem_id_gw(Hyd_Model_Groundwater *to_gw) {
	for (int i = 0; i < this->NEQ; i++) {
		to_gw->groundwater_elems[i].set_glob_elem_id(this->groundwater_elems[i].get_glob_elem_id());
	}


}
//____________________________________
//private
//Generate the geometrical boundary of the raster polygon
void Hyd_Model_Groundwater::generate_geo_bound_raster_polygon(void) {
	//set the index of the fp_model to the raster
	this->raster.set_index_gw_model(this->Param_GW.get_groundwater_number());
	//set the geometrical info to the raster
	this->raster.set_geometric_info(this->Param_GW.get_geometrical_info());
	//set the groundwater as a geomtrical boundary
	this->raster.make_boundary2polygon();
}
//allocate the elements
void Hyd_Model_Groundwater::allocate_elems(void) {
	//init special members of the model
	//number of elements
	this->NEQ = this->Param_GW.GWNofX*this->Param_GW.GWNofY;
	this->NEQ_real = this->NEQ;
	//number of x-elements
	this->half_bandwidth = this->Param_GW.GWNofX;
	if (this->Param_GW.GWNofX <= BOUND_J) {
		this->ilu_number = -2;
	}

	try {
		this->groundwater_elems = new Hyd_Element_Groundwater[this->NEQ];
	}
	catch (bad_alloc&t) {
		Error msg = this->set_error(0);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//set the element number
	int count = 0;
	for (int j = 0; j < this->Param_GW.GWNofY; j++) {
		for (int i = 0; i < this->Param_GW.GWNofX; i++) {
			this->groundwater_elems[count].set_elem_number(count);
			this->groundwater_elems[count].hyd_sz = &this->hyd_sz;
			this->groundwater_elems[count].set_systemid(this->system_id);
			count++;
		}
	}
}
//Delete the elements
void Hyd_Model_Groundwater::delete_elems(void) {
	if (this->groundwater_elems != NULL) {
		delete[] this->groundwater_elems;
		this->groundwater_elems = NULL;
	}
}
//Allocate the optimized data
void Hyd_Model_Groundwater::allocate_opt_data(void) {
	this->delete_opt_data();
	try {
		this->opt_s_linear = new double[this->Param_GW.GWNofY*this->Param_GW.GWNofX];
		this->opt_h = new double*[this->Param_GW.GWNofY];
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			this->opt_h[i] = NULL;
			this->opt_h[i] = new double[this->Param_GW.GWNofX];
		}

		this->opt_dsdt = new double*[this->Param_GW.GWNofY];
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			this->opt_dsdt[i] = NULL;
			this->opt_dsdt[i] = new double[this->Param_GW.GWNofX];
		}

		this->opt_z = new double*[this->Param_GW.GWNofY];
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			this->opt_z[i] = NULL;
			this->opt_z[i] = new double[this->Param_GW.GWNofX];
		}

		this->opt_s = new double*[this->Param_GW.GWNofY];
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			this->opt_s[i] = NULL;
			this->opt_s[i] = new double[this->Param_GW.GWNofX];
		}

		this->opt_cx = new double*[this->Param_GW.GWNofY];
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			this->opt_cx[i] = NULL;
			this->opt_cx[i] = new double[this->Param_GW.GWNofX];
		}

		this->opt_cy = new double*[this->Param_GW.GWNofY];
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			this->opt_cy[i] = NULL;
			this->opt_cy[i] = new double[this->Param_GW.GWNofX];
		}

		this->opt_zxmax = new double*[this->Param_GW.GWNofY];
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			this->opt_zxmax[i] = NULL;
			this->opt_zxmax[i] = new double[this->Param_GW.GWNofX];
		}
		this->opt_zymax = new double*[this->Param_GW.GWNofY];
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			this->opt_zymax[i] = NULL;
			this->opt_zymax[i] = new double[this->Param_GW.GWNofX];
		}

		this->flow_elem = new bool*[this->Param_GW.GWNofY];
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			this->flow_elem[i] = NULL;
			this->flow_elem[i] = new bool[this->Param_GW.GWNofX];
		}
		this->noflow_x = new bool*[this->Param_GW.GWNofY];
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			this->noflow_x[i] = NULL;
			this->noflow_x[i] = new bool[this->Param_GW.GWNofX];
		}
		this->noflow_y = new bool*[this->Param_GW.GWNofY];
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			this->noflow_y[i] = NULL;
			this->noflow_y[i] = new bool[this->Param_GW.GWNofX];
		}
		this->id_reduced = new int[this->NEQ];
		for (int i = 0; i < this->NEQ; i++) {
			this->id_reduced[i] = -1;
		}

		Sys_Memory_Count::self()->add_mem(sizeof(int)*this->NEQ + sizeof(double) * 5 * this->NEQ + sizeof(double) * 3 * this->NEQ + sizeof(bool) * 5 * this->NEQ, _sys_system_modules::HYD_SYS);
	}
	catch (bad_alloc&t) {
		Error msg = this->set_error(15);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the optimized data
void Hyd_Model_Groundwater::delete_opt_data(void) {
	if (this->opt_h != NULL) {
		for (int i = 0; i < this->Param_GW.GWNofY; i++) {
			delete []this->opt_h[i];
			delete []this->opt_dsdt[i];
			delete []this->opt_z[i];
			delete []this->opt_s[i];
			this->opt_s_linear = NULL;
			delete []this->opt_cx[i];
			delete []this->opt_cy[i];
			delete []this->opt_zxmax[i];
			delete []this->opt_zymax[i];
			delete []this->flow_elem[i];
			delete []this->noflow_x[i];
			delete []this->noflow_y[i];
		}

		delete []this->opt_h;
		delete []this->opt_dsdt;
		this->opt_h = NULL;
		delete []this->opt_z;
		delete []this->opt_s;
		this->opt_s_linear=NULL;
		delete []this->opt_cx;
		delete []this->opt_cy;
		delete []this->opt_zxmax;
		delete []this->opt_zymax;
		delete []this->flow_elem;
		delete []this->noflow_x;
		delete []this->noflow_y;
		if (this->id_reduced != NULL) {
			delete []this->id_reduced;
			this->id_reduced = NULL;
		}
		Sys_Memory_Count::self()->minus_mem(sizeof(int)*this->NEQ + sizeof(double) * 5 * this->NEQ + sizeof(double) * 3 * this->NEQ + sizeof(bool) * 5 * this->NEQ, _sys_system_modules::HYD_SYS);
	}


}
//Allocate the optimized data for coupling condition
void Hyd_Model_Groundwater::allocate_opt_data_coup(void) {

	try {
		this->coup_cond_id = new int[this->number_coup_cond];
		this->coup_cond_dsdt = new double[this->number_coup_cond];
		this->coup_cond_rid = new int[this->number_coup_cond];
	}
	catch (bad_alloc&t) {
		Error msg = this->set_error(16);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	Sys_Memory_Count::self()->add_mem(sizeof(double)*this->number_coup_cond + 2 * sizeof(int)*this->number_coup_cond, _sys_system_modules::HYD_SYS);
}
//Delete the optimized data for coupling condition
void Hyd_Model_Groundwater::delete_opt_data_coup(void) {
	if (this->coup_cond_id != NULL) {
		delete []this->coup_cond_id;
		delete []this->coup_cond_dsdt;
		delete []this->coup_cond_rid;
		this->coup_cond_id = NULL;
		this->coup_cond_dsdt = NULL;
		this->coup_cond_rid = NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->number_coup_cond + 2 * sizeof(int)*this->number_coup_cond, _sys_system_modules::HYD_SYS);
		this->number_coup_cond = 0;
	}
}
//Allocate the optimized data for boundary condition
void Hyd_Model_Groundwater::allocate_opt_data_bound(bool flag) {
	bool get_h = true;
	if (get_h == flag) {
		try {
			//this->bound_cond_id = new int[this->number_bound_cond];
			//this->bound_cond_dsdt = new double[this->number_bound_cond];
			//this->bound_cond_rid = new int[this->number_bound_cond];

			this->hbound_cond_id = new int[this->number_hbound_cond];
			this->hbound_cond_dsdt = new double[this->number_hbound_cond];
			this->hbound_cond_rid = new int[this->number_hbound_cond];

		}
		catch (bad_alloc& t) {
			Error msg = this->set_error(17);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		Sys_Memory_Count::self()->add_mem(sizeof(double) * this->number_hbound_cond + 2 * sizeof(int) * this->number_hbound_cond, _sys_system_modules::HYD_SYS);

	}

	if (get_h != flag) {
		try {
			//this->bound_cond_id = new int[this->number_bound_cond];
			//this->bound_cond_dsdt = new double[this->number_bound_cond];
			//this->bound_cond_rid = new int[this->number_bound_cond];


			this->qbound_cond_id = new int[this->number_qbound_cond];
			this->qbound_cond_dsdt = new double[this->number_qbound_cond];
			this->qbound_cond_rid = new int[this->number_qbound_cond];
		}
		catch (bad_alloc& t) {
			Error msg = this->set_error(17);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		Sys_Memory_Count::self()->add_mem(sizeof(double) * this->number_qbound_cond + 2 * sizeof(int) * this->number_qbound_cond, _sys_system_modules::HYD_SYS);

	}
	

}
//Delete the optimized data for boundary condition
void Hyd_Model_Groundwater::delete_opt_data_bound(bool flag) {
	bool get_h = true;
	if (get_h == flag) {
		if (this->hbound_cond_id != NULL) {
			delete[]this->hbound_cond_id;
			delete[]this->hbound_cond_dsdt;
			delete[]this->hbound_cond_rid;
			this->hbound_cond_id = NULL;
			this->hbound_cond_dsdt = NULL;
			this->hbound_cond_rid = NULL;
			Sys_Memory_Count::self()->minus_mem(sizeof(double) * this->number_hbound_cond + 2 * sizeof(int) * this->number_hbound_cond, _sys_system_modules::HYD_SYS);

			this->number_hbound_cond = 0;
		}
	}

	if (get_h != flag) {
		if (this->qbound_cond_id != NULL) {
			delete[]this->qbound_cond_id;
			delete[]this->qbound_cond_dsdt;
			delete[]this->qbound_cond_rid;
			this->qbound_cond_id = NULL;
			this->qbound_cond_dsdt = NULL;
			this->qbound_cond_rid = NULL;
			Sys_Memory_Count::self()->minus_mem(sizeof(double) * this->number_qbound_cond + 2 * sizeof(int) * this->number_qbound_cond, _sys_system_modules::HYD_SYS);

			this->number_qbound_cond = 0;
		}
	}

}
//Allocate the reduced optimized data
void Hyd_Model_Groundwater::allocate_opt_data_reduced(void) {
	this->delete_opt_data_reduced();
	try {
		this->id_y = new int[this->NEQ_real];



	}
	catch (bad_alloc&t) {
		Error msg = this->set_error(20);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	for (int i = 0; i < this->NEQ_real; i++) {
		this->id_y[i] = -1;

	}

	Sys_Memory_Count::self()->add_mem(sizeof(int)*this->NEQ_real, _sys_system_modules::HYD_SYS);
}
//Delete the reduced optimized data
void Hyd_Model_Groundwater::delete_opt_data_reduced(void) {
	if (this->id_y != NULL) {
		delete[]this->id_y;

		this->id_y = NULL;

		Sys_Memory_Count::self()->minus_mem(sizeof(int)*this->NEQ_real, _sys_system_modules::HYD_SYS);
	}
}
//Initialize the optimized data
void Hyd_Model_Groundwater::init_opt_data(void) {
	int counter = 0;
	this->NEQ_real = 0;
	for (int i = 0; i < this->Param_GW.GWNofY; i++) {
		for (int j = 0; j < this->Param_GW.GWNofX; j++) {
			if (this->groundwater_elems[counter].get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW ||
				this->groundwater_elems[counter].get_elem_type() == _hyd_gw_elem_type::LINE_ELEM_GW) {
				this->flow_elem[i][j] = true;
				this->id_reduced[counter] = this->NEQ_real;
				/*
				ostringstream cout;
				cout << "counter: "<< counter<< "id_reduced: " << this->id_reduced[counter] << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout); 
				*/
				//change here
				this->NEQ_real++;
			}
			else {
 				this->flow_elem[i][j] = false;
			}
			this->opt_h[i][j] = 0.0;
			this->opt_z[i][j] = this->groundwater_elems[counter].get_z_value();
			this->opt_s[i][j] = this->groundwater_elems[counter].element_type->get_s_value();
			this->opt_cx[i][j] = this->groundwater_elems[counter].element_type->get_cx();
			this->opt_cy[i][j] = this->groundwater_elems[counter].element_type->get_cy();

			this->noflow_x[i][j] = this->groundwater_elems[counter].element_type->get_flow_data().no_flow_x_flag;
			this->noflow_y[i][j] = this->groundwater_elems[counter].element_type->get_flow_data().no_flow_y_flag;
			//this->NEQ_real++;
			counter++;
		}
	}


}
//Initialize the optimized data for boundary and coupling
void Hyd_Model_Groundwater::init_opt_data_bound_coup(void) {
	//init boundary data
	//count
	int counter = 0;

	for (int i = 0; i < this->NEQ; i++) {
		if (this->groundwater_elems[i].element_type->get_h_flag() == true) {

			counter++;
		}
	}
	this->delete_opt_data_bound(true);
	this->number_hbound_cond = counter;
	this->allocate_opt_data_bound(true);

	counter = 0;
	//set it
	for (int i = 0; i < this->NEQ; i++) {
		if (this->groundwater_elems[i].element_type->get_h_flag() == true) {
			this->hbound_cond_id[counter] = i;
			this->hbound_cond_dsdt[counter] = 0.0;
			this->hbound_cond_rid[counter] = this->calc_reduced_id(i);
			counter++;
		}
	}


	counter = 0;
	for (int i = 0; i < this->NEQ; i++) {
		if (this->groundwater_elems[i].element_type->get_q_flag() == true) {

			counter++;
		}
	}
	this->delete_opt_data_bound(false);
	this->number_qbound_cond = counter;
	this->allocate_opt_data_bound(false);
	counter = 0;
	//set it
	for (int i = 0; i < this->NEQ; i++) {
		if (this->groundwater_elems[i].element_type->get_q_flag() == true) {
			this->qbound_cond_id[counter] = i;
			this->qbound_cond_dsdt[counter] = 0.0;
			this->qbound_cond_rid[counter] = this->calc_reduced_id(i);
			counter++;
		}
	}





	//Review
	//init coup data
	//count
	counter = 0;
	for (int i = 0; i < this->NEQ; i++) {
		if (this->groundwater_elems[i].element_type->get_coupling_flag() == true) {
			counter++;
		}
	}
	this->delete_opt_data_coup();
	this->number_coup_cond = counter;
	this->allocate_opt_data_coup();
	counter = 0;
	//set it
	for (int i = 0; i < this->NEQ; i++) {
		if (this->groundwater_elems[i].element_type->get_coupling_flag() == true) {
			this->coup_cond_id[counter] = i;
			this->coup_cond_dsdt[counter] = 0.0;
			this->coup_cond_rid[counter] = this->calc_reduced_id(i);
			counter++;
		}
	}



}

//Update the optimized data by the elements per syncronization step
void Hyd_Model_Groundwater::update_opt_data_by_elems(void) {
	//boundary condition
	for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].element_type->reset_boundary_discharge_gw();
	}

	double bound = 0;
	double coup = 0;
	for (int i = 0; i < this->number_qbound_cond; i++) {
		bound = 0;
		if (this->groundwater_elems[this->qbound_cond_id[i]].element_type->get_q_flag() == true) {
			
			this->qbound_cond_dsdt[i] = this->groundwater_elems[this->qbound_cond_id[i]].element_type->get_bound_discharge(false) / (this->Param_GW.area * this->groundwater_elems[this->qbound_cond_id[i]].element_type->get_flow_data().p_value);
			
			bound = this->qbound_cond_dsdt[i];
			this->groundwater_elems[this->qbound_cond_id[i]].element_type->add_boundary_discharge_gw(bound);
			this->groundwater_elems[this->qbound_cond_id[i]].element_type->set_ds2dt_per_type(3,bound);
			
		}
	}
	for (int i = 0; i < this->number_hbound_cond; i++) {
		//if head flag is true boundary disvcharge is actually just a height
		if (this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_h_flag() == true) {
			int index = 0;
			bound = 0;
			for (int j = 0; j < this->Param_GW.GWNofY; j++) {
				for (int k = 0; k < this->Param_GW.GWNofX; k++) {
					this->opt_s_linear[index] = this->opt_s[j][k];
					index++;
				}
			}
			index = 0;

			if (this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_boundary_data().h_value == -1 || this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_boundary_data().h_value == -2) {
				this->hbound_cond_dsdt[i] = this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_bound_discharge(true);
			}
			else {
				//different directions to account for anisotropy (currently not in use)
				if (this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_boundary_data().boundary_direction == _hyd_bound_direction_type::x_dir) {
					this->hbound_cond_dsdt[i] = this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_cx_bound() * (this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_bound_discharge(true) - this->opt_s_linear[this->hbound_cond_id[i]]);
				}
				else if (this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_boundary_data().boundary_direction == _hyd_bound_direction_type::y_dir) {
					this->hbound_cond_dsdt[i] = this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_cy_bound() * (this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_bound_discharge(true) - this->opt_s_linear[this->hbound_cond_id[i]]);
				}
				else if (this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_boundary_data().boundary_direction == _hyd_bound_direction_type::top_dir) {
					this->hbound_cond_dsdt[i] = this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_ctop_bound() * (this->groundwater_elems[this->hbound_cond_id[i]].element_type->get_bound_discharge(true) - this->opt_s_linear[this->hbound_cond_id[i]]);
				}
			}
			
			
			bound = this->hbound_cond_dsdt[i];
			this->groundwater_elems[this->hbound_cond_id[i]].element_type->add_boundary_discharge_gw(bound);
			this->groundwater_elems[this->hbound_cond_id[i]].element_type->set_ds2dt_per_type(2, bound);
			
		}
	}
	

	//coupling condition
	for (int i = 0; i < this->number_coup_cond; i++) {
		this->coup_cond_dsdt[i] = this->groundwater_elems[this->coup_cond_id[i]].element_type->get_coupling_discharge();//(this->Param_GW.area*this->groundwater_elems[this->coup_cond_rid[i]].element_type->get_flow_data().p_value);
		
	}

	for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].element_type->calculate_ds_dt();

	}
}
//Update the elements by the optimized data per syncronization step
void Hyd_Model_Groundwater::update_elems_by_opt_data(void) {
	int counter = 0;
	for (int i = 0; i < this->Param_GW.GWNofY; i++) {
		for (int j = 0; j < this->Param_GW.GWNofX; j++) {
			this->groundwater_elems[counter].element_type->set_solver_result_value((this->opt_h[i][j]));
			this->groundwater_elems[counter].element_type->set_ds2dt_value((this->opt_dsdt[i][j]));




			counter++;
		}
	}

	/*for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].element_type->calculate_ds_dt();
		
	}*/
}
//Initialize the reduced id
void Hyd_Model_Groundwater::init_reduced_id(void) {
	int counter = 0;
	int counter1 = 0;
	//ostringstream cout;
	for (int i = 0; i < this->Param_GW.get_no_elems_y(); i++) {
		for (int j = 0; j < this->Param_GW.get_no_elems_x(); j++) {
			if (this->flow_elem[i][j] == true) {
				if (this->noflow_y[i][j] == false) {
					
					this->id_y[counter] = this->calc_reduced_id(counter1 + this->Param_GW.get_no_elems_x());
					
				}
				//change here
				counter++;
			}
			//counter++;
			counter1++;
		}
	}
	
	
}
//Calculate the reduced id by a given id
int Hyd_Model_Groundwater::calc_reduced_id(const int id) {

	if (id >= 0 && id < this->NEQ) {
		return this->id_reduced[id];
	}
	else {
		return -1;
	}

}
//Calculate and set maximum step size REVIEW

void Hyd_Model_Groundwater::calc_set_max_step_size(const double next_time_point) {
	
	double delta_t = next_time_point - this->old_time_point_syncron;
	double vmax_x = -99999.0;
	double vmax_y = -99999.0;
	double bufferx = 0.0;
	double buffery = 0.0;
	/*
	for (int i = 0; i < this->NEQ; i++) {
		if (this->groundwater_elems[i].element_type->get_flow_data().poleni_flag_x == false && abs(this->groundwater_elems[i].element_type->get_flowvelocity_vx()) > vmax_x) {
			vmax_x = abs(this->floodplain_elems[i].element_type->get_flowvelocity_vx());
		}
		if (this->floodplain_elems[i].element_type->get_flow_data().poleni_flag_y == false && abs(this->floodplain_elems[i].element_type->get_flowvelocity_vy()) > vmax_y) {
			vmax_y = abs(this->floodplain_elems[i].element_type->get_flowvelocity_vy());
		}
	}
	if (vmax_x > 0.0) {
		bufferx = (1.3*this->Param_FP.width_x) / vmax_x;
	}
	else {
		bufferx = this->max_solver_step;
	}
	if (vmax_y > 0.0) {
		buffery = (1.3*this->Param_FP.width_y) / vmax_y;
	}
	else {
		buffery = this->max_solver_step;
	}

	bufferx = min(bufferx, buffery);
	int step = delta_t / bufferx;
	if (step > 0) {
		bufferx = (double)(delta_t / step);
	}
	else {
		bufferx = this->max_solver_step;
	}
	if (bufferx < 10.0) {
		bufferx = 10.0;
	}
	*/
	bufferx = 10;
	
	if (bufferx < this->max_solver_step) {
		CVodeSetMaxStep(this->cvode_mem, bufferx);
	}
	else {
		CVodeSetMaxStep(this->cvode_mem, this->max_solver_step);
	}
	

}

//read in the groundwater parameters the elem nr the z-value, the kstnr and the init condition
void Hyd_Model_Groundwater::read_elems(void) {
	ifstream ifile;
	//open file
	ifile.open(this->Param_GW.groundwater_file.c_str(), ios_base::in);

	if (ifile.is_open() != true) {
		Error msg = this->set_error(2);
		throw msg;
	}
	//error throw
	string myline;
	int counter = 0;
	int pos = -1;
	int line_counter = 0;
	//search for the begin
	do {
		getline(ifile, myline, '\n');
		line_counter++;
		_Hyd_Parse_IO::erase_comment(&myline);
		_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
		_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
		pos = myline.rfind("!BEGIN");
		if (pos >= 0) {
			myline.erase(pos + 6);
		}
	} while (myline != "!BEGIN" && ifile.eof() != true);
	if (ifile.eof() == true) {
		Error msg = this->set_error(3);
		throw msg;
	}

	//read the elemnts in
	for (int i = 0; i < this->Param_GW.GWNofY*this->Param_GW.GWNofX; i++) {//loop over ny
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		//delete the comments
		pos = 0;
		getline(ifile, myline, '\n');
		line_counter++;
		_Hyd_Parse_IO::erase_comment(&myline);
		_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
		_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
		if (myline.empty() == true) {
			i--;
			continue;
		}
		pos = myline.rfind("!END");
		if (pos >= 0) {
			myline.erase(pos + 4);
		}
		//stop when end is reach
		if (myline == "!END" || ifile.eof() == true) {
			break;
		}
		try {
			this->groundwater_elems[counter].input_value(myline);
		}
		catch (Error msg) {
			ostringstream info;
			info << "Filename               : " << this->Param_GW.groundwater_file << endl;
			info << "Error occurs near line: " << line_counter << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//check the order
		if (counter != this->groundwater_elems[counter].get_elem_number()) {
			Error msg = this->set_error(4);
			ostringstream info;
			info << "Given  index of element in file  : " << this->groundwater_elems[counter].get_elem_number() << endl;
			info << "Needed index of element          : " << counter << endl;
			info << "Error occurs near line           : " << line_counter << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//sum up the counter
		counter++;
	}
	//check if all is read
	if (counter != this->NEQ) {
		Error msg = this->set_error(5);
		ostringstream info;
		info << "Given number of elements          : " << this->Param_GW.GWNofX*this->Param_GW.GWNofY << endl;
		info << "Founded number of elements in file: " << counter << endl;
		info << "Error occurs near line            : " << line_counter << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//check for the !END-flag
	getline(ifile, myline, '\n');
	line_counter++;
	_Hyd_Parse_IO::erase_comment(&myline);
	_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
	_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
	pos = 0;
	pos = myline.rfind("!END");
	if (pos >= 0) {
		myline.erase(pos + 4);
	}
	//no !END-flag is found
	if (myline != "!END") {
		ostringstream info;
		info << "Filename " << this->Param_GW.groundwater_file.c_str() << endl;
		info << "Warning occurs near line: " << line_counter << endl;
		Warning msg = this->set_warning(1);
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
}
//Transfer the groundwater element data to a database
void Hyd_Model_Groundwater::transfer_element_members2database(QSqlDatabase *ptr_database) {
	ostringstream cout;
	cout << "Transfer " << this->Param_GW.GWNofX*this->Param_GW.GWNofY << " elements of the Groundwatermodel to database ..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//evaluate the global identifier

	int id_glob = Hyd_Element_Groundwater::elem_table->maximum_int_of_column(Hyd_Element_Groundwater::elem_table->get_column_name(hyd_label::elemdata_glob_id), ptr_database) + 1;

	//Set the query
	QSqlQuery query_buff(*ptr_database);

	//get the header for the query
	string query_header;
	query_header = Hyd_Element_Groundwater::get_insert_header_data_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;

	for (int i = 0; i < this->NEQ; i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << i << " (" << this->get_number_elements() << ") data of HYD raster elements are transfered to database..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}

		buffer_data = this->groundwater_elems[i].get_datastring2database(ptr_database, id_glob, this->Param_GW.get_groundwater_number(), this->Param_GW.get_geometrical_info());

		query_data << buffer_data << " ,";
		//count the global index
		id_glob++;
		counter++;

		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);

			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(2);
				ostringstream info;
				info << "Table Name                : " << Hyd_Element_Groundwater::elem_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				info << "Data string               : " << query_data.str() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(2);
			}
			//delete them
			query_total.str("");
			query_data.str("");
			query_buff.clear();
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(2);
			ostringstream info;
			info << "Table Name                : " << Hyd_Element_Groundwater::elem_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}

	//QSqlQuery model(*ptr_database);
	//for(int i=0; i< this->NEQ; i++){
	//	if(i%1000==0 && i>0){
	//		cout << "Transfer hydraulic raster elements "<< i <<" to " << i+1000 <<" to database..."<< endl;
	//		Sys_Common_Output::output_hyd->output_txt(&cout);
	//	}
	//	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	//	this->floodplain_elems[i].transfer_element_members2database(id_glob, &model, ptr_database, this->Param_FP.get_floodplain_number(), this->Param_FP.get_geometrical_info());
	//	id_glob++;
	//}
}
//Transfer a hydraulic boundary szenario from file to a database per element
void Hyd_Model_Groundwater::transfer_hydraulic_boundary_sz2database_per_elem(QSqlDatabase *ptr_database) {



	//this->floodplain_elems[i].transfer_hydraulic_boundary_sz2database(ptr_database, this->Param_FP.FPNumber);
		//}
	ostringstream cout;


	//Set the query
	QSqlQuery query_buff(*ptr_database);

	//get the header for the query
	string query_header;
	query_header = Hyd_Element_Groundwater::get_insert_header_bound_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter = 0;
	string buffer_data;

	for (int i = 0; i < this->Param_GW.GWNofY*this->Param_GW.GWNofX; i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << i << " (" << this->get_number_elements() << ") boundary data of HYD raster elements are transfered to database..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}

		buffer_data = this->groundwater_elems[i].get_bound_datastring2database(ptr_database, this->Param_GW.get_groundwater_number());
		if (buffer_data != label::not_set) {
			query_data << buffer_data << " ,";
			//count the global index
			counter++;
		}


		//send packages of 100
		if (counter == 100) {
			query_total << query_header << query_data.str();
			//delete last komma
			string buff = query_total.str();
			buff.erase(buff.length() - 1);
			Data_Base::database_request(&query_buff, buff, ptr_database);

			counter = 0;
			if (query_buff.lastError().isValid()) {
				Warning msg = this->set_warning(5);
				ostringstream info;
				info << "Table Name                : " << Hyd_Element_Groundwater::boundary_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				info << "Data string               : " << query_data.str() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(2);
			}
			//delete them
			query_total.str("");
			query_data.str("");
			query_buff.clear();
		}
	}
	//send the rest
	if (counter != 0) {
		query_total << query_header << query_data.str();
		//delete last komma
		string buff = query_total.str();
		buff.erase(buff.length() - 1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter = 0;
		if (query_buff.lastError().isValid()) {
			Warning msg = this->set_warning(5);
			ostringstream info;
			info << "Table Name                : " << Hyd_Element_Groundwater::boundary_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}


}
//set the geometry of the cells (area and mid point)
void Hyd_Model_Groundwater::set_elem_geometry(void) {
	int count = 0;
	double x_coor = 0.0;
	double y_coor = 0.0;
	Geo_Point mid_point;
	mid_point.set_point_name(label::elem_mid);

	for (int j = 0; j < this->Param_GW.GWNofY; j++) {
		//calc y_coordinate
		y_coor = (double)j*this->Param_GW.width_y;
		y_coor = y_coor + this->Param_GW.width_y / 2.0;

		for (int i = 0; i < this->Param_GW.GWNofX; i++) {
			//calc x_coordinate
			x_coor = (double)i*this->Param_GW.width_x;
			x_coor = x_coor + this->Param_GW.width_x / 2.0;
			//set coordinates to point
			mid_point.set_point_coordinate(x_coor, y_coor);
			//transform it
			mid_point = mid_point.transform_coordinate_system(this->Param_GW.angle, this->Param_GW.GWLowLeftX, this->Param_GW.GWLowLeftY);
			//give it to the elements
			this->groundwater_elems[count].set_mid_point(&mid_point);
			count++;
		}
	}
}
//initialize the elements REVIEW thickness
void Hyd_Model_Groundwater::init_elements(Hyd_Param_Conductivity *conductivity_table, Hyd_Param_Porosity *porosity_table, const bool clone) {
	ostringstream cout;
	cout << "Initialize " << this->NEQ << " elements of the groundwater..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//_hyd_floodplain_geo_info buffer_go_info=this->Param_FP.get_geometrical_info();

	_hyd_neighbouring_gw_elems buffer_neigh_elems;

	try {
		for (int i = this->NEQ - 1; i >= 0; i--) {
			//set a noinfo element
			this->groundwater_elems[i].set_no_info_element(this->Param_GW.noinfo_value);
			//set the neighbouring elements
			buffer_neigh_elems.y_direction = this->set_neighbouring_elements(i, _hyd_neighbouring_direction_gw::Y_DIR_GW);
			buffer_neigh_elems.x_direction = this->set_neighbouring_elements(i, _hyd_neighbouring_direction_gw::X_DIR_GW);
			buffer_neigh_elems.minus_y_direction = this->set_neighbouring_elements(i, _hyd_neighbouring_direction_gw::MINUS_Y_DIR_GW);
			buffer_neigh_elems.minus_x_direction = this->set_neighbouring_elements(i, _hyd_neighbouring_direction_gw::MINUS_X_DIR_GW);


			this->groundwater_elems[i].init_element_type(buffer_neigh_elems, this->Param_GW.get_ptr_width_x(), this->Param_GW.get_ptr_width_y(), this->Param_GW.get_ptr_elem_area(), conductivity_table, porosity_table, clone);

		}

		//connection for the river elemnts
		/*
		for (int i = this->NEQ - 1; i >= 0; i--) {
			//set the neighbouring elements
			buffer_neigh_elems.y_direction = this->set_neighbouring_elements(i, _hyd_neighbouring_direction_gw::Y_DIR_GW);
			buffer_neigh_elems.x_direction = this->set_neighbouring_elements(i, _hyd_neighbouring_direction_gw::X_DIR_GW);
			buffer_neigh_elems.minus_y_direction = this->set_neighbouring_elements(i, _hyd_neighbouring_direction_gw::MINUS_Y_DIR_GW);
			buffer_neigh_elems.minus_x_direction = this->set_neighbouring_elements(i, _hyd_neighbouring_direction_gw::MINUS_X_DIR_GW);


			//this->groundwater_elems[i].element_type->set_connected_element(buffer_neigh_elems);
		}
		*/
	}
	catch (Error msg) {
		throw msg;
	}
}
//set the init condition to the result vector
void Hyd_Model_Groundwater::set_initcond2resultvector(void) {
	int counter = 0;
	for (int i = 0; i < this->NEQ; i++) {
		if (this->groundwater_elems[i].get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW ||
			this->groundwater_elems[i].get_elem_type() == _hyd_gw_elem_type::LINE_ELEM_GW) {
			this->results_real[counter] = this->groundwater_elems[i].get_flow_data().init_condition;
			counter++;
		}
		//change here
		//counter++;
	}
}
//allocate the instationary boundary curve
void Hyd_Model_Groundwater::allocate_instat_boundarycurves(void) {
	try {
		this->instat_boundary_curves_gw = new Hyd_Instationary_Boundary_GW[this->Param_GW.number_instat_boundary];
	}
	catch (bad_alloc&) {
		Error msg = this->set_error(0);
		throw msg;
	}
	//set the szenario data
	for (int i = 0; i < this->Param_GW.number_instat_boundary; i++) {
		this->instat_boundary_curves_gw[i].hyd_sz.set_members(&this->hyd_sz);
		this->instat_boundary_curves_gw[i].set_systemid(this->system_id);
	}
}
//Read the instationary boundary curves per file
void Hyd_Model_Groundwater::read_instationary_boundary_file(void) {
	//instationary boundary curves
	try {
		if (this->Param_GW.number_instat_boundary > 0) {
			//read in the instationary boundary curves
			ostringstream cout;
			cout << "Read in " << this->Param_GW.number_instat_boundary << " instationary boundary curves of the Groundwatermodel from file " << this->Param_GW.inst_boundary_file << " ..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//allocate instat boundary
			this->allocate_instat_boundarycurves();
			//read it in
			for (int i = 0; i < this->Param_GW.number_instat_boundary; i++) {
				this->instat_boundary_curves_gw[i].read_value(this->Param_GW.inst_boundary_file, i);
			}
		}
	}
	catch (Error msg) {
		throw msg;
	}
}
//Read in the instationary boundary curves from a database
void Hyd_Model_Groundwater::read_instat_boundary_curves(const bool output_flag, QSqlDatabase *ptr_database) {
	QSqlTableModel query_result(0, *ptr_database);
	int number = 0;

	number = Hyd_Instationary_Boundary_GW::select_relevant_curves_database(&query_result, this->Param_GW.GWNumber, _hyd_model_type_gw::GW_GROUNDWATER_MODEL, output_flag, &this->hyd_sz);

	if (number <= 0) {
		this->Param_GW.number_instat_boundary = 0;
		return;
	}
	else {
		this->Param_GW.number_instat_boundary = number;
		this->allocate_instat_boundarycurves();
		for (int i = 0; i < number; i++) {
			this->instat_boundary_curves_gw[i].read_value(&query_result, i);
		}
	}
}
//Output the instationary boundary curves
void Hyd_Model_Groundwater::output_instationary_boundary_curves(void) {
	if (this->Param_GW.number_instat_boundary > 0) {
		ostringstream cout;
		cout << "Output the instationary boundary curves (" << this->Param_GW.number_instat_boundary << ")..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//output the instat boundary curve patarmeters
		for (int i = 0; i < this->Param_GW.number_instat_boundary; i++) {
			this->instat_boundary_curves_gw[i].output_member();
		}
	}
}
//connet the elments to the instat boundary curves
void Hyd_Model_Groundwater::connect_elems2instat_boundarycurves(void) {
	if (this->Param_GW.number_instat_boundary > 0) {
		ostringstream cout;
		cout << "Connect the groundwater elements with the instationary boundary curves..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		//search for the link
		for (int i = 0; i < this->Param_GW.number_instat_boundary; i++) {//loop over all curves
			for (int j = 0; j < this->NEQ; j++) {//loop over all elems
				try {
					this->groundwater_elems[j].element_type->connect_instat_boundarycurve(&this->instat_boundary_curves_gw[i]);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Elementnumber: " << j << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
	}
	//check if all are found
	for (int j = 0; j < this->NEQ; j++) {//loop over all elems
		if (this->groundwater_elems[j].get_boundary_data().found_flag != true) {
			Error msg = this->set_error(1);
			ostringstream info;
			info << "Elementnumber: " << j << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Allocate the noflow polygon
void Hyd_Model_Groundwater::allocate_noflow_polygons(void) {
	try {
		this->noflow_polygons = new Hyd_Groundwater_Polygon[this->Param_GW.number_noflow_polys];
	}
	catch (bad_alloc &) {
		Error msg = this->set_error(0);
		throw msg;
	}
	//set the noflow type
	for (int i = 0; i < this->Param_GW.number_noflow_polys; i++) {
		this->noflow_polygons[i].set_elementtype_inside(_hyd_gw_elem_type::NOFLOW_ELEM_GW);
	}
}
//Read the noflow polygons per file
void Hyd_Model_Groundwater::read_noflow_polygons_file(void) {
	ostringstream cout;
	try {
		//noflow polygons
		if (this->Param_GW.number_noflow_polys > 0) {
			cout << "Read in " << this->Param_GW.number_noflow_polys << " noflow polygons of the Groundwatermodel from file " << this->Param_GW.noflow_file << " ..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//allocate noflow polygons
			this->allocate_noflow_polygons();
			//read it in
			for (int i = 0; i < this->Param_GW.number_noflow_polys; i++) {
				this->noflow_polygons[i].input_members(i, this->Param_GW.noflow_file);
			}
		}
	}
	catch (Error msg) {
		throw msg;
	}
}
//Read in the noflow polygons from a database
void Hyd_Model_Groundwater::read_noflow_polygons_database(const bool output_flag, QSqlDatabase *ptr_database) {
	QSqlQueryModel query_result;
	int number = 0;

	number = Hyd_Groundwater_Polygon::select_relevant_polygons_database(&query_result, ptr_database, this->Param_GW.GWNumber, output_flag);

	if (number <= 0) {
		this->Param_GW.number_noflow_polys = 0;
		return;
	}
	else {
		this->Param_GW.number_noflow_polys = number;
		this->allocate_noflow_polygons();
		for (int i = 0; i < number; i++) {
			this->noflow_polygons[i].input_members(&query_result, ptr_database, i);
		}
	}
}
//Output the noflow polygons
void Hyd_Model_Groundwater::output_noflow_polygons(void) {
	if (this->Param_GW.number_noflow_polys > 0) {
		ostringstream cout;
		cout << "Output the noflow polygons (" << this->Param_GW.number_noflow_polys << ")..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//output the instat boundary curve patarmeters
		for (int i = 0; i < this->Param_GW.number_noflow_polys; i++) {
			this->noflow_polygons[i].output_members();
		}
	}
}
//Delete the noflow polygons
void Hyd_Model_Groundwater::delete_noflow_polygons(void) {
	if (this->noflow_polygons != NULL) {
		delete[]this->noflow_polygons;
		this->noflow_polygons = NULL;
	}
}
//Connect the elements with the noflow polygons
void Hyd_Model_Groundwater::connect_elems2noflowpolygons(void) {
	if (this->Param_GW.number_noflow_polys > 0) {
		ostringstream cout;
		cout << "Intercept the noflow polygons with the raster ..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for (int i = 0; i < this->Param_GW.number_noflow_polys; i++) {
			this->raster.set_rasterpolygon_inside(&(this->noflow_polygons[i]));
		}
	}
}
//Allocate the line polysegments
void Hyd_Model_Groundwater::allocate_line_polysegments(void) {
	try {
		this->line_polysegments = new Hyd_Groundwater_Polysegment[this->Param_GW.number_polylines];
	}
	catch (bad_alloc &) {
		Error msg = this->set_error(0);
		throw msg;
	}

	for (int i = 0; i < this->Param_GW.number_polylines; i++) {
		this->line_polysegments[i].set_line_type(_hyd_groundwater_polysegment_type::GW_BARRIER);
		this->line_polysegments[i].set_systemid(this->system_id);
	}
}
//Read the line polysegments per file
void Hyd_Model_Groundwater::read_line_polysegments_file(void) {
	ostringstream cout;
	try {
		//line polysegments
		if (this->Param_GW.number_polylines > 0) {
			//read in the instationary boundary curves
			cout << "Read in " << this->Param_GW.number_polylines << " line polysegments of the Groundwatermodel from file " << this->Param_GW.line_file << " ..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//allocate instat boundary
			this->allocate_line_polysegments();
			//read it in
			for (int i = 0; i < this->Param_GW.number_polylines; i++) {
				this->line_polysegments[i].input_members(i, this->Param_GW.line_file);
			}
		}
	}
	catch (Error msg) {
		throw msg;
	}
}
//Read in the line polysegments from a database
void Hyd_Model_Groundwater::read_line_polysegments_database(const bool output_flag, QSqlDatabase *ptr_database) {
	QSqlQueryModel query_result;
	int number = 0;

	number = Hyd_Groundwater_Polysegment::select_relevant_polysegments_database(&query_result, ptr_database, this->system_id, this->Param_GW.GWNumber, false, output_flag);

	if (number <= 0) {
		this->Param_GW.number_polylines = 0;
		return;
	}
	else {
		this->Param_GW.number_polylines = number;
		this->allocate_line_polysegments();
		for (int i = 0; i < number; i++) {
			this->line_polysegments[i].input_members(&query_result, ptr_database, i);
		}
	}
}
//Output the line polysegments
void Hyd_Model_Groundwater::output_line_polysegments(void) {
	if (this->Param_GW.number_polylines > 0) {
		ostringstream cout;
		cout << "Output the line(s) (" << this->Param_GW.number_polylines << ")..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//output the instat boundary curve patarmeters
		for (int i = 0; i < this->Param_GW.number_polylines; i++) {
			this->line_polysegments[i].output_members();
		}
	}
}
//Delete the line polysegments
void Hyd_Model_Groundwater::delete_line_polysegments(void) {
	ostringstream cout;
	if (this->line_polysegments != NULL) {
		cout << "entered if condition with " << this->line_polysegments << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		delete[]this->line_polysegments;
		cout << "deleted polysegments" << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		this->line_polysegments = NULL;
		cout << "set polysegments to NULL" << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
}
//Connect the elements with the line polysegments
void Hyd_Model_Groundwater::connect_elems2line_polysegments(Hyd_Param_Conductivity* conductivity_table) {
	if (this->Param_GW.number_polylines > 0) {
		ostringstream cout;
		cout << "Intercept the line with the raster ..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for (int i = 0; i < this->Param_GW.number_polylines; i++) {
			this->raster.interception_polysegments2raster(&(this->line_polysegments[i]),conductivity_table);
		}
	}
}
//Set the neighbouring elements with a given element index
Hyd_Element_Groundwater* Hyd_Model_Groundwater::set_neighbouring_elements(const int index, _hyd_neighbouring_direction_gw dir) {
	Hyd_Element_Groundwater *ptr_buff = NULL;
	int counter = 0;
	switch (dir) {
	case _hyd_neighbouring_direction_gw::Y_DIR_GW:
		counter = (int)(index / this->Param_GW.GWNofX);
		if (counter == this->Param_GW.GWNofY - 1) {
			ptr_buff = NULL;
		}
		else {
			ptr_buff = &(this->groundwater_elems[index + this->Param_GW.GWNofX]);
		}
		break;
	case _hyd_neighbouring_direction_gw::X_DIR_GW:
		counter = (index + 1) % this->Param_GW.GWNofX;
		if (counter == 0) {
			ptr_buff = NULL;
		}
		else {
			ptr_buff = &(this->groundwater_elems[index + 1]);
		}
		break;
	case _hyd_neighbouring_direction_gw::MINUS_Y_DIR_GW:
		counter = (int)(index / this->Param_GW.GWNofX);
		if (counter == 0) {
			ptr_buff = NULL;
		}
		else {
			ptr_buff = &(this->groundwater_elems[index - this->Param_GW.GWNofX]);
		}
		break;
	case _hyd_neighbouring_direction_gw::MINUS_X_DIR_GW:
		counter = index % this->Param_GW.GWNofX;
		if (counter == 0) {
			ptr_buff = NULL;
		}
		else {
			ptr_buff = &(this->groundwater_elems[index - 1]);
		}

		break;
	default:
		ptr_buff = NULL;
	}
	return ptr_buff;
}
//set function to solve to the solver REVIEW
void Hyd_Model_Groundwater::set_function2solver(void) {
	int flag = -1;

	//set the function where the diff equation is specified
	//The function CVodeMalloc provides required problem and solution speci¯cations, allocates internal memory, and initializes cvode.
	//flag = CVodeMalloc(this->cvode_mem, f2D_equation2solve, 0.0, this->results, CV_SS, this->Param_FP.get_relative_solver_tolerance(),this->Param_FP.get_absolute_solver_tolerance()); REVIEW
	flag = CVodeInit(this->cvode_mem, f2D_equation2solve_gw, 0.0, this->results);

	this->setted_rel_tol = this->Param_GW.get_relative_solver_tolerance();
	this->setted_abs_tol = *(this->Param_GW.get_absolute_solver_tolerance());
	this->change_abs_tol = this->setted_abs_tol;
	this->change_rel_tol = this->setted_rel_tol;

	if (flag < 0) {
		Error msg = this->set_error(11);
		ostringstream info;
		info << "Solver function: CVodeMalloc(this->cvode_mem, f2D_equation2solve_gw, global_params->get_startime(), this->results, CV_SS, this->Param_GW.get_relative_solver_tolerance(), this->Param_GW.get_absolute_solver_tolerance())" << endl;
		info << "CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//The function CVodeSetFdata speci¯es the user data block f data, for use by the user right-hand side function f, and attaches it to the main cvode memory block
	//flag = CVodeSetFdata(this->cvode_mem, this);
	flag = CVodeSetUserData(this->cvode_mem, this);

	if (flag < 0) {
		Error msg = this->set_error(11);
		ostringstream info;
		info << "Solver function: CVodeSetFdata(this->cvode_mem, this)" << endl;
		info << "CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	flag = CVodeSStolerances(this->cvode_mem, this->Param_GW.get_relative_solver_tolerance(), *this->Param_GW.get_absolute_solver_tolerance());
	//flag=CVodeSStolerances(this->cvode_mem, this->Param_FP.get_relative_solver_tolerance(),1.0e-5);
	if (flag < 0) {
		Error msg = this->set_error(11);
		ostringstream info;
		info << "Solver function: CVodeSStolerances(this->cvode_mem, this->Param_GW.get_relative_solver_tolerance(),*this->Param_GW.get_absolute_solver_tolerance())" << endl;
		info << "CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//output final statistics of the solver
void Hyd_Model_Groundwater::output_solver_statistics(void) {
	ostringstream cout;
	cout << "SOLVER" << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	_Hyd_Model::output_solver_statistics(&cout);
}
//output the maximum values of the elements
void Hyd_Model_Groundwater::output_maximum_values(void) {
	ostringstream cout;
	//maximum values
	Hyd_Element_Groundwater::output_header_maximum_calculated_values(&cout);
	for (int i = 0; i < this->NEQ; i++) {
		this->groundwater_elems[i].output_maximum_calculated_values(&cout);
		Sys_Common_Output::output_hyd->output_txt(&cout, true);
	}
}
//Count element types
void Hyd_Model_Groundwater::count_element_types(void) {
	for (int i = 0; i < this->NEQ; i++) {
		switch (this->groundwater_elems[i].get_elem_type()) {
		case _hyd_gw_elem_type::LINE_ELEM_GW:
			this->number_line_elems++;
			break;
		case _hyd_gw_elem_type::NOFLOW_ELEM_GW:
			this->number_noflow_elems++;
			break;
		case _hyd_gw_elem_type::NOINFO_ELEM_GW:
			this->number_noinfo_elems++;
			break;
		case _hyd_gw_elem_type::OTHER_GW_ELEM:
			this->number_gwinside_elems++;
			break;
		case _hyd_gw_elem_type::RIVER_ELEM_GW:
			this->number_river_elems++;
			break;
		case _hyd_gw_elem_type::STANDARD_ELEM_GW:
			this->number_standard_elems++;
			break;
		default:
			Error msg = this->set_error(10);
			ostringstream info;
			info << "Elementno. " << i << endl;
			throw msg;
		}
	}
}
//Output the hydrological balance of the groundwater model REVIEW

void Hyd_Model_Groundwater::output_hydrological_balance(void) {
	ostringstream cout;
	cout << "HYDROLOGICAL BALANCE " << label::cubicmeter << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	cout << " Total watervolume init-cond.             :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_watervolume_init << endl;
	cout << " Total watervolume left                   :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_watervolume << endl;
	if (abs(this->total_volume_error_zero_outflow) > constant::zero_epsilon) {
		cout << " Total error dry outflow                  :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_volume_error_zero_outflow << endl;
	}
	cout << " Total boundaryvolume                     :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_boundary_volume.volume_total << endl;
	if (abs(this->total_boundary_volume.volume_out) > constant::zero_epsilon || abs(this->total_boundary_volume.volume_in) > constant::zero_epsilon) {
		cout << "  Boundaryvolume in                       :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_boundary_volume.volume_in << endl;
		cout << "  Boundaryvolume out                      :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_boundary_volume.volume_out << endl;
	}
	//REVIEW
	cout << " Total couplingvolume (2d)                :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_coupling_gw_volume.volume_total << endl;
	if (abs(this->total_coupling_gw_volume.volume_out) > constant::zero_epsilon || abs(this->total_coupling_gw_volume.volume_in) > constant::zero_epsilon) {
		cout << "  Couplingvolume in   (2d)                :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_coupling_gw_volume.volume_in << endl;
		cout << "  Couplingvolume out  (2d)                :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_coupling_gw_volume.volume_out << endl;
	}
	
	cout << " Total couplingvolume (1d-structure)      :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_structure_coupling_rv_volume.volume_total << endl;
	if (abs(this->total_structure_coupling_rv_volume.volume_out) > constant::zero_epsilon || abs(this->total_structure_coupling_rv_volume.volume_in) > constant::zero_epsilon) {
		cout << "  Couplingvolume in   (1d-structure)      :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_structure_coupling_rv_volume.volume_in << endl;
		cout << "  Couplingvolume out  (1d-structure)      :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_structure_coupling_rv_volume.volume_out << endl;
	}
	
	cout << " Total couplingvolume (1d-direct)         :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_direct_coupling_rv_volume.volume_total << endl;
	if (abs(this->total_direct_coupling_rv_volume.volume_out) > constant::zero_epsilon || abs(this->total_direct_coupling_rv_volume.volume_in) > constant::zero_epsilon) {
		cout << "  Couplingvolume in   (1d-direct)         :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_direct_coupling_rv_volume.volume_in << endl;
		cout << "  Couplingvolume out  (1d-direct)         :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_direct_coupling_rv_volume.volume_out << endl;
	}
	
	cout << " Total couplingvolume (1d-leakage)         :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_leakage_coupling_rv_volume.volume_total << endl;
	if (abs(this->total_leakage_coupling_rv_volume.volume_out) > constant::zero_epsilon || abs(this->total_leakage_coupling_rv_volume.volume_in) > constant::zero_epsilon) {
		cout << "  Couplingvolume in   (1d-direct)         :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_leakage_coupling_rv_volume.volume_in << endl;
		cout << "  Couplingvolume out  (1d-direct)         :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->total_leakage_coupling_rv_volume.volume_out << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	cout << " Sum of volumes                           :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->sum_volumes << endl;
	cout << " Error [%]                                :" << W(15) << P(1) << FORMAT_FIXED_REAL << this->error_hydrological_balance << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
}
//Calculate the hydrological balance of the groundwater model
void Hyd_Model_Groundwater::calculate_total_hydrological_balance(void) {
	//reset the values
	this->reset_total_hydrological_balance();

	//calculated it new
	for (int i = 0; i < this->NEQ; i++) {
		this->total_watervolume = total_watervolume + this->groundwater_elems[i].element_type->get_watervolume_element();
		this->total_watervolume_init = this->total_watervolume_init + this->groundwater_elems[i].element_type->get_watervolume_init();
		if (this->groundwater_elems[i].element_type->get_bound_flag() == true) {
			this->groundwater_elems[i].element_type->add_hydro_balance_boundary(&this->total_boundary_volume);
		}
		//REVIEW
		
		if (this->groundwater_elems[i].element_type->get_coupling_flag() == true) {
			this->groundwater_elems[i].element_type->add_hydro_balance_coupling_gw(&this->total_coupling_gw_volume);
			this->groundwater_elems[i].element_type->add_hydro_balance_leakage_coupling_rv(&this->total_leakage_coupling_rv_volume);
		}
		
	}

	this->total_volume_error_zero_outflow = this->error_zero_outflow_volume*this->Param_GW.area*-1.0;

	//make the sum
	this->sum_volumes = this->total_watervolume - (this->total_watervolume_init + this->total_boundary_volume.volume_total + this->total_coupling_gw_volume.volume_total + this->total_structure_coupling_rv_volume.volume_total
		+ this->total_direct_coupling_rv_volume.volume_total + this->total_leakage_coupling_rv_volume.volume_total + this->total_volume_error_zero_outflow);

	//calculate the error
	double abs_sum = 0.0;
	abs_sum = abs(this->total_boundary_volume.volume_in) + abs(this->total_boundary_volume.volume_out) +
		abs(this->total_coupling_gw_volume.volume_in) + abs(this->total_coupling_gw_volume.volume_out) +
		abs(this->total_structure_coupling_rv_volume.volume_in) + abs(this->total_structure_coupling_rv_volume.volume_out) +
		abs(this->total_direct_coupling_rv_volume.volume_in) + abs(this->total_direct_coupling_rv_volume.volume_out) +
		abs(this->total_leakage_coupling_rv_volume.volume_in) + abs(this->total_leakage_coupling_rv_volume.volume_out);

	if (abs_sum > 1e-3) {
		this->error_hydrological_balance = (this->sum_volumes / abs_sum)*100.0;
	}
	else {
		this->error_hydrological_balance = 0.0;
	}
}

//Reset the total hydrological balance values REVIEW
void Hyd_Model_Groundwater::reset_total_hydrological_balance(void) {
	//reset the values
	this->total_watervolume = 0.0;
	this->sum_volumes = 0.0;
	this->error_hydrological_balance = 0.0;
	this->total_boundary_volume.volume_in = 0.0;
	this->total_boundary_volume.volume_out = 0.0;
	this->total_boundary_volume.volume_total = 0.0;
	this->total_coupling_gw_volume.volume_in = 0.0;
	this->total_coupling_gw_volume.volume_out = 0.0;
	this->total_coupling_gw_volume.volume_total = 0.0;
	this->total_structure_coupling_rv_volume.volume_in = 0.0;
	this->total_structure_coupling_rv_volume.volume_out = 0.0;
	this->total_structure_coupling_rv_volume.volume_total = 0.0;
	this->total_direct_coupling_rv_volume.volume_in = 0.0;
	this->total_direct_coupling_rv_volume.volume_out = 0.0;
	this->total_direct_coupling_rv_volume.volume_total = 0.0;
	this->total_leakage_coupling_rv_volume.volume_in = 0.0;
	this->total_leakage_coupling_rv_volume.volume_out = 0.0;
	this->total_leakage_coupling_rv_volume.volume_total = 0.0;
	this->total_volume_error_zero_outflow = 0.0;

	this->old_time_point_syncron = 0.0;

	this->q_zero_outflow_error = 0.0;
	this->old_timepoint_error = 0.0;
	this->old_delta_t_error = 0.0;
	this->old_discharge_error = 0.0;
	this->old_discharge_2_error = 0.0;
}
//Get the model description
string Hyd_Model_Groundwater::get_model_description(void) {
	ostringstream txt;
	txt << "Groundwatermodel " << this->Param_GW.GWNumber << ", name : " << this->Param_GW.GWName << endl;
	return txt.str();
}
/*
//Get a noflow line from mid up to down of the geometrical boundary (protection by gw interception) REVIEW if necessarry
void Hyd_Model_Groundwater::get_mid_up2down_noflowline_geo_bound(Hyd_Groundwater_Polysegment *polyline) {
	polyline->set_no_flow_flag(true);
	polyline->set_line_type(_hyd_groundwater_polysegment_type::GW_LINE);
	polyline->set_element_type(_hyd_gw_elem_type::OTHER_GW_ELEM);
	polyline->set_number_of_points(2);
	Geo_Point buff[2];
	buff[0] = this->raster.geometrical_bound.get_segment(0)->calc_mid_point();
	buff[1] = this->raster.geometrical_bound.get_segment(2)->calc_mid_point();
	polyline->set_points(buff);
}
//Set a noflow line from mid left to right of the geometrical boundary (protection by gw interception)
void Hyd_Model_Groundwater::get_mid_left2right_noflowline_geo_bound(Hyd_Groundwater_Polysegment *polyline) {
	polyline->set_no_flow_flag(true);
	polyline->set_line_type(_hyd_groundwater_polysegment_type::GW_LINE);
	polyline->set_element_type(_hyd_gw_elem_type::OTHER_GW_ELEM);
	polyline->set_number_of_points(2);
	Geo_Point buff[2];
	buff[0] = this->raster.geometrical_bound.get_segment(1)->calc_mid_point();
	buff[1] = this->raster.geometrical_bound.get_segment(3)->calc_mid_point();
	polyline->set_points(buff);
}
*/
//Set warning(s)
Warning Hyd_Model_Groundwater::set_warning(const int warn_type) {
	string place = "Hyd_Model_Groundwater::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
	case 0://input datas can not submitted
		place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
		reason = "Can not submit the general parameters of the groundwater model to the database";
		help = "Check the database";
		type = 2;
		break;
	case 1://no !END-flag is found in file
		place.append("read_elems(void)");
		reason = "The the element data block is not closed with !END in file";
		help = "Check the file";
		type = 1;
		break;
	case 2://input datas can not submitted
		place.append("transfer_element_members2database(QSqlDatabase *ptr_database)");
		reason = "Can not submit the element data of the HYD raster to the database";
		help = "Check the database";
		type = 2;
		break;
	case 3://result datas can not submitted
		place.append("output_result_max2database(QSqlDatabase *ptr_database)");
		reason = "Can not submit the result element data of the HYD raster to the database";
		help = "Check the database";
		type = 2;
		break;
	case 4://result datas can not submitted
		place.append("output_result2database(QSqlDatabase *ptr_database, const double timepoint, const int timestep_number, const string time)");
		reason = "Can not submit the result element data of the HYD raster to the database";
		help = "Check the database";
		type = 2;
		break;
	case 5://input datas can not submitted
		place.append("transfer_hydraulic_boundary_sz2database_per_elem(QSqlDatabase *ptr_database)");
		reason = "Can not submit the boundary element data of the HYD raster to the database";
		help = "Check the database";
		type = 2;
		break;

	default:
		place.append("set_warning(const int warn_type)");
		reason = "Unknown flag!";
		help = "Check the flags";
		type = 5;
	}
	msg.set_msg(place, reason, help, reaction, type);
	msg.make_second_info(info.str());
	return msg;
};
//set the error
Error Hyd_Model_Groundwater::set_error(const int err_type) {
	string place = "Hyd_Model_Groundwater::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;
	switch (err_type) {
	case 0://bad alloc
		place.append("allocate_noflow_polygons(void)/allocate_elems(void)/allocate_line_polysegments(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 1://not all curves were found by the connection
		place.append("connect_elems2instat_boundarycurves(void)");
		reason = "Not all boundary curves are found";
		help = "Check the file of boundarycurves or the data of elements";
		type = 1;
		break;
	case 2://can not open the groundwater file
		place.append("read_elems(void)");
		reason = "Can not open the file with the groundwater elements";
		help = "Check the file";
		info << " Filename " << this->Param_GW.groundwater_file << endl;
		type = 5;
		break;
	case 3://do not find the begin block the groundwater file
		place.append("read_elems(void)");
		reason = "Do not find the begin-keyword !BEGIN";
		help = "Check the file";
		info << " Filename " << this->Param_GW.groundwater_file << endl;
		type = 1;
		break;
	case 4://founded element number do not correspond to the index
		place.append("read_elems(void)");
		reason = "Elementnumber in file do not correspond to the index of the groundwater elements; the numbering have to be sequentiell starting by 0";
		help = "Check the file";
		info << " Filename " << this->Param_GW.groundwater_file << endl;
		type = 5;
		break;
	case 5://not all needed elements are found
		place.append("read_elems(void)");
		reason = "Not all needed groundwater elements are found";
		help = "Check the file";
		info << " Filename " << this->Param_GW.groundwater_file << endl;
		type = 5;
		break;
	case 6://could not open the tecplot file
		place.append("output_geometrie2tecplot(void)");
		reason = "Could not open the file for the tecplot output (geometrie) of the groundwater model";
		help = "Check the file";
		type = 5;
		break;
	case 7://could not open the tecplot file
		place.append("output_result2tecplot(const double timepoint, const int timestep_number)");
		reason = "Could not open the file for the tecplot output (geometrie) of the groundwater model";
		help = "Check the file";
		type = 5;
		break;
	case 8://start point have to be outside or at boundary of the groundwater polygon REVIEW
		place.append("intercept_coast_model(Hyd_Groundwater_Polysegment *coastline)");
		reason = "First point of the coastline can not be inside a groundwater polygon";
		help = "Check the coastline";
		type = 11;
		break;
	case 9://end point have to be outside or at boundary of the groundwater polygon REVIEW
		place.append("intercept_coast_model(Hyd_Groundwater_Polysegment *coastline)");
		reason = "Last point of the coastline can not be inside a groundwater polygon";
		help = "Check the coastline";
		type = 11;
		break;
	case 10://not all elements have a type
		place.append("count_element_types(void)");
		reason = "Not all elements have a type; there is a bug in the code";
		help = "Check the element generation";
		type = 6;
		break;
	case 11://problem with the setting of the function to the solver
		place.append("set_function2solver(Hyd_Param_Global *global_params)");
		reason = "Can not set the function to solve to the solver";
		help = "Check the solver settings";
		type = 17;
		break;
	case 12://the number of found elements are not equal to nx*ny
		place.append("input_elems_database(QSqlDatabase *ptr_database, QSqlQueryModel *bound_result, const int number_bound, const bool output_flag, const bool just_elems )");
		reason = "The found number of elements does not equal to nx*ny";
		help = "Check the table";
		type = 2;
		break;
	case 13://the two groundwater models are not equal
		place.append("compare_models(Hyd_Model_Groundwater *compare_model)");
		reason = "The checked groundwater models are not equal";
		help = "Check the groundwater models and ensure their equality";
		type = 18;
		break;
	case 14://the number of found elements are not equal to nx*ny
		place.append("set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database)");
		reason = "The found number of elements does not equal to nx*ny";
		help = "Check the table";
		type = 2;
		break;
	case 15://bad alloc
		place.append("allocate_opt_data(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 16://bad alloc
		place.append("allocate_opt_data_coup(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 17://bad alloc
		place.append("allocate_opt_data_bound(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 18://could not open the bluekenue file
		place.append("output_geometrie2bluekenue(void)");
		reason = "Could not open the file for the bluekenue output (geometrie) of the floodplain model";
		help = "Check the file";
		type = 5;
		break;
	case 19://could not open the bluekenue file for results
		place.append("output_result2bluekenue(const double timepoint, const int timestep_number)");
		reason = "Could not open the file for the bluekenue output (results) of the groundwater model";
		help = "Check the file";
		type = 5;
		break;
	case 20://bad alloc
		place.append("allocate_opt_data_reduced(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	default:
		place.append("set_error(const int err_type)");
		reason = "Unknown flag!";
		help = "Check the flags";
		type = 6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
//__________________________________________
//(static)this eqaution is the differeential equation which is to solve. It receives most data from the groundwater element and groundwater type classes.
//There is actually quite few math here. Most is the application of values that have been calculated in other classes
int f2D_equation2solve_gw(realtype time, N_Vector results, N_Vector ds_dt, void *groundwater_data) {
	
	ostringstream cout;

	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	//cast the groundwater_data
	Hyd_Model_Groundwater *gw_data = (class Hyd_Model_Groundwater*)groundwater_data;

	//connect the N_Vectors
	realtype *result_data = NULL;
	result_data = NV_DATA_S(results);

	realtype *ds_dt_data = NULL;
	ds_dt_data = NV_DATA_S(ds_dt);
	// Initialize dh_dt with zero
	N_VConst(0.0, ds_dt);

	//optimization
	double flow_depth = 0.0;
	double flow_depth_neigh = 0.0;
	double delta_h = 0.0;
	double abs_delta_h = 0.0;
	double ds_dt_buff = 0.0;
	double reduction_term = 0.0;
	int counter = 0;
	int counter1 = 0;
	int noflow_counter = 0;

	//set data
	for (int i = 0; i < gw_data->Param_GW.get_no_elems_y(); i++) { //loop over all elems
		for (int j = 0; j < gw_data->Param_GW.get_no_elems_x(); j++) {
			if (gw_data->flow_elem[i][j] == true) {
				if (result_data[counter] <= constant::dry_hyd_epsilon) { //if there is no water in element
					gw_data->opt_h[i][j] = 0.0;							 // water depth is 0
					gw_data->opt_s[i][j] = gw_data->opt_z[i][j];		 // water table is equal to terrain level
				}
				else {													// if there is water
					gw_data->opt_h[i][j] = result_data[counter];		
					gw_data->opt_s[i][j] = gw_data->opt_z[i][j] + result_data[counter];
				}
				gw_data->opt_s_linear[counter] = gw_data->opt_s[i][j];

				counter++;
			}
			//change here
			//counter++;
		}
	}
	counter = 0;
	//add boundary and coupling
	
	for (int i = 0; i < gw_data->number_hbound_cond; i++) {
		ds_dt_data[gw_data->hbound_cond_rid[i]] = ds_dt_data[gw_data->hbound_cond_rid[i]] + gw_data->hbound_cond_dsdt[i];	//simple addition
	}

	for (int i = 0; i < gw_data->number_qbound_cond; i++) {
		ds_dt_data[gw_data->qbound_cond_rid[i]] = ds_dt_data[gw_data->qbound_cond_rid[i]] + gw_data->qbound_cond_dsdt[i];	//simple addition
	}
	
	for (int i = 0; i < gw_data->number_coup_cond; i++) {
		ds_dt_data[gw_data->coup_cond_rid[i]] = ds_dt_data[gw_data->coup_cond_rid[i]] + gw_data->coup_cond_dsdt[i];
	}
	//alt calculation
	////coupling
	//for (int i = 0; i < gw_data->Param_GW.get_no_elems_y(); i++) { // loop over all elements
	//	for (int j = 0; j < gw_data->Param_GW.get_no_elems_x(); j++) {
	//		if (gw_data->flow_elem[i][j] == true) {					// if there is flow
	//			if (counter == gw_data->coup_cond_rid[counter1]-noflow_counter){
	//				ds_dt_data[counter] = ds_dt_data[counter] + gw_data->coup_cond_dsdt[counter1];
	//				counter1++;
	//			}
	//			
	//		
	//		}
	//		else
	//		{
	//			noflow_counter += 1;
	//		}
	//		counter++;
	//		if (counter1 >= gw_data->number_coup_cond) {
	//			break;
	//		}
	//	}
	//	//change here
	//	//counter++;
	//}
	counter1 = 0;
	noflow_counter = 0;
	counter = 0;
	
	//check data
	int ny = 0;
	int id = 0;
	ny = gw_data->Param_GW.get_no_elems_x();
	double ds_dt_check = 0.0;


	//#pragma omp parallel
	//{
	// omp_set_num_threads(3);
	//#pragma omp for reduction(+:counter)
	clock_t c_start = clock();
	//calculate new data
	for (int i = 0; i < gw_data->Param_GW.get_no_elems_y(); i++) { // loop over all elements
		for (int j = 0; j < gw_data->Param_GW.get_no_elems_x(); j++) {
			if (gw_data->flow_elem[i][j] == true) {					// if there is flow
				id = i + j * ny;
				

				//in x-direction
				if (gw_data->noflow_x[i][j] == false) {				//if it is not a noflow element
					//darcy x
					ds_dt_buff = gw_data->opt_cx[i][j] * (gw_data->opt_s[i][j+1] - gw_data->opt_s[i][j]);
					//set the result
					ds_dt_data[counter] = ds_dt_data[counter] + ds_dt_buff;
					ds_dt_data[counter + 1] = ds_dt_data[counter + 1] - ds_dt_buff;
					//gw_data->groundwater_elems[id].element_type->set_ds2dt_per_type(0, ds_dt_buff);
					
					ds_dt_check = ds_dt_data[counter];
					


					//gw_data->groundwater_elems[id+1].element_type->set_ds2dt_per_type(0, -ds_dt_buff);
					
				}
				//in x-direction
				if (gw_data->noflow_y[i][j] == false) {				//if it is not a noflow element
					//darcy y
					ds_dt_buff = gw_data->opt_cy[i][j] * (gw_data->opt_s[i+1][j] - gw_data->opt_s[i][j]);
					//set the result
					ds_dt_data[counter] = ds_dt_data[counter] + ds_dt_buff;
					ds_dt_data[gw_data->id_y[counter]] = ds_dt_data[gw_data->id_y[counter]] - ds_dt_buff;
					//gw_data->groundwater_elems[id].element_type->set_ds2dt_per_type(1, ds_dt_buff);
					
					
					ds_dt_check = ds_dt_check+ds_dt_data[counter];
					
					
					//gw_data->groundwater_elems[id+ny].element_type->set_ds2dt_per_type(1, -ds_dt_buff);
					//for development
					/*
					//if (ds_dt_buff > 0.0001 || gw_data->opt_s[i][j] > 70) {
						cout << "i: " << i << " j: " << j << " opt_s: " << gw_data->opt_s[i][j] << " opt_y: " << gw_data->opt_s[i+1][j] << "ds_dt_y: " << ds_dt_buff << "; y_id: " << gw_data->id_y[counter]<< endl;
						Sys_Common_Output::output_hyd->output_txt(&cout);
					//}
					*/
					
				}

				counter++;
			}
			
			//counter++;

			
			
			
		}
		//change here
		//counter++;
	}
	//clock_t c_end = clock();
	//my_time=my_time+1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;
	//my_c=my_c+1;
	//printf("Time %i  %f  \n",my_c,my_time);
//}
	counter1 = 0;
	gw_data->q_zero_outflow_error = 0.0;
	//gw_data->h_zero_outflow_error = 0.0;
	//check for zero outflow of boundary and coupling
	//boundary
	for (int i = 0; i < gw_data->number_hbound_cond; i++) {
		if (gw_data->hbound_cond_dsdt[i] < 0.0) {
			if (result_data[gw_data->hbound_cond_rid[i]] <= constant::dry_hyd_epsilon) {
				//no error all can be taken out
				if (ds_dt_data[gw_data->hbound_cond_rid[i]] < 0.0) {
					if (ds_dt_data[gw_data->hbound_cond_rid[i]] >= gw_data->hbound_cond_dsdt[i]) {
						gw_data->q_zero_outflow_error = gw_data->q_zero_outflow_error + ds_dt_data[gw_data->hbound_cond_rid[i]];
						ds_dt_data[gw_data->hbound_cond_rid[i]] = 0.0;
			
						
						
					}
					else {
						gw_data->q_zero_outflow_error = gw_data->q_zero_outflow_error + gw_data->hbound_cond_dsdt[i];
						ds_dt_data[gw_data->hbound_cond_rid[i]] = ds_dt_data[gw_data->hbound_cond_rid[i]] - gw_data->hbound_cond_dsdt[i];
						gw_data->groundwater_elems[gw_data->hbound_cond_id[i]].element_type->set_ds2dt_per_type(2, 0.0);
						
					}
				}
			}
		}
	}
	for (int i = 0; i < gw_data->number_qbound_cond; i++) {
		if (gw_data->qbound_cond_dsdt[i] < 0.0) {
			if (result_data[gw_data->qbound_cond_rid[i]] <= constant::dry_hyd_epsilon) {
				//no error all can be taken out
				if (ds_dt_data[gw_data->qbound_cond_rid[i]] < 0.0) {
					if (ds_dt_data[gw_data->qbound_cond_rid[i]] >= gw_data->qbound_cond_dsdt[i]) {
						gw_data->q_zero_outflow_error = gw_data->q_zero_outflow_error + ds_dt_data[gw_data->qbound_cond_rid[i]];
						ds_dt_data[gw_data->qbound_cond_rid[i]] = 0.0;


					}
					else {
						gw_data->q_zero_outflow_error = gw_data->q_zero_outflow_error + gw_data->qbound_cond_dsdt[i];
						ds_dt_data[gw_data->qbound_cond_rid[i]] = ds_dt_data[gw_data->qbound_cond_rid[i]] - gw_data->qbound_cond_dsdt[i];
						gw_data->groundwater_elems[gw_data->qbound_cond_id[i]].element_type->set_ds2dt_per_type(3, 0.0);
						
					}
				}
			}
		}
	}
	//coupling
	
	for (int i = 0; i < gw_data->number_coup_cond; i++) {
		if (gw_data->coup_cond_dsdt[i] < 0.0) {
			if (result_data[gw_data->coup_cond_rid[i]] <= constant::dry_hyd_epsilon) {
				//no error all can be taken out
				if (ds_dt_data[gw_data->coup_cond_rid[i]] < 0.0) {
					if (ds_dt_data[gw_data->coup_cond_rid[i]] >= gw_data->coup_cond_dsdt[i]) {
						gw_data->q_zero_outflow_error = gw_data->q_zero_outflow_error + ds_dt_data[gw_data->coup_cond_rid[i]];
						ds_dt_data[gw_data->coup_cond_rid[i]] = 0.0;
						
					}
					else {
						gw_data->q_zero_outflow_error = gw_data->q_zero_outflow_error + gw_data->coup_cond_dsdt[i];
						ds_dt_data[gw_data->coup_cond_rid[i]] = 0.0;
				
					}
				}
			}
		}
	}
	

	double delta_time = time - gw_data->old_timepoint_error;
	if (abs(delta_time) >= constant::sec_epsilon) {
		if (delta_time >= 0.0) {
			gw_data->old_timepoint_error = time;
			gw_data->error_zero_outflow_volume = gw_data->error_zero_outflow_volume + (gw_data->old_discharge_error*(1.0 / 3.0) + gw_data->q_zero_outflow_error*(2.0 / 3.0))*delta_time;
			gw_data->old_delta_t_error = delta_time;
			gw_data->old_discharge_2_error = gw_data->old_discharge_error;
			if (gw_data->old_discharge_error < 0.0) {
				gw_data->old_discharge_error = (gw_data->old_discharge_error*(1.0 / 3.0) + gw_data->q_zero_outflow_error*(2.0 / 3.0));
			}
			else {
				gw_data->old_discharge_error = gw_data->q_zero_outflow_error;
				gw_data->old_discharge_2_error = gw_data->old_discharge_error;
			}
		}
		else {
			gw_data->error_zero_outflow_volume = gw_data->error_zero_outflow_volume - gw_data->old_discharge_error*gw_data->old_delta_t_error;
			gw_data->old_timepoint_error = gw_data->old_timepoint_error - gw_data->old_delta_t_error;
			delta_time = time - gw_data->old_timepoint_error;
			gw_data->error_zero_outflow_volume = gw_data->error_zero_outflow_volume + (gw_data->old_discharge_2_error*(1.0 / 3.0) + gw_data->q_zero_outflow_error*(2.0 / 3.0))*delta_time;
			gw_data->old_delta_t_error = delta_time;
			gw_data->old_discharge_error = (gw_data->old_discharge_2_error*(1.0 / 3.0) + gw_data->q_zero_outflow_error*(2.0 / 3.0));
			gw_data->old_timepoint_error = time;
		}
	}

	counter = 0;
	for (int i = 0; i < gw_data->Param_GW.get_no_elems_y(); i++) {
		for (int j = 0; j < gw_data->Param_GW.get_no_elems_x(); j++) {
			if (gw_data->flow_elem[i][j] == true) {
				gw_data->opt_dsdt[i][j] = ds_dt_data[counter];
				counter++;
				/*
				ostringstream cout;
				cout << gw_data->opt_dsdt[i][j] << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				*/
			}
			//change here
			//counter++;
		}
	}

	return 0;
}
