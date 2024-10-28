// comext.cpp : main project file.

#include "stdafx.h"

//using namespace System;

#include<map>
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>

#include<algorithm>

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

std::string& trimright(std::string& str, const char* pszDelimiters =" ", int off = 0)
{
	//this->erase(this->find_last_not_of(_TXT(" "))+1);
		//return *this;

		string::size_type idx = string::npos;
		if (!off)
			idx = str.find_last_not_of(pszDelimiters);
		else
			idx = str.find_last_not_of(pszDelimiters, off);

		if (idx != string::npos)
		{
			str.erase(idx+1);
		}
		else {
			str.erase();
		}

		//MessageBox(0,this->c_str(), L"trim", MB_OK);

		return str;
	}

//returns the position of the next character after the party
string::size_type party_exists(string str)
{
	string ustr = str;
	std::vector<string> str_party;
	str_party.push_back(" pmp");
	str_party.push_back(" np");
	str_party.push_back(" lks-kam");
	str_party.push_back(" ind.");
	str_party.push_back(" lp");
	str_party.push_back(" pdp laban");
	str_party.push_back(" pdpsp");
	str_party.push_back(" op");

	transform(ustr.begin(),ustr.end(),ustr.begin(),tolower);
	std::vector<string>::iterator it1;

	string::size_type ret = string::npos;

	for (it1=str_party.begin(); it1<str_party.end(); ++it1)
	{
		ret = ustr.find(*it1);

		//cout << ustr << " " << *it1 << "\n";
		if(ret!=string::npos) {
			//cout<< *it1 << "\n";
			return ret + it1->length() + 1;
		}
	}
	return ret;
}
string::size_type mm_exists(string str)
{
	string ustr = str;
	std::vector<string> str_mm;
	str_mm.push_back("january");
	str_mm.push_back("february");
	str_mm.push_back("march");
	str_mm.push_back("april");
	str_mm.push_back("may");
	str_mm.push_back("june");
	str_mm.push_back("july");
	str_mm.push_back("august");
	str_mm.push_back("september");
	str_mm.push_back("october");
	str_mm.push_back("november");
	str_mm.push_back("december");

	transform(ustr.begin(),ustr.end(),ustr.begin(),tolower);

	std::vector<string>::iterator it1;

	string::size_type ret = string::npos;
	for (it1=str_mm.begin(); it1<str_mm.end(); ++it1)
	{
		ret = ustr.find(*it1);
		if(ret!=string::npos)
			return ret;
	}
	return ret;
	
}

string::size_type replace(string& str, const string& from, const string& to)
{
	string::size_type cnt(string::npos);
	
	if(from != to && !from.empty()) {
		string::size_type pos1(0);
		string::size_type pos2(0);
		const string::size_type from_len(from.size());
		const string::size_type to_len(to.size());
		cnt = 0;
		
		while((pos1 = str.find(from, pos2)) != string::npos) {
			str.replace(pos1, from_len, to);
			pos2 = pos1 + to_len;
			++cnt;
		}
	}
	return cnt;
}

