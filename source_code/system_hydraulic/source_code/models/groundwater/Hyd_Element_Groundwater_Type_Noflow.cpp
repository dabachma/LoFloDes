#include "source_code\Hyd_Headers_Precompiled.h"
//#include "Hyd_Element_Groundwater_Type_Noflow.h"

//constructor
Hyd_Element_Groundwater_Type_Noflow::Hyd_Element_Groundwater_Type_Noflow(void) {
	this->h_value = -999;
	this->s_value = -999;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Element_Groundwater_Type_Noflow), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Element_Groundwater_Type_Noflow::~Hyd_Element_Groundwater_Type_Noflow(void) {
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Element_Groundwater_Type_Noflow), _sys_system_modules::HYD_SYS);
}
//________
//public
//Initialize the element (here it is not needed)
void Hyd_Element_Groundwater_Type_Noflow::init_element(_hyd_neighbouring_gw_elems neigh_elem, double *, double *, double *, Hyd_Param_Conductivity *, Hyd_Param_Porosity *, const bool) {
	this->h_value = -999;
	this->s_value = -999;
	//if (neigh_elem.x_direction !=NULL && neigh_elem.x_direction->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW) {
	//	if (neigh_elem.x_direction->element_type->get_flow_data().no_flow_x_flag == false) {
	//		neigh_elem.x_direction->element_type->get_flow_data().no_flow_x_flag == true;
	//	}
	//}
	//if (neigh_elem.minus_x_direction != NULL && neigh_elem.minus_x_direction->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW) {
	//	if (neigh_elem.minus_x_direction->element_type->get_flow_data().no_flow_x_flag == false) {
	//		neigh_elem.minus_x_direction->element_type->get_flow_data().no_flow_x_flag == true;
	//	}
	//}
	//if (neigh_elem.y_direction != NULL && neigh_elem.y_direction->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW) {
	//	if (neigh_elem.y_direction->element_type->get_flow_data().no_flow_y_flag == false) {
	//		neigh_elem.y_direction->element_type->get_flow_data().no_flow_y_flag == true;
	//	}
	//}
	//if (neigh_elem.minus_y_direction != NULL && neigh_elem.minus_y_direction->get_elem_type() == _hyd_gw_elem_type::STANDARD_ELEM_GW) {
	//	if (neigh_elem.minus_y_direction->element_type->get_flow_data().no_flow_y_flag == false) {
	//		neigh_elem.minus_y_direction->element_type->get_flow_data().no_flow_y_flag == true;
	//	}
	//}
}
//__________
// 
///Get the local waterlevel (h_value)
double Hyd_Element_Groundwater_Type_Noflow::get_h_value(void) {
	return this->h_value;
}
///Get the global waterlevel (s_value)
double Hyd_Element_Groundwater_Type_Noflow::get_s_value(void) {
	return this->s_value;
}
//private
//Output the setted members REVIEW
void Hyd_Element_Groundwater_Type_Noflow::output_setted_members(ostringstream *cout) {
	*cout << W(14) << P(2) << FORMAT_FIXED_REAL << (*this->z_value);
	//x-direction
	*cout << W(14) << "NO-flow" << W(7) << "-";
	//y-direction
	*cout << W(18) << "NO-flow" << W(7) << "-";
	//boundary
	*cout << W(16) << "NO-bound" << W(10) << "-";

	//material values
	*cout << W(10) << -1 << W(12) << "-";
	//init and connection cell
	*cout << W(12) << "-" << W(10) << "-";
}
//Ouput the result members per internal timestep
void Hyd_Element_Groundwater_Type_Noflow::output_result_values(ostringstream *cout) {
	*cout << W(12) << functions::convert_boolean2string(false);
	*cout << W(15) << P(4) << FORMAT_FIXED_REAL << 0.0;
	*cout << W(15) << P(4) << FORMAT_FIXED_REAL << (*this->z_value);
	*cout << W(15) << P(4) << FORMAT_FIXED_REAL << 0.0;
	*cout << W(17) << P(4) << FORMAT_FIXED_REAL << 0.0;
	*cout << W(15) << P(4) << FORMAT_FIXED_REAL << 0.0;
	*cout << W(15) << P(4) << FORMAT_FIXED_REAL << 0.0;
	*cout << endl;
}
//Ouput the maximum calculated values
void Hyd_Element_Groundwater_Type_Noflow::output_maximum_calculated_values(ostringstream *cout) {
	//*cout  << W(12)<<functions::convert_boolean2string(false)<< W(12)<< P(0) << FORMAT_FIXED_REAL<< 0.0;
	//*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0; 
	//*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0; 
	//*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0;
	//*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0;
	//*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0;
	//*cout  << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0;
	//*cout<< endl;

	*cout << "    Not affected " << endl;
}
