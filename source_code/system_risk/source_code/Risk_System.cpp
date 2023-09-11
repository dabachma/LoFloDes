#include "source_code\Risk_Headers_Precompiled.h"
//#include "Risk_System.h"


//Default constructor
Risk_System::Risk_System(void){

	//this->thread_type=_risk_thread_type::risk_thread_unknown;
	



	

	Sys_Memory_Count::self()->add_mem(sizeof(Risk_System),_sys_system_modules::RISK_SYS);//count the memory

}
//Default destructor
Risk_System::~Risk_System(void){

	//close database
	//if(this->qsqldatabase.isOpen()){
		//this->qsqldatabase.close();
	//}
	Hyd_Multiple_Hydraulic_Systems::set_stop_thread_flag(false);
	Dam_Damage_System::set_stop_thread_flag(false);

	Sys_Memory_Count::self()->minus_mem(sizeof(Risk_System),_sys_system_modules::RISK_SYS);//count the memory

}
//________

