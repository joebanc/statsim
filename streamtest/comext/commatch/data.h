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

//typedef std::vector<T> varvecp1d;
//typedef vector<varvecp1d> varvecp2d;

class __declspec(dllexport) daevariant 
{
public:
	daevariant(void);
	~daevariant(void);

	// various constructor flavours
	daevariant(const int intVal);
	daevariant(const long longVal);
	daevariant(const float floatVal);
	daevariant(wstring strVal);
	daevariant(string strVal);

	// various assignment operator flavours
	daevariant& operator=(const int i_val);
	daevariant& operator=(const long l_val);
	daevariant& operator=(const float f_val);
	daevariant& operator=(wstring ws_val);
	daevariant& operator=(string s_val);

	// various returns
	operator int();
	operator long();
	operator float();
	operator wstring();
	operator string();

private:
	int int_val;
	long long_val;
	float float_val;
	wstring wstr_val;
	string str_val;
};

template<class T> class daegenvecp : private std::vector<daevariant*>
{
public:
	virtual void resize(size_t _newsize);
	virtual void resize(size_t _newsize, T _newval);
	virtual void push_back(T& _newval);

};

/*
template<class T> class daegenmatp : private std::vector<daegenvecp>
{
public:
	virtual void resize(size_t _newsize);
	virtual void resize(size_t _newsize, T _newval);
	virtual void push_back(daegenvecp& vec);

};
*/