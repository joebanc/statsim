#include "StdAfx.h"
#include ".\daevector.h"

daevariant::daevariant(void) {
	data.type = UNKOWN;
	data.iVal = 0;
}

daevariant::~daevariant(void) {
	if(STRING == data.type){
		//delete data.sVal;
	}
 }

// various constructor flavours
daevariant::daevariant(const int& intVal) { 
	data.type = INT;
	data.iVal = intVal;
}
daevariant::daevariant(const long& longVal) { 
	data.type = LONG;
	data.lVal = longVal;
}
daevariant::daevariant(const float& floatVal) { 
	data.type = FLOAT;
	data.fVal = floatVal;
}
daevariant::daevariant(const LPCSTR strVal) { 
	data.type = STRING;
	
	data.sVal = new std::string(strVal);
}
daevariant::daevariant(const std::string& strVal) { 
	data.type = STRING;
	
	data.sVal = new std::string(strVal);

/*	_ASSERTE(strVal);
    int nLen = strlen(strVal) + 1;
    data.sVal = new CHAR [nLen];
    strcpy_s(data.sVal, nLen, strVal);
*/

}
/*
daevariant::daevariant(const daevariant& dVal) { 
	iVal = dVal.iVal;
	lVal = dVal.lVal;
	fVal = dVal.fVal;

	//_ASSERTE(dVal.sVal);
    int nLen = strlen(dVal.sVal) + 1;
    sVal = new CHAR [nLen];
	strcpy_s(sVal, nLen, dVal.sVal);
}
*/
daevariant::daevariant(daevariant const& other)
{
	*this = other;// redirect to the copy assignment
}

// various assignment operator flavours
daevariant& daevariant::operator=(const int& intVal) { 
	if(STRING == data.type){
		//delete data.sVal;
	}
	data.type = INT;
	data.iVal = intVal;
	return (*this); 
}
daevariant& daevariant::operator=(const long& longVal)	{ 
	if(STRING == data.type){
		//delete data.sVal;
	}
	data.type = LONG;
	data.lVal = longVal;
	return (*this); 
}

daevariant& daevariant::operator=(const float& floatVal)	{ 
	if(STRING == data.type){
		//delete data.sVal;
	}
	data.type = FLOAT;
	data.fVal = floatVal;
	return (*this); 
}
daevariant& daevariant::operator=(const LPCSTR strVal)	{ 
	if(STRING == data.type){
		delete data.sVal;
	}
	data.type = STRING;
	
	data.sVal= new std::string(strVal);
	return (*this); 

}
daevariant& daevariant::operator=(const std::string& strVal)	{ 
	if(STRING == data.type){
		delete data.sVal;
	}
	data.type = STRING;
	
	data.sVal= new std::string(strVal);

/*	_ASSERTE(strVal);
    int nLen = strlen(strVal) + 1;
    data.sVal = new CHAR [nLen];
    strcpy_s(data.sVal, nLen, strVal);
*/	
	return (*this); 
}
/*
daevariant& daevariant::operator=(const daevariant& dVal)	{ 
	iVal = dVal.iVal;
	lVal = dVal.lVal;
	fVal = dVal.fVal;
	sVal = 0;
	//copy the value
	//_ASSERTE(dVal.sVal);
    int nLen = strlen(dVal.sVal) + 1;
    char* sVal = new CHAR[nLen];
	strcpy_s(sVal, nLen, dVal.sVal);
	//delete [] sVal;
	//sVal= buff;
	
	return (*this); 
}
*/
daevariant& daevariant::operator=(const daevariant& other)	{ 
	if(this != &other)
	{
		if(STRING == data.type)
		{
			//delete data.sVal;
		}
		switch(other.data.type)
		{
		case STRING:
			{
			data.type = STRING;
			
			data.sVal = new std::string(*(other.data.sVal));
			data.type = STRING;
 			break;
			}
		default:
			{
			memcpy(this, &other, sizeof(daevariant));
			break;
			}           
		}
	}
	return *this;
}

// various returns
daevariant::operator int() {
	if(INT == data.type)
	{
		return data.iVal;
	}
	
}

daevariant::operator long() { 
	if(LONG == data.type)
	{
		return data.lVal;
	}
}

daevariant::operator float() { 
	if(FLOAT == data.type)
	{
		return data.fVal;
	}
}

daevariant::operator std::string() { 
	if(STRING == data.type)
	{
		return *data.sVal;
	}
}

daevariant::operator LPCSTR() { 
	if(STRING == data.type)
	{
		char *strVal = new char[data.sVal->length()+1];
		strcpy_s(strVal, data.sVal->length()+1, data.sVal->c_str());
		return strVal;
	}
}

/*
genvec::genvec()
{
}

genvec::~genvec()
{
}


template<class T> 
statsimvec<T>::statsimvec()
{
}

template<class T> 
statsimvec<T>::~statsimvec()
{
}
*/

