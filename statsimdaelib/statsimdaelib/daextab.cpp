#include "StdAfx.h"
#include "statsimdaelib.h"
#include ".\daextab.h"
#include "daeconv.h"
#include <numeric>


daextab::daextab(stlstrvec2d& src_array, int colidx, int colidy)
{
	ncols = 0;
	nrows = 0;

	value_array.clear();
	
	colval.clear();
	rowval.clear();
	collabs.clear();
	rowlabs.clear();

	tabulate(src_array, colidx, colidy);
}

daextab::~daextab()
{
}

void daextab::tabulate(stlstrvec2d& src_array, int colidx, int colidy)
{	
	stlstrvec2d::iterator it1;
	for (it1 = src_array.begin(); it1<src_array.end(); ++it1)
	{
		ncols = colval.size();	//size of column values
		nrows = rowval.size();	//size of row values

		intvec1d newrowvec;	//new row vector

		for (int i=0; i<ncols; i++)
		{
			newrowvec.push_back(0);
		}

		std::string xval( it1->at(colidx) );	//rowval
		std::string yval( it1->at(colidy) );	//colval

		// new category
		if( exists(colval, yval)==DAE_NEXIST ){
			colval.push_back(yval);	//put new category

			newrowvec.push_back(0); //put new col for new row vector for new row cat

			intvec2d::iterator it2;	//iterator for the value_array
			
			for (it2 = value_array.begin(); it2<value_array.end(); ++it2) {
				it2->push_back(0);	//augment for the non-existing column
			}

		}
		

		if( exists(rowval, xval)==DAE_NEXIST ) {
			rowval.push_back(xval);	//put new category
			value_array.push_back(newrowvec);	//augment array
			//MessageBox(0, xval.c_str(), L"trim2", MB_OK);

		}

		//std::stringstream s_buff;
		//s_buff<<xrowid <<L"\t" <<xcolid;
		
		//MessageBox(0, s_buff.str().c_str(), L"trim", MB_OK);

	}

	//reiterate to sort??
	std::sort(rowval.begin(), rowval.end());
	std::sort(colval.begin(), colval.end());

	for (it1 = src_array.begin(); it1<src_array.end(); ++it1)
	{

		std::string xval( it1->at(colidx) );	//rowval
		std::string yval( it1->at(colidy) );	//colval

		int xrowid = exists(rowval, xval);
		int xcolid = exists(colval, yval);
		if (xrowid!=DAE_NEXIST && xcolid!=DAE_NEXIST) {
			//MessageBox(0,xval.c_str(), L"trim", MB_OK);
			value_array[xrowid][xcolid]++;
		}
	}

}

int daextab::row_tot(int i)
{

	int rtot = std::accumulate(value_array[i].begin(), value_array[i].end(), 0);
	return rtot;

}
int daextab::gr_tot()
{
	int grtot=0;
	intvec2d::iterator it1;

	for (it1 = value_array.begin(); it1<value_array.end(); ++it1)
	{
		grtot = grtot + std::accumulate(it1->begin(), it1->end(), 0);
	}

	return grtot;
	

}
float daextab::row_ratio(int i, int colnum, int coldenom)
{
	if (i<0 || colnum<0 || coldenom<0)
		return OVERFLOW;

	if (i>value_array.size()) 
		return OVERFLOW;
	
	intvec2d::iterator it1;

	for (it1 = value_array.begin(); it1<value_array.end(); ++it1) {
		if (colnum > it1->size() || coldenom > it1->size() )
			return OVERFLOW;
	}

	float r_val = (float) value_array[i][colnum] / (float) value_array[i][coldenom];
	//std::stringstream ss;
	//ss<<L"num: " <<value_array[i][colnum]<< L", denom: "<<value_array[i][coldenom]<<L", ratio: "<<r_val;
	//MessageBox(0, ss.str().c_str(), L"row ratio", MB_OK);

	return r_val;

}

float daextab::col_ratio(int j, int rownum, int rowdenom)
{
	if (j<0 || rownum<0 || rowdenom<0)
		return OVERFLOW;

	if (j>value_array.size()) 
		return OVERFLOW;
	
	intvec2d::iterator it1;

	for (it1 = value_array.begin(); it1<value_array.end(); ++it1) {
		if (rownum > it1->size() || rownum > it1->size() )
			return OVERFLOW;
	}

	float r_val = (float) value_array[rownum][j] / (float) value_array[rowdenom][j];

	return r_val;
}

int daextab::col_tot(int j)
{
	int ctot = 0;

	intvec2d::iterator it1;

	for (it1 = value_array.begin(); it1<value_array.end(); ++it1)
	{

		ctot = ctot + it1->at(j);
	}

	return ctot;
}


int daextab::at(int i, int j)
{
	return value_array[i][j];
}
int daextab::n_x()
{
	return nrows;
}
int daextab::n_y()
{
	return ncols;
}

std::string daextab::srow(int i)
{
	return rowval[i];
}

std::string daextab::scol(int j)
{
	return colval[j];
}



daefdxt::daefdxt(stlstrvec2d& src_array, int colidx, int colidy, unsigned int xcl_size, 
		int x_min, int x_upper)
{
	ncols = 0;
	nrows = 0;

	value_array.clear();
	
	colval.clear();
	rowfence.clear();
	collabs.clear();
	rowlabs.clear();

	tabulate(src_array, colidx, colidy, xcl_size, x_min, x_upper);

}

