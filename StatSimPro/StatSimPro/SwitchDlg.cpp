// SwitchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StatSimPro.h"
#include "SwitchDlg.h"
#include "daeconv.h"
#include "func.h"

#include "unzip.h"
//#include "csv_parser.hpp"
#include "libcsv/csv.h"
//#include "csv_v.h"
//#include "parser/simplecsv.h"
//#include "parser/csvparser.h"

extern CStatSimConn* pGlobalConn;
extern long hpq_id;

// CSwitchDlg dialog
extern CStatSimConn* pGlobalConn;
extern CStatSimRS* pelemRS;
extern CString userName, userPwd, sHost, sPort, sDB;
extern CStatSimRS ***pdictRS;
extern daedict ***pDCT;

extern bool IS_ODBC_INT;

extern CString dbPrefix;

extern CMFCRibbonStatusBarPane *g_wndStatusBarPane;
extern CProgressCtrl  m_ProgStatBar;

extern LPCSTR** qnrtables;
extern int *ntypes, nqnr;
extern int *rtlen;
extern LPCSTR** RTYPES;
extern long *qnrIDs;

extern float USDtoPHP;	//exchange rate
extern BOOL IsDirectProc;	//whether processing is by batch or direct
extern int nProcBatches;
extern vector<BOOL> arrayProcExist;
extern vector<LPCSTR> arrayBatchDB;

extern long hpq_id;
extern BOOL _PAP_,_APP_;
extern BOOL g_askBasicInd;
extern CStringArray sGeoLevels;

using namespace std;

IMPLEMENT_DYNAMIC(CSwitchDlg, CDialog)

struct parsedta {
  long unsigned ncols;
  long unsigned nrows;
  unsigned short ncritcols;
  bool wheader;
  bool sqlheader;
  string* sqlbody;
  string* sqlinit;
  vector<string> cheadvec;
  vector<string> critstr;
  vector<vector<string>> critarr;
};

static int rowbegun;

void cb1 (void *s, size_t i, void *dta) {
	string cval("");
	csv_fwrite0(&cval,s,i, NULL);

	//if (cval.find("Daniel Integrated Farm")!=string::npos)
	//	AfxMessageBox((CString) cval.c_str());

	//col_sql_form((daestring&) cval)
	//Process sql
	bool whead= ((struct parsedta *)dta)->wheader;
	bool sqlhead= ((struct parsedta *)dta)->sqlheader;
	unsigned long n_j = ((struct parsedta *)dta)->ncols,
		n_i = ((struct parsedta *)dta)->nrows;
	unsigned short n_cc = ((struct parsedta *)dta)->ncritcols;
	string* locsqlbody = ((struct parsedta *)dta)->sqlbody;	//local sql pointer, not to be deleted
	string* locsqlinit = ((struct parsedta *)dta)->sqlinit;	//local sql pointer, not to be deleted

	//pass values to critical array of cols up to n_cc
	if (n_j==0)
		((struct parsedta *)dta)->critstr.clear();

	if (n_j<n_cc)
		((struct parsedta *)dta)->critstr.push_back(cval);
	//////////////////////////////////////////////////////

	if (n_i==0) {	//if first row (idx)
		if (whead) { 
			if (n_j<n_cc) 
				((struct parsedta *)dta)->cheadvec.push_back(cval);
			if (sqlhead) {
				if (n_j==0)
					locsqlinit->append("(`").append(cval).append("`");
				else
					locsqlinit->append(",`").append(cval).append("`");
			}
			else 
				return;
			//else {
			//	col_sql_form((daestring&) cval);
			//	if (n_j==0)
			//		locsqlbody->assign("\n VALUES(").append(cval);	//this is a reset
			//	else
			//		locsqlbody->append(",").append(cval);			
			//}
		}
		else {
			col_sql_form((daestring&) cval);
			if (n_j==0) 							
				locsqlbody->assign("\n VALUES(").append(cval);	//this is a reset
			else
				locsqlbody->append(",").append(cval);			
		}
	}
	else {
		col_sql_form((daestring&) cval);
		if (n_j==0) 							
			locsqlbody->assign("\n VALUES(").append(cval);	//this is a reset
		else
			locsqlbody->append(",").append(cval);	
	}

	//increment cols
	((struct parsedta *)dta)->ncols++;
  
}

void cb2 (int c, void *dta) {
	//reset number of columns
	((struct parsedta *)dta)->ncols=0;

	//do nothing but increment rows when 0
	if (((struct parsedta *)dta)->nrows==0) {
		((struct parsedta *)dta)->nrows++;
		return;
	}

	((struct parsedta *)dta)->sqlbody->append(");");
	
	//Execute SQL
	string sql;
	sql.append(*((struct parsedta *)dta)->sqlinit).append(*((struct parsedta *)dta)->sqlbody);
	//AfxMessageBox((CString) sql.c_str());
	if (pGlobalConn->ExecuteSQL((CString) sql.c_str(), false)<0)
		((struct parsedta *)dta)->critarr.push_back(((struct parsedta *)dta)->critstr);

	((struct parsedta *)dta)->nrows++;
}

static int is_space(unsigned char c) {
  if (c == CSV_SPACE || c == CSV_TAB) return 1;
  return 0;
}

static int is_term(unsigned char c) {
  if (c == CSV_CR || c == CSV_LF) return 1;
  return 0;
}



CSwitchDlg::CSwitchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSwitchDlg::IDD, pParent)
{

}

CSwitchDlg::~CSwitchDlg()
{
}

void CSwitchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDOK, m_Ok);
	DDX_Control(pDX, IDC_AUTOSELECTOPT, m_Opt0);
	DDX_Control(pDX, IDC_SPECPATHOPT, m_Opt1);
	DDX_Control(pDX, IDC_SELARCHIVE, m_OptCan);
	DDX_Control(pDX, IDC_COMBOLEVEL, m_ComboQnr);
}


