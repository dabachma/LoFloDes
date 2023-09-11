#include "Main_Headers_Precompiled.h"
//#include "Sys_Version_Update.h"

//Default constructor
Sys_Version_Update::Sys_Version_Update(void){
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Version_Update)), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Version_Update::~Sys_Version_Update(void){
	Sys_Memory_Count::self()->minus_mem((sizeof(Sys_Version_Update)), _sys_system_modules::SYS_SYS);
}
//____________
//public
//Check for update hydraulic table for observation points (20.12.2011)
//void Sys_Version_Update::check_update_hyd_table_obs_point(QSqlDatabase *ptr_database){
//	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam ||
//		Sys_Project::get_project_type()==_sys_project_type::proj_fpl ||
//		Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file ||
//		Sys_Project::get_project_type()==_sys_project_type::proj_fpl_file){
//		return;
//	}
//	bool error=false;
//	//check it
//	try{
//		Hyd_Observation_Point_Manager::set_table(ptr_database, true);
//	}
//	catch(Error msg){
//		error=true;
//	}
//	//create it
//	if(error==true){
//		if(Hyd_Observation_Point_Manager::obs_point_table->table_name.found_flag==false){
//			Hyd_Observation_Point_Manager::close_table();
//			Hyd_Observation_Point_Manager::create_table(ptr_database);
//		}
//	}
//
//	Hyd_Observation_Point_Manager::close_table();
//}
////Check and update the text of the hydraulic table of the polysegment (20.12.2011)
//void Sys_Version_Update::check_update_hyd_table_polysegment_name(const string project_file){
//	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam ||
//		Sys_Project::get_project_type()==_sys_project_type::proj_fpl ||
//		Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file ||
//		Sys_Project::get_project_type()==_sys_project_type::proj_fpl_file){
//		return;
//	}
//
//	ifstream ifile;
//	string myline;
//	string buffer;
//
//	//open file for reading
//	ifile.open(project_file.c_str(), ios_base::in);
//	if(ifile.is_open()==false){
//		Error msg=this->set_error(0);
//		ostringstream info;
//		info << "Filename: " << project_file << endl;
//		msg.make_second_info(info.str());
//		throw msg;
//	}
//	string table_info;
//	bool table_begin=false;
//	int pos=0;
//	bool found=false;
//	//read in file
//	do{
//		getline(ifile, myline,'\n');
//		if(table_begin==false){
//			pos=myline.find(sys_label::table_begin);
//			if(pos>=0){
//				table_begin=true;
//			}
//		}
//		pos=-1;
//		if(table_begin==true){
//			buffer=myline;
//			_Hyd_Parse_IO::erase_comment(&buffer);
//			pos=buffer.find("Name    name");
//			if(pos>=0 && found==false){
//				found=true;
//				buffer.replace(pos,pos+hyd_label::polyseg_name.length()-2, hyd_label::polyseg_name);
//				myline=buffer;
//			}
//		}
//		table_info.append(myline);
//		table_info.append("\n");
//	}
//	while(ifile.eof()==false);
//	ifile.close();
//
//	if(table_info.find_last_of("\n")==table_info.length()-1){
//		table_info.erase(table_info.length()-1);
//	}
//
//	ofstream ofile;
//	//open for writing
//	ofile.open(project_file.c_str(), ios_base::out);
//	if(ofile.is_open()==false){
//		Error msg=this->set_error(0);
//		ostringstream info;
//		info << "Filename: " << project_file << endl;
//		msg.make_second_info(info.str());
//		throw msg;
//	}
//
//	ofile << table_info;
//	ofile.flush();
//	//close file
//	ofile.close();
//}

//____________
//private

//Set error(s)
Error Sys_Version_Update::set_error(const int err_type){
	string place="Sys_Version_Update::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://can not open project
			place.append("check_update_hyd_table_polysegment_name(const string project_file)");
			reason="Can not open the project file";
			help="Check the file";
			type=5;
			break;
		case 1://can not open project
			place.append("check_update_hyd_table_system_member_sync(const string project_file)");
			reason="Can not open the project file";
			help="Check the file";
			type=5;
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

