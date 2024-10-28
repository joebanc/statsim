// commatch.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "func.h"
#include "data.h"

//using namespace System;

#include<map>
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
using std::vector;
using std::istringstream;
using std::ifstream;
using std::wstring;
using std::cout;
using std::cin;
using std::cerr;
using std::map;
using std::multimap;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hConsole;
	//int k;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

/*	SetConsoleTitle(L"Panget");

	SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE);
	string cmd;

	cout << "> ";
	cin >> cmd;
	cout << "> ";
	cin >> cmd;

	// you can loop k higher to see more color choices
	for(k = 1; k < 255; k++)
	{
		// pick the colorattribute k you want
		SetConsoleTextAttribute(hConsole, k);
		cout << k << " I want to be nice today!" << endl;
	}
	cin.get();
	cout << ">";
	cin >> cmd;


	return 0;
*/	// out_of_range
	wofstream output(getexepath().append(L"\\data\\locongov_07-10_matchpar_dat.csv"), ofstream::binary);
	
	multimap<wstring,int> words;
	map<int,wstring> lines;
	wstring path1 = getexepath().append(L"\\data\\locongov_07-10_matchpar.txt"),	//current
		path2 = getexepath().append(L"\\data\\locongov_07-10_matchpar.txt"),	//past
		str;
	wifstream input1(path1);
	wifstream input2(path2);
	if(input1.fail() )
	{
		cerr<<"\nThe file "<<path1.c_str()<<" could not be opened.";
		return -1;
	}
	if(input2.fail() )
	{
		cerr<<"\nThe file "<<path2.c_str()<<" could not be opened.";
		return -1;
	}

	wstring reg, prov, mun, ofc, dist, name;
	wpstrvec2d data1;
	wpstrvec2d data2;
	int ncol1 = 9, ncol2 = 8;

	try {

		buildarray(input1, data1, 5);
		wcout<<"Finished array 1"<<"\n";
		transpose(data1);
		wcout<<"Finished transpose 1"<<"\n";

		intvec1d info_cols; info_cols.push_back(1); info_cols.push_back(2);
		det_party(data1, info_cols);
		wcout<<"Parties determined"<<"\n";
		write(output, data1, L",");

/*		buildarray(input1, data1, ncol1);
		wcout<<"Finished array 1"<<"\n";
		buildarray(input2, data2, ncol2);
		wcout<<"Finished array 2"<<"\n";
		

		transpose(data1);
		wcout<<"Finished transpose 1"<<"\n";
		transpose(data2);
		wcout<<"Finished transpose 2"<<"\n";
		
		bool _strict, dest_dta1 = true, dest_dta2 = true;
		cout<<"try to match same name? (0/1)\t";
		cin>>_strict;

		wpstrvec2d data_m;
		int col1[2], col2[2];
		col1[0]=col2[0]=7;
		col1[1]=col2[1]=8;
		//match(data_m, data1, data2, _strict);

		//get into writing immendiately and destroy the array
		match(data_m, data1, data2, col1, col2, 6, _strict);//, &output);
		//match_and_destroy(data_m, data1, data2, col1, col2, IDX, &output, _strict);
		
		//string s;
		//wcout<<data_m[0][0];
		//cin>>s;

		write(output, data_m, L",");
*/
	}
	catch ( exception &e ) {
		cerr << "Caught: " << e.what( ) << endl;
		cerr << "Type: " << typeid( e ).name( ) << endl;
		int input;
		cin >>input;
	}


	//cout<<"\nEnter a word to search: ";
	//cin>>str;

	return 0;
}



