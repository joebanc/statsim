#include<map>
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>

#include<algorithm>
#include<windows.h>

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

#define IDENTICAL 0
#define IDX 0

//int *const myint; //constant pointer to int
//int const *myint; //pointer to constant int
//getsomething() const; //read-only functions

typedef std::vector<wstring*> wpstrvec1d;
typedef std::vector<wpstrvec1d> wpstrvec2d;
typedef std::vector<wstring> wstrvec1d;
typedef std::vector<wstrvec1d> wstrvec2d;
typedef std::vector<int> intvec1d;
typedef std::vector<const wchar_t*> lpcwvec1d;
typedef std::vector<lpcwvec1d> lpcwvec2d;

string tostring(wstring& wstr)
{
	string str(wstr.begin(), wstr.end());
	return str;

}
wstring makeupper(wstring& str)	//makes the argument upper case
{ 
	transform(str.begin(),str.end(),str.begin(),toupper); 
	return str; 
}
wstring makelower(wstring& str)	//makes the argument lower case
{ 
	transform(str.begin(),str.end(),str.begin(),tolower); 
	return str; 
}
wstring upper(wstring str)	//returns argument's upper case
{ 
	return makeupper(str);
}
wstring lower(wstring str)	//returns argument's lower case
{ 
	return makelower(str);
}

wstring::size_type replace(wstring& str, const wstring& from, const wstring& to)
{
	wstring::size_type cnt(wstring::npos);
	
	if(from != to && !from.empty()) {
		wstring::size_type pos1(0);
		wstring::size_type pos2(0);
		const wstring::size_type from_len(from.size());
		const wstring::size_type to_len(to.size());
		cnt = 0;
		
		while((pos1 = str.find(from, pos2)) != wstring::npos) {
			str.replace(pos1, from_len, to);
			pos2 = pos1 + to_len;
			++cnt;
		}
	}
	return cnt;
}
void destroy(wpstrvec1d& c) 
{ 
	while(!c.empty()) 
	{
		delete c.back(), c.pop_back(); 
	}
}

void destroy(wpstrvec2d& c) 
{ 


	while(!c.empty()) 
	{
		destroy( c.back() ), c.pop_back(); 
	}

	return;

	wpstrvec2d::iterator it1;
	for (it1 = c.begin(); it1<c.end(); ++it1)
	{
		destroy(*it1);
		c.pop_back();
	}

}

int copy_values(wpstrvec2d& from_data, wpstrvec2d& to_data, int ncol_front = 0)	//ncol_front is number of columns in front to reserve
{

	wpstrvec2d::iterator it1;
	for (it1 = from_data.begin(); it1<from_data.end(); ++it1)
	{
		wpstrvec1d it1_vec;
		wpstrvec1d::iterator it2;

		wstring* p_spush = 0; 

		for (int i=0; i<ncol_front; i++) {
			p_spush = new wstring(L"");
			it1_vec.push_back(p_spush);
		}

		for (it2 = it1->begin(); it2 < it1->end(); ++it2)
		{
			p_spush = new wstring(**it2);	//value pointed by the **it2
			it1_vec.push_back(p_spush);
		}

		to_data.push_back(it1_vec);
	
	}

	return 1;

}

void transpose(wpstrvec2d& data)  //must be standard matrix
{
	wpstrvec2d buff;

	int ncols = data.size(), 
		nrows = data[0].size();

	buff.resize(nrows);
	
	for (int i = 0; i<nrows; i++)
	{
		
		buff[i].resize(ncols);

		for (int j = 0; j<ncols; j++)
		{
			buff[i][j] = data[j][i];
		}
	}

	//pass to data
	data.clear();
	data.resize(nrows);
	for (int i = 0; i<nrows; i++)
	{
		data[i].resize(ncols);
		for (int j = 0; j<ncols; j++)
		{
			data[i][j] = buff[i][j];
		}
	}

}