int extcongov10()
{

	ofstream output("X:\\Documents\\AIM\\democracy\\comelec data\\data\\2010_cong_cand_dat.txt", ofstream::binary);

	multimap<string,int> words;
	map<int,string> lines;
	string str;
	ifstream input("X:\\Documents\\AIM\\democracy\\comelec data\\data\\2010_cong_cand.txt");
	if(input.fail())
	{
		cerr<<"\nThe file could not be opened.";
		return -1;
	}

	string reg, prov, mun, ofc, dist, name;
	vector<vector<string>> data;
	int ncol = 7;
	data.resize(ncol);
	
	string search, search0("August 18, 2011 2:15:46PM"), search1("PROVINCE : "), search2("CITY OF"), 
		search3("MEMBER, HOUSE OF REPRESENTATIVE"), search4("PROVINCIAL GOVERNOR"),
		search5("PROVINCIAL VICE-GOVERNOR"), search6("PROVINCIAL BOARD MEMBER"), search7("2010");

	//int i=1;
	enum {REP=1, GOV, VGOV, BOKAL};
	enum {REG=0, PROV, MUN, OFC, DIST, NAME, WIN};
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
		skipnext = false,
		pushed = false;
	
	while(getline(input,str))
	{
		bool newpage = false;
		int nspace = 0;

		for (int n = 0; n<str.length(); n++)
		{
			if(str.substr(n, 1).compare(" ")==0)
				nspace++;
		}
		//cout<<str<<"\t"<<nspace<<"\n";

		if(str.find(search0)!=string::npos)
		{

			newpage = true;
			continue;
		}
		else
		{
			newpage = false;

			if(str.find("REGION : ")!=string::npos)
			{
				getregn = true;
				continue;
			}
			else
			{
			if(str.find("PROVINCE : ")!=string::npos)
			{
				getprov = true;
				continue;
			}
			else
			{
			if(getprov==true && getregn==true)
			{
				newreg = str;
				if(oldreg.compare(newreg)!=0)
				{
					oldreg=newreg;
				}
				getregn=false;
				continue;
			}
			else
			{
			if(getprov==true && getregn==false)
			{
				newprov = str;

				if(oldprov.compare(newprov)!=0) //not equal
				{
					oldprov = newprov;
				}
				getprov=false;
				continue;
			}
			else
			{
				if(str.find(search2)!=string::npos || str.find(" CITY")!=string::npos) //search "CITY / MUN :"
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
					if(str.find(search3)!=string::npos) //equals "REP"
					{
						newofc = str;
						//cout<<str<<"\n";
						
						if(oldreg.find("NCR")==string::npos)
							oldmun = "";
						
						if(oldofc.compare(newofc)!=0) //not equal
						{
							oldofc = newofc;
						}
						continue;
					}
					else
					{
						if(str.find(search4)!=string::npos) //search "GOV"
						{
							newofc = str;
							olddist = "";
							newdist = "";
							oldmun = "";

							if(oldofc.compare(newofc)!=0) //not equal
							{
								oldofc = newofc;

							}
							continue;
						}
						else
						{
							if(str.find(search5)!=string::npos) //search "VGOV"
							{
								newofc = str;
								olddist = "";
								newdist = "";
							
								oldmun = "";

								if(oldofc.compare(newofc)!=0) //not equal
								{
									oldofc = newofc;
							}
								continue;
							}
							else
							{
								
								if(str.find(search6)!=string::npos ) //search "BOKAL"
								{
									
								newofc = str;
							
								oldmun = "";

								if(oldofc.compare(newofc)!=0) //not equal
								{
									oldofc = newofc;
								}
								continue;
								}
							else
							{
								
								if(str.find(" DISTRICT")!=string::npos ) //search "DISTRICT"
								{
									newdist = str;
									if(olddist.compare(newdist)!=0) //not equal
									{
										olddist = newdist;
									}
													continue;
								}
								else
								{
									if(str.find("May 10, 2010 Automated National and Local Elections")!=string::npos ||
										str.find("Records and Statistics Division, ERSD")!=string::npos ||
										str.find("CERTIFIED LIST OF CANDIDATES FOR CONGRESSIONAL AND PROVINCIAL POSITIONS")!=string::npos ||
										str.find("Republic of the Philippines")!=string::npos ||
										str.find("COMMISSION ON ELECTIONS")!=string::npos ||
										str.find("Records and Statistics Division, ERSD")!=string::npos ||
										str.find("Intramuros, Manila")!=string::npos ||
										str.find("REGION")!=string::npos ||
										str.find("OFFICE/DISTRICT/")!=string::npos ||
										str.find("NAME OF CANDIDATE ALIAS")!=string::npos ||
										str.find("NICKNAME")!=string::npos ||
										str.find("PARTY ")!=string::npos ||
										str.find("AFFILIATION REMARKS")!=string::npos ||
										str.find("PROCLAMATION ")!=string::npos ||
										str.find("VOTES ")!=string::npos ||
										str.find("OBTAINED ")!=string::npos ||
										str.find("DATE OF")!=string::npos )
									{
										continue;
									}
									else
									{

										if( atoi(str.substr(0,str.find_first_of(" ") ).c_str())>0 )//&& oldreg.find("NCR")!=string::npos )
										{
											str = str.erase(0,str.find_first_of(" "));
										}
										//string substring(str.substr(lastspc, str.length()-lastspc);
											//if(substring.rfind(",")!=npos)
											//	substring.replace(substring.rfind(","));

										int lastspc = trimright(str, " ").find_last_of(" ");
										int lastcomma = str.find_last_of(",");

										//cout<<lastspc;
										//cout<<lastcomma;

										//cout<<lastcomma;
										//cin>>lastspc;
										
										//(atoi(str.substr(str.length()-2, 2).c_str())>=50  && str.length()>1 );
									 
										if( (str.find(" 2010")!=string::npos || str.find(" 2008")!=string::npos
											|| str.find(" 2011")!=string::npos || str.find(" 2009")!=string::npos) ) 
										{
											procyr = true;

											namebuff<<str.c_str()<<" ";
											data[REG].push_back(oldreg);

											data[PROV].push_back(oldprov);
											data[MUN].push_back(oldmun);
											data[OFC].push_back(oldofc);
											data[DIST].push_back(olddist);
											data[NAME].push_back(namebuff.str());
											
											if (mm_exists(namebuff.str())!=string::npos )
											{ 
												data[WIN].push_back("1");
											}
											else {
												data[WIN].push_back("0");
											}

											pushed = true;
											namebuff.str("");
											continue;
										}

										else if( oldprov.find("LANAO DEL SUR")!=string::npos 
											||oldprov.find("BASILAN")!=string::npos) 
										{
											procyr = true;

											string::size_type partypos = party_exists(str);											
											
											if (partypos !=string::npos) {
												cout<<str <<"\n";
												if( partypos>= str.length() || partypos+1>= str.length()) {
													cout << str <<"\n";
													cout << oldreg <<"\n";
													//cin >> procyr;
											
													namebuff<<str.c_str()<<" ";
													data[REG].push_back(oldreg);
													
													data[PROV].push_back(oldprov);
													data[MUN].push_back(oldmun);
													data[OFC].push_back(oldofc);
													data[DIST].push_back(olddist);
													data[NAME].push_back(namebuff.str());
													data[WIN].push_back("0");
													
													pushed = true;
													namebuff.str("");
													continue;
												}
											}
											//continue;

										}
/*										else if( oldprov.find("BASILAN")!=string::npos 
											&& (str.find(" 0 ")!=string::npos) ) 
										{
											procyr = true;

											namebuff<<str.c_str()<<" ";
											data[REG].push_back(oldreg);

											data[PROV].push_back(oldprov);
											data[MUN].push_back(oldmun);
											data[OFC].push_back(oldofc);
											data[DIST].push_back(olddist);
											data[NAME].push_back(namebuff.str());
											data[WIN].push_back("0");

											namebuff.str("");
											continue;
										}
*/
										else if( nspace>=1 && lastspc!=string::npos)
										{
											string substring(str.substr(lastspc, str.length()-lastspc));
											if(substring.rfind(",")!=string::npos) {
												//string::size_type ret = replace(substring, ",", "");
											}

											//cout<<lastspc<<"\n";
												
											//if( atoi(str.substr(lastspc, str.length()-lastspc).c_str())>=1 )
											//if( (str.length()-lastcomma)<=5) {
											//if( atoi(str.substr(lastcomma+1, str.length()-lastcomma+1).c_str())>=1 )
											if( atoi(substring.c_str() )>=1 )
											
											{
												//int f;
												//cout << substring;
												//cin >> f;

												//cout<<str<<" "<<atoi(str.substr(lastspc, str.length()-lastspc+3).c_str())<<"\n";
												procyr = true;
												namebuff<<str.c_str()<<" ";

												data[REG].push_back(oldreg);
												data[PROV].push_back(oldprov);
												data[MUN].push_back(oldmun);
												data[OFC].push_back(oldofc);
												data[DIST].push_back(olddist);
												data[NAME].push_back(namebuff.str());
												data[WIN].push_back("0");
												
												pushed = true;
												namebuff.str("");
												continue;
											}
											else
											{
												namebuff<<str.c_str()<<" ";
												pushed=false;
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
												data[WIN].push_back("0");
												
												pushed= true;
												namebuff.str("");
												continue;
											}
											else
											{
												pushed = false;
												namebuff<<str.c_str()<<" ";
												continue;										
											}
										}
														
/*										//if(atoi(str.substr(str.length()-3, 3).c_str())>=250 )
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
*/										
									}
								}
							}	
						}
					}
				}
			}
			}
			}}}}
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
	for (int i=0; i<data[WIN].size(); i++)
	{
		for (int j=REG; j<=WIN; j++)
		{
			stringstream buffer;
			if(j==WIN)
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
	//cin>>search;
/*	multimap<string,int>::iterator it1=words.lower_bound(search);
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