BEGIN_MESSAGE_MAP(CSwitchDlg, CDialog)
	ON_BN_CLICKED(IDC_SELARCHIVE, &CSwitchDlg::OnBnClickedSelcan)
	ON_BN_CLICKED(IDC_AUTOSELECTOPT, &CSwitchDlg::OnBnClickedAutoselectopt)
	ON_BN_CLICKED(IDC_SPECPATHOPT, &CSwitchDlg::OnBnClickedSpecpathopt)
	ON_BN_CLICKED(IDOK, &CSwitchDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBOLEVEL, &CSwitchDlg::OnCbnSelchangeCombolevel)
END_MESSAGE_MAP()


// CSwitchDlg message handlers

void CSwitchDlg::OnBnClickedAutoselectopt()
{
	currSelOpt = AUTOSEL;
	m_Ok.EnableWindow();
}

void CSwitchDlg::OnBnClickedSpecpathopt()
{
	currSelOpt = SPECPATH;
	m_Ok.EnableWindow();
}

void CSwitchDlg::OnBnClickedSelcan()
{
	currSelOpt = SELCAN;
	m_Ok.EnableWindow();
	
	CFileDialog dlg (TRUE,_T("zip"),NULL, OFN_FILEMUSTEXIST,
		_T("CBMS Scan-Portal archive (*.can, *.zip)|*.can;*.zip|")); 
	
	if (dlg.DoModal() == IDOK) {
		m_sCanPath = dlg.GetPathName();
		m_OptCan.SetWindowText(dlg.GetFileName());
	}

}
void CSwitchDlg::OnBnClickedOk()
{

	OnOK();

	//assumes zip/can contains all the data; will empty existing and replace with one in m_sCanPath

	long ID = m_ComboQnr.GetItemData( m_ComboQnr.GetCurSel() );
	if (currSelOpt == AUTOSEL) {
		pImportAutoSelDlg = new CImportAutoSelDlg(ID);
		pImportAutoSelDlg->DoModal();
	
	}
	else if (currSelOpt == SPECPATH) {
		pImportSelPathDlg = new CImportSelPathDlg(ID);
		pImportSelPathDlg->DoModal();
	}
	else {

		CString prompt;
		int msgres;
		prompt.Format(_T("This will replace the loaded CBMS-APP SCAN file in the database.  Do you wish to continue?"));
		msgres = AfxMessageBox (prompt, MB_YESNO);
		if (msgres==IDNO) {
			return;
		}

		//unzip selected file
		HZIP hz = OpenZip(m_sCanPath,0);
		ZIPENTRY ze; GetZipItem(hz,-1,&ze); int numitems=ze.index;
		for (int i=0; i<numitems; i++)	{
			GetZipItem(hz,i,&ze);
			UnzipItem(hz,i,ze.name);
		}
		CloseZip(hz);
		
		CString sSQL, sPathSQL;
		bool ignore=TRUE,	//insert ignore
			wheader=TRUE,	//file has header
			sqlheader=FALSE;  //if sqlheader is true, there must be header, for now

		string delim = ",", enclose = "\"";	//what's escape char? ask CCS!

		int idx;
		for (int i=0; i<nqnr; i++) {
			if (qnrIDs[i]==ID)
				idx=i;
		}

		for (int i=0; i<ntypes[idx]; i++) {//drop tables first

			if (_APP_==TRUE && _PAP_==TRUE) {
				CString sTable = _T("_app_") + (CString) qnrtables[idx][i];
				if (TableExists(sTable, pGlobalConn)) {
					sSQL.Format(_T("DROP TABLE `%s`;"), sTable);
					pGlobalConn->ExecuteSQL(sSQL);
				}
			}
			else {
				if (TableExists((CString) qnrtables[idx][i], pGlobalConn)) {
					sSQL.Format(_T("DROP TABLE `%s`;"), (CString) qnrtables[idx][i]);
					pGlobalConn->ExecuteSQL(sSQL);
				}
			}

		}

		//Create tables but rename to if PAP&APP
		pGlobalConn->CreateTable(pDCT[idx], qnrtables[idx], RTYPES[idx], ntypes[idx], _T(""));
		if (_APP_==TRUE && _PAP_==TRUE) {
			for (int i=0; i<ntypes[idx]; i++) {
				sSQL.Format(_T("ALTER TABLE `%s` RENAME TO `_app_%s`;"),  (CString) qnrtables[idx][i], (CString) qnrtables[idx][i]);
				pGlobalConn->ExecuteSQL(sSQL);
			}
		}


		//Path to STATSIM
		string path;
		path.assign(ConstChar(GetExeFilePath()));

		for (int i=0; i<ntypes[idx]; i++) {		

			CString sTable, sTable0;
			if (_APP_==TRUE && _PAP_==TRUE) {
				sTable.Format(_T("_app_%s"), (CString)qnrtables[idx][i]);
				sTable0.Format(_T("_app_%s"), (CString)qnrtables[idx][0]);
			}
			else {
				sTable.Format(_T("%s"), (CString)qnrtables[idx][i]);
				sTable0.Format(_T("%s"), (CString)qnrtables[idx][0]);
			}
			string mdcfile;
			mdcfile.assign(path);
			if (i==0)
				mdcfile.append("\\main.csv");
			else
				mdcfile.append("\\").append(qnrtables[idx][i]).append(".csv");

			FILE *fp;
			struct csv_parser p;
			unsigned char options = 0;
			struct parsedta *cts = new parsedta;
			cts->ncols = 0; cts->nrows = 0; cts->wheader = true; cts->sqlheader = false; cts->ncritcols = 6; 
			cts->sqlbody = new string(""); cts->sqlinit = new string("");

			if (ignore) {
				cts->sqlinit->assign("INSERT IGNORE INTO `").append(ConstChar(sTable)).append("`");
			}
			else {
				cts->sqlinit->assign("INSERT INTO `").append(ConstChar(sTable)).append("`");
			}
			
			if (csv_init(&p, options) != 0) {
				fprintf(stderr, "Failed to initialize csv parser\n");
				exit(EXIT_FAILURE);
			}

			csv_init(&p, 0);
			//options = CSV_STRICT;
			//csv_set_opts(&p, options);
			//csv_set_space_func(&p, is_space);
			//csv_set_term_func(&p, is_term);
			//csv_set_quote(&p, '"');
			
			int k;
			char c;
			fp = fopen(mdcfile.c_str(), "rb");
			if (!fp) {
				stringstream msg;
				msg << "Failed to open " << mdcfile << ":" << strerror(errno);
				AfxMessageBox((CString) msg.str().c_str());
				continue;
			}
			
			while ((k=getc(fp)) != EOF) {
				c = k;
				if (csv_parse(&p, &c, 1, cb1, cb2, cts) != 1) {
					stringstream msg;
					msg << "Error: " << csv_strerror(csv_error(&p));;
					AfxMessageBox((CString) msg.str().c_str());
				}
			}

			unsigned int k_c = ((struct parsedta *)cts)->critarr.size();

			if (k_c>0) {
				ofstream out;
				string outpath;
				//create directory first
				CreateDirectory(_T("C:\\CBMSDatabase\\System\\Output"), NULL);
				outpath.append("c:\\CBMSDatabase\\system\\output\\").append(qnrtables[idx][i]).append("_err.csv");
				out.open(outpath.c_str(), std::ofstream::out | std::ofstream::app);
				vector<string>::iterator ith;
				for (ith=((struct parsedta *)cts)->cheadvec.begin(); ith<((struct parsedta *)cts)->cheadvec.end(); ith++) {
								
					string _delim, str;
					if(ith==((struct parsedta *)cts)->cheadvec.begin())
						_delim = "";
					else
						_delim = ",";

					str.append(*ith).append(_delim);
					out.write(str.c_str(), str.length() );
					
				}
				out.write("\n",1);
				vector<vector<string>>::iterator it1;
				for (it1 = ((struct parsedta *)cts)->critarr.begin(); it1<((struct parsedta *)cts)->critarr.end(); ++it1)
				{
					vector<string>::iterator it2;
					for (it2 = it1->begin(); it2<it1->end(); ++it2)
					{
						string _delim, str;
						if(it2==it1->begin())
							_delim = "";
						else
							_delim = ",";
						
						str.append(*it2).append(_delim);
						out.write(str.c_str(), str.length() );
					}
					
					out.write("\n",1);

				}

				out.close();
				stringstream msg;
				msg<<"There are some errors loading "<<qnrtables[idx][i]<<".  Please see "<<outpath<<" for details.";
				AfxMessageBox((CString) msg.str().c_str());

			}


			fclose(fp);
			csv_free(&p);
			delete cts->sqlbody;
			delete cts->sqlinit;
			cts->cheadvec.clear();
			cts->critstr.clear();
			cts->critarr.clear();
			delete cts;
			
			if (i!=0) {

				sSQL.Format(_T("ALTER TABLE `%s` ADD COLUMN `urb` INT(1) ZEROFILL UNSIGNED NULL, ADD COLUMN `regn` INT(2) ZEROFILL UNSIGNED NULL, ADD COLUMN `prov` INT(2)  ZEROFILL UNSIGNED NULL, ADD COLUMN `mun` INT(2)  ZEROFILL UNSIGNED NULL, ADD COLUMN `brgy` INT(3)  ZEROFILL UNSIGNED NULL, ADD COLUMN `purok` INT(2)  ZEROFILL UNSIGNED NULL, ADD COLUMN `hcn` INT(6)  ZEROFILL UNSIGNED NULL"), sTable);
				
				if (i==1)
					sSQL.Append(_T(", CHANGE COLUMN `memno` `memno` INT(2) UNSIGNED ZEROFILL NULL DEFAULT NULL;"));
				else
					sSQL.Append(_T(";"));                           

				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("ALTER TABLE `%s` ADD INDEX `IDX` (`hpq_hh_id` ASC, `id` ASC);"), sTable);
				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("UPDATE `%s`, `%s` SET `%s`.`regn`=`%s`.`regn`, `%s`.`prov`=`%s`.`prov`, `%s`.`mun`=`%s`.`mun`, `%s`.`brgy`=`%s`.`brgy`, `%s`.`purok`=`%s`.`purok`, `%s`.`hcn`=`%s`.`hcn` WHERE `%s`.`%s_id`=`%s`.`id`;"), 
					sTable, sTable0,
					sTable, sTable0,
					sTable, sTable0,
					sTable, sTable0,
					sTable, sTable0,
					sTable, sTable0,
					sTable, sTable0,
					sTable, sTable0,
					sTable0);

				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("ALTER TABLE `%s` ADD INDEX `hpqIDX` (`regn` ASC, `prov` ASC, `mun` ASC, `brgy` ASC, `purok` ASC, `hcn` ASC);"), sTable);
				pGlobalConn->ExecuteSQL(sSQL);

				//set urbanity
				sSQL.Format(_T("UPDATE `%s`, `brgy` SET `%s`.`urb`=`brgy`.`urb` WHERE (`%s`.`regn`=`brgy`.`regn` AND `%s`.`prov`=`brgy`.`prov` AND `%s`.`mun`=`brgy`.`mun` AND `%s`.`brgy`=`brgy`.`brgy`);"), 
					sTable, sTable, sTable, sTable, sTable, sTable);

				pGlobalConn->ExecuteSQL(sSQL);

			}
			else { //set urbanity; temporary; must be based on PSGC classification
				
				if ( hpq_id==1020135200 || hpq_id==1020135300 || hpq_id==1020135400 ){ //return the respondent (except USLS and LSU
					sSQL = _T("insert into hpq_mem(`hpq_hh_id`, `id`, `memno`,`msname`,`mfname`,`mmname`,`nucfam`,`reln`,`reln_o`,`sex`,`birth_date`,`age`,`age_yr`,`birth_reg`,`civstat`,`ethgrp`,`ethgrp_o`,`ofw`,`mlenresid`,`country_resid`,`country_resid_o`,`prov_resid_code`,`mun_resid_code`,`brgy_resid_code`,`mun_resid_txt`,`brgy_resid_txt`,`educind`,`gradel`,`sch_type`,`gradel_calc`,`ynotsch`,`ynotsch_o`,`educal`,`psced7`,`course_o`,`literind`,`regvotind`,`voted_last_election`,`jobind`,`entrepind`,`njob`,`occup`,`psoc4`,`indust`,`psic4`,`jstatus`,`work_ddhrs`,`work_wkhrs`,`fadd_work_hrs`,`fxtra_wrk`,`workcl`,`fjob`,`first_fjob`,`jsearch_meth`,`jsearch_meth_o`,`wks_fjob`,`ynotlookjob`,`ynotlookjob_o`,`lastlookjob`,`joppind`,`wtwind`,`wagcshm`,`wagkndm`,`sss_ind`,`pregind`,`solo_parent`,`pwd_ind`,`pwd_type`,`pwd_type_o`,`pwd_id`,`scid_ind`,`mcrimeind`,`mtheftind`,`mrapeind`,`minjurind`,`mcarnapind`,`mcattrustlind`,`mocrimind`,`mocrim`,`mtheftloc`,`mrapeloc`,`minjurloc`,`mcarnaploc`,`mcattrustlloc`,`mocrimloc`) \
							  select `id`, 99, 99, `resp_msname`,`resp_mfname`,`resp_mmname`,`resp_nucfam`,`resp_reln`,`resp_reln_o`,`resp_sex`,`resp_birth_date`,`resp_age`,`resp_age_yr`,`resp_birth_reg`,`resp_civstat`,`resp_ethgrp`,`resp_ethgrp_o`,`resp_ofw`,`resp_mlenresid`,`resp_country_resid`,`resp_country_resid_o`,`resp_prov_resid_code`,`resp_mun_resid_code`,`resp_brgy_resid_code`,`resp_mun_resid_txt`,`resp_brgy_resid_txt`,`resp_educind`,`resp_gradel`,`resp_sch_type`,`resp_gradel_calc`,`resp_ynotsch`,`resp_ynotsch_o`,`resp_educal`,`resp_psced7`,`resp_course_o`,`resp_literind`,`resp_regvotind`,`resp_voted_last_election`,`resp_jobind`,`resp_entrepind`,`resp_njob`,`resp_occup`,`resp_occup_code`,`resp_indust`,`resp_indust_code`,`resp_jstatus`,`resp_work_ddhrs`,`resp_work_wkhrs`,`resp_fadd_work_hrs`,`resp_fxtra_wrk`,`resp_workcl`,`resp_fjob`,`resp_first_fjob`,`resp_jsearch_meth`,`resp_jsearch_meth_o`,`resp_wks_fjob`,`resp_ynotlookjob`,`resp_ynotlookjob_o`,`resp_lastlookjob`,`resp_joppind`,`resp_wtwind`,`resp_wagcshm`,`resp_wagkndm`,`resp_sss_ind`,`resp_pregind`,`resp_solo_parent`,`resp_pwd_ind`,`resp_pwd_type`,`resp_pwd_type_o`,`resp_pwd_id`,`resp_scid_ind`,`resp_mcrimeind`,`resp_mtheftind`,`resp_mrapeind`,`resp_minjurind`,`resp_mcarnapind`,`resp_mcattrustlind`,`resp_mocrimind`,`resp_mocrim`,`resp_mtheftloc`,`resp_mrapeloc`,`resp_minjurloc`,`resp_mcarnaploc`,`resp_mcattrustlloc`,`resp_mocrimloc` \
							  from hpq_hh;");
					pGlobalConn->ExecuteSQL(sSQL);
				}

				sSQL.Format(_T("ALTER TABLE `%s` ADD COLUMN `int_mm` INT(2)  ZEROFILL UNSIGNED NULL, ADD COLUMN `int_dd` INT(2)  ZEROFILL UNSIGNED NULL, ADD COLUMN `int_yy` INT(4)  ZEROFILL UNSIGNED NULL, ADD COLUMN `urb` INT(1)  ZEROFILL UNSIGNED NULL, ADD COLUMN `hsize` INT(3)  ZEROFILL UNSIGNED NULL, CHANGE COLUMN `regn` `regn` INT(2) UNSIGNED ZEROFILL NULL DEFAULT NULL, CHANGE COLUMN `prov` `prov` INT(2) UNSIGNED ZEROFILL NULL DEFAULT NULL, CHANGE COLUMN `mun` `mun` INT(2) UNSIGNED ZEROFILL NULL DEFAULT NULL, CHANGE COLUMN `brgy` `brgy` INT(3) UNSIGNED ZEROFILL NULL DEFAULT NULL, CHANGE COLUMN `purok` `purok` INT(2) UNSIGNED ZEROFILL NULL DEFAULT NULL, CHANGE COLUMN `hcn` `hcn` INT(6) UNSIGNED ZEROFILL NULL DEFAULT NULL;"), sTable);
				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("ALTER TABLE `%s` ADD INDEX `IDX` (`id` ASC);"), sTable);
				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("UPDATE `%s` SET `%s`.`urb`=2, `%s`.`int_mm`=month(int_date), `%s`.`int_dd`=day(int_date), `%s`.`int_yy`=year(int_date), `%s`.`hsize`=`phsize`;"), 
					sTable,
					sTable,
					sTable,
					sTable,
					sTable,
					sTable);
				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("ALTER TABLE `%s` ADD INDEX `hpqIDX` (`regn` ASC, `prov` ASC, `mun` ASC, `brgy` ASC, `purok` ASC, `hcn` ASC);"), sTable);
				pGlobalConn->ExecuteSQL(sSQL);

				//set urbanity
				sSQL.Format(_T("UPDATE `%s`, `brgy` SET `%s`.`urb`=`brgy`.`urb` WHERE (`%s`.`regn`=`brgy`.`regn` AND `%s`.`prov`=`brgy`.`prov` AND `%s`.`mun`=`brgy`.`mun` AND `%s`.`brgy`=`brgy`.`brgy`);"), 
					sTable, sTable, sTable, sTable, sTable, sTable);

				pGlobalConn->ExecuteSQL(sSQL);

			}
		}
		
		CString msg;
		msg.Format(_T("%s has been loaded to the database."), m_sCanPath); 
		AfxMessageBox(msg);
		
		//Clean up
		for (int i=0; i<ntypes[idx]; i++) {		
			string mdcfile;
			mdcfile.assign(path);

			if (i==0)
				mdcfile.append("\\main.csv");
			else
				mdcfile.append("\\").append(qnrtables[idx][i]).append(".csv");
			
			if (_tremove((CString) mdcfile.c_str())!=0)
			{
				msg.Format(_T("Error deleting temporary '%s'"), (CString) mdcfile.c_str());
				AfxMessageBox(msg);
			}

		}

		int msgResult;
		msg.Format(_T("Do you want to start the processing?"));
		msgResult = AfxMessageBox (msg, MB_YESNO);
		if (msgResult==IDYES) {
			ConUpElt(FALSE); //Configure and update
		}	
	}
}

