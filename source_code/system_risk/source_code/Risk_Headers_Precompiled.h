#pragma once
#ifndef RISK_HEADERS_PRECOMPILED_H
#define RISK_HEADERS_PRECOMPILED_H

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
#include "Common_Const.h"
//add this to the cpp-files

//main risk system
#include "Risk_System.h"




//namespace
using namespace std;

#endif