void writerow(wofstream& output, wpstrvec1d& data, wstring _del = L"\t")
{
	wpstrvec1d::iterator it2;

	for (it2 = data.begin(); it2 < data.end(); ++it2)
	{
		wstring delim, str;

		if(it2==data.begin())
			delim = L"";
		else
			delim = _del;

		str = delim.append(**it2);
		output.write(str.c_str(), str.length() );
		//cout<<str.c_str()<<"\n";
	}
	output.write(L"\n", 1 );
}


void write(wofstream& output, wpstrvec2d& data, wstring _del = L"\t")
{
	wpstrvec2d::iterator it1;
	for (it1 = data.begin(); it1< data.end(); ++it1)
	{
		writerow(output, *it1, _del);
	}

	cout<<"Finished write";

}



void buildarray(wifstream& input, wpstrvec2d& data, int _ncol, bool w_id = false, wstring _del = L"\t\n")
{
	int ncol = _ncol;
	if(w_id)
		ncol = _ncol+1;	// +1 for index

	data.resize(ncol); 
	wstring str;

	long j = 0;
	while(getline(input,str))
	{

		int i0 = 0;
		wstring* p_spush = 0;
		if(w_id){
			i0=1;

			wstringstream ibuff;
			ibuff<<j+1;
			
			p_spush = new wstring( ibuff.str() );
			data[0].push_back(p_spush);
			//wcout<<*p_spush<<"\n";
		}

		int count = 0, off = 0; 
		
		for (int i=i0; i<ncol; i++) {
			wstring delim;
			//cout<<str<<"\n";
			if( i==ncol-1) {
				delim = L"\n";
			}
			else {
				delim = _del;
			}
			//delim = _del;
			count = str.find_first_of(delim, off) - off;

			//if(i==0)
			//	off=wstring::npos;
			//else if(i==1)
			//	off=0;

			p_spush = new wstring( str.substr(off, count) );
			data[i].push_back( p_spush );

			//cout<<"offset: "<< off << " count: "<<count <<"\n";
			//cout<<data[i][j]<<delim;


			off = off + count+1;


		}
		//cin>>j;
		j++;
	}
}


wstring getexefilepath()
{
	wchar_t result[ MAX_PATH ];
	return wstring( result, GetModuleFileName( NULL, result, MAX_PATH ) );
}

wstring getexepath()
{
	wstring str = getexefilepath();
	return str.substr(0,str.find_last_of(L"\\"));
}


void match(wpstrvec2d& dta_m, wpstrvec2d& _dta1, wpstrvec2d& _dta2, 
	bool same_itm = true, int _colidx1 = 2, int _colidx2 = 6)
{
	dta_m.clear();
	copy_values(_dta1, dta_m);

	wpstrvec2d::iterator it_m;
	for (it_m = dta_m.begin(); it_m<dta_m.end(); ++it_m)
	{
		wpstrvec2d::iterator it2;
		int nmatches = 0;

		for (it2 = _dta2.begin(); it2<_dta2.end(); ++it2)
		{

			wstring str_m1 = lower( * it_m->at(_colidx1) ), 
				str_m2 = lower( * it_m->at(_colidx1+1)),
				str2 = lower( * it2->at(_colidx2) );

			str_m1.insert(0, L"'").append(L"'");
			str_m2.insert(0, L"'").append(L"'");

			replace(str2, L"\"", L"'");
			replace(str2, L",", L"',");
			replace(str2, L" ", L"' '");

			wstring* p_spush = 0;
			if (!same_itm) {
				if ( str2.find( str_m1 )!=wstring::npos ) {
					nmatches++;	//increment the number of matches
					p_spush = new wstring(*it2->at(_colidx2));
					it_m->push_back(p_spush);
				}
				else {
					p_spush = new wstring(L"");
					it_m->push_back(p_spush);
				}
			}
			else {
				if ( str2.find( str_m1 )!=wstring::npos && str2.find( str_m2 )!=wstring::npos ) {
					nmatches++;	//increment the number of matches

					p_spush = new wstring(*it2->at(_colidx2));
					it_m->push_back(p_spush);

					p_spush = new wstring(*it2->at(_colidx2-2));
					it_m->push_back(p_spush);

					p_spush = new wstring(*it2->at(_colidx2-3));
					it_m->push_back(p_spush);

					p_spush = new wstring(*it2->at(_colidx2-4));
					it_m->push_back(p_spush);

				}
				else {
					p_spush = new wstring(L"");
					it_m->push_back(p_spush);
				}
			}

		}

		wstringstream str;
		str << nmatches;

		wpstrvec1d::iterator it0 = it_m->begin();
		wstring* p_spush = new wstring(str.str());
		it_m->insert(it0, p_spush);

		string s;
		wcout<<*it_m->at(3)<<L"\n";
		//cin>>s;
		cout<<"matched"<<"\n";

	}

}


