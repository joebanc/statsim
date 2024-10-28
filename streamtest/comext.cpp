// comext.cpp : main project file.

#include "stdafx.h"

using namespace System;

#include<map>
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
using std::vector;
using std::istringstream;
using std::ifstream;
using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::map;
using std::multimap;
int main()
{
	multimap<string,int> words;
	map<int,string> lines;
	string str;
	ifstream input("X:\\Documents\\AIM\\democracy\\comelec data\\data\\2007_local.txt");
	if(input.fail())
	{
		cerr<<"\nThe file could not be opened.";
		return -1;
	}

	string reg, prov, ofc, dist, name;
	vector<vector<string>> data;
	int ncol = 5;
	data.resize(ncol);
	
	int i=1;
	while(getline(input,str))
	{
		istringstream in(str);
		string s;
		while(in>>s)
		{
			words.insert(make_pair(s,i));
		}
		lines.insert(make_pair(i,str));
		i++;
	}
	

	string search;
	cout<<"\nEnter a word to search: ";
	cin>>search;
	multimap<string,int>::iterator it1=words.lower_bound(search);
	multimap<string,int>::iterator it2=words.upper_bound(search);
	while(it1!=it2)
	{
		int x=it1->second;
		map<int,string>::iterator iter=lines.find(x);
		cout<<'\n'<<x<<" ) "<<iter->second<<'\n';
		it1++;
		while(true)
		{
			if(it1!=it2 && it1->second==x)
			{
				it1++;
				
			}
			else
			{
				break;
			}
		}
	}
	cout<<"\nThe number of matches = "<<words.count(search)<<'\n';
	return 0;
}