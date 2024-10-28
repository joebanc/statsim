// StatSimAPI.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "StatSimAPI.h"
#include "daeconv.h"
#ifndef DAESTRING
	#include "daestring.h"
#endif
#ifndef DAEDICT
	#include "daedict.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//constant questionnaire version - temporarily set
int QNR_VER = 10200704;

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CStatSimAPIApp

BEGIN_MESSAGE_MAP(CStatSimAPIApp, CWinApp)
END_MESSAGE_MAP()


// CStatSimAPIApp construction

CStatSimAPIApp::CStatSimAPIApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CStatSimAPIApp object

CStatSimAPIApp theApp;


// CStatSimAPIApp initialization

BOOL CStatSimAPIApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

/*
LPCSTR ConstChar(CString buffer)
{

	int size = (buffer.GetLength() + 1);
	LPTSTR tempstr = new TCHAR[size];
	//_tcscpy_s(tempstr, size, buffer);
	_tcscpy(tempstr, buffer);

	return tempstr;

}
*/
std::string sql_itm_val(LPCSTR cVal, daedict **pDict, USHORT type, 
								  int col, LPCSTR strEnclose,
								  BOOL bsReplace, BOOL eReplace)
{
	daestring itemValue;
	daestring itemType, sFind, sReplace,
		sEnclose(strEnclose);
	UINT intValue;
	USHORT shortValue;
	float floatValue;

	if ( sEnclose.trim().length()>0 ) {
		sReplace= _ANSI("\\") + sEnclose.left(1);
		sFind = sEnclose.left(1);
	}

	//_tochar sTemp(pDict[type]->GetVarType(col));
	itemType = pDict[type]->GetVarType(col);

	itemValue.assign(cVal);
	//MessageBox(0, itemValue.c_str(), L"wala", MB_OK);
	//itemValue.erase(itemValue.find_last_not_of(_ANSI(" "))+1);
	//itemValue.erase(0, itemValue.find_first_not_of(_ANSI(" ")));
	itemValue.trim();
	//MessageBox(0, itemValue.c_str(), L"wala", MB_OK);

	//MessageBox(0, itemValue.c_str(), L"wala", MB_OK);
	//check for null values		
	if (itemValue.length()==0) {
		itemValue.assign(_ANSI("NULL"));
	}


	else {
		
		if(itemType.compare(_ANSI("tinyint"))==0) {
			shortValue = atoi(cVal);;
			itemValue.format(_ANSI("%d"), shortValue);
		}
		
		else if(itemType.compare(_ANSI("smallint"))==0) {
			shortValue = atoi(cVal);;
			itemValue.format(_ANSI("%d"), shortValue);
		}

		else if(itemType.compare(_ANSI("int"))==0) {
			intValue = atoi(cVal);;
			itemValue.format(_ANSI("%d"), intValue);
		}
		
		else if(itemType.compare(_ANSI("mediumint"))==0) {
			intValue = atoi(cVal);;
			itemValue.format(_ANSI("%d"), intValue);
		}

		else if(itemType.compare(_ANSI("float"))==0) {
			
			floatValue = (float) atof(cVal);;
			itemValue.format(_ANSI("%f"), floatValue);
		}
		
		else if(itemType.compare(_ANSI("varchar"))==0) {
			
			//itemValue.Replace("'", _ANSI("")); //supressed due to errors
			if (bsReplace)
				itemValue.replace( _ANSI("\\"), _ANSI("\\\\") );
			
			if (eReplace)
				itemValue.replace(sFind, sReplace);

			itemValue = sEnclose.left(1) + itemValue + sEnclose.right(1);
		}
		
		else {
			//itemValue.Replace("'", _ANSI("")); //supressed due to errors
			if (bsReplace)
				itemValue.replace( _ANSI("\\"), _ANSI("\\\\") );
			
			if (eReplace)
				itemValue.replace(sFind, sReplace);

			itemValue = sEnclose.left(1) + itemValue + sEnclose.right(1);
		}
	}
	//MessageBox(0,itemValue->c_str(), L"wala", MB_OK);
	return itemValue;			

}