BOOL CSwitchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Ok.EnableWindow(FALSE);
	
	m_OptCan.SetButtonStyle(BS_AUTORADIOBUTTON);
	m_Opt0.SetButtonStyle(BS_AUTORADIOBUTTON);
	m_Opt1.SetButtonStyle(BS_AUTORADIOBUTTON);

	CStatSimRS* pRS = 0;
	pRS = new CStatSimRS(pGlobalConn, (CString) _T("SELECT * FROM `~qnr` ORDER BY `qnrID`;"));

	pRS->PutValuesTo(&m_ComboQnr, _MBCS("label"), _MBCS("qnrID"), _MBCS("description"));

	delete pRS; pRS = 0;

	int iSel = GetItemIndex(&m_ComboQnr, hpq_id);
	m_ComboQnr.SetCurSel(iSel);

	//CString msg; msg.Format(_T("%d"), hpq_id);
	//AfxMessageBox(msg);

	//default is key option
	//m_Opt0.SetCheck(BST_CHECKED);
	//OnBnClickedAutoselectopt();

	OnCbnSelchangeCombolevel();

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSwitchDlg::OnCbnSelchangeCombolevel()
{
	int iSel = m_ComboQnr.GetCurSel();
	DWORD qnr = m_ComboQnr.GetItemData(iSel);

	CString sVer, sVerYr, sPAP;
	int isPAP;
	sVer.Format(_T("%d"), qnr);
	sVerYr = sVer.Mid(2,4);
	//AfxMessageBox(sVerYr);
	isPAP = _ttoi(sVer.Right(1));
	
	//AfxMessageBox(sVerYr);

	if (qnr!=hpq_id) {
		m_Opt0.EnableWindow(FALSE);
		m_Opt0.SetCheck(BST_UNCHECKED);
		//default is key option
		m_Opt1.SetCheck(BST_CHECKED);
		OnBnClickedSpecpathopt();
	}

	else {
		m_Opt0.EnableWindow();
		m_Opt0.SetCheck(BST_CHECKED);
		//default is key option
		m_Opt1.SetCheck(BST_UNCHECKED);
		OnBnClickedAutoselectopt();
	}

	if (_ttoi(sVerYr.Right(4))<=2012 || isPAP) {//disable when earliear than 2012 or PAP

		m_OptCan.SetWindowTextW(_T("Select *.can/*.zip (Not available in this form version)"));
		m_OptCan.EnableWindow(FALSE);
		m_OptCan.ShowWindow(SW_HIDE);		
	}
	else {
		m_OptCan.SetWindowTextW(_T("Select *.can/*.zip"));
		m_OptCan.EnableWindow();
		m_OptCan.ShowWindow(SW_SHOW);
		m_OptCan.SetCheck(BST_CHECKED);
		m_Opt0.EnableWindow(FALSE);
		m_Opt0.SetCheck(BST_UNCHECKED);
		m_Opt1.EnableWindow(FALSE);
		m_Opt1.SetCheck(BST_UNCHECKED);
		OnBnClickedSelcan();
	}

}

