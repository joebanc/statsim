// comext.cpp : main project file.

#include "stdafx.h"
#include "comext_10_cong_gov.h"
#include "comext_07_cong_gov.h"
#include "comext_04_cong_gov.h"
#include "comext_01_cong_gov.h"
#include "comext_04_loc.h"
#include "comext_01_loc.h"

int main()
{
	try {
		
		return extcongov10();

	}
	catch ( exception &e ) {
		cerr << "Caught: " << e.what( ) << endl;
		cerr << "Type: " << typeid( e ).name( ) << endl;\
	}


}