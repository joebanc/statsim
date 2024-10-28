#include "stdafx.h"
#include "StatSimAPI.h"
#include "StatSimDct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatSimDct::CStatSimDct(CStatSimRS *pdictRS)
{
	dctArray = NULL;
	Construct(pdictRS);
}

CStatSimDct::CStatSimDct(daestream *pdictStream)
{

}

CStatSimDct::~CStatSimDct()
{
	for (int i=0; i<nAttr; i++)
		delete [] dctArray[i];

	delete [] dctArray;

}

void CStatSimDct::Construct(CStatSimRS *pdictRS)
{

	if (pdictRS->GetRecordCount()>0) {
		pdictRS->MoveFirst();
	}

	else {

		AfxMessageBox(_T("Dictionary is empty!"));
		return;
	}

	ULONG i;
	USHORT j;

	matsize = pdictRS->GetRecordCount();
	nAttr = 6;
	
	dctArray = new LPCSTR*[matsize];

	//[var] = [0]
	//[type] = [1]
	//[width] = [2]
	//[dec] = [3]
	//[unsigned] = [4]
	//[zerofill] = [5]


	for (i=0; i<pdictRS->GetRecordCount(); i++) {
		
		dctArray[i] = new LPCSTR[nAttr];

		for (j=0; j<nAttr; j++) {	//currently until zerofill only

			dctArray[i][j] = ConstChar(pdictRS->SQLFldValue(j)) ;
			//AfxMessageBox(dctArray[i][j]);
					
		}

		pdictRS->MoveNext();
	}

}

LPCSTR CStatSimDct::GetVarName(ULONG index)
{

	return dctArray[index][0];
	

}

LPCSTR CStatSimDct::GetVarType(ULONG index)
{

	return dctArray[index][1];

}

LPCSTR CStatSimDct::GetVarLen(ULONG index)
{

	return dctArray[index][2];

}

LPCSTR CStatSimDct::GetVarPrec(ULONG index)
{

	return dctArray[index][3];

}
LPCSTR CStatSimDct::GetVarUnsigned(ULONG index)
{

	return dctArray[index][4];

}

LPCSTR CStatSimDct::GetVarZerofill(ULONG index)
{

	return dctArray[index][5];

}

LPCSTR** CStatSimDct::GetArray()
{
	return dctArray;

}
 
ULONG CStatSimDct::nVars()
{

	return matsize;

}

ULONG CStatSimDct::GetVarBytes(ULONG index)
{
	CString strType(GetVarType(index));
	USES_CONVERSION;

	ULONG bsize;
	USHORT varlen = atoi(GetVarLen(index)),
		vardec = atoi(GetVarPrec(index));

	switch(SQLTypeDct( ConstChar(strType) )) {
						
	case SQL_BIT:					
		bsize = 1;				
		break;				
						
	case SQL_TINYINT:					
		bsize = 1;				
		break;				
						
	case SQL_SMALLINT:					
		bsize = 2;				
		break;				
						
	case SQL_INTEGER:					
		bsize = 4;				
		break;				
											
	case SQL_BIGINT:					
		bsize = 8;				
		break;				
						
	case SQL_FLOAT:					
		bsize = 4;				
		break;				
						
	case SQL_DOUBLE:					
		bsize = 8;				
		break;				

	case SQL_DECIMAL:					
		bsize = varlen + 2;				
		break;									
						
	case SQL_CHAR:					
		bsize = varlen;				
		break;				
						
	case SQL_VARCHAR:					
		bsize = varlen + 1;				
		break;				
						
	case SQL_LONGVARCHAR:					
		bsize = varlen + 2;				
		break;				
						
	case SQL_BINARY:					
		bsize = varlen + 2;				
		break;				
						
	case SQL_VARBINARY:					
		bsize = varlen + 3;				
		break;				
						
	case SQL_LONGVARBINARY:					
		bsize = varlen + 4;				
		break;				
						
	default:					
		bsize = varlen;				
		break;				
						
	}					
	return bsize;					

}

USHORT CStatSimDct::SQLTypeDct(LPCSTR strType)
{

	CString sType(strType);
	sType.MakeLower();

	if (sType == "bit") {
		return SQL_BIT;
	}

	else if (sType == "tinyint") {
		return SQL_TINYINT;
	}

	else if (sType == "smallint") {
		return SQL_SMALLINT;
	}

	else if (sType == "mediumint") {
		return SQL_INTEGER;
	}

	else if ((sType == "int")||(sType == "integer")) {
		return SQL_INTEGER;
	}
	
	else if (sType == "bigint") {
		return SQL_BIGINT;
	}

	else if (sType == "float") {
		return SQL_FLOAT;
	}

	else if (sType == "double") {
		return SQL_DOUBLE;
	}

	else if (sType == "decimal") {
		return SQL_DECIMAL;
	}

	else if (sType == "char") {
		return SQL_CHAR;
	}

	else if (sType == "varchar") {
		return SQL_VARCHAR;
	}

	else if (sType == "tinytext") {
		return SQL_VARCHAR;
	}

	else if (sType == "tinyblob") {
		return SQL_LONGVARCHAR;
	}

	else if (sType == "text") {
		return SQL_LONGVARCHAR;
	}

	else if (sType == "blob") {
		return SQL_BINARY;
	}

	else if (sType == "blob") {
		return SQL_BINARY;
	}

	else if (sType == "mediumtext") {
		return SQL_VARBINARY;
	}

	else if (sType == "mediumblob") {
		return SQL_VARBINARY;
	}

	else if (sType == "longtext") {
		return SQL_LONGVARBINARY;
	}

	else if (sType == "longblob") {
		return SQL_LONGVARBINARY;
	}

	else {
		return SQL_UNKNOWN_TYPE;
	}

}