int ExtCh(std::wstring srcBuff, std::string& destbuff, UINT start, UINT length)
{

	if (!srcBuff.length())
		return 0;

	if(srcBuff.length()<start)
		return 0;

	if(srcBuff.length()-start<length)
		return 0;

	wstring wBuff = srcBuff.substr(start,length);

	//CString msg;
	//msg.Format(_T("pos: %d, len: %d, text: %s"), start, length, (CString) wBuff.c_str());
	//AfxMessageBox(msg);

	std::string sRet(wBuff.begin(),wBuff.end());
	daestring sTemp = sRet;

	//daestring sTemp = srcBuff.substr(start,length);
	sTemp.trim();
	destbuff.swap(sTemp);
	
	return 1;

}

LPCSTR ConstChar(CString buffer)
{

	int size = (buffer.GetLength() + 1);	
	LPSTR tempstr = new char[size];
	
	WideCharToMultiByte(CP_ACP, 0, buffer, -1, tempstr, size, 0, NULL);	
	//wcstombs(tempstr, buffer, size);

	return tempstr;

}

LPCSTR** vector_to_array(std::vector<LPCSTR> *sItems, int ndim, BOOL sort, LPCSTR sortby)
{

	int nJ = ndim,
		nI = sItems[0].size();

	LPCSTR** sArray = new LPCSTR*[nI];

	for (int i=0; i<nI; i++) {
		sArray[i] = new LPCSTR[nJ];
		for (int j=0; j<nJ; j++) {
			sArray[i][j] = sItems[j][i];
		}
	}

	return sArray;

}
std::vector<LPCSTR>* array_to_vector(LPCSTR **sItems, 
											   int nCols, long nRows, BOOL, LPCSTR sortby)
{
	if (!sItems)
		return NULL;

	std::vector<LPCSTR> *sArray = new std::vector<LPCSTR>[nRows];

	for (long i=0; i<nRows; i++) {
		for (int j=0; j<nCols; j++) {
			sArray[i].push_back(sItems[j][i]);
		}
	}

	return sArray;

}
CString MFCStr(LPCSTR buffer)
{

	CString retStr(buffer);
	return retStr;

}

CString GetString(int nId)
{
   CString s;
   s.LoadString(nId);
   return s;
}

int GetItemIndex(CComboBox *pCB, DWORD k)
{
	DWORD id;

	for (int i=0; i<pCB->GetCount(); i++) {
		id = pCB->GetItemData(i);
		if (id==k)
			return i;
	}

	return CB_ERR;


}

int GetArrayIndex(int *intArray, int k, int nSize)
{
	int id;

	for (int i=0; i<nSize; i++) {
		id = intArray[i];
		if (id==k)
			return i;
	}

	return -1;


}

BOOL IsNull(CString sTest)
{

	sTest.TrimLeft();
	sTest.TrimRight();

	if (sTest.GetLength()<=0)
		return TRUE;
	else
		return FALSE;


}

BOOL NoSpace(CString sTest)
{
	CString sTestR = sTest;

	sTest.TrimLeft();
	sTest.TrimRight();
	sTestR.Replace( _T(" "), _T("") );

	if (sTest.GetLength()!=sTestR.GetLength())
		return FALSE;
	else
		return TRUE;


}

int* SortArray(int* intArray, int nSize, BOOL asc)
{
    std::vector <int> intv;
	//std::vector <int>::iterator iterv;

	int* retArray;
	retArray = (int*) malloc( nSize*sizeof(int) );
	
	int i;
	for ( i = 0 ; i < nSize ; i++ )
	{
		intv.push_back( intArray[i] );
	}
	
	// To sort in descending order. specify binary predicate
	if (asc) {
		std::sort( intv.begin(), intv.end() );
	}
	else {
		std::sort( intv.begin(), intv.end(), std::greater<int>() );
	}
	
	for ( i = 0 ; i < nSize ; i++ ) 
	{
		retArray[i] = intv[i];
	}

	return retArray;
	
}

