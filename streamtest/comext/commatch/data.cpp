#include "stdafx.h"
#include "data.h"

daevariant::daevariant(void) {
}

daevariant::~daevariant(void) {
}

// various constructor flavours
daevariant::daevariant(const int intVal) { 
	int_val = intVal;
	long_val = 0;
	float_val = 0;
	str_val = "";
	wstr_val = L"";
}
daevariant::daevariant(const long longVal) { 
	int_val = 0;
	long_val = longVal;
	float_val = 0;
	str_val = "";
	wstr_val = L"";
}
daevariant::daevariant(const float floatVal) { 
	int_val = 0;
	long_val = 0;
	float_val = floatVal;
	str_val = "";
	wstr_val = L"";
}
daevariant::daevariant(wstring strVal) { 
	int_val = 0;		
	long_val = 0;
	float_val = 0;
	str_val = "";
	wstr_val = strVal;
}
daevariant::daevariant(string strVal) { 
	int_val = 0;		
	long_val = 0;
	float_val = 0;
	str_val = strVal;
	wstr_val = L"";
}

// various assignment operator flavours
daevariant& daevariant::operator=(const int intVal) { 
	int_val = intVal;
	str_val = "";
	wstr_val = L"";
	long_val = 0;
	float_val = 0;
	return (*this); 
}
daevariant& daevariant::operator=(const long longVal)	{ 
	int_val = 0;
	str_val = "";
	wstr_val = L"";
	long_val = longVal;
	float_val = 0;
	return (*this); 
}

daevariant& daevariant::operator=(const float floatVal)	{ 
	int_val = 0;
	str_val = "";
	wstr_val = L"";
	long_val = 0;
	float_val = floatVal;
	return (*this); 
}

daevariant& daevariant::operator=(wstring strVal)	{ 
	int_val = 0;
	str_val = "";
	wstr_val = strVal;
	long_val = 0;
	float_val = 0;
	return (*this); 
}

daevariant& daevariant::operator=(string strVal)	{ 
	int_val = 0;
	str_val = strVal;
	wstr_val = L"";
	long_val = 0;
	float_val = 0;
	return (*this); 
}

// various returns
daevariant::operator int() { 		
	return int_val; 
}

daevariant::operator long() { 
	return long_val; 
}

daevariant::operator float() { 
	return float_val; 
}

daevariant::operator string() { 
	return str_val; 
}
daevariant::operator wstring() { 
	return wstr_val; 
}