void match(wpstrvec2d& dta_m, wpstrvec2d& _dta1, wpstrvec2d& _dta2, 
	int* _vcolidx1, int* _vcolidx2, int _colinfo, bool same_itm = true,
	bool dest_dta1 = false, bool dest_dta2 = false, wofstream* output = 0)
{
	dta_m.clear();
	copy_values(_dta1, dta_m);

	if(dest_dta1) {
		destroy(_dta1);
		wcout<<L"Data 1 was destroyed\n";
	}

	wpstrvec2d::iterator it_m;
	for (it_m = dta_m.begin(); it_m<dta_m.end(); ++it_m)
	{
		wpstrvec2d::iterator it2;
		int nmatches = 0;
		for (it2 = _dta2.begin(); it2<_dta2.end(); ++it2)
		{

			wstring str_m1 = lower( *it_m->at(_vcolidx1[0]) ), 
				str_m2 = lower( *it_m->at(_vcolidx1[1])),
				str_2_1 = lower( *it2->at(_vcolidx2[0]) ),
				str_2_2 = lower( *it2->at(_vcolidx2[1]) );


			//str_m1.insert(0, L"'").append(L"'");
			//str_m2.insert(0, L"'").append(L"'");

			//replace(str_2_1, L"\"", L"'");
			//replace(str_2_1, L",", L"',");
			//replace(str_2_1, L" ", L"' '");
			//replace(str_2_2, L"\"", L"'");
			//replace(str_2_2, L",", L"',");
			//replace(str_2_2, L" ", L"' '");

			wstring* p_spush = 0;
			if (!same_itm) {
				if (str_m1.compare(str_2_1) != 0) {
					
					nmatches++;	//increment the number of matches
					
					p_spush = new wstring(*it2->at(_colinfo));
					it_m->push_back(p_spush);
				}
				//else {
				//	p_spush = new wstring(L"");
				//	it_m->push_back(p_spush);
				//}
			}
			else {
				if (str_m1.compare(str_2_1)==IDENTICAL && str_m2.compare(str_2_2)==IDENTICAL ){
	
					nmatches++;	//increment the number of matches
					
					p_spush = new wstring(*it2->at(_colinfo));
					it_m->push_back(p_spush);

					p_spush = new wstring(*it2->at(_colinfo-1));
					it_m->push_back(p_spush);

					p_spush = new wstring(*it2->at(_colinfo-2));
					it_m->push_back(p_spush);

					p_spush = new wstring(*it2->at(_colinfo-3));
					it_m->push_back(p_spush);

					p_spush = new wstring(*it2->at(_colinfo-4));
					it_m->push_back(p_spush);

				}
				//else {
				//	p_spush = new wstring(L"");
				//	it_m->push_back(p_spush);
				//}
			}
			//if(dest_dta2) {
			//	destroy(*it2);
			//	wcout<<L"Data 2 iterator was destroyed\n";
			//}
		}

		wstringstream str;
		str << nmatches;

		wpstrvec1d::iterator it0 = it_m->begin();
		wstring* p_spush = new wstring(str.str());
		it_m->insert(it0, p_spush);

		string s;
		wcout<<*it_m->at(_vcolidx1[0])<<L" "<<*it_m->at(_vcolidx1[1])<<L"\n";
		//cin>>s;
		cout<<"matched"<<"\n";

		if (output) {
			writerow(*output, *it_m, L",");
			destroy(*it_m);
			wcout<<L"Data m iterator was destroyed\n";
		}

	}

}