CRect ResizeWindow(int nControl, CDialog *pDlg, float m)
{
   CRect rectW, rectC;   
         
   pDlg->GetWindowRect(rectW);
   pDlg->GetDlgItem(nControl)->GetWindowRect(rectC);   
   
   int nSpace = pDlg->GetDC()->GetTextExtent( _T("ABC") ).cy;	//get the size of ABC

   pDlg->SetWindowPos(NULL,rectW.left,rectW.left,
                rectW.Width(), rectC.bottom - rectW.top + (int) m*nSpace, 
                SWP_NOZORDER);

   pDlg->CenterWindow();

   return rectW;

}

CString GetString(CEdit* pEdit)
{

	int lcount = pEdit->GetLineCount(),
		editLen, getLineRes;

	CString sText = _T(""), sBuff;
	//char *buff;

	for (int i=0; i<lcount; i++) {

		if (lcount==1) {

			pEdit->GetWindowText(sBuff);

		}

		else {
			
			editLen = pEdit->LineLength( pEdit->LineIndex(i) );	
			getLineRes = pEdit->GetLine(i, sBuff.GetBuffer(editLen), editLen);
			sBuff.ReleaseBuffer(editLen);
			
		}
		
		if (i==0)
			sText.Format(TEXT("%s"), sBuff);
		else 
			sText.Format(TEXT("%s %s"), sText, sBuff);

		//free(buff);

	}

	sText.TrimLeft(); sText.TrimRight(); 
	return sText;

}

CString GetString(CComboBox* pCB)
{
	CString sText;

	int iSel = pCB->GetCurSel();
	
	if (iSel!=CB_ERR)
		pCB->GetLBText(iSel, sText);
	else
		sText = "";

	return sText;

}

CString NormStr(LPCSTR sStr)
{

	CString sCritStr;
	sCritStr.Format(_T("%s"), (CString) sStr);

	sCritStr.TrimLeft();
	sCritStr.TrimRight();
	sCritStr.MakeLower();

	return sCritStr;

}

HINSTANCE Execute(LPCSTR sExePath, HWND hWnd, LPCSTR sOperation, 
							LPCSTR sParam, LPCSTR sDir, int nShowCmd)
{

	HINSTANCE exe = ShellExecute(hWnd, (CString) sOperation, (CString) sExePath,
							(CString) sParam, (CString) sDir, nShowCmd);

	return exe;

/*

	switch(exe) {

	case NULL:

		AfxMessageBox(_T(“The operating system is out of memory or resources.”));

		break;

	case ERROR_FILE_NOT_FOUND:

		AfxMessageBox(_T(“The specified file was not found.”));

		break;

	case ERROR_PATH_NOT_FOUND:

		AfxMessageBox(_T(“The specified path was not found.”));

		break;

	case ERROR_BAD_FORMAT:

		AfxMessageBox(_T(“The .exe file is invalid (non-Microsoft Win32 .exe or error in .exe image).”));

		break;

	case SE_ERR_ACCESSDENIED:

		AfxMessageBox(_T(“The operating system denied access to the specified file.”));

		break;

	case SE_ERR_ASSOCINCOMPLETE:

		AfxMessageBox(_T(“The file name association is incomplete or invalid.”));

		break;

	case SE_ERR_DDEBUSY:

		AfxMessageBox(_T(“The Dynamic Data Exchange (DDE) transaction could not be completed because other DDE transactions were being processed.”));
		 
		break;

	case SE_ERR_DDEFAIL:

		AfxMessageBox(_T(“The DDE transaction failed.”));

		break;

	case SE_ERR_DDETIMEOUT:

		AfxMessageBox(_T(“The DDE transaction could not be completed because the request timed out.”));

		break;

	case SE_ERR_DLLNOTFOUND:

		AfxMessageBox(_T(“The specified dynamic-link library (DLL) was not found.”));

		break;

	case SE_ERR_FNF:

		AfxMessageBox(_T(“The specified file was not found.”));

		break;

	case SE_ERR_NOASSOC:

		AfxMessageBox(_T(“There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable.”));

		break;
  
	case SE_ERR_OOM:

		AfxMessageBox(_T(“There was not enough memory to complete the operation.”));

		break;

	case SE_ERR_PNF:

		AfxMessageBox(_T(“The specified path was not found.”));

		break;

	case SE_ERR_SHARE:

		AfxMessageBox(_T(“A sharing violation occurred.”));

		break;

	default:

		AfxMessageBox(_T(_T(“An unknown error occured"));

		break;
	
	}

*/
/*	USES_CONVERSION;
	PROCESS_INFORMATION proc = {0};
	STARTUPINFO start = {0};
	
	// Initialize the STARTUPINFO structure:
	
	start.cb = sizeof (start);      
	start.wShowWindow = SW_SHOWNORMAL;
	
	// pass first
	LPTSTR sExec = (LPTSTR) sExePath;
	// Start the shelled application:
	
	int exe = CreateProcess(0, sExec, 0, 0, TRUE,
		NORMAL_PRIORITY_CLASS, 0, 0, &start, &proc); 
    
	return exe;
*/
}

