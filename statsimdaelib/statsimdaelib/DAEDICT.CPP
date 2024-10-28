#include "stdafx.h"
#include "daedict.h"
#include "daestring.h"
#include "daeconv.h"

//temporar?
#include <sqlext.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

daedict::daedict(varvec2d* pArray)
{
	dctArray = (*pArray);
	matsize = pArray->size();
}

daedict::daedict(daestream *pdictStream)
{
	//not yet available
}

daedict::~daedict()
{
	//nothing to destruct, dctArray is passed only
}

LPCSTR daedict::GetVarName(ULONG index)
{

	//_tochar sRet(dctArray[index][0]);
	//return sRet;
	return dctArray[index][0];
	

}

LPCSTR daedict::GetVarType(ULONG index)
{

	//_tochar sRet(dctArray[index][1]);
	//return sRet;
	return dctArray[index][1];

}

LPCSTR daedict::GetVarLen(ULONG index)
{

	//_tochar sRet(dctArray[index][2]);
	//return sRet;
	return dctArray[index][2];

}

LPCSTR daedict::GetVarPrec(ULONG index)
{

	//_tochar sRet(dctArray[index][3]);
	//return sRet;
	return dctArray[index][3];

}
LPCSTR daedict::GetVarUnsigned(ULONG index)
{

	//_tochar sRet(dctArray[index][4]);
	//return sRet;
	return dctArray[index][4];

}

LPCSTR daedict::GetVarZerofill(ULONG index)
{

	//_tochar sRet(dctArray[index][5]);
	//return sRet;
	return dctArray[index][5];

}

LPCSTR** daedict::GetArray()
{
	return dctArray;

}
 
ULONG daedict::nVars()
{

	return matsize;

}

ULONG daedict::GetVarBytes(ULONG index)
{
	LPCSTR strType = GetVarType(index);

	ULONG bsize;
	USHORT varlen = atoi(GetVarLen(index)),
		vardec = atoi(GetVarPrec(index));

	switch(SQLTypeDct( strType )) {
						
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

USHORT daedict::SQLTypeDct(LPCSTR strType)
{

	//_tochar sTemp0(strType);
	daestring sType(strType);
	sType.makelower();
	//_tochar sTemp1(sType.c_str());

	if (sType == _ANSI("bit")) {
		return SQL_BIT;
	}

	else if (sType == _ANSI("tinyint")) {
		return SQL_TINYINT;
	}

	else if (sType == _ANSI("smallint")) {
		return SQL_SMALLINT;
	}

	else if (sType == _ANSI("mediumint")) {
		return SQL_INTEGER;
	}

	else if ( (sType == _ANSI("int")) || (sType == _ANSI("integer")) ) {
		return SQL_INTEGER;
	}
	
	else if (sType == _ANSI("bigint")) {
		return SQL_BIGINT;
	}

	else if (sType == _ANSI("float")) {
		return SQL_FLOAT;
	}

	else if (sType == _ANSI("double")) {
		return SQL_DOUBLE;
	}

	else if (sType == _ANSI("decimal")) {
		return SQL_DECIMAL;
	}

	else if (sType == _ANSI("char")) {
		return SQL_CHAR;
	}

	else if (sType == _ANSI("varchar")) {
		return SQL_VARCHAR;
	}

	else if (sType == _ANSI("tinytext")) {
		return SQL_VARCHAR;
	}

	else if (sType == _ANSI("tinyblob")) {
		return SQL_LONGVARCHAR;
	}

	else if (sType == _ANSI("text")) {
		return SQL_LONGVARCHAR;
	}

	else if (sType == _ANSI("blob")) {
		return SQL_BINARY;
	}

	else if (sType == _ANSI("blob")) {
		return SQL_BINARY;
	}

	else if (sType == _ANSI("mediumtext")) {
		return SQL_VARBINARY;
	}

	else if (sType == _ANSI("mediumblob")) {
		return SQL_VARBINARY;
	}

	else if (sType == _ANSI("longtext")) {
		return SQL_LONGVARBINARY;
	}

	else if (sType == _ANSI("longblob")) {
		return SQL_LONGVARBINARY;
	}

	else {
		return SQL_UNKNOWN_TYPE;
	}

}