std::string getdelimcol(ifstream & in, 
	char delim, char enc, char newline, char defenc, bool sqlform)
{
    daestring col;
	string endlstr = "";
    char prev = 0;
    unsigned quotes = 0;
    bool finis = false;
    for (int ch; !finis && (ch = in.get()) != EOF; ) {
        switch(ch) {
        case '"':
            ++quotes;
            break;
        case ',':
            if (quotes == 0 || (prev == '"' && (quotes & 1) == 0)) {
                finis = true;
            }
            break;
        case '\n':
            if (quotes == 0 || (prev == '"' && (quotes & 1) == 0)) {
                finis = true;
            }
            break;
        default:;
        }
        col += prev = ch;
    }

	if (col.find_last_of(delim)==col.length()-1)
		col.erase(col.length()-1);	//remove delimiter

	if (col.find_last_of(newline)==col.length()-1) {
		col.erase(col.length()-1);	//remove newline
		endlstr = "\n";
	}
	
	size_t quote1=col.find_last_of(enc);
	if (quote1==col.length()-1) {	//find and remove enclose
		col.erase(quote1);
	}
	size_t quote0=col.find_first_of(enc);
	if (quote0==0) {	//find and remove enclose
		col.erase(quote0,1);
	}
	
	if (sqlform)
		col_sql_form(col);

	//put newline character; NB: push_back(ch) does not work here, might be returned by ref?
	col.append(endlstr);

	return col;

}
int col_sql_form(daestring& scol, char defenc)
{
	scol.trim();
	if (!scol.length()) {
		scol.assign("NULL");
		return 1;
	}

	scol.replace((string) "'", (string) "\\'");	//replaces "'" with "\'"
	scol.replace((string) "\"", (string) "\\\"");	//replaces """ with "\""
	scol.insert(0, "'").append("'");

	return 1;
}
std::string get_csv_row(ifstream & in)
{
    std::string col;
    unsigned quotes = 0;
    char prev = 0;
    bool finis = false;
    for (int ch; !finis && (ch = in.get()) != '\n'; ) {
        switch(ch) {
        case '"':
            ++quotes;
            break;
        case ',':
            if (quotes == 0 || (prev == '"' && (quotes & 1) == 0)) {
                finis = true;
            }
            break;
        default:;
        }
        col += prev = ch;
    }
    return col;
}