CString ZeroFill(CString sNum, int nLen, BOOL left)
{
	CString sReturn = NormStr( ConstChar(sNum) );
	
	int lDiff, i;
	
	lDiff = nLen - sReturn.GetLength();

	if (lDiff < 0) {
		AfxMessageBox(_T("Length of string longer than the supposed number of characters."));
		return _T("");
	}

	for (i=0; i<lDiff; i++) {
		if (left)
			sReturn = _T("0") + sReturn;
		else 
			sReturn = sReturn + _T("0");

	}

	return sReturn;

}

CString Mulchar(LPCSTR ch, short x)
{

	CString sCh(ch), cCh(ch);		//return and current
	for (short i=0; i<x-1; i++)
		sCh += cCh;

	return sCh;


}

CString WithChar(LPCSTR ch, short x)
{

	CString sCh(ch);
	if (x>0)
		return sCh;
	else
		return _T("");


}

BOOL Exists(LPCSTR* sArray, LPCSTR sCrit, int nItems)
{

	BOOL found = FALSE;

	for (int i=0; i<nItems; i++) {
		if ( strcmp(sArray[i], sCrit)==IDENTICAL ) {
			found = TRUE;
			return found;
		}
	}
	
	return found;

}
BOOL Exists(std::vector<LPCSTR>& sArray, LPCSTR sCrit, int nItems)
{
	BOOL found = FALSE;

	std::vector<LPCSTR>::iterator it1;

	for (it1=sArray.begin(); it1<sArray.end(); ++it1) {
		if ( strcmp( *it1, sCrit)==IDENTICAL ) {
		//if ( (*it1).compare(sCrit)==IDENTICAL ) {
			found = TRUE;
			return found;
		}
	}
	
	return found;

}
BOOL Exists(CStringArray& sArray, LPCSTR sCrit, int nItems)
{
	BOOL found = FALSE;

	for (int i=0; i< sArray.GetSize(); i++)
	{
		if ( strcmp(ConstChar(sArray[i]), sCrit)==IDENTICAL ) {
			found = TRUE;
			return found;
		}
	}
	
	return found;

}

BOOL Exists(long* iArray, long iCrit, int nItems)
{

	BOOL found = FALSE;

	for (int i=0; i<nItems; i++) {
		if ( iArray[i]==iCrit ) {
			found = TRUE;
			return found;
		}
	}
	
	return found;

}

std::vector<LPCSTR> unique_array(LPCSTR* sArray, int size)
{

	std::vector<LPCSTR> tv, uv;
	int i;

	for (i=0; i<size; i++) {
		tv.push_back(sArray[i]);
	}

	// sort first the array
	std::sort( tv.begin(), tv.end() );

	//start with second element to compare
	uv.push_back(tv[0]);
	for (i=1; i<size; i++) {
		if ( strcmp(tv[i], tv[i-1])!=IDENTICAL )
			uv.push_back(tv[i]);
	}

	return uv;

}