int destroyvec( varvec3d values, bool wptr )
{
	int i, j;
	//trim memory allocation

	for (i=0; i<values.size(); i++) {
		for (j=0; j<values[i].size(); j++) {
			
			if (wptr) {
				
				varvec1d::iterator var_k;
				
				for (var_k=values[i][j].begin(); 
					var_k!=values[i][j].end(); 
					++var_k) {
						delete [] *var_k;
				}
			}
			values[i][j].clear();
			varvec1d(values[i][j]).swap(values[i][j]);
		}
		values[i].clear();
		//varvec2d(values[i]).swap(values[i]);
	}
	values.clear();
	varvec3d(values).swap(values);

	return 1;

}

int destroyvec( varpvec3d values, bool wptr )
{
	int i, j;
	//trim memory allocation
	//varvec1d::iterator var_k;

	for (i=0; i<values.size(); i++) {
		for (j=0; j<values[i].size(); j++) {
			
			if (wptr) {
				
				varpvec1d::iterator var_k;
				
				for (var_k=values[i][j].begin(); 
					var_k!=values[i][j].end(); 
					++var_k) {
						delete [] *var_k;
				}
			}
			values[i][j].clear();
			//varvec1d(values[i][j]).swap(values[i][j]);
		}
		values[i].clear();
		//varvec2d(values[i]).swap(values[i]);
	}
	values.clear();
	//varvec3d(values).swap(values);

	return 1;

}

int destroyvec( varvec2d values, bool wptr )
{
	int i;
	//trim memory allocation
	for (i=0; i<values.size(); i++) {

		if (wptr) {

			varvec1d::iterator var_j;

			for (var_j=values.at(i).begin(); 
				var_j!=values.at(i).end(); 
				++var_j) {
					delete [] *var_j;
			}
		}
		
		values.at(i).clear();
		varvec1d(values.at(i)).swap(values.at(i));
	}

	values.clear();
	varvec2d(values).swap(values);

	return 1;

}


int destroyvec( varvec1d values, bool wptr )
{
	if (wptr) {
		varvec1d::iterator var_i;

		for (var_i=values.begin(); 
			var_i!=values.end(); 
			++var_i) {
				delete [] *var_i;
		}
	}
	
	values.clear();
	varvec1d(values).swap(values);

	return 1;

}



varvec2d::varvec2d(void)	
{
	m_pStrArray = 0;
	m_pIntArray = 0;
	m_pLongArray = 0;
	m_pFloatArray = 0;
}
varvec2d::~varvec2d(void)	
{
	//error?
	return;

	long i,j;
		
	if (m_pStrArray) {
		for(i=0; i<this->size(); i++) {
			if (m_pStrArray[i]) {
				delete [] m_pStrArray[i];
				m_pStrArray[i]= 0;
			}
		}
		delete [] m_pStrArray;
		m_pStrArray=0;
	}

	if (m_pIntArray) {
		for(i=0; i<this->size(); i++) {
			if (m_pIntArray[i]) {
				delete [] m_pIntArray[i];
				m_pIntArray[i]= 0;
			}
		}
		delete [] m_pIntArray;
		m_pIntArray=0;
	}

	if (m_pLongArray) {
		for(i=0; i<this->size(); i++) {
			if (m_pLongArray[i]) {
				delete [] m_pLongArray[i];
				m_pLongArray[i]= 0;
			}
		}
		delete [] m_pLongArray;
		m_pLongArray=0;
	}
	
	if (m_pFloatArray) {
		for(i=0; i<this->size(); i++) {

			if (m_pFloatArray[i]) {
				delete [] m_pFloatArray[i];
				m_pFloatArray[i]= 0;
			}
		}
		delete [] m_pFloatArray;
		m_pFloatArray=0;
	}
	
}
// return operators
varvec2d::operator LPCSTR**()  
{
	long i,j;
	if (this->size()) {
		m_pStrArray = new LPCSTR*[this->size()];

		for (i=0; i<this->size(); i++) {
			
			if ( this->at(i).size() ) { 
				m_pStrArray[i] = new LPCSTR[this->at(i).size()];

				for (j=0; j<this->at(i).size(); j++) {
					m_pStrArray[i][j] = this->at(i).at(j);
				}
				
			}
			
		}
		
	}
		
	
	return m_pStrArray;

}

tstrvec1d::tstrvec1d()
{

	m_pArray = 0;
	
}
tstrvec1d::~tstrvec1d()	
{
	if(m_pArray) {
		delete [] m_pArray;
		m_pArray = 0;
	}
	
	//do this?

	/*
	while(!this->empty()) {
		
		delete this->back();
		this->pop_back();
	}
	*/

	
}

// various constructor flavours

// various assignment operator flavours

// return operators
tstrvec1d::operator LPCSTR*()  
{
	if (this->size()) {
		m_pArray = &(*this->begin());
		/*
		m_pArray = new std::string*[this->size()];
		for (int i=0; i<this->size(); i++) {
			m_pArray[i] = *this[i];
		}
		*/
	}
	
	return m_pArray;
}
