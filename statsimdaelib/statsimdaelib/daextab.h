#pragma once
#ifndef _VECTOR_
	#include <vector>
#endif

#include <sstream>

//Define daextab
#ifndef DAEXTAB
	#define DAEXTAB

#ifndef DAEVECTOR
	#include "daevector.h"
#endif

#define OVERFLOW -999999999
class __declspec(dllexport) daextab
{
public:
	daextab(stlstrvec2d& src_array, int colidx, int colidy);
	~daextab();

	int at(int i, int j);
	int n_x();
	int n_y();

	int row_tot(int i);
	int col_tot(int j);
	float row_ratio(int i, int colnum, int coldenom);
	float col_ratio(int j, int rownum, int rowdenom);
	int gr_tot();

	std::string srow(int i);
	std::string scol(int j);
	intvec2d value_array;
	
private:
	stlstrvec1d colval, rowval,
		collabs, rowlabs;

	int nrows, ncols;

	void tabulate(stlstrvec2d& src_array, int colidx, int colidy);

};

class __declspec(dllexport) daefdxt	//integer currently
{
public:
	daefdxt(stlstrvec2d& src_array, int colidx, int colidy, unsigned int xcl_size, 
		int x_min=0, int x_upper=80);
	~daefdxt();

	int at(int i, int j);
	int n_x();
	int n_y();
	int row_tot(int i);
	int col_tot(int j);
	float row_ratio(int i, int colnum, int coldenom);
	float col_ratio(int j, int rownum, int rowdenom);
	int gr_tot();

	std::string* srow(int i);
	std::string* scol(int j);
	intvec2d value_array;

private:
	intvec1d colval;
	intvec2d rowfence;
	stlstrvec1d collabs, rowlabs;

	int nrows, ncols;
	
	int value_in(intvec2d fence_array, int value);

	void tabulate(stlstrvec2d& src_array, int colidx, int colidy, unsigned int xcl_size, 
		int x_min, int x_upper);
	
};
#endif
///////////////////
