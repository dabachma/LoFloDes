#include "source_code\Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_RV2FP_Dikebreak.h"


//constructor
Hyd_Coupling_RV2FP_Dikebreak::Hyd_Coupling_RV2FP_Dikebreak(void){

	
	this->index_rv_model=-1;
	this->ptr_river=NULL;

	this->left_flag=false;
	this->index_related_profile=-1;
	this->distance_start_related=0.0;

	this->point_list=NULL;
	this->start_coupling_point=NULL;
	this->upstream_coupling_point=NULL;
	this->downstream_coupling_point=NULL;


	this->list_possible_start_points=NULL;
	this->number_possible_start_points=0;



	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_RV2FP_Dikebreak), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_RV2FP_Dikebreak::~Hyd_Coupling_RV2FP_Dikebreak(void){
	this->delete_list_possible_start_points();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_RV2FP_Dikebreak), _sys_system_modules::HYD_SYS);
}
//_________________
//public
//Input of the parameters per file via a parser Hyd_Parse_Coupling_Dikebreak
void Hyd_Coupling_RV2FP_Dikebreak::input_parameter_per_file(const int index, const string file){
	this->index=index;
	//set prefix for output
	ostringstream prefix;
	prefix << "DB_RV2FP_" << this->index <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	Hyd_Parse_Coupling_RV2FP_Dikebreak parser;

	ostringstream cout;
	cout << "Read the dikebreak coupling parameters (rv2fp) from file " <<file << "..." <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//set the file name 
	parser.set_input_filename(file);
	try{
		//open file
		parser.open_input_file();
		//read in the parameters
		parser.parse_params(this->index);
	}
	catch(Error msg){
		parser.close_input_file();
		ostringstream info;
		info <<"Index of the dikebreak coupling (rv2fp) " << this->index << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//close the file
	parser.close_input_file();

	//set the gathered parameters
	this->set_dikebreak_coupling_params(parser.get_params());
	//check the given parameters
	try{
		this->check_dikebreak_parameter();
	}
	catch(Error msg){
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Input of the dike-/wallbreak coupling parameters from a database selection
void Hyd_Coupling_RV2FP_Dikebreak::input_parameter_per_database(const QSqlQueryModel *results, const int index, const bool output_flag){

	//set the index
	this->index=results->record(index).value((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_glob_id)).c_str()).toInt();

	if(output_flag==true){
		//set prefix for output
		ostringstream prefix;
		prefix << "DB_RV2FP_" << this->index <<"> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		ostringstream cout;
		cout << "Read the dike-/wallbreak coupling parameters from database of a river to floodplain coupling..." <<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	

	}

	//read out the results
	this->index_rv_model=results->record(index).value((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rv_id)).c_str()).toInt();
	this->index_related_profile=results->record(index).value((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_prof_id)).c_str()).toInt();
	this->distance_start_related=results->record(index).value((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rel_dist)).c_str()).toDouble();
	this->left_flag=results->record(index).value((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_left_bank)).c_str()).toBool();
	this->start_waterlevel_local=results->record(index).value((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_start_h)).c_str()).toDouble();
	this->max_breach_width=results->record(index).value((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_max_w)).c_str()).toDouble();

	if(output_flag==true){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Set the start parameters of a break via a given data structure
void Hyd_Coupling_RV2FP_Dikebreak::input_parameter(const int index_breach, const _hyd_breach_start_parameters params, const bool left_flag){
	this->index=index_breach;
	this->index_rv_model=params.id_rv_model;
	this->index_related_profile=params.id_related_station;
	this->distance_start_related=params.distance2related;
	this->left_flag=left_flag;
	this->start_waterlevel_local=params.start_height;
	this->max_breach_width=params.max_breach_width;

}
//Transfer the dikebreak/wallbreak data of a file into a database table
void Hyd_Coupling_RV2FP_Dikebreak::transfer_break_data2database_table(QSqlDatabase *ptr_database){
    //set prefix for output
        ostringstream prefix;
        prefix << "DB_RV2FP_" << this->index <<"> ";
        Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
        ostringstream cout;
        cout <<"Transfer dike-/wallbreak data to database of a river to floodplain coupling ..."<<endl;
        Sys_Common_Output::output_hyd->output_txt(&cout);
        //mysql query with the table_model
        QSqlQuery query_buff(*ptr_database);
        //the table is set (the name and the column names) and allocated
        try{
            _Hyd_Coupling_Dikebreak::set_table(ptr_database);
        }
        catch(Error msg){
            throw msg;
        }

        //give the table name to the query model
        //model.setTable((_Hyd_Coupling_Dikebreak::break_data_table->get_table_name()).c_str());

        //evaluate the global identifier
        int id_glob=_Hyd_Coupling_Dikebreak::break_data_table->maximum_int_of_column(Hyd_Coupling_RV2FP_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_glob_id),ptr_database)+1;;


        ostringstream query_header;
        query_header << "INSERT INTO  " << _Hyd_Coupling_Dikebreak::break_data_table->get_table_name();
        query_header <<" ( ";
        query_header << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_glob_id) <<" , ";
        query_header << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::areastate_id) <<" , ";
        query_header << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::measure_id) <<" , ";
        query_header << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::applied_flag) <<" , ";
        query_header << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rv_id) <<" , ";
        query_header << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_prof_id) <<" , ";
        query_header << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rel_dist) <<" , ";
        query_header << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_left_bank) <<" , ";
        query_header << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_start_h) <<" , ";
        query_header << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_max_w) <<" , ";
        query_header << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_point) <<" ) ";

        query_header << " VALUES ";

        query_header << " ( ";
        query_header << id_glob <<" , ";
        query_header << this->system_id.area_state <<" , ";
        query_header << this->system_id.measure_nr <<" , ";
        query_header << functions::convert_boolean2string(true) <<" , ";
        query_header << this->index_rv_model <<" , ";
        query_header << this->index_related_profile <<" , ";
        query_header << this->distance_start_related <<" , ";
        query_header << functions::convert_boolean2string(this->left_flag) <<" , ";
        query_header << this->start_waterlevel_local <<" , ";
        query_header << this->max_breach_width <<" , ";
        query_header << Geo_Point::get_point2sql_string(this->x_coordinate, this->y_coordinate).c_str()<< "  ";
        query_header << " )  ";


        Data_Base::database_request(&query_buff, query_header.str(), ptr_database);






        //set data
        //model.setData(model.index(0,model.record().indexOf((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_glob_id)).c_str())),id_glob);
        //model.setData(model.index(0,model.record().indexOf((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::areastate_id)).c_str())),this->system_id.area_state);
        //model.setData(model.index(0,model.record().indexOf((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::measure_id)).c_str())),this->system_id.measure_nr);
        //model.setData(model.index(0,model.record().indexOf((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::applied_flag)).c_str())),true);
        //model.setData(model.index(0,model.record().indexOf((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rv_id)).c_str())),this->index_rv_model);
        //model.setData(model.index(0,model.record().indexOf((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_prof_id)).c_str())),this->index_related_profile);
        //model.setData(model.index(0,model.record().indexOf((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rel_dist)).c_str())),this->distance_start_related);
        //model.setData(model.index(0,model.record().indexOf((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_left_bank)).c_str())),this->left_flag);
        //model.setData(model.index(0,model.record().indexOf((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_start_h)).c_str())),this->start_waterlevel_local);
        //model.setData(model.index(0,model.record().indexOf((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_max_w)).c_str())),this->max_breach_width);
        //QString buff_txt=Geo_Point::get_point2sql_string(this->x_coordinate, this->y_coordinate).c_str();
        //model.setData(model.index(0,model.record().indexOf((_Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_point)).c_str())),buff_txt);

        ////submit it to the datbase
        //Data_Base::database_submit(&model);


        if(query_buff.lastError().isValid()){
            Warning msg=this->set_warning(2);
            ostringstream info;
            info << "Table Name                : " << _Hyd_Coupling_Dikebreak::break_data_table->get_table_name() << endl;
            info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
            info << "Global Id                 : " << id_glob <<endl;
            msg.make_second_info(info.str());
            msg.output_msg(2);
        }
        //rewind the prefix
        Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Set an additional coupling point, when the break is user-defined
