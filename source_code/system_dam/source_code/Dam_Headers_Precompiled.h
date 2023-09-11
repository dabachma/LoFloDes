#pragma once
#ifndef  DAM_HEADERS_PRECOMPILED_H
#define DAM_HEADERS_PRECOMPILED_H

//MS specific (Overloading of not deprecated Functions)
#ifdef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
  #undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#endif

#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
  #define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif 

#define _CRT_SECURE_NO_DEPRECATE 

#define _SCL_SECURE_NO_WARNINGS
// End of MS specific

//system class for all damage calculations
#include "Dam_Damage_System.h"

//general
//geometrical raster class
#include "Dam_Raster.h"
//class for the impact values for the damage calculation for one floodplain element
#include "Dam_Impact_Values.h"
//class for the impact values for the damage calculation for one floodplain 
#include "Dam_Impact_Value_Floodplain.h"
//base class of the elements
#include "_Dam_Element.h"
//base for a damage function (2-value)
#include "_Dam_2value_Function.h"
//base class for various systems
#include "_Dam_System.h"
//geometrical polygon for interception
#include "Dam_Polygon.h"

//raster class for a import of hydraulic results
#include "Dam_Hyd_Result_Raster.h" 
//element class for a import of hydraulic results used in the raster class
#include "Dam_Hydimport_Element.h"


//economical
//system for the economical damage calculation
#include "Dam_Ecn_System.h"
//damage function for the relative damage
#include "Dam_Ecn_Damage_Function.h"
//economical damage element
#include "Dam_Ecn_Element.h"
//economicla raster
#include "Dam_Ecn_Raster.h"

//ecological

//display the damage results as summary
#include "DamGui_Result_Summary_Wid.h"
//display the damage results for each scenario
#include "DamGui_Result_Wid.h"
//ask for an multiple raster import
#include "DamGui_Number_Raster_Dia.h"

//widgets for displaying the data
#include "DamGui_Raster_Wid.h"
#include "DamGui_Ecn_Func_Wid.h"


//raster dialog class
#include "DamGui_Raster_Dia.h"


#endif