void match_and_destroy(wpstrvec2d& dta_m, wpstrvec2d& _dta1, wpstrvec2d& _dta2, 
	int* _vcolidx1, int* _vcolidx2, int _colinfo, wofstream* output = 0, bool same_itm = true)
{
	dta_m.clear();
	copy_values(_dta1, dta_m);

	destroy(_dta1);
	wcout<<L"Data 1 was destroyed\n";
	
	while(!dta_m.empty()) {
		
		wpstrvec1d it_m = dta_m.back();

		wpstrvec2d::iterator it2;
		int nmatches = 0;
		for (it2 = _dta2.begin(); it2<_dta2.end(); ++it2)
		{

			wstring str_m1 = lower( *it_m.at(_vcolidx1[0]) ), 
				str_m2 = lower( *it_m.at(_vcolidx1[1])),
				str_2_1 = lower( *it2->at(_vcolidx2[0]) ),
				str_2_2 = lower( *it2->at(_vcolidx2[1]) );


			//str_m1.insert(0, L"'").append(L"'");
			//str_m2.insert(0, L"'").append(L"'");

			//replace(str_2_1, L"\"", L"'");
			//replace(str_2_1, L",", L"',");
			//replace(str_2_1, L" ", L"' '");
			//replace(str_2_2, L"\"", L"'");
			//replace(str_2_2, L",", L"',");
			//replace(str_2_2, L" ", L"' '");

			wstring* p_spush = 0;
			if (!same_itm) {
				if (str_m1.compare(str_2_1) != 0) {
					
					nmatches++;	//increment the number of matches
					
					p_spush = new wstring(*it2->at(_colinfo));
					it_m.push_back(p_spush);
				}
				//else {
				//	p_spush = new wstring(L"");
				//	it_m.push_back(p_spush);
				//}
			}
			else {
				if (str_m1.compare(str_2_1)==IDENTICAL && str_m2.compare(str_2_2)==IDENTICAL ){
	
					nmatches++;	//increment the number of matches
					
					p_spush = new wstring(*it2->at(_colinfo));
					it_m.push_back(p_spush);

				}
				//else {
				//	p_spush = new wstring(L"");
				//	it_m.push_back(p_spush);
				//}
			}
			//if(dest_dta2) {
			//	destroy(*it2);
			//	wcout<<L"Data 2 iterator was destroyed\n";
			//}
		}

		wstringstream str;
		str << nmatches;

		wpstrvec1d::iterator it0 = it_m.begin();
		wstring* p_spush = new wstring(str.str());
		it_m.insert(it0, p_spush);

		string s;
		wcout<< *it_m.at(_vcolidx1[0])<<L" "<<*it_m.at(_vcolidx1[0])<<L"\n";
		//cin>>s;
		cout<<"matched"<<"\n";

		writerow(*output, it_m, L",");
		destroy(it_m);
		wcout<<L"Data m iterator was destroyed\n";
		dta_m.pop_back();
	}

}