void Hyd_Coupling_RV2FP_Dikebreak::set_additional_coupling_point(Hyd_Coupling_RV2FP_Merged *couplings){
	if(couplings==NULL){
		return;
	}

	double distance=0.0;
	Hyd_Coupling_Point_RV2FP new_point;
	Hyd_Coupling_Point_RV2FP *next_point;
	Hyd_Coupling_Point_RV2FP_List *relevant;

	//search for the river
	if(this->index_rv_model==couplings->get_river_index()){
		//search for the side
		if(this->left_flag==true){
			relevant=&(couplings->list_left);
		}
		else{
			relevant=&(couplings->list_right);
		}
		//add
		if(relevant->find_distance2related_profile(this->index_related_profile, &distance)==true){
			next_point=relevant->find_coupling_point(distance+this->distance_start_related, &new_point);
			if(next_point!=NULL){
				new_point.set_couplingpoint_members(next_point);
				new_point.set_point_name(hyd_label::coupling_point_RV2FP);
				relevant->set_new_couplingpoint(&new_point);
			}
			else{
				//Error
				Error msg=this->set_error(5);
				throw msg;

			}
		}
	}
}
//The couplings are initialized with the already performed RV2FP couplings 
void Hyd_Coupling_RV2FP_Dikebreak::init_coupling(Hyd_Coupling_RV2FP_Merged *couplings, const int number){

	//flag if the connection is found
	bool found_flag=false;
	//found index of the couplings
	int found_index=-1;

	//set prefix for output
	ostringstream prefix;
	prefix << "DB_RV2FP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Coupling (index " << this->index <<") between river model " << this->index_rv_model << " via a dikebreak ";
	cout << "at distance of "<< this->distance_start_related<< label::m <<" from profile index "<< this->index_related_profile;
	cout << "..."<< endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//search for the starting coupling point
	for(int i=0; i<number; i++){
		if(couplings[i].get_river_index()==this->index_rv_model){
			found_index=i;
			//left bank
			if(this->left_flag==true){
				//set the list
				this->point_list=&couplings[i].list_left;
			}
			else{
				this->point_list=&couplings[i].list_right;
			}
			if(this->point_list==NULL){
				this->start_coupling_point=NULL;
			}
			else{
				//related profile is inbetween
				if(this->index_related_profile>0 && this->point_list->find_distance2related_profile(this->index_related_profile, &this->distance_along_polysegment)==true){
					this->set_total_distance_along_polysegment(this->distance_along_polysegment+this->distance_start_related-constant::meter_epsilon);
					this->start_coupling_point=this->point_list->find_coupling_point(this->get_total_distance_along_polysegment(), this);

				}
				//related profile is inflow => the start of the line
				else if(this->index_related_profile==0){
					this->set_total_distance_along_polysegment(this->distance_start_related);
					this->start_coupling_point=this->point_list->find_coupling_point(this->get_total_distance_along_polysegment(), this);
				}
				else{
					this->start_coupling_point=NULL;
				}

			}
			if(this->start_coupling_point!=NULL){
				found_flag=true;
				this->index_fp_model_start=this->start_coupling_point->get_floodplain_index();
				break;
			}
		}
	}

	//investigate further the starting coupling point
	if(found_flag==true){
		found_flag=this->set_start_coupling_point();
	}
	if(found_flag==false){
		Warning msg=this->set_warning(0);
		msg.output_msg(2);
		//reaction
		this->coupling_flag=false;
		this->point_list=NULL;
	}
	else{
		this->coupling_flag=true;
		this->ptr_river=couplings[found_index].get_river_model();
		//open the file for output
		//try{
		//	this->init_output2file(couplings[found_index].get_river_model());
		//}
		//catch(Error msg){
		//	Sys_Common_Output::output_hyd->rewind_userprefix();
		//	throw msg;
		//}

	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//The couplings are initialized with the already performed RV2FP couplings; here a list of river profiles is also given for the creation of a list of possible starting breach points (scenario based risk approach)
void Hyd_Coupling_RV2FP_Dikebreak::init_coupling(Hyd_Coupling_RV2FP_Merged *couplings, const int number, _Hyd_River_Profile **involved_profiles, const int number_involved_profiles, const int section_id_fpl, _hyd_output_flags *output_flags){
	this->index_section_fpl=section_id_fpl;
	//flag if the connection is found
	bool found_flag=false;
	//found index of the couplings
	int found_index=-1;

	//search for the starting coupling point
	for(int i=0; i<number; i++){
		if(couplings[i].get_river_index()==this->index_rv_model){
			found_index=i;
			//left bank
			if(this->left_flag==true){
				//set the list
				this->point_list=&couplings[i].list_left;
			}
			else{
				this->point_list=&couplings[i].list_right;
			}
			if(this->point_list==NULL){
				this->start_coupling_point=NULL;
			}
			else{
				//related profile is inbetween
				if(this->index_related_profile>0 && this->point_list->find_distance2related_profile(this->index_related_profile, &this->distance_along_polysegment)==true){
					this->set_total_distance_along_polysegment(this->distance_along_polysegment+this->distance_start_related);
					this->start_coupling_point=this->point_list->find_coupling_point(this->get_total_distance_along_polysegment(), this);

				}
				//related profile is inflow => the start of the line
				else if(this->index_related_profile==0){
					this->set_total_distance_along_polysegment(this->distance_start_related);
					this->start_coupling_point=this->point_list->find_coupling_point(this->get_total_distance_along_polysegment(), this);
				}
				else{
					this->start_coupling_point=NULL;
				}

			}
			if(this->start_coupling_point!=NULL){
				found_flag=true;
				this->index_fp_model_start=this->start_coupling_point->get_floodplain_index();
				break;
			}
		}
	}

	//investigate further the starting coupling point
	if(found_flag==true){
		found_flag=this->set_start_coupling_point();
		this->create_starting_coupling_point_list(involved_profiles, number_involved_profiles);
	}
	if(found_flag==false){
		Warning msg=this->set_warning(0);
		msg.output_msg(2);
		//reaction
		this->coupling_flag=false;
		this->point_list=NULL;
	}
	else{
		this->coupling_flag=true;
		this->ptr_river=couplings[found_index].get_river_model();
		//open the file for output
		try{
			if (output_flags->tecplot_1d_required == true) {
				this->init_output2file_tecplot();
			}
			if (output_flags->paraview_1d_required == true) {
				this->init_output2file_csv();
			}
		}
		catch(Error msg){
			throw msg;
		}

	}
}
//The couplings are initialized with the already performed RV2FP couplings; the coupling point index, where the break should begin is directly given (catchment area risk approach)
void Hyd_Coupling_RV2FP_Dikebreak::init_coupling(Hyd_Coupling_RV2FP_Merged *couplings, const int number, const int section_id_fpl, const int point_id, _hyd_output_flags *output_flags){
	this->index_section_fpl=section_id_fpl;
	int found_index=-1;
	//search for the coupling point list
	for(int i=0; i<number; i++){
		if(couplings[i].get_river_index()==this->index_rv_model){
			found_index=i;
			//left bank
			if(this->left_flag==true){
				//set the list
				this->point_list=&couplings[i].list_left;
			}
			else{
				this->point_list=&couplings[i].list_right;
			}
			if(this->point_list==NULL){
				this->start_coupling_point=NULL;
			}
			else{
				this->set_total_distance_along_polysegment(this->distance_along_polysegment+this->distance_start_related);
				this->start_coupling_point=this->point_list->get_ptr_couplingpoint(point_id);
			}
			if(this->start_coupling_point!=NULL){
				this->coupling_flag=true;
				this->index_fp_model_start=this->start_coupling_point->get_floodplain_index();
				break;
			}
		}
	}

	if(this->coupling_flag==true){
		this->set_start_coupling_point();
		if(this->start_coupling_point!=NULL){
			this->set_total_distance_along_polysegment(this->start_coupling_point->get_total_distance_along_polysegment());
			this->set_point_coordinate(this->start_coupling_point->get_xcoordinate(), this->start_coupling_point->get_ycoordinate());
			this->set_distance_up(this->start_coupling_point->get_distance_up());
			this->set_distance_down(this->start_coupling_point->get_distance_down());
			this->start_waterlevel_global=this->start_waterlevel_local+this->start_coupling_point->get_mid_basepoint_height_profile();
		}
		//open the file for output
		try{
			this->ptr_river=couplings[found_index].get_river_model();
			if (output_flags->tecplot_1d_required == true) {
				this->init_output2file_tecplot();
			}
			if (output_flags->paraview_1d_required == true) {
				this->init_output2file_csv();
			}
		}
		catch(Error msg){
			throw msg;
		}
	}
}
//Synchronise the models
void Hyd_Coupling_RV2FP_Dikebreak::synchronise_models(const double timepoint, const double delta_t, const bool time_check){

	_Hyd_Coupling_Point::syncronisation_coupled_models();
	this->delta_t=delta_t;
	if(time_check==true){
		return;
	};

	//check for begin
	if(this->coupling_flag==true){
		//breach has not yet started
		if(this->started_flag==false && this->init_phase_finished==false && this->delta_t>0.0){
			this->observe_starting_coupling_points();
			this->start_coupling_point->calculate_mid_waterlevel();
			//start the breach
			if(this->start_waterlevel_global<=this->start_coupling_point->get_mid_waterlevel()){
				
				this->started_flag=true;


				this->start_coupling_point->set_index_break_class(this->index);
				this->starting_time_point=timepoint;
				//stating breach width is set to 1.0
				this->start_coupling_point->set_begin_break_width(this->init_breach_width);
				this->total_breach_width=this->init_breach_width;
				this->breach_counter++;
				this->breach_time=this->breach_time+this->delta_t;
				//1 m/sec is set as the reduction of the height until the basepoint height
				if(this->start_coupling_point->set_overflow_height_reduction(this->numerical_breach_v*this->delta_t)==true){
					this->init_phase_finished=true;	
				};

			}
		}
		//breach is in the initial phase => breach sill is lowering
		else if(this->started_flag==true && this->init_phase_finished==false){
			//calculate maximum values (timestep before)
			this->calculate_mean_q_v(timepoint-this->delta_t);
			//counters
			this->breach_counter++;
			this->breach_time=this->breach_time+this->delta_t;
			//1 m/sec is set as the reduction of the height until the basepoint height
			if(this->start_coupling_point->set_overflow_height_reduction(this->numerical_breach_v*this->delta_t)==true){
				this->init_phase_finished=true;	
			}
		}
		//breach width gets greater
		else if(this->started_flag==true && this->init_phase_finished==true){
			//calculate mean and maximum values (timestep before)
			this->calculate_mean_q_v(timepoint-this->delta_t);
			this->calculate_total_mean_delta_h();
			//counters
			this->breach_counter++;
			this->breach_time=this->breach_time+this->delta_t;
			//calculate delta_h breach
			this->downstream_coupling_point->calculate_delta_h_break();
			this->upstream_coupling_point->calculate_delta_h_break();
			//calculate the opening 
			if(this->total_breach_width<this->max_breach_width){
				this->calculate_downstream_opening();
				this->calculate_upstream_opening();
			}
			else{
				this->upstream_open_flag=false;
				this->downstream_open_flag=false;
			}

		}

		this->break_info_list.counter_breach.append(this->breach_counter);
		this->break_info_list.delta_h2start.append(this->start_coupling_point->get_mid_waterlevel()-this->start_waterlevel_global);
		this->break_info_list.downstream_breach.append(this->breach_width_downstream);
		this->break_info_list.downstream_delta_h.append(*this->deltah_downstream);
		this->break_info_list.upstream_breach.append(this->breach_width_upstream);
		this->break_info_list.upstream_delta_h.append(*this->deltah_upstream);
		this->break_info_list.downstream_wall_stress.append(this->counter_stress_downstream);
		this->break_info_list.upstream_wall_stress.append(this->counter_stress_upstream);
		this->break_info_list.mean_q.append(this->mean_q);
		this->break_info_list.mean_v.append(this->mean_v);
		this->break_info_list.time.append(timepoint);
		this->break_info_list.time_breach.append(this->breach_time);
		this->break_info_list.total_breach.append(this->total_breach_width);
	}
}
//Output the members
void Hyd_Coupling_RV2FP_Dikebreak::output_members(void){
	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "DB_RV2FP_"<<this->index<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << P(2)<< FORMAT_FIXED_REAL;
	cout <<"GENERAL" <<endl;
	cout << " Index		                                 : " << W(7) << this->index << endl;
	cout << " User defined                               : " << W(7) << functions::convert_boolean2string(this->user_defined) << endl;
	cout <<"RIVER" <<endl;
	cout << " River model number                         : " << W(7) << this->index_rv_model << endl;
	cout << " Left bank (true)/right bank (false)        : " << W(7) << functions::convert_boolean2string(this->left_flag) << endl;
	cout << " Related profile index                      : " << W(7) << this->index_related_profile << endl;
	cout << " Distance to related profile                : " << W(7) << this->distance_start_related << label::m << endl;
	cout <<"DIKEBREAK" << endl;
	cout <<" Start waterlevel                            : " << W(7) << this->start_waterlevel_local<< label::m <<endl;
	cout <<" Maximal breach width                        : " << W(7) << this->max_breach_width<< label::m <<endl;

	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the header for the coupled model indizes (static)
void Hyd_Coupling_RV2FP_Dikebreak::output_header_coupled_indices(ostringstream *cout){
	*cout << "River to floodplain as dikebreak starting points..." << endl ;
	*cout << "No." << W(12) << "Id_RV" << W(14)<< "Id_RVprofile" << W(14);
	*cout<< "Id_FP" << W(15) << "Id_FPelem";
	*cout << W(15)<<"Dist_rv_begin"<< label::m << endl;
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the indizes of the coupled model
void Hyd_Coupling_RV2FP_Dikebreak::output_index_coupled_models(ostringstream *cout, const int number){
	*cout << number << W(10);
	if(this->coupling_flag==true){
		*cout << this->index_rv_model << W(14);
		*cout << this->start_coupling_point->get_upstream_rvprofile()->get_profile_number() << W(14);
		*cout << this->index_fp_model_start <<W(15);
		*cout << this->start_coupling_point->get_floodplain_element()->get_elem_number()<< W(15);
		*cout << this->distance_along_polysegment << endl;
	}
	else{
		*cout <<"    No dikebreak coupling is applied between river and floodplain model; see the warnings!" << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output result members to tecplot file
void Hyd_Coupling_RV2FP_Dikebreak::output_results2file_tecplot(double const ){
	if(this->file_name!=label::not_set){
		for(int i=0; i< this->break_info_list.time.count(); i++){
						this->output_file << W(9)<<P(0) << FORMAT_FIXED_REAL << this->break_info_list.time.at(i) <<W(16); 
			this->output_file << this->break_info_list.counter_breach.at(i) << W(20) ;
			this->output_file << this->break_info_list.time_breach.at(i) << W(20);
			this->output_file <<P(3) << FORMAT_FIXED_REAL << this->break_info_list.total_breach.at(i) << W(20) ;
			if(this->started_flag==true){
				this->output_file << this->break_info_list.upstream_breach.at(i)+0.5*this->init_breach_width << W(20)<< this->break_info_list.downstream_breach.at(i) +0.5*this->init_breach_width<< W(29) ;
			}
			else{
				this->output_file << 0.0 << W(20)<< 0.0<< W(29) ;
			}
			if(this->started_flag==true){
				this->output_file << this->break_info_list.upstream_delta_h.at(i)<< W(29) ;
				this->output_file << this->break_params_upstream->resistance<< W(29) ;
			}
			else{
				this->output_file << 0.0<< W(29) ;
				this->output_file << 0.0<< W(29) ;
			}
			
			if(this->started_flag==true){
				this->output_file << this->break_info_list.downstream_delta_h.at(i)<< W(29) ;
				this->output_file << this->break_params_downstream->resistance<< W(29) ;
			}
			else{
				this->output_file << 0.0<< W(29) ;
				this->output_file << 0.0<< W(29) ;
			}
	 
			this->output_file << this->break_info_list.mean_q.at(i) << W(29) ;
			this->output_file << this->break_info_list.mean_v.at(i) << W(29) ;

			if(this->wall_breach_flag==true){
				this->output_file << this->break_info_list.upstream_wall_stress.at(i)<< W(29) << this->break_info_list.downstream_wall_stress.at(i)<< W(29);
			}
			this->output_file << this->break_info_list.delta_h2start.at(i) << endl;			

			/*for(int i=0; i< this->number_possible_start_points; i++){

				this->output_file << i <<" "<< this->list_possible_start_points[i]->get_mid_waterlevel()<< " " <<this->list_possible_start_points[i]->get_mid_basepoint_height()<< endl;
			}
			this->output_file <<"max observed " << this->max_observed_waterlevel << endl;*/
		}
	}


}
///Output result members to csv file
void Hyd_Coupling_RV2FP_Dikebreak::output_results2file_csv(double const global_time) {
	if (this->file_name_csv != label::not_set) {
		for (int i = 0; i < this->break_info_list.time.count(); i++) {
			this->output_file_csv << W(9) << P(0) << FORMAT_FIXED_REAL << this->break_info_list.time.at(i) << W(16) << ",";
			this->output_file_csv << this->break_info_list.counter_breach.at(i) << W(20) << ",";
			this->output_file_csv << this->break_info_list.time_breach.at(i) << W(20) << ",";
			this->output_file_csv << P(3) << FORMAT_FIXED_REAL << this->break_info_list.total_breach.at(i) << W(20) << ",";
			if (this->started_flag == true) {
				this->output_file_csv << this->break_info_list.upstream_breach.at(i) + 0.5*this->init_breach_width << W(20) << "," << this->break_info_list.downstream_breach.at(i) + 0.5*this->init_breach_width << W(29) << ",";
			}
			else {
				this->output_file_csv << 0.0 << W(20) << "," << 0.0 << W(29) << ",";
			}
			if (this->started_flag == true) {
				this->output_file_csv << this->break_info_list.upstream_delta_h.at(i) << W(29) << ",";
				this->output_file_csv << this->break_params_upstream->resistance << W(29) << ",";
			}
			else {
				this->output_file_csv << 0.0 << W(29) << ",";
				this->output_file_csv << 0.0 << W(29) << ",";
			}

			if (this->started_flag == true) {
				this->output_file_csv << this->break_info_list.downstream_delta_h.at(i) << W(29) << ",";
				this->output_file_csv << this->break_params_downstream->resistance << W(29) << ",";
			}
			else {
				this->output_file_csv << 0.0 << W(29) << ",";
				this->output_file_csv << 0.0 << W(29) << ",";
			}

			this->output_file_csv << this->break_info_list.mean_q.at(i) << W(29) << ",";
			this->output_file_csv << this->break_info_list.mean_v.at(i) << W(29) << ",";

			if (this->wall_breach_flag == true) {
				this->output_file_csv << this->break_info_list.upstream_wall_stress.at(i) << W(29) << "," << this->break_info_list.downstream_wall_stress.at(i) << W(29) << ",";
			}
			this->output_file_csv << this->break_info_list.delta_h2start.at(i) << endl;

			/*for(int i=0; i< this->number_possible_start_points; i++){

				this->output_file << i <<" "<< this->list_possible_start_points[i]->get_mid_waterlevel()<< " " <<this->list_possible_start_points[i]->get_mid_basepoint_height()<< endl;
			}
			this->output_file <<"max observed " << this->max_observed_waterlevel << endl;*/
		}
	}
}
//Output final result members
void Hyd_Coupling_RV2FP_Dikebreak::output_final_results(void){
	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	if(this->user_defined==true){
		prefix << "DB_RV2FP_"<<this->index<<"> ";
	}
	else{
		prefix << "DB_RV2FP_FPL_"<<this->index_section_fpl<<"> ";
	}
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout <<"BREACH" << endl;
	if(this->coupling_flag==true){
		cout << " Is started                                 : " << W(7)<< functions::convert_boolean2string(this->started_flag) << endl;
		cout << " Start time                                 : " << W(7) << P(0) << FORMAT_FIXED_REAL << this->starting_time_point << label::sec<< endl;
		cout << " Left bank (true)/ right bank (false)       : " << W(7)<< functions::convert_boolean2string(this->left_flag) << endl;
		cout << " Width total                                : " << W(7) << P(1) << FORMAT_FIXED_REAL << this->total_breach_width << label::m<< endl;
		cout << " Width upstream                             : " << W(7) << P(1) << FORMAT_FIXED_REAL << this->breach_width_upstream+0.5*this->init_breach_width << label::m<< endl;
		cout << " Width downstream                           : " << W(7) << P(1) << FORMAT_FIXED_REAL << this->breach_width_downstream+0.5*this->init_breach_width << label::m<< endl;
		if(this->total_breach_width>=this->max_breach_width){
			cout << " Maximum breach width reached               : " << W(7) << "true" << endl;
		}
		cout << " Maximal obeserved waterlevel               : " << W(7) << P(1) << FORMAT_FIXED_REAL << this->max_observed_waterlevel << label::m<<endl;
		cout << " Dist. breachend upstream to river-begin    : " << W(7) << P(1) << FORMAT_FIXED_REAL << this->get_total_distance_along_polysegment()-(this->breach_width_upstream+0.5*this->init_breach_width) << label::m<< endl;
		cout << " Dist. breachend downstream to river-begin  : " << W(7) << P(1) << FORMAT_FIXED_REAL << this->get_total_distance_along_polysegment()+(this->breach_width_downstream+0.5*this->init_breach_width) << label::m<< endl;
		cout << " Breach upstream continues                  : " << W(7)<< functions::convert_boolean2string(this->upstream_open_flag) << endl;
		cout << " Breach downstream continues                : " << W(7)<< functions::convert_boolean2string(this->downstream_open_flag) << endl;
		cout <<"FLOW THROUGH BREACH" <<endl;
		cout << " Maximum mean velocity (abs)                : " << W(7) << P(1) << FORMAT_FIXED_REAL << this->max_coupling_v.maximum << label::m_per_sec<< endl;
		cout << " Time of max. mean velocity                 : " << W(7) << P(0) << FORMAT_FIXED_REAL<< this->max_coupling_v.time_point<< label::sec << endl;
		cout << " Total mean volume                          : " << W(7) << P(1) << FORMAT_FIXED_REAL<< abs(this->coupling_volume.volume_total) << label::cubicmeter<< endl;
		cout << " Mean volume to river " << this->index_rv_model<<"                     : " << W(7) << P(1) << FORMAT_FIXED_REAL<< this->coupling_volume.volume_in  << label::cubicmeter << endl;
		cout << " Mean volume to floodplain                  : " << W(7) << P(1) << FORMAT_FIXED_REAL<< this->coupling_volume.volume_out  << label::cubicmeter << endl;
	}
	else{
		cout << " No breach development was possible; check the warnings" <<endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output final results to database
void Hyd_Coupling_RV2FP_Dikebreak::output_final_results(QSqlDatabase *ptr_database, const string id_break, const int bound_sc){

	if(this->started_flag==false){
		return;
	}

	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_Coupling_Dikebreak::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	QSqlQuery model(*ptr_database);

	int glob_id=0;
	glob_id=_Hyd_Coupling_Dikebreak::break_result_table->maximum_int_of_column(_Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::glob_id),ptr_database)+1;

	ostringstream query_string;
	query_string << "INSERT INTO  " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name();
	query_string <<" ( ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::glob_id) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::areastate_id) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::measure_id) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::applied_flag) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(risk_label::sz_break_id) <<" , ";

	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_glob_id) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_user_defined) <<" , ";

	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_start_time) <<" , ";

	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_width_total) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_width_upstream) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_width_downstream) <<" , ";

	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_upstream_stop) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_downstream_stop) <<" , ";
	
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_max_v) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_max_v_time) <<" , ";

	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_total_volume) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_volume_fp) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_volume_rv) <<" , ";

	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_x_start) <<" , ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_y_start) <<" , ";

	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_polyline) <<" ) ";

	query_string << " VALUES ( ";
	query_string << glob_id << " , " ;
	query_string << this->system_id.area_state<< " , " ;
	query_string << this->system_id.measure_nr<< " , " ;
	query_string <<""<< functions::convert_boolean2string(true) << " , " ;
	query_string << bound_sc<< " , " ;
	query_string <<"'"<< id_break<< "' , " ;

	if(this->user_defined==true){
		query_string << this->index << " , " ;
	}
	else{
		query_string << this->index_section_fpl << " , " ;
	}
	query_string << functions::convert_boolean2string(this->user_defined) << " , " ;

	query_string << this->starting_time_point << " , " ;
	
	query_string << this->total_breach_width << " , " ;
	query_string << this->breach_width_upstream+0.5*this->init_breach_width << " , " ;
	query_string << this->breach_width_downstream+0.5*this->init_breach_width << " , " ;

	query_string << functions::convert_boolean2string(!this->upstream_open_flag) << " , " ;
	query_string << functions::convert_boolean2string(!this->downstream_open_flag) << " , " ;

	query_string << this->max_coupling_v.maximum << " , " ;
	query_string << this->max_coupling_v.time_point << " , " ;

	query_string << abs(this->coupling_volume.volume_total) << " , " ;
	query_string << this->coupling_volume.volume_out << " , " ;
	query_string << this->coupling_volume.volume_in << " , " ;

	query_string << FORMAT_FIXED_REAL << P(8);
	query_string << this->get_xcoordinate() << " , " ;
	query_string << this->get_ycoordinate() << " , " ;

	//set the polgon points
	string polyline_string;
	polyline_string=this->get_breach_polyline();
	query_string << polyline_string <<" ) ";


	Data_Base::database_request(&model, query_string.str(), ptr_database);
	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Index break               : " << this->index << endl;
		info << "User defined break        : " << functions::convert_boolean2string(this->user_defined) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}

}
//Reset the hydrological balance values and the maximum calculated values
void Hyd_Coupling_RV2FP_Dikebreak::reset_hydro_balance_max_values(void){
	
	_Hyd_Coupling_Dikebreak::reset_hydro_balance_max_values();
	//downstream value
	this->downstream_coupling_point=this->start_coupling_point;
	this->deltah_downstream=this->downstream_coupling_point->get_delta_h_break();
	this->downstream_open_flag=true;
	this->distance_next_point_downstream=0.0;
	//upstream value
	this->upstream_coupling_point=this->start_coupling_point;
	this->deltah_upstream=this->upstream_coupling_point->get_delta_h_break();
	this->upstream_open_flag=true;
	this->distance_next_point_upstream=this->distance_down;

	if(this->left_flag==true){
		this->break_params_downstream=this->downstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_left();
		this->break_params_upstream=this->upstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_left();
		this->wall_breach_flag=this->break_params_upstream->abrupt_fails_flag;
	}
	else{
		this->break_params_downstream=this->downstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_right();
		this->break_params_upstream=this->upstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_right();
		this->wall_breach_flag=this->break_params_upstream->abrupt_fails_flag;
	}
}
//Clone the dikebreak coupling
void Hyd_Coupling_RV2FP_Dikebreak::clone_couplings(Hyd_Coupling_RV2FP_Dikebreak *coupling, Hyd_Coupling_RV2FP_Merged *merged_couplings, const int number){
	_Hyd_Coupling_Point::operator =(*coupling);
	this->system_id=coupling->get_sys_system_id();

	this->index=coupling->index;
	this->index_section_fpl=coupling->index_section_fpl;
	this->index_rv_model=coupling->index_rv_model;
	this->index_fp_model_start=coupling->index_fp_model_start;
	this->left_flag=coupling->left_flag;
	this->index_related_profile=coupling->index_related_profile;
	this->distance_start_related=coupling->distance_start_related;
	this->start_waterlevel_local=coupling->start_waterlevel_local;
	this->start_waterlevel_global=coupling->start_waterlevel_global;
	this->max_breach_width=coupling->max_breach_width;

	this->wall_breach_flag=coupling->wall_breach_flag;
	this->coupling_flag=coupling->coupling_flag;
	this->user_defined=coupling->user_defined;

	
	for(int i=0; i< number; i++){
		if(this->index_rv_model==merged_couplings[i].get_river_index()){
			if(this->left_flag==true){
				this->point_list=&merged_couplings[i].list_left;
				break;
			}
			else{
				this->point_list=&merged_couplings[i].list_right;
				break;
			}
		}
	}

	//if(this->point_list!=NULL){
	//	this->start_coupling_point=this->point_list->get_ptr_couplingpoint(*coupling->start_coupling_point);
	//}

	this->init_coupling(merged_couplings, number);

	//downstream value
	this->downstream_coupling_point=this->start_coupling_point;
	this->deltah_downstream=this->downstream_coupling_point->get_delta_h_break();
	this->downstream_open_flag=true;
	this->distance_next_point_downstream=0.0;
	//upstream value
	this->upstream_coupling_point=this->start_coupling_point;
	this->deltah_upstream=this->upstream_coupling_point->get_delta_h_break();
	this->upstream_open_flag=true;
	this->distance_next_point_upstream=this->distance_down;

	if(this->left_flag==true){
		this->break_params_downstream=this->downstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_left();
		this->break_params_upstream=this->upstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_left();
		this->wall_breach_flag=this->break_params_upstream->abrupt_fails_flag;
	}
	else{
		this->break_params_downstream=this->downstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_right();
		this->break_params_upstream=this->upstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_right();
		this->wall_breach_flag=this->break_params_upstream->abrupt_fails_flag;
	}

}
//Initiate the output to a file
void Hyd_Coupling_RV2FP_Dikebreak::init_output2file_tecplot(void){
	if(this->coupling_flag==false){
		return;
	}
	string buffer;
	
	//get the file name from the river model
	buffer=this->ptr_river->Param_RV.get_crude_filename_result_1d();
	this->break_info_list.counter_breach.clear();
	this->break_info_list.delta_h2start.clear();
	this->break_info_list.downstream_breach.clear();
	this->break_info_list.downstream_delta_h.clear();
	this->break_info_list.upstream_breach.clear();
	this->break_info_list.upstream_delta_h.clear();
	this->break_info_list.downstream_wall_stress.clear();
	this->break_info_list.upstream_wall_stress.clear();
	this->break_info_list.mean_q.clear();
	this->break_info_list.mean_v.clear();
	this->break_info_list.time.clear();
	this->break_info_list.time_breach.clear();
	this->break_info_list.total_breach.clear();
	buffer += "/";
	buffer += hyd_label::tecplot;
	buffer += "/";
	
	if(buffer!=label::not_set){
		stringstream suffix;
		if(this->user_defined==true){
			suffix << "BREACH_RV_"<<this->index_rv_model<<"_ID_"<<this->index<< hyd_label::dat;
		}
		else{
			suffix << "BREACH_RV_"<<this->index_rv_model<<"_FPL_"<<this->index_section_fpl<<hyd_label::dat;
		}
		buffer.append(suffix.str());
		this->file_name=buffer;
		//open the file
		try{
			this->open_output_file();
		}
		catch(Error msg){
			throw msg;
		}
	}
}
///Initiate the output to a csv file
void Hyd_Coupling_RV2FP_Dikebreak::init_output2file_csv(void) {
	if (this->coupling_flag == false) {
		return;
	}
	string buffer;

	//get the file name from the river model
	buffer = this->ptr_river->Param_RV.get_crude_filename_result_1d();
	this->break_info_list.counter_breach.clear();
	this->break_info_list.delta_h2start.clear();
	this->break_info_list.downstream_breach.clear();
	this->break_info_list.downstream_delta_h.clear();
	this->break_info_list.upstream_breach.clear();
	this->break_info_list.upstream_delta_h.clear();
	this->break_info_list.downstream_wall_stress.clear();
	this->break_info_list.upstream_wall_stress.clear();
	this->break_info_list.mean_q.clear();
	this->break_info_list.mean_v.clear();
	this->break_info_list.time.clear();
	this->break_info_list.time_breach.clear();
	this->break_info_list.total_breach.clear();
	buffer += "/";
	buffer += hyd_label::paraview;
	buffer += "/";

	if (buffer != label::not_set) {
		stringstream suffix;
		if (this->user_defined == true) {
			suffix << "BREACH_RV_" << this->index_rv_model << "_ID_" << this->index << hyd_label::csv;
		}
		else {
			suffix << "BREACH_RV_" << this->index_rv_model << "_FPL_" << this->index_section_fpl << hyd_label::csv;
		}
		buffer.append(suffix.str());
		this->file_name_csv = buffer;
		//open the file
		try {
			this->open_output_file_csv();
		}
		catch (Error msg) {
			throw msg;
		}
	}

}
//________________
//private
//Allocate the list of possible starting coupling points
void Hyd_Coupling_RV2FP_Dikebreak::allocate_list_possible_start_points(void){
	this->delete_list_possible_start_points();

	try{
		this->list_possible_start_points=new Hyd_Coupling_Point_RV2FP*[number_possible_start_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//init
	for(int i=0; i< this->number_possible_start_points; i++){
		this->list_possible_start_points[i]=NULL;
	}
}
//Delete the list of possible starting coupling points
void Hyd_Coupling_RV2FP_Dikebreak::delete_list_possible_start_points(void){
	if(this->list_possible_start_points!=NULL){
		delete []this->list_possible_start_points;
		this->list_possible_start_points=NULL;
	}
}
//Create a list of possible starting coupling points
void Hyd_Coupling_RV2FP_Dikebreak::create_starting_coupling_point_list(_Hyd_River_Profile **involved_profiles, const int number_involved_profiles){

	if(number_involved_profiles==0 ||involved_profiles==NULL ){
		return;
	}

	//count
	int counter=0;
	bool start=false;
	for(int i=0; i< this->point_list->get_number_couplings(); i++){
		if(this->point_list->get_ptr_couplingpoint(i)->get_index_upstream_rvprofile()==involved_profiles[0]->get_profile_number()
			&& start==false){
			start=true;
		}
		if(start==true && this->point_list->get_ptr_couplingpoint(i)->get_break_flag()==true){
			counter++;
		}
		if(i<this->point_list->get_number_couplings()-1 && start==true){
			if(this->point_list->get_ptr_couplingpoint(i)->get_index_downstream_rvprofile()==involved_profiles[number_involved_profiles-1]->get_profile_number()&&
				this->point_list->get_ptr_couplingpoint(i+1)->get_index_downstream_rvprofile()!=involved_profiles[number_involved_profiles-1]->get_profile_number()){
				start=false;
			}
		}
	}
	//set the list
	this->number_possible_start_points=counter;
	this->allocate_list_possible_start_points();
	counter=0;
	start=false;
	for(int i=0; i< this->point_list->get_number_couplings(); i++){
		if(this->point_list->get_ptr_couplingpoint(i)->get_index_upstream_rvprofile()==involved_profiles[0]->get_profile_number()
			&& start==false){
			start=true;
		}
		if(start==true && this->point_list->get_ptr_couplingpoint(i)->get_break_flag()==true){
			this->list_possible_start_points[counter]=this->point_list->get_ptr_couplingpoint(i);
			counter++;
		}
		if(i<this->point_list->get_number_couplings()-1 && start==true){
			if(this->point_list->get_ptr_couplingpoint(i)->get_index_downstream_rvprofile()==involved_profiles[number_involved_profiles-1]->get_profile_number()&&
				this->point_list->get_ptr_couplingpoint(i+1)->get_index_downstream_rvprofile()!=involved_profiles[number_involved_profiles-1]->get_profile_number()){
				start=false;
			}
		}
	}

	/*ostringstream cout;
	cout << " coupling point list "<<endl;
	for(int i=0; i< this->number_possible_start_points; i++){
		cout <<i<<" "<< this->list_possible_start_points[i]->get_downstream_rvprofile()->get_profile_number() <<" " << this->list_possible_start_points[i]->get_upstream_rvprofile()->get_profile_number() <<endl;

	}*/
	/*Sys_Common_Output::output_hyd->output_txt(&cout);*/
}
//Observe and switch, if required, the possible starting coupling points
void Hyd_Coupling_RV2FP_Dikebreak::observe_starting_coupling_points(void){
	if(this->list_possible_start_points==NULL){
		return;
	}
	int index_max=-1;

	for(int i=0; i< this->number_possible_start_points; i++){
		this->list_possible_start_points[i]->calculate_mid_waterlevel();
	}

	this->sort_list_possible_start_points();
	for(int i=0; i< this->number_possible_start_points; i++){
		if(this->list_possible_start_points[i]->get_mid_waterlevel()-this->list_possible_start_points[i]->get_mid_basepoint_height_profile()>this->max_observed_waterlevel){
			this->max_observed_waterlevel=this->list_possible_start_points[i]->get_mid_waterlevel()-this->list_possible_start_points[i]->get_mid_basepoint_height_profile();
		}
	}

	double buff_max=0.0;
	for(int i=0; i< this->number_possible_start_points; i++){
		if((this->list_possible_start_points[i]->get_mid_waterlevel()-this->list_possible_start_points[i]->get_mid_basepoint_height_profile())>this->start_waterlevel_local &&
			this->list_possible_start_points[i]->get_index_break_class()<0){
				if(this->list_possible_start_points[i]->get_mid_waterlevel()-this->list_possible_start_points[i]->get_mid_basepoint_height()>buff_max){
					buff_max=this->list_possible_start_points[i]->get_mid_waterlevel()-this->list_possible_start_points[i]->get_mid_basepoint_height();
					index_max=i;
				}
			
		}
	}

	if(index_max>=0){

		//switch starting point
		this->start_coupling_point=this->list_possible_start_points[index_max];
		this->set_start_coupling_point();
		if(this->start_coupling_point!=NULL){
			this->set_total_distance_along_polysegment(this->start_coupling_point->get_total_distance_along_polysegment());
			this->set_point_coordinate(this->start_coupling_point->get_xcoordinate(), this->start_coupling_point->get_ycoordinate());
			this->set_distance_up(this->start_coupling_point->get_distance_up());
			this->set_distance_down(this->start_coupling_point->get_distance_down());
			this->start_waterlevel_global=this->start_waterlevel_local+this->start_coupling_point->get_mid_basepoint_height_profile();
		}
	}
}
//Sort the list of the possible starting points after the waterlevel
void Hyd_Coupling_RV2FP_Dikebreak::sort_list_possible_start_points(void){
/**
Bubble-sort algorithm for the sorting the detailed results array after the annuality.
*/
	Hyd_Coupling_Point_RV2FP *temp;

	double buff1=0.0;
	double buff2=0.0;

	for(int i = this->number_possible_start_points-1; i > 0; i--){
		for (int j = 1; j <= i; j++){
			buff1=this->list_possible_start_points[j-1]->get_mid_waterlevel()-this->list_possible_start_points[j-1]->get_mid_basepoint_height();
			buff2=this->list_possible_start_points[j]->get_mid_waterlevel()-this->list_possible_start_points[j]->get_mid_basepoint_height();

			if (buff1 < buff2){
				temp = this->list_possible_start_points[j-1];
				this->list_possible_start_points[j-1] = this->list_possible_start_points[j];
				this->list_possible_start_points[j] = temp;
			}
		}
	}
}
//Set the dikebreak coupling member with a structure where the parser has gathered the informations (_hyd_rv2fp_dikebreak_coupling_params)
void Hyd_Coupling_RV2FP_Dikebreak::set_dikebreak_coupling_params(_hyd_rv2fp_dikebreak_coupling_params parameter){
	this->index_rv_model=parameter.rv_model_no;
	this->distance_start_related=parameter.distance;
	this->index_related_profile=parameter.profile_index;
	this->start_waterlevel_local=parameter.begin_waterlevel;
	this->max_breach_width=parameter.max_breach_width;
	this->left_flag=parameter.left_flag;
}
//Calculate the downstream breach opening
void Hyd_Coupling_RV2FP_Dikebreak::calculate_downstream_opening(void){
	if(this->downstream_open_flag==true){
		double d_width_buff=0.0;
		double available_time=this->delta_t;
		double needed_time=0.0;
		bool stop_flag=false;

		//abrupt
		if(this->break_params_downstream->abrupt_fails_flag==true){
			//not in transition phase=> stress are sum up
			if(this->transition_downstream_flag==false){
				//next point
				if(abs(this->distance_next_point_downstream)<=constant::meter_epsilon){
					//set a new coupling point
					this->set_new_downstream_coupling_point(this->point_list->get_next_point_upwards(this->downstream_coupling_point));
				}
				d_width_buff=this->calculate_abrupt_breach(this->break_params_downstream, *this->deltah_downstream,&this->counter_stress_downstream);
				if(d_width_buff>0.0){
					this->transition_downstream_flag=true;
					if((this->distance_next_point_downstream-d_width_buff)<0.0){
						//next point (see above)
						d_width_buff=this->distance_next_point_downstream;
						//distance is set to 0.0
						this->distance_next_point_downstream=0.0;						
					}
					else{
						//reduce the distance to the coupling point
						this->distance_next_point_downstream=this->distance_next_point_downstream-d_width_buff;

					}
					//set the transition width which must be set the next steps
					this->transition_width_downstream=d_width_buff;	
				}
			}
			//transition phase
			else{
				//delta breach width
				d_width_buff=this->numerical_breach_v*this->delta_t;
				
				if(this->transition_width_downstream-d_width_buff<0.0){
					//transition phase is finished; set the rest
					d_width_buff=this->transition_width_downstream;
					this->transition_width_downstream=0.0;
					this->transition_downstream_flag=false;
				}
				else{
					//calculate the rest of the transition width
					this->transition_width_downstream=this->transition_width_downstream-d_width_buff;
				}
				//add it to the breach widths
				this->total_breach_width=this->total_breach_width+d_width_buff;
				//check if total is reach
				if(this->total_breach_width>this->max_breach_width){
					//reduce it
					d_width_buff=d_width_buff-(this->total_breach_width-this->max_breach_width);
					this->total_breach_width=this->max_breach_width;
				}

				this->breach_width_downstream=this->breach_width_downstream+d_width_buff;
				//add it to the coupling point
				this->downstream_coupling_point->add_break_width(d_width_buff);

			}
		}
		//continuous
		else{
			do{
				//calculate the gradient
				d_width_buff=this->calculate_continuous_breach(this->break_params_downstream, *this->deltah_downstream); 
				
				if(d_width_buff>0.0){
					//calculate needed time
					needed_time=this->distance_next_point_downstream/d_width_buff;
					//distance is sufficient
					if((available_time-needed_time)<=constant::sec_epsilon){
						//calculate the width
						d_width_buff=this->delta_t*d_width_buff;
						//add it to the breach widths
						this->total_breach_width=this->total_breach_width+d_width_buff;
						//check if total is reach
						if(this->total_breach_width>this->max_breach_width){
							//reduce it
							d_width_buff=d_width_buff-(this->total_breach_width-this->max_breach_width);
							this->total_breach_width=this->max_breach_width;
						}

						this->breach_width_downstream=this->breach_width_downstream+d_width_buff;
						//add it to the coupling point
						this->downstream_coupling_point->add_break_width(d_width_buff);
						//reduce the distance to the coupling point
						this->distance_next_point_downstream=this->distance_next_point_downstream-d_width_buff;
						//stop here
						stop_flag=true;
					}
					else{
						//distance is not sufficient=> change the coupling point
						//new available time
						available_time=available_time-needed_time;
						//calculate the width
						d_width_buff=needed_time*d_width_buff;

						//add it to the breach widths
						this->total_breach_width=this->total_breach_width+d_width_buff;
						//check if total is reach
						if(this->total_breach_width>this->max_breach_width){
							//reduce it
							d_width_buff=d_width_buff-(this->total_breach_width-this->max_breach_width);
							this->total_breach_width=this->max_breach_width;
						}

						this->breach_width_downstream=this->breach_width_downstream+d_width_buff;

						//add it to the coupling point
						this->downstream_coupling_point->add_break_width(d_width_buff);
						//reduce the distance to the coupling point
						this->distance_next_point_downstream=this->distance_next_point_downstream-d_width_buff;

						//set a new coupling point
						this->set_new_downstream_coupling_point(this->point_list->get_next_point_upwards(this->downstream_coupling_point));
						//check if a further opening is possible
						if(this->downstream_open_flag==false){
							stop_flag=true;	
						}
					}
				}
				else{
					//no breach opening
					stop_flag=true;
				}
			}
			while(stop_flag==false);	
		}
	}
}
//Calculate the upstream breach opening
void Hyd_Coupling_RV2FP_Dikebreak::calculate_upstream_opening(void){
	if(this->upstream_open_flag==true){
		double d_width_buff=0.0;
		double available_time=this->delta_t;
		double needed_time=0.0;
		bool stop_flag=false;

		//abrupt
		if(this->break_params_upstream->abrupt_fails_flag==true){
			//not in transition phase=> stress are sum up
			if(this->transition_upstream_flag==false){
				//next point
				if(abs(this->distance_next_point_upstream)<=constant::meter_epsilon){
					//set a new coupling point
					this->set_new_upstream_coupling_point(this->point_list->get_next_point_downwards(this->upstream_coupling_point));
				}
				d_width_buff=this->calculate_abrupt_breach(this->break_params_upstream, *this->deltah_upstream,&this->counter_stress_upstream);
				if(d_width_buff>0.0){
					this->transition_upstream_flag=true;
					if((this->distance_next_point_upstream-d_width_buff)<0.0){
						//next point (see above)
						d_width_buff=this->distance_next_point_upstream;
						//distance is set to 0.0
						this->distance_next_point_upstream=0.0;						
					}
					else{
						//reduce the distance to the coupling point
						this->distance_next_point_upstream=this->distance_next_point_upstream-d_width_buff;

					}
					//set the transition width which must be set the next steps
					this->transition_width_upstream=d_width_buff;	
				}
			}
			//transition phase
			else{
				//delta breach width
				d_width_buff=this->numerical_breach_v*this->delta_t;
				
				if(this->transition_width_upstream-d_width_buff<0.0){
					//transition phase is finished; set the rest
					d_width_buff=this->transition_width_upstream;
					this->transition_width_upstream=0.0;
					this->transition_upstream_flag=false;
				}
				else{
					//calculate the rest of the transition width
					this->transition_width_upstream=this->transition_width_upstream-d_width_buff;
				}
				//add it to the breach widths
				this->total_breach_width=this->total_breach_width+d_width_buff;
				//check if total is reach
				if(this->total_breach_width>this->max_breach_width){
					//reduce it
					d_width_buff=d_width_buff-(this->total_breach_width-this->max_breach_width);
					this->total_breach_width=this->max_breach_width;
				}
				this->breach_width_upstream=this->breach_width_upstream+d_width_buff;

				//add it to the coupling point
				this->upstream_coupling_point->add_break_width(d_width_buff);
			}
		
		}
		//continuous
		else{
			do{
				//calculate the gradient
				d_width_buff=this->calculate_continuous_breach(this->break_params_upstream, *this->deltah_upstream);
				
				if(d_width_buff>0.0){
					//calculate needed time
					needed_time=this->distance_next_point_upstream/d_width_buff;
					//distance is sufficient
					if((available_time-needed_time)<=constant::sec_epsilon){
						//calculate the width
						d_width_buff=this->delta_t*d_width_buff;

						//add it to the breach widths
						this->total_breach_width=this->total_breach_width+d_width_buff;
						//check if total is reach
						if(this->total_breach_width>this->max_breach_width){
							//reduce it
							d_width_buff=d_width_buff-(this->total_breach_width-this->max_breach_width);
							this->total_breach_width=this->max_breach_width;
						}
						this->breach_width_upstream=this->breach_width_upstream+d_width_buff;

						//add it to the coupling point
						this->upstream_coupling_point->add_break_width(d_width_buff);
						//reduce the distance to the coupling point
						this->distance_next_point_upstream=this->distance_next_point_upstream-d_width_buff;
						//stop here
						stop_flag=true;
					}
					else{
						//distance is not sufficient=> change the coupling point
						//new available time
						available_time=available_time-needed_time;
						//calculate the width
						d_width_buff=needed_time*d_width_buff;
						//add it to the breach widths
						this->total_breach_width=this->total_breach_width+d_width_buff;
						//check if total is reach
						if(this->total_breach_width>this->max_breach_width){
							//reduce it
							d_width_buff=d_width_buff-(this->total_breach_width-this->max_breach_width);
							this->total_breach_width=this->max_breach_width;
						}
						this->breach_width_upstream=this->breach_width_upstream+d_width_buff;

						//add it to the coupling point
						this->upstream_coupling_point->add_break_width(d_width_buff);
						//reduce the distance to the coupling point
						this->distance_next_point_upstream=this->distance_next_point_upstream-d_width_buff;


						//set a new coupling point
						this->set_new_upstream_coupling_point(this->point_list->get_next_point_downwards(this->upstream_coupling_point));
						//check if a further opening is possible
						if(this->upstream_open_flag==false){
							stop_flag=true;	
						}
					}
				}
				else{
					//no breach opening
					stop_flag=true;
				}
			}
			while(stop_flag==false);
		}
	}
}
//Set a new pointer to an upstream coupling point 
void Hyd_Coupling_RV2FP_Dikebreak::set_new_upstream_coupling_point(Hyd_Coupling_Point_RV2FP *new_coupling_point){
	this->upstream_open_flag=false;
	if(new_coupling_point!=NULL && new_coupling_point->get_coupling_flag()==true && this->started_flag==true && new_coupling_point->get_stop_break_flag()==false && new_coupling_point->get_index_break_class()<0){
		if(this->left_flag==true){
			if(new_coupling_point->get_upstream_rvprofile()->get_break_flag_left()==true){
				if(new_coupling_point->get_upstream_rvprofile()->get_break_parameter_left()->abrupt_fails_flag==this->break_params_upstream->abrupt_fails_flag){
					this->upstream_coupling_point=new_coupling_point;
					this->upstream_coupling_point->set_index_break_class(this->index);
					this->upstream_coupling_point->set_mid_basepoint2breakheight();
					this->upstream_coupling_point->calculate_delta_h_break();
					this->break_params_upstream=this->upstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_left();
					this->deltah_upstream=this->upstream_coupling_point->get_delta_h_break();
					this->upstream_open_flag=true;
					this->distance_next_point_upstream=this->upstream_coupling_point->get_distance_down();

				}

			}
		}
		else{
			if(new_coupling_point->get_upstream_rvprofile()->get_break_flag_right()==true){
				if(new_coupling_point->get_upstream_rvprofile()->get_break_parameter_right()->abrupt_fails_flag==this->break_params_upstream->abrupt_fails_flag){
					this->upstream_coupling_point=new_coupling_point;
					this->upstream_coupling_point->set_index_break_class(this->index);
					this->upstream_coupling_point->set_mid_basepoint2breakheight();
					this->upstream_coupling_point->calculate_delta_h_break();
					this->break_params_upstream=this->upstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_right();
					this->deltah_upstream=this->upstream_coupling_point->get_delta_h_break();
					this->upstream_open_flag=true;
					this->distance_next_point_upstream=this->upstream_coupling_point->get_distance_down();
				}
			}
		}
	}
}
//Set a new pointer to a downstream coupling point 
void Hyd_Coupling_RV2FP_Dikebreak::set_new_downstream_coupling_point(Hyd_Coupling_Point_RV2FP *new_coupling_point){
	this->downstream_open_flag=false;
	if(new_coupling_point!=NULL && new_coupling_point->get_coupling_flag()==true && this->started_flag==true && this->downstream_coupling_point->get_stop_break_flag()==false && new_coupling_point->get_index_break_class()<0){
		if(this->left_flag==true){
			if(new_coupling_point->get_upstream_rvprofile()->get_break_flag_left()==true){
				if(new_coupling_point->get_upstream_rvprofile()->get_break_parameter_left()->abrupt_fails_flag==this->break_params_downstream->abrupt_fails_flag){
					this->downstream_coupling_point=new_coupling_point;
					this->downstream_coupling_point->set_index_break_class(this->index);
					this->downstream_coupling_point->set_mid_basepoint2breakheight();
					this->downstream_coupling_point->calculate_delta_h_break();
					this->break_params_downstream=this->downstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_left();
					this->deltah_downstream=this->downstream_coupling_point->get_delta_h_break();
					this->downstream_open_flag=true;
					this->distance_next_point_downstream=this->downstream_coupling_point->get_distance_down();

				}

			}
		}
		else{
			if(new_coupling_point->get_upstream_rvprofile()->get_break_flag_right()==true){
				if(new_coupling_point->get_upstream_rvprofile()->get_break_parameter_right()->abrupt_fails_flag==this->break_params_downstream->abrupt_fails_flag){
					this->downstream_coupling_point=new_coupling_point;
					this->downstream_coupling_point->set_index_break_class(this->index);
					this->downstream_coupling_point->set_mid_basepoint2breakheight();
					this->downstream_coupling_point->calculate_delta_h_break();
					this->break_params_downstream=this->downstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_right();
					this->deltah_downstream=this->downstream_coupling_point->get_delta_h_break();
					this->downstream_open_flag=true;
					this->distance_next_point_downstream=this->downstream_coupling_point->get_distance_down();
				}
			}
		}
	}
}
//Calculate the mean discharge/mean velocity over the breach per calculation step
void Hyd_Coupling_RV2FP_Dikebreak::calculate_mean_q_v(const double timepoint){
	this->mean_q=0.0;
	this->mean_v=0.0;
	if(this->coupling_flag==true && this->started_flag==true){
		int counter=0;
		for(int i=0; i< this->point_list->get_number_couplings() ; i++){
			if(this->index==this->point_list->get_ptr_couplingpoint(i)->get_index_break_class()){
				counter++;
				this->mean_q=this->mean_q+(-1.0)*this->point_list->get_ptr_couplingpoint(i)->get_q_break();
				this->mean_v=this->mean_v+this->point_list->get_ptr_couplingpoint(i)->get_v_break();
			}
		}
		if(counter>0){
			this->mean_q = this->mean_q;
			this->mean_v=this->mean_v/counter;
		}
		//calculate max v
		this->calculate_maximum_values(timepoint,this->mean_v, &this->max_coupling_v);
		//calculate mean volume
		this->calculate_hydrological_balance(this->mean_q, &this->coupling_volume);
	}
}
//Check the dikebreak parameters
void Hyd_Coupling_RV2FP_Dikebreak::check_dikebreak_parameter(void){
	_Hyd_Coupling_Dikebreak::check_dikebreak_parameter();
	//index of the river profile is < 0
	if(this->index_related_profile<0){
		Error msg=this->set_error(1);
		throw msg;
	}
}
//Output the header to the result tecplot file
void Hyd_Coupling_RV2FP_Dikebreak::output_header_result2file_tecplot(void){

	if(this->file_name!=label::not_set){
		//output the file header
		this->output_file << "TITLE = "<< "\" Breachno. " << this->index<<" at Riverno. " << this->index_rv_model;
		this->output_file << "; Distance to start "<< this->distance_along_polysegment<<label::m << "; Left bank " <<functions::convert_boolean2string(this->left_flag) ;
		this->output_file <<"; Type abrupt "<<functions::convert_boolean2string(this->break_params_downstream->abrupt_fails_flag) << " \"" << endl;
		this->output_file << "VARIABLES = " << endl;
		this->output_file<< " \" t (global) " << label::sec << " \" \" No. \" \" " << " t (local) " << label::sec << "\"   \"" <<  " b (total) " << label::m << "\"   \""  ;
		this->output_file << " b (upstream) " << label::m <<   "\"   \"" <<  " b (downstream)" << label::m << "\"   \"" << " delta h (upstream) " << label::m ;
		this->output_file	<< "\"   \"" <<  " R (upstream)";
		if(this->wall_breach_flag==true){
			this->output_file<<label::stress_unit;
		}
		else{
			this->output_file<<label::m_per_sec;
		}
		this->output_file << "\"   \""<< " delta h (downstream) " << label::m <<   "\"   \"";
		this->output_file<<  " R (downstream)";
		if(this->wall_breach_flag==true){
			this->output_file<<label::stress_unit;
		}
		else{
			this->output_file<<label::m_per_sec;
		}
		this->output_file <<   "\"   \"" <<  " q (mean)" << label::qm_per_sec << "\"   \"" <<  " v (mean)" << label::m_per_sec ;
		if(this->wall_breach_flag==true){
			this->output_file<< "\"   \"" << " S (upstream) " << label::stress_unit<< "\"   \"" << " S (downstream) " << label::stress_unit ;
		}
		this->output_file <<   "\"   \"" <<  " delta h (begin)" << label::m << "\"  ";
		this->output_file<< "\" "<<  endl ;


		//if(this->start_coupling_point!=NULL){
		//	this->output_file<<"#Further break information"<<endl;
		//	this->output_file<<"#Index break point       "<< this->start_coupling_point->get_point_index()<<endl;
		//	this->output_file<<"#Local break height [m]  "<< this->start_waterlevel_local<<endl;
		//	this->output_file<<"#Global break height [m] "<< this->start_waterlevel_global<<endl;
		//	this->output_file<<"#Index break             "<< this->index <<endl;
		//}

	}
}
//Output the header to the result csv file
void Hyd_Coupling_RV2FP_Dikebreak::output_header_result2file_csv(void) {

	if (this->file_name_csv != label::not_set) {
		//output the file header
		this->output_file_csv << "  t (global) " << label::sec << " , No. , " << " t (local) " << label::sec << ", " << " b (total) " << label::m << ", ";
		this->output_file_csv << " b (upstream) " << label::m << "," << " b (downstream)" << label::m << ", " << " delta h (upstream) " << label::m;
		this->output_file_csv << ", " << " R (upstream)";
		if (this->wall_breach_flag == true) {
			this->output_file_csv << label::stress_unit;
		}
		else {
			this->output_file_csv << label::m_per_sec;
		}
		this->output_file_csv << ", " << " delta h (downstream) " << label::m << ", ";
		this->output_file_csv << " R (downstream)";
		if (this->wall_breach_flag == true) {
			this->output_file_csv << label::stress_unit;
		}
		else {
			this->output_file_csv << label::m_per_sec;
		}
		this->output_file_csv << ", " << " q (mean)" << label::qm_per_sec << ", " << " v (mean)" << label::m_per_sec;
		if (this->wall_breach_flag == true) {
			this->output_file_csv << ", " << " S (upstream) " << label::stress_unit << ", " << " S (downstream) " << label::stress_unit;
		}
		this->output_file_csv << ", " << " delta h (begin)" << label::m << endl;
	



	}

}
//Investigate and set the starting coupling point
bool Hyd_Coupling_RV2FP_Dikebreak::set_start_coupling_point(void){
	if(this->start_coupling_point->get_break_flag()==true && this->start_coupling_point->get_index_break_class()<0){
		//set the global starting height
		this->start_waterlevel_global=this->start_waterlevel_local+this->start_coupling_point->get_mid_basepoint_height_profile();
		//set the upstream and downstream coupling pointer; for starting they are equal to the starting point
		//downstream value
		this->downstream_coupling_point=this->start_coupling_point;
		this->deltah_downstream=this->downstream_coupling_point->get_delta_h_break();
		this->downstream_open_flag=true;
		this->distance_next_point_downstream=0.0;
		//upstream value
		this->upstream_coupling_point=this->start_coupling_point;
		this->deltah_upstream=this->upstream_coupling_point->get_delta_h_break();
		this->upstream_open_flag=true;
		this->distance_next_point_upstream=this->distance_down;

		if(this->left_flag==true){
			this->break_params_downstream=this->downstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_left();
			this->break_params_upstream=this->upstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_left();
			this->wall_breach_flag=this->break_params_upstream->abrupt_fails_flag;
		}
		else{
			this->break_params_downstream=this->downstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_right();
			this->break_params_upstream=this->upstream_coupling_point->get_upstream_rvprofile()->get_break_parameter_right();
			this->wall_breach_flag=this->break_params_upstream->abrupt_fails_flag;
		}
		return true;
	}
	else{
		return false;
	}
}
//Get a string of the breach polyline
string Hyd_Coupling_RV2FP_Dikebreak::get_breach_polyline(void){
	string polystring;

	int number=2;
	//count the points
	for(int i=0; i< this->point_list->get_number_couplings() ; i++){
		if(this->index==this->point_list->get_ptr_couplingpoint(i)->get_index_break_class() &&
			this->point_list->get_ptr_couplingpoint(i)->get_break_is_total()==true){
			number++;
		}
	}
	
	//allocate the points
	Geo_Point *point=NULL;
	try{
		point=new Geo_Point[number];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(4);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//evaluate the points and set it
	_Hyd_Coupling_Point buffer;
	double buffer_dist=0.0;
	
	buffer_dist=this->get_total_distance_along_polysegment()-(this->breach_width_upstream+0.5*this->init_breach_width);
	if (this->point_list->find_coupling_point(buffer_dist, &buffer) == NULL) {
		Error msg = this->set_error(6);
		throw msg;
	};

	point[0].set_point(&buffer);
	point[0].set_point_name("Breach point");

	buffer_dist=this->get_total_distance_along_polysegment()+(this->breach_width_upstream+0.5*this->init_breach_width);
	if (this->point_list->find_coupling_point(buffer_dist, &buffer) == NULL) {
		Error msg = this->set_error(6);
		throw msg;
	};
	
	point[number-1].set_point(&buffer);
	point[number-1].set_point_name("Breach point");

	int counter=1;
	for(int i=0; i< this->point_list->get_number_couplings() ; i++){
		if(this->index==this->point_list->get_ptr_couplingpoint(i)->get_index_break_class()&&
			this->point_list->get_ptr_couplingpoint(i)->get_break_is_total()==true){
			point[counter].set_point(this->point_list->get_ptr_couplingpoint(i));
			point[counter].set_point_name("Breach point");
			counter++;
		}
	}


	//produce the string
	ostringstream buff;
	buff << FORMAT_FIXED_REAL << P(3);
	buff << " ST_GEOMFROMTEXT('LINESTRING(";
	for(int i=0; i< number-1; i++){
		buff<<point[i].get_xcoordinate() << "  ";
		buff<<point[i].get_ycoordinate()<< ", ";
	}
	if(number!=0){
		buff<<point[number-1].get_xcoordinate() << "  ";
		buff<<point[number-1].get_ycoordinate();
	}

	buff << ")') ";

	polystring=buff.str();

	if(point!=NULL){
		delete []point;
		point=NULL;
	}

	return polystring;
}
//Set error(s)
Error Hyd_Coupling_RV2FP_Dikebreak::set_error(const int err_type){
	string place="Hyd_Coupling_RV2FP_Dikebreak::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_list_possible_start_points(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;

		case 1://related profile index is < 0
			place.append("check_dikebreak_parameter(Hyd_Model_River *river_model)");
			reason="The given related profile index for the dikebreak is < 0";
			help="Check the given related profile index";
			info << "Dikebreak coupling index: " << this->index << endl;
			info << "River index             : " << this->index_rv_model << endl;
			type=16;
			break;
		case 4://bad alloc
			place.append("get_breach_polyline(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 5://break coupling not possible
			place.append("set_additional_coupling_point(Hyd_Coupling_RV2FP_Merged *couplings)");
			reason="The break coupling in the dike line can't be setted";
            help="Check the coupling point corresponding to the given break point \nif a coupling could be established (e.g. noinfo 2D-elements prohibit a coupling; nobreak flag in dikeline)";
            info << "Dikebreak coupling index: " << this->index << endl;
			type=16;
			break;
		case 6://no point found
			place.append("get_breach_polyline(void)");
			reason = "No breach points found for an output";
			help = "Check the coupling point corresponding to the given break point \nif a coupling could be established";
			info << "Dikebreak coupling index: " << this->index << endl;
			type = 16;
			break;
			
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
//Set warning(s)
Warning Hyd_Coupling_RV2FP_Dikebreak::set_warning(const int warn_type){
	string place="Hyd_Coupling_RV2FP_Dikebreak::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://no coupling between the river an dthe floodplain is possible
			place.append("init_coupling(Hyd_Coupling_RV2FP *couplings, const int number)") ;
			reason="A dikebreak coupling between a river- and a floodplain model is not possible";
			reaction="The setted coupling is ignored";
			help= "Check if the given distances/indices are in a floodplain model/rivermodel \nCheck if both profiles have a setted basepoint at the river bank of the dikebreak \nCheck multiple break settings\n Check the height of the coupled floodplain element";
			info << "Dikebreak coupling index: " << this->index << endl;
			info << "River index             : " << this->index_rv_model << endl;
			info << "Index of related profile: " << this->index_related_profile<<endl;
			type=10;	
			break;
		case 1://input datas can not submitted
			place.append("output_final_results(QSqlDatabase *ptr_database, const string id_break, const int bound_sc)");
			reason="Can not submit the final results of the breach development to the database";
			help="Check the database";
			type=2;
			break;
		case 2://input datas can not submitted
			place.append("transfer_break_data2database_table(QSqlDatabase *ptr_database)");
			reason="Can not submit the dike-/wallbreak coupling data to the database";
			help="Check the database";
			type=2;
			break;


		default:
			place.append("set_warning(const int warn_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;
};
