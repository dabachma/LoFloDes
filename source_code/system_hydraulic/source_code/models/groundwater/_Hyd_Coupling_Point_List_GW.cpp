#include "source_code\Hyd_Headers_Precompiled.h"
//#include "_Hyd_Coupling_Point_List_GW.h"

//constructor
_Hyd_Coupling_Point_List_GW::_Hyd_Coupling_Point_List_GW(void) :block_elems(50), min_distance(10.0) {
	this->number = 0;
	this->defining_polysegment = NULL;
}
//destructor
_Hyd_Coupling_Point_List_GW::~_Hyd_Coupling_Point_List_GW(void) {

}
//____________
//public
//Set the defining line
void _Hyd_Coupling_Point_List_GW::set_defining_polysegment_gw(Hyd_Groundwater_Polysegment *polysegment) {
	this->defining_polysegment = polysegment;

}
void _Hyd_Coupling_Point_List_GW::set_defining_polysegment_gw(Hyd_Floodplain_Polysegment *polysegment) {
	this->defining_polysegment = new Hyd_Groundwater_Polysegment;
	this->defining_polysegment->clone_polysegment(polysegment);

}
//Get a pointer of the defining line
Hyd_Groundwater_Polysegment* _Hyd_Coupling_Point_List_GW::get_defining_polysegment_gw(void) {
	return this->defining_polysegment;
}
//Add the relevant coupling points of the defining_polysegment
void _Hyd_Coupling_Point_List_GW::add_relevant_polysegment_points(Hyd_Groundwater_Polygon *gw_boundary) {
	Hyd_Groundwater_Polygon *buffer;
	buffer = gw_boundary;
}
//Clone the coupling point list
void _Hyd_Coupling_Point_List_GW::clone_list(_Hyd_Coupling_Point_List_GW *list) {
	this->number = list->number;
}
//______________________
//private
//Set error(s)
Error _Hyd_Coupling_Point_List_GW::set_error(const int err_type) {
	string place = "_Hyd_Coupling_Point_List_GW::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;

	switch (err_type) {
	case 0://bad alloc
		place.append("calculate_distances(void)");
		reason = "No defining polysegment is set";
		help = "Set it before (set_defining_polysegment(Hyd_Groundwater_Polysegment *polysegment)) using this method";
		type = 6;
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