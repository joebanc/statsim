// comext.cpp : main project file.

#include "stdafx.h"

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
using std::string;
using std::cout;
using std::cin;
using std::cerr;
using std::map;
using std::multimap;
using namespace std;

int extloc01()
{

	ofstream output("X:\\Documents\\AIM\\democracy\\comelec data\\data\\2001_local_dat.txt", ofstream::binary);

	multimap<string,int> words;
	map<int,string> lines;
	string str;
	ifstream input("X:\\Documents\\AIM\\democracy\\comelec data\\data\\2001_local.txt");
	if(input.fail())
	{
		cerr<<"\nThe file could not be opened.";
		return -1;
	}

	string reg, prov, mun, ofc, dist, name;
	vector<vector<string>> data;
	int ncol = 6;
	data.resize(ncol);
	
	string search, search0("Republic of the Philippines"), search1("PROVINCE :"), search2("CITY/MUN :"), 
		search3("MAYOR"), search4("VICE-MAYOR"),
		search5("COUNCILOR"), search6(" DISTRICT"), search7("2001");

	//int i=1;
	enum {MAYOR=1, VICE, COUNCILOR};
	enum {REG=0, PROV, MUN, OFC, DIST, NAME};
		//bool newprov = false;
	string oldreg(""), newreg("");
	string oldprov(""), newprov("");
	string oldmun(""), newmun("");
	string oldofc(""), newofc("");
	string olddist(""), newdist("");
	stringstream namebuff;
	bool procyr = false, 
		getprov = false, 
		getregn = false,
		skipnext = false;
	
	while(getline(input,str))
	{
		bool newpage = false;
		int nspace = 0;

		for (int n = 0; n<str.length(); n++)
		{
			if(str.substr(n, 1).compare(" ")==0)
				nspace++;
		}

		if (getregn) {
			newreg = str;
			if(oldreg.compare(newreg)!=0)
				oldreg=newreg;

			cout<<str;
			//cin>>newreg;
			getregn = false;
			continue;
		}
		else
		{
		if (getprov) {
			newprov = str;
			if(oldprov.compare(newprov)!=0) //not equal
				oldprov = newprov;
			

			getprov = false;
			continue;
		}
		else{
		//cout<<str<<"\t"<<nspace<<"\n";
		if (skipnext==true && str.find("Manila")!=string::npos) {
			skipnext=false;
			continue;
		}
		else{

		if(str.find(search0)!=string::npos)
		{

			newpage = true;
			continue;
		}
		else
		{
			newpage = false;

										
		if(str.find("CERTIFIED LIST OF ELECTED CITY/MUNICIPAL CANDIDATES")!=string::npos)
		{

			getprov = true;
			continue;
		}
		else
		{
			if(str.find("COMMISSION ON ELECTIONS")!=string::npos)
			{
				skipnext = true;
				continue;
			}
			else
			{
			if(str.find(search1)!=string::npos) //search "PROVINCE :"
			{
				if(str.length()>search1.length()+1) {
					newprov = str;
					if(oldprov.compare(newprov)!=0) //not equal
						oldprov = newprov;
					
					getprov = false;
					continue;
				}
				else {
					getregn = true;
					continue;
				}
			}
			else
			{
			if(str.find("REGION :")!=string::npos ) //search "REGION :"
			{
				if(str.length()>9) {
					newreg = str;
					if(oldreg.compare(newreg)!=0)
						oldreg=newreg;
					
					//cin>>newreg;
					getregn = false;
				}
				
				continue;
			}
			else
			{
				if(str.find(search2)!=string::npos) //search "CITY / MUN :"
				{
					newmun = str;
					
					if(oldmun.compare(newmun)!=0) //not equal
					{
						oldmun = newmun;
					}
					continue;
				}
				else
				{
					if(str.find(search3)!=string::npos) //equals "MAYOR"
					{
						newofc = str;
						olddist = "";
						newdist = "";
						
						if(oldofc.compare(newofc)!=0) //not equal
						{
							oldofc = newofc;
						}
						continue;
					}
					else
					{
						if(str.find(search4)!=string::npos) //search "VICE"
						{
							newofc = str;
							olddist = "";
							newdist = "";

							if(oldofc.compare(newofc)!=0) //not equal
							{
								oldofc = newofc;

							}
							continue;
						}
						else
						{
							if(str.find(search5)!=string::npos) //search "COUNCILOR"
							{
								newofc = str;
							
								if(oldofc.compare(newofc)!=0) //not equal
								{
									oldofc = newofc;
							}
								continue;
							}
							else
							{
								
								if(str.find(search6)!=string::npos && str.find("NCR ")==string::npos) //search "DISTRICT:"
								{
									newdist = str;
									if(olddist.compare(newdist)!=0) //not equal
									{
										olddist = newdist;
									}
									cout<<str;
									continue;
								}
								else
								{
									if(str.find("May 14, 2001 National and Local Elections")!=string::npos ||
										str.find("Records and Statistics Division, ERSD")!=string::npos ||
										str.find("Manila")!=string::npos ||
										str.find("OFFICE/ ")!=string::npos ||
										str.find("DISTRICT ")!=string::npos || 
										str.find("Page ")!=string::npos || 
										str.find("NAME ")!=string::npos ||
										str.find("NICKNAME AFFILIATION")!=string::npos ||
										str.find("PARTY ")!=string::npos ||
										str.find("VOTES ")!=string::npos ||
										str.find("OBTAINED")!=string::npos ||
										str.find("DATE OF ")!=string::npos ||
										str.find("PROCLAMATION ")!=string::npos ||
										str.find("Proclamation ")!=string::npos ||
										str.find("REMARKS ")!=string::npos )
										{
										continue;
									}
									else
									{

										if( atoi(str.substr(0,1).c_str())>0 && oldreg.find("NCR")!=string::npos )













										{
											str = str.erase(0,1);
										}
										//string substring(str.substr(lastspc, str.length()-lastspc);
											//if(substring.rfind(",")!=npos)
											//	substring.replace(substring.rfind(","));

										int lastspc = str.find_last_of(" ");
										
										//(atoi(str.substr(str.length()-2, 2).c_str())>=50  && str.length()>1 );
										if( (str.find(",2004")!=string::npos ||str.find(",2003")!=string::npos ||str.find("May 17,200")!=string::npos || str.find(",2001")!=string::npos || str.find(",2002")!=string::npos || str.find(", 2001")!=string::npos || str.find(",2005")!=string::npos || str.find("No date of ")!=string::npos
											|| str.find("- DO - ")!=string::npos || str.find("- do -")!=string::npos || 
											str.find("SPC NO.")!=string::npos || str.find(", 2005")!=string::npos ||str.find("2007")!=string::npos ||str.find("(Manifest Error)")!=string::npos || str.find(",2001")!=string::npos) 
											|| (str.find("INDEPENDENT")!=string::npos && oldmun.find("PASIL")!=string::npos && oldprov.find("KALINGA")!=string::npos)) 
							 			{
											procyr = true;

											namebuff<<str.c_str()<<" ";
											data[REG].push_back(oldreg);

											data[PROV].push_back(oldprov);
											data[MUN].push_back(oldmun);
											data[OFC].push_back(oldofc);
											data[DIST].push_back(olddist);
											data[NAME].push_back(namebuff.str());

											namebuff.str("");
										}
										else if( nspace>=1 )
										{
											//string substring(str.substr(lastspc, str.length()-lastspc);
											//if(substring.rfind(",")!=npos)
											//	substring.replace(substring.rfind(","));
												
											if( atoi(str.substr(lastspc, str.length()-lastspc).c_str())>=1 )
											
											{
												//cout<<str<<" "<<atoi(str.substr(lastspc, str.length()-lastspc+3).c_str())<<"\n";
												procyr = true;
												namebuff<<str.c_str()<<" ";

												data[REG].push_back(oldreg);
												data[PROV].push_back(oldprov);
												data[MUN].push_back(oldmun);
												data[OFC].push_back(oldofc);
												data[DIST].push_back(olddist);
												data[NAME].push_back(namebuff.str());
												
												namebuff.str("");
											}
											else
											{
												namebuff<<str.c_str()<<" ";
												continue;										
											}

										}

										else if (nspace ==0)
										{
											if( atoi(str.c_str())>=1 )
											{
												procyr = true;
												
												namebuff<<str.c_str()<<" ";
												data[REG].push_back(oldreg);
												data[PROV].push_back(oldprov);
												data[MUN].push_back(oldmun);
												data[OFC].push_back(oldofc);
												data[DIST].push_back(olddist);
												data[NAME].push_back(namebuff.str());
												
												namebuff.str("");
											}
											else
											{
												namebuff<<str.c_str()<<" ";
												continue;										
											}
										}
														
										//if(atoi(str.substr(str.length()-3, 3).c_str())>=250 )
										else
										{
											procyr = false;

											namebuff<<str.c_str()<<" ";
											continue;
										}
										//cout<<oldprov<<"\n";
										//cout<<oldmun<<"\n";
										//cout<<oldofc<<"\n";
										//cout<<str<<"\n";
										
									}
								}
							}	
						}
					}
				}
			}
			}
			}}}}}}
			//cout<<str<<"\n";
			//continue;
			//break;
			//cin>>search3;
		/*
		multimap<string,int>::iterator it1=str.lower_bound(search1);
		if(it1->)
			cout<<search1;

		istringstream in(str);
		while(in>>s)
		{
			words.insert(make_pair(s,i));
		}
		lines.insert(make_pair(i,str));
		*/
		//i++;

	}
	
	//cout<<"\nEnter a word to search: ";
	for (int i=0; i<data[NAME].size(); i++)
	{
		for (int j=REG; j<=NAME; j++)
		{
			stringstream buffer;
			if(j==NAME)
				buffer<<data[j][i].c_str()<<"\n";
			else
				buffer<<data[j][i].c_str()<<"\t";
				
			//cout<<buffer.str();
			output.write(buffer.str().c_str(), buffer.str().length() );
			//cout<<"\nEnter a word to search: ";
			//cin>>search;
			//continue;
		}
	}

	cout<<"\nEnter a word to search: ";
/*	cin>>search;
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
*/	return 0;
}