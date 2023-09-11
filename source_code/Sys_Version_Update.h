#pragma once
#ifndef SYS_VERSION_UPDATE_H
#define SYS_VERSION_UPDATE_H
/**\class Sys_Version_Update
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2011
*/

///Class for check and update old ProMaIDes-versions \ingroup sys
/**

*/
class Sys_Version_Update
{
public:
	///Default constructor
	Sys_Version_Update(void);
	///Default destructor
	~Sys_Version_Update(void);

	//method
	///Check and update the hydraulic table for observation points (20.12.2011)
	//void check_update_hyd_table_obs_point(QSqlDatabase *ptr_database);
	///Check and update the text of the hydraulic table of the polysegment (20.12.2011)
	//void check_update_hyd_table_polysegment_name(const string project_file);


private:




	///Set error(s)
	Error set_error(const int err_type);
};
#endif