daefdxt::~daefdxt()
{
}
int daefdxt::value_in(intvec2d fence_array, int value)
{
	int id_found = -1;

	for (int i=0; i<fence_array.size(); i++) {
		if (fence_array[i].size()<2)
			return id_found;

		if (fence_array[i][0]<=value && value<fence_array[i][1]) {
			id_found = i;
			return id_found;
		}
	}
	
	return id_found;


}
void daefdxt::tabulate(stlstrvec2d& src_array, int colidx, int colidy, unsigned int xcl_size, 
		int x_min, int x_upper)
{
	int last_lbound = x_upper;	//last lower boundary (>=) 

	int nclass = 1 + (x_upper-x_min)/xcl_size;

	if ((x_upper-x_min)%xcl_size>0) {
		last_lbound = x_min+xcl_size*(nclass-1);
	}
	for (int i=0; i<nclass; i++) {
		//std::stringstream rowbuff;
		daestring rowbuff;

		intvec1d currfence;
		currfence.push_back(x_min+(i*xcl_size));

		if (i<nclass-1) {
			rowbuff.format("%d to less than %d", x_min+(i*xcl_size), x_min+((i+1)*xcl_size));
			//rowbuff<< x_min+(i*xcl_size) << _MBCS(" to less than ") << x_min+((i+1)*xcl_size) ;
			currfence.push_back(x_min+((i+1)*xcl_size));
		}
		else {
			rowbuff.format("%d  and above", x_min+(i*xcl_size));
			//rowbuff<< x_min+(i*xcl_size) << _MBCS(" and above");
			currfence.push_back(2147483647);
		}
			
		rowfence.push_back(currfence);

		//string s(rowbuff.str());
		rowlabs.push_back(rowbuff);
		//MessageBox(0,s, L"diag", MB_OK);
	}

	stlstrvec2d::iterator it1;
	nrows = nclass;		//size of row values
	value_array.resize(nrows);

	for (it1 = src_array.begin(); it1<src_array.end(); ++it1)
	{
		ncols = colval.size();	//size of column values

		int xval = atoi( it1->at(colidx).c_str() );	//rowval
		int yval = atoi( it1->at(colidy).c_str() );	//colval

		// new category
		if( exists(colval, yval)==DAE_NEXIST ){
			colval.push_back(yval);	//put new category

			intvec2d::iterator it2;	//iterator for the value_array
			
			for (it2 = value_array.begin(); it2<value_array.end(); ++it2) {
				it2->push_back(0);	//augment for the non-existing column
			}

		}
		
	}

	//reiterate to sort??
	std::sort(colval.begin(), colval.end());

	for (int j=0; j<colval.size(); j++) {
		std::stringstream labbuff;
		labbuff<<colval[j];

		string s(labbuff.str());
		collabs.push_back(s);	//pass column labels
	}

	for (it1 = src_array.begin(); it1<src_array.end(); ++it1)
	{

		int xval = atoi( it1->at(colidx).c_str() );	//rowval
		int yval = atoi( it1->at(colidy).c_str() );	//colval

		int xrowid = value_in(rowfence, xval);
		int xcolid = exists(colval, yval);
		if (xrowid!=DAE_NEXIST && xcolid!=DAE_NEXIST) {
		
			value_array[xrowid][xcolid]++;

			//std::stringstream msg;
			//msg<<value_array[xrowid][xcolid];

			//MessageBox(0,msg.str().c_str(), L"diag", MB_OK);
		}
	}

	//MessageBox(0,L"ok", L"diag", MB_OK);

}

int daefdxt::at(int i, int j)
{
	return value_array[i][j];
}
int daefdxt::n_x()
{
	return nrows;
}
int daefdxt::n_y()
{
	return ncols;
}

std::string* daefdxt::srow(int i)
{
	return &rowlabs[i];
}

std::string* daefdxt::scol(int j)
{
	return &collabs[j];
}
int daefdxt::row_tot(int i)
{

	int rtot = std::accumulate(value_array[i].begin(), value_array[i].end(), 0);
	return rtot;

}
int daefdxt::gr_tot()
{
	int grtot=0;
	intvec2d::iterator it1;

	for (it1 = value_array.begin(); it1<value_array.end(); ++it1)
	{
		grtot = grtot + std::accumulate(it1->begin(), it1->end(), 0);
	}

	return grtot;
	

}
int daefdxt::col_tot(int j)
{
	int ctot = 0;

	intvec2d::iterator it1;

	for (it1 = value_array.begin(); it1<value_array.end(); ++it1)
	{

		ctot = ctot + it1->at(j);
	}

	return ctot;
}

float daefdxt::row_ratio(int i, int colnum, int coldenom)
{
	if (i<0 || colnum<0 || coldenom<0)
		return OVERFLOW;

	if (i>value_array.size()) 
		return OVERFLOW;
	
	intvec2d::iterator it1;

	for (it1 = value_array.begin(); it1<value_array.end(); ++it1) {
		if (colnum > it1->size() || coldenom > it1->size() )
			return OVERFLOW;
	}

	float r_val = (float) value_array[i][colnum] / (float) value_array[i][coldenom];

	return r_val;
}

float daefdxt::col_ratio(int j, int rownum, int rowdenom)
{
	if (j<0 || rownum<0 || rowdenom<0)
		return OVERFLOW;

	if (j>value_array.size()) 
		return OVERFLOW;
	
	intvec2d::iterator it1;

	for (it1 = value_array.begin(); it1<value_array.end(); ++it1) {
		if (rownum > it1->size() || rownum > it1->size() )
			return OVERFLOW;
	}

	float r_val = (float) value_array[rownum][j] / (float) value_array[rowdenom][j];

	return r_val;
}