string::size_type exists(wstring str, wstring* str_found)
{
	wstring ustr = str;
	std::vector<wstring> str_party;
	str_party.push_back(L" bigkis ");
	str_party.push_back(L" kbl ");
	str_party.push_back(L" kbl");
	str_party.push_back(L" pmp ");
	str_party.push_back(L" np ");
	str_party.push_back(L" np");
	str_party.push_back(L" lks-kam ");
	str_party.push_back(L" lakas-cmd ");
	str_party.push_back(L"lakas-cmd ");
	str_party.push_back(L" lakas-cmd");
	str_party.push_back(L" lakas-cmd/kampi ");
	str_party.push_back(L" lakas-cmd/npc ");
	str_party.push_back(L" sarro/kampi ");
	str_party.push_back(L" sarro/lakas-cmd ");
	str_party.push_back(L" kampi ");
	str_party.push_back(L"kampi ");
	str_party.push_back(L" kampi");
	str_party.push_back(L" ind. ");
	str_party.push_back(L" lp ");
	str_party.push_back(L" lp/kkk ");
	str_party.push_back(L" pdp laban ");
	str_party.push_back(L" pdp-laban ");
	str_party.push_back(L" pdpsp ");
	str_party.push_back(L" pdsp ");
	str_party.push_back(L" op ");
	str_party.push_back(L" aksyon ");

	str_party.push_back(L" pcm ");

	str_party.push_back(L" pmp/npc ");
	str_party.push_back(L" pmp/npc/uno ");
	str_party.push_back(L" ldp ");
	str_party.push_back(L" partido padajon ");

	str_party.push_back(L" navote");
	str_party.push_back(L" npc ");
	str_party.push_back(L" npc");
	str_party.push_back(L"npc ");
	str_party.push_back(L" npc/una ");
	str_party.push_back(L" bakud-npc ");
	str_party.push_back(L" bakud ");

	str_party.push_back(L" uno ");
	str_party.push_back(L"uno ");
	str_party.push_back(L" uno");

	str_party.push_back(L" ppp");

	str_party.push_back(L" independent ");
	str_party.push_back(L" balane ");
	str_party.push_back(L" lm ");
	str_party.push_back(L" kabaca ");
	str_party.push_back(L"b.bayan");

	str_party.push_back(L" sigaw");
	str_party.push_back(L" capiz");
	str_party.push_back(L" atun");
	str_party.push_back(L" pp surigao");

	transform(ustr.begin(),ustr.end(),ustr.begin(),tolower);
	std::vector<wstring>::iterator it1;

	wstring::size_type ret = wstring::npos;

	for (it1=str_party.begin(); it1<str_party.end(); ++it1)
	{
		ret = ustr.find(*it1);

		if(ret!=wstring::npos) {
			//do not initialise pointer here!
			//wcout<<ustr<<L" : "<<*it1<< L"\n";
			//wcin>>ustr;
			*str_found = *it1;
			return ret + it1->length() + 1;
		}
	}
	return ret;
}

void det_party(wpstrvec2d& _dta, intvec1d _vcolidx)
{
	wpstrvec2d::iterator it1;
	for (it1=_dta.begin(); it1<_dta.end(); ++it1) {
		wstring::size_type f;
		wstring* p_spush = 0;

		p_spush = new wstring(L"");
		f = exists( *it1->at(_vcolidx[0]),  p_spush);
		it1->push_back(p_spush);
		
		p_spush = new wstring(L"");
		f = exists( *it1->at(_vcolidx[1]),  p_spush) ;
		it1->push_back(p_spush);

	}

}
string::size_type replace_s(string& str, const string& from, const string& to)
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

int extdat(vector<vector<string>> &data, char delim = '\t')
{

	ifstream input(getexepath().append(L"\\data\\transfers.txt"));
	if(input.fail())
	{
		cerr<<"\nThe file could not be opened.";
		return -1;
	}

	multimap<string,int> words;
	map<int,string> lines;

	string line;
	while (getline(input, line, '\n')) {
		vector<string> vline;
		int tabpos=0;
		for (int i=0; i<line.length(); i++) {
			if (line.at(i)=='\t') {
				string str;
				str=line.substr(tabpos,i-tabpos+1);
				replace_s(str, "\t", "");
				vline.push_back(str);
				tabpos = i+1;
				//cout <<str <<"\n";
			}
			if (i==line.length()-1) {
				string str;
				str=line.substr(tabpos,i-tabpos+1);
				vline.push_back(str);
				tabpos = i+1;
				//cout <<str <<"\n";
			}
		

		}
		data.push_back(vline);
	}

}
		//while ((str.find_first_of("\t"))!=string::npos) {
		//	vline.push_back(str.substr(0,tabpos));
		//	str = str.substr(tabpos, str.length());
		//	cout<<str;
		//}

/*
	std::fstream fin(getexepath().append(L"\\data\\transfers.txt"));
	while(fin){
   
		std::string data2;
		char sink = '\0';

		// skip to first tab
		fin.ignore(1024,'\t');

		fin >> data2;
		// do stuff with data2

		stringstream d(data2.c_str());
		cout << data2 << "\n";

		// skip to next line
		fin.ignore(1024,'\n');
	}

*/