//to be generalized with delimiter, enclose
std::string get_csv_contents(ifstream & in, char delim = ',', char enc = '"', char newline = '\n', char defenc = '\'')
{
    daestring col;
	string retcol = "";
    unsigned quotes = 0;
    char prev = 0;
    bool finis = false;
    for (int ch; !finis && (ch = in.get()) != EOF; ) {
		switch(ch) {
        case '"':
            ++quotes;
            break;
        case ',':
            if (quotes == 0 || (prev == '"' && (quotes & 1) == 0)) {
                finis = true;
            }
            break;
        default:;
        }
        col += prev = ch;
    }
    return col;
}
int doit()
{
    ifstream in("csv.txt");
    if (!in) {
        cout << "Open error :(" << endl;
        exit(EXIT_FAILURE);
    }
    for (std::string col; in; ) {
        col = getdelimcol(in),
        cout << "<[" << col << "]>" << std::endl;
    }
    if (!in && !in.eof()) {
        cout << "Read error :(" << endl;
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
/*
void OnBnClickedOk()
{

	OnOK();

	//assumes zip/can contains all the data; will empty existing and replace with one in m_sCanPath

	long ID = m_ComboQnr.GetItemData( m_ComboQnr.GetCurSel() );
	if (currSelOpt == AUTOSEL) {
		pImportAutoSelDlg = new CImportAutoSelDlg(ID);
		pImportAutoSelDlg->DoModal();
	
	}
	else if (currSelOpt == SPECPATH) {
		pImportSelPathDlg = new CImportSelPathDlg(ID);
		pImportSelPathDlg->DoModal();
	}
	else {

		CString prompt;
		int msgres;
		prompt.Format(_T("This will replace the loaded CBMS-APP SCAN file in the database.  Do you wish to continue?"));
		msgres = AfxMessageBox (prompt, MB_YESNO);
		if (msgres==IDNO) {
			return;
		}

		//unzip selected file
		HZIP hz = OpenZip(m_sCanPath,0);
		ZIPENTRY ze; GetZipItem(hz,-1,&ze); int numitems=ze.index;
		for (int i=0; i<numitems; i++)	{
			GetZipItem(hz,i,&ze);
			UnzipItem(hz,i,ze.name);
		}
		CloseZip(hz);
		
		//AfxMessageBox(GetExeFilePath());
		//ImportData(sPath, pGlobalConn, ID);

		CString sSQL, sPathSQL;
		bool ignore=TRUE;
		string delim = ",", enclose = "\"";	//what's escape char? ask CCS!

		int idx;
		for (int i=0; i<nqnr; i++) {
			if (qnrIDs[i]==ID)
				idx=i;
		}

		for (int i=0; i<ntypes[idx]; i++) {//drop tables first

			if (TableExists((CString) qnrtables[idx][i], pGlobalConn)) {
				sSQL.Format(_T("DROP TABLE `%s`;"), (CString) qnrtables[idx][i]);
				//AfxMessageBox(sSQL);
				pGlobalConn->ExecuteSQL(sSQL);
			}

		}

		//AfxMessageBox(_T("1"));
		pGlobalConn->CreateTable(pDCT[idx], qnrtables[idx], RTYPES[idx], ntypes[idx], _T(""));
		//AfxMessageBox(_T("2"));

		string path;//, wrkfile;

		path.assign(ConstChar(GetExeFilePath()));
		//wrkfile.assign(path);
		//wrkfile.append("\\statsim_mdc.sql");
		//sPathSQL = wrkfile.c_str();
		//sPathSQL.Replace(_T("\\"),_T("/"));

		for (int i=0; i<ntypes[idx]; i++) {		

			CString sSQLInit, sSQLValues;

			string mdcfile;
			mdcfile.assign(path);
			int nlines=0;

			//wrkfile.assign(path);
			//wrkfile.append("\\").append(qnrtables[idx][i]).append(".sql");
			if (i==0)
				mdcfile.append("\\main.csv");
			else
				mdcfile.append("\\").append(qnrtables[idx][i]).append(".csv");
			

			//CString msg;
			//msg = mdcfile.c_str();
			//_towchar ofile(mdcfile.c_str(), FALSE);
			//msg.Format(_T("table: %s"), ofile);
			//AfxMessageBox(msg);

			ifstream input(mdcfile);
			//ofstream output(wrkfile);

			//if (!output)
			//{
			//	CString msg;
			//	msg.Format(_T("Error accessing %s. Try to check permissions."), GetExeFilePath());
			//	AfxMessageBox(msg);
			//	return;
			//}
			if(!input)
			{
				AfxMessageBox((CString) mdcfile.c_str());
				CString msg;
				msg.Format(_T("Error accessing files in %s. Can be due to incompatible dictionary."), m_sCanPath);
				AfxMessageBox(msg);
				return;
			}
			
			daestring line;
			string::size_type len;

			if (ignore) {
				sSQLInit.Format(_T("INSERT IGNORE INTO `%s` VALUES "), (CString) qnrtables[idx][i]);
			}
			else {
				sSQLInit.Format(_T("INSERT INTO `%s` VALUES "), (CString) qnrtables[idx][i]);
			}
			
			//while(input >> line)
			while(!std::getline(input, line, '\n').eof()) {
				nlines++;
				if (nlines==1) //skip the first line--header
					continue;	//or break?

				len = line.length();
				if (!len)	//skip the line if zero length
					continue;

				stringstream sql;
				sql << "INSERT IGNORE INTO `" << qnrtables[idx][i] << "` VALUES (";

				if (nlines>2)	{	//comma sep for lines starting from 3 (header, first, ..)
					sSQLValues = sSQLValues + _T(",\n(");
				}
				else {
					sSQLValues = _T("(");
				}

				string delimenc, tabenc, nullenc;
				delimenc.append(enclose).append(delim).append(enclose);	//build "," for instance
				tabenc.append(enclose).append("\t").append(enclose);
				nullenc.append(enclose).append(enclose);	//blank string

				//line.replace((string) "\r", (string) "");	//replaces "\r" with blank
				
				line.replace(delimenc, tabenc);	//replaces "," with "\t" for instance
				line.replace(nullenc, (string) "NULL");	//replaces "" with NULL
				
				//TEMPORARY - FOR UCT editing (x2 to replace subsequent)
				line.replace(",,", (string) ",NULL,");	//replaces ",," with ,NULL,
				line.replace(",,", (string) ",NULL,");	//replaces ",," with ,NULL,
				///////////////////////////////////////////////////////////////////

				line.replace((string) "'", (string) "\\'");	//replaces "'" with "\'"

				//line += "\n";	//append newline
				
				istringstream ss_line( line );
				//vector <string> record;
				
				int fldctr = 0;
				while (ss_line)	{	
					
					daestring cur_str;
					//CString sqlitem;
					
					if (!getline( ss_line, cur_str, '\t' )) {
						
						cur_str.replace(enclose, (string) "'");	//replaces " with '
						//sqlitem.Format(_T("'%s'"), (LPCSTR) cur_str.c_str());	//string does not work on format!
						//sqlitem= cur_str.c_str();
						//sSQLValues = sSQLValues + sqlitem + _T(")");

						sql << cur_str << ")";
						break;

					}
					else {

						cur_str.replace(enclose, (string) "'");	//replaces " with none
						//sqlitem = cur_str.c_str();
						//sqlitem.Format(_T("'%s'"), (LPCSTR) cur_str.c_str());

						if (fldctr>0) {
							sql << "," << cur_str;
							//sSQLValues = sSQLValues + _T(",") + sqlitem;
						}
						else {
							sql << cur_str;
							//sSQLValues = sSQLValues + sqlitem;
						}

						fldctr++;

					}

					//record.push_back( s );
				}
				
				
				sql << ";";
				sSQL = sql.str().c_str();
				
				//AfxMessageBox(sSQL);
				pGlobalConn->ExecuteSQL(sSQL);
				//AfxMessageBox(sSQLValues);
				//data.push_back( record );
				//CString msg;
				//msg.Format(_T("%d"), fldctr);
				//AfxMessageBox(msg);
			}

			//output << line;

			//output <<";";
			input.close();	
			//output.close();
						
			//sSQL = sSQLInit + sSQLValues + _T(";");

			//sSQL.Format(_T("LOAD DATA INFILE '%s' INTO TABLE `%s` FIELDS TERMINATED BY '\\t' OPTIONALLY ENCLOSED BY '\"' LINES TERMINATED BY '\\n';"), sPathSQL, (CString) qnrtables[idx][i]);
			//sSQL.Format(_T("SOURCE %s;"), sPathSQL);

			//input.open(wrkfile);
			//string sql;
			//sql.assign( (std::istreambuf_iterator<char>(input) ),
            //   (std::istreambuf_iterator<char>()    ) );	//pass input to sql
			//input >> sql;

			//sSQL = sql.c_str();
			//AfxMessageBox(sSQL);
			//pGlobalConn->ExecuteSQL(sSQL);


			//reconf child tables; temp?
			if (i!=0) {

				sSQL.Format(_T("ALTER TABLE `%s` ADD COLUMN `urb` INT(1) ZEROFILL UNSIGNED NULL, ADD COLUMN `regn` INT(2) ZEROFILL UNSIGNED NULL, ADD COLUMN `prov` INT(2)  ZEROFILL UNSIGNED NULL, ADD COLUMN `mun` INT(2)  ZEROFILL UNSIGNED NULL, ADD COLUMN `brgy` INT(3)  ZEROFILL UNSIGNED NULL, ADD COLUMN `purok` INT(2)  ZEROFILL UNSIGNED NULL, ADD COLUMN `hcn` INT(6)  ZEROFILL UNSIGNED NULL"), (CString) qnrtables[idx][i]);
				
				if (i==1)
					sSQL.Append(_T(", CHANGE COLUMN `memno` `memno` INT(2) UNSIGNED ZEROFILL NULL DEFAULT NULL;"));
				else
					sSQL.Append(_T(";"));                           

				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("ALTER TABLE `%s` ADD INDEX `IDX` (`hpq_hh_id` ASC, `id` ASC);"), (CString) qnrtables[idx][i]);
				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("UPDATE `%s`, `%s` SET `%s`.`regn`=`%s`.`regn`, `%s`.`prov`=`%s`.`prov`, `%s`.`mun`=`%s`.`mun`, `%s`.`brgy`=`%s`.`brgy`, `%s`.`purok`=`%s`.`purok`, `%s`.`hcn`=`%s`.`hcn` WHERE `%s`.`%s_id`=`%s`.`id`;"), 
					(CString) qnrtables[idx][i], (CString) qnrtables[idx][0],
					(CString) qnrtables[idx][i], (CString) qnrtables[idx][0],
					(CString) qnrtables[idx][i], (CString) qnrtables[idx][0],
					(CString) qnrtables[idx][i], (CString) qnrtables[idx][0],
					(CString) qnrtables[idx][i], (CString) qnrtables[idx][0],
					(CString) qnrtables[idx][i], (CString) qnrtables[idx][0],
					(CString) qnrtables[idx][i], (CString) qnrtables[idx][0],
					(CString) qnrtables[idx][i], (CString) qnrtables[idx][0],
					(CString) qnrtables[idx][0]);

				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("ALTER TABLE `%s` ADD INDEX `hpqIDX` (`regn` ASC, `prov` ASC, `mun` ASC, `brgy` ASC, `purok` ASC, `hcn` ASC);"), (CString) qnrtables[idx][i]);
				pGlobalConn->ExecuteSQL(sSQL);

				//set urbanity
				sSQL.Format(_T("UPDATE `%s`, `brgy` SET `%s`.`urb`=`brgy`.`urb` WHERE `%s`.`brgy`=`brgy`.`brgy`;"), 
					(CString) qnrtables[idx][i], (CString) qnrtables[idx][i],
					(CString) qnrtables[idx][i]);

				pGlobalConn->ExecuteSQL(sSQL);

			}
			else { //set urbanity; temporary; must be based on PSGC classification
				sSQL.Format(_T("ALTER TABLE `%s` ADD COLUMN `int_mm` INT(2)  ZEROFILL UNSIGNED NULL, ADD COLUMN `int_dd` INT(2)  ZEROFILL UNSIGNED NULL, ADD COLUMN `int_yy` INT(4)  ZEROFILL UNSIGNED NULL, ADD COLUMN `urb` INT(1)  ZEROFILL UNSIGNED NULL, ADD COLUMN `hsize` INT(3)  ZEROFILL UNSIGNED NULL, CHANGE COLUMN `regn` `regn` INT(2) UNSIGNED ZEROFILL NULL DEFAULT NULL, CHANGE COLUMN `prov` `prov` INT(2) UNSIGNED ZEROFILL NULL DEFAULT NULL, CHANGE COLUMN `mun` `mun` INT(2) UNSIGNED ZEROFILL NULL DEFAULT NULL, CHANGE COLUMN `brgy` `brgy` INT(3) UNSIGNED ZEROFILL NULL DEFAULT NULL, CHANGE COLUMN `purok` `purok` INT(2) UNSIGNED ZEROFILL NULL DEFAULT NULL, CHANGE COLUMN `hcn` `hcn` INT(6) UNSIGNED ZEROFILL NULL DEFAULT NULL;"), (CString) qnrtables[idx][i]);
				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("ALTER TABLE `%s` ADD INDEX `IDX` (`id` ASC);"), (CString) qnrtables[idx][i]);
				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("UPDATE `%s` SET `%s`.`urb`=2, `%s`.`int_mm`=month(int_date), `%s`.`int_dd`=day(int_date), `%s`.`int_yy`=year(int_date), `%s`.`hsize`=`phsize`;"), 
					(CString) qnrtables[idx][i],
					(CString) qnrtables[idx][i],
					(CString) qnrtables[idx][i],
					(CString) qnrtables[idx][i],
					(CString) qnrtables[idx][i],
					(CString) qnrtables[idx][i]);
				pGlobalConn->ExecuteSQL(sSQL);

				sSQL.Format(_T("ALTER TABLE `%s` ADD INDEX `hpqIDX` (`regn` ASC, `prov` ASC, `mun` ASC, `brgy` ASC, `purok` ASC, `hcn` ASC);"), (CString) qnrtables[idx][i]);
				pGlobalConn->ExecuteSQL(sSQL);

				//set urbanity
				sSQL.Format(_T("UPDATE `%s`, `brgy` SET `%s`.`urb`=`brgy`.`urb` WHERE `%s`.`brgy`=`brgy`.`brgy`;"), 
					(CString) qnrtables[idx][i], (CString) qnrtables[idx][i],
					(CString) qnrtables[idx][i]);

				pGlobalConn->ExecuteSQL(sSQL);

			}
		}
		//sPath = GetExeFilePath();
		//sPath.Append(L"\\hpq_mem.csv");
		//ImportData(sPath, pGlobalConn, ID);
		
		CString msg;
		msg.Format(_T("%s has been loaded to the database."), m_sCanPath); 
		AfxMessageBox(msg);
		
		//Clean up
		for (int i=0; i<ntypes[idx]; i++) {		
			string mdcfile;
			mdcfile.assign(path);

			if (i==0)
				mdcfile.append("\\main.csv");
			else
				mdcfile.append("\\").append(qnrtables[idx][i]).append(".csv");
			
			if (_tremove((CString) mdcfile.c_str())!=0)
			//if (_tremove(_T("C:\\CBMSDatabase\\System\\STATSIM\\main.csv"))!= 0)
			{
				msg.Format(_T("Error deleting temporary '%s'"), (CString) mdcfile.c_str());
				AfxMessageBox(msg);
			}

		}

		int msgResult;
		msg.Format(_T("Do you want to start the processing?"));
		msgResult = AfxMessageBox (msg, MB_YESNO);
		if (msgResult==IDYES) {
			ConUpElt(FALSE); //Configure and update
		}


	}
}
*/



/**
 * Example Usage of libcsv_parser++
 *
 * These are some of the characters you may use in this program
 *
 * @li DEC is how it would be represented in decimal form (base 10)
 * @li HEX is how it would be represented in hexadecimal format (base 16)
 *
 * @li  DEC     HEX             Character Name
 * @li  0       0x00    null
 * @li  9       0x09    horizontal tab
 * @li  10      0x0A    line feed, new line
 * @li  13      0x0D    carriage return
 * @li  27      0x1B    escape
 * @li  32      0x20    space
 * @li  33      0x21    double quote
 * @li  39      0x27    single quote
 * @li  44      0x2C    comma
 * @li  92      0x5C    backslash
 */

/**
 * Example Program - showing usage of the csv_parser class
 *
 * In this example, we include the csv_parser header file as <csv_parser/csv_parser.hpp>
 *
 * Then we declare the variables we are going to use in the program
 *
 * @li The name of the input file is "example_input.csv"
 * @li The field terminator is the comma character
 * @li The record terminator is the new line character 0x0A
 * @li The field enclosure character is the double quote.
 *
 * In this example we will be parsing the document as the fields are optionally enclosed.
 *
 * The first record in the CSV file will be skipped.
 *
 * Please view the source code of this file more closely for details.
 *
 * @todo Add more examples using different parsing modes and different enclosure and line terminator characters.
 * @toto Include an example where the filename, field_terminator char, line_terminator char, enclosure_char and other program variables are loaded from a file.
 *
 * @param int The number of arguments passed
 * @param argv The array of arguements passed to the main function
 * @return int The status of the program returned to the operating system upon termination.
 *
 * @see csv_parser
 *
 * @author Israel Ekpo <israel.ekpo@israelekpo.com>
 */
