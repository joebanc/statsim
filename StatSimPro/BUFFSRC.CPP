void CStatSimHTML::DrawTotPop(CString sTitle, CString sSubTitle) 
{
	if (error==SQL_ERROR) {
		return;
	}

	CString sTagVal, sLevel, sField, sTagLabel;
	LPCTSTR sFloat;
	float floatval, numerator, denominator, quotient;
	int n=3, *in = new int[n], *out = new int[n], intval, i;

	for (i=0; i<n; i++) {
		in[i]=0;
		out[i]=0;
	}


	try
	{

		sLevel = m_sLevel;

		if (m_pRS->GetRecordCount()>0) {
			m_pRS->MoveFirst();
		}
		
		for (long i=0; i<m_pRS->GetRecordCount(); i++) {
			
			sTagVal = sTitle;

	_ftprintf(m_pFile, _T("<html>"));
	_ftprintf(m_pFile, _T("<head>"));
	_ftprintf(m_pFile, _T("<style id="statsim_Styles">"));
	_ftprintf(m_pFile, _T("<!--table"));
	_ftprintf(m_pFile, _T("	{mso-displayed-decimal-separator:\"\\.\";"));
	_ftprintf(m_pFile, _T("	mso-displayed-thousand-separator:\"\\,\";}"));
	_ftprintf(m_pFile, _T(".xl1510377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:11.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Calibri, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:nowrap;}"));
	_ftprintf(m_pFile, _T(".xl6510377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:navy;"));
	_ftprintf(m_pFile, _T("	font-size:16.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl6610377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:navy;"));
	_ftprintf(m_pFile, _T("	font-size:12.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl6710377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:#333399;"));
	_ftprintf(m_pFile, _T("	font-size:12.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl6810377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:nowrap;}"));
	_ftprintf(m_pFile, _T(".xl6910377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:#333399;"));
	_ftprintf(m_pFile, _T("	font-size:12.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl7010377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:8.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl7110377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:8.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl7210377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:none;"));
	_ftprintf(m_pFile, _T("	border-bottom:none;"));
	_ftprintf(m_pFile, _T("	border-left:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl7310377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:none;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:nowrap;}"));
	_ftprintf(m_pFile, _T(".xl7410377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:2.0pt double windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:2.0pt double windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl7510377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:right;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:2.0pt double windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:2.0pt double windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl7610377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:2.0pt double windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:2.0pt double windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	background:silver;"));
	_ftprintf(m_pFile, _T("	mso-pattern:black none;"));
	_ftprintf(m_pFile, _T("	white-space:nowrap;}"));
	_ftprintf(m_pFile, _T(".xl7710377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:2.0pt double windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:2.0pt double windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	background:silver;"));
	_ftprintf(m_pFile, _T("	mso-pattern:black none;"));
	_ftprintf(m_pFile, _T("	white-space:nowrap;}"));
	_ftprintf(m_pFile, _T(".xl7810377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl7910377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:right;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl8010377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:right;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl8110377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:none;"));
	_ftprintf(m_pFile, _T("	border-left:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl8210377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:right;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:none;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl8310377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:right;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:none;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl8410377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:right;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:2.0pt double windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:2.0pt double windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl8510377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl8610377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:right;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl8710377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:nowrap;}"));
	_ftprintf(m_pFile, _T(".xl8810377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:nowrap;}"));
	_ftprintf(m_pFile, _T(".xl8910377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:400;"));
	_ftprintf(m_pFile, _T("	font-style:italic;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:nowrap;}"));
	_ftprintf(m_pFile, _T(".xl9010377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:#993300;"));
	_ftprintf(m_pFile, _T("	font-size:14.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:middle;"));
	_ftprintf(m_pFile, _T("	border-top:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:none;"));
	_ftprintf(m_pFile, _T("	border-left:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl9110377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:#993300;"));
	_ftprintf(m_pFile, _T("	font-size:14.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:middle;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:none;"));
	_ftprintf(m_pFile, _T("	border-left:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl9210377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:#993300;"));
	_ftprintf(m_pFile, _T("	font-size:14.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:middle;"));
	_ftprintf(m_pFile, _T("	border-top:none;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid black;"));
	_ftprintf(m_pFile, _T("	border-left:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl9310377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:none;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl9410377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:none;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl9510377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:1.5pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:1.5pt solid black;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl9610377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:#993300;"));
	_ftprintf(m_pFile, _T("	font-size:8.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:none;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl9710377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:#993300;"));
	_ftprintf(m_pFile, _T("	font-size:8.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:none;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl9810377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:#993300;"));
	_ftprintf(m_pFile, _T("	font-size:8.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:1.0pt solid black;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl9910377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:#993300;"));
	_ftprintf(m_pFile, _T("	font-size:8.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:none;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:1.0pt solid black;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl10010377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:#993300;"));
	_ftprintf(m_pFile, _T("	font-size:8.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:center;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	border-top:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-right:1.5pt solid black;"));
	_ftprintf(m_pFile, _T("	border-bottom:1.0pt solid windowtext;"));
	_ftprintf(m_pFile, _T("	border-left:none;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:normal;}"));
	_ftprintf(m_pFile, _T(".xl10110377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:left;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:nowrap;}"));
	_ftprintf(m_pFile, _T(".xl10210377"));
	_ftprintf(m_pFile, _T("	{padding-top:1px;"));
	_ftprintf(m_pFile, _T("	padding-right:1px;"));
	_ftprintf(m_pFile, _T("	padding-left:1px;"));
	_ftprintf(m_pFile, _T("	mso-ignore:padding;"));
	_ftprintf(m_pFile, _T("	color:black;"));
	_ftprintf(m_pFile, _T("	font-size:10.0pt;"));
	_ftprintf(m_pFile, _T("	font-weight:700;"));
	_ftprintf(m_pFile, _T("	font-style:normal;"));
	_ftprintf(m_pFile, _T("	text-decoration:none;"));
	_ftprintf(m_pFile, _T("	font-family:Arial, sans-serif;"));
	_ftprintf(m_pFile, _T("	mso-font-charset:0;"));
	_ftprintf(m_pFile, _T("	mso-number-format:General;"));
	_ftprintf(m_pFile, _T("	text-align:general;"));
	_ftprintf(m_pFile, _T("	vertical-align:bottom;"));
	_ftprintf(m_pFile, _T("	mso-background-source:auto;"));
	_ftprintf(m_pFile, _T("	mso-pattern:auto;"));
	_ftprintf(m_pFile, _T("	white-space:nowrap;}"));
	_ftprintf(m_pFile, _T("-->"));
	_ftprintf(m_pFile, _T("</style>"));
	_ftprintf(m_pFile, _T("</head>"));
	_ftprintf(m_pFile, _T("<body>"));
	_ftprintf(m_pFile, _T("<!--If the same item is republished from Excel, all information between the DIV"));
	_ftprintf(m_pFile, _T("tags will be replaced.-->"));
	_ftprintf(m_pFile, _T("<!----------------------------->"));
	_ftprintf(m_pFile, _T("<!--START OUTPUT -->"));
	_ftprintf(m_pFile, _T("<!----------------------------->"));
	_ftprintf(m_pFile, _T("<div id="id="statsim_Styles" align=left>"));
	_ftprintf(m_pFile, _T("<table border=0 cellpadding=0 cellspacing=0 width=746 style='border-collapse:"));
	_ftprintf(m_pFile, _T(" collapse;table-layout:fixed;width:560pt'>"));
	_ftprintf(m_pFile, _T(" <col width=377 style='mso-width-source:userset;mso-width-alt:13787;width:283pt'>"));
	_ftprintf(m_pFile, _T(" <col width=65 style='mso-width-source:userset;mso-width-alt:2377;width:49pt'>"));
	_ftprintf(m_pFile, _T(" <col width=66 style='mso-width-source:userset;mso-width-alt:2413;width:50pt'>"));
	_ftprintf(m_pFile, _T(" <col width=59 span=3 style='mso-width-source:userset;mso-width-alt:2157;"));
	_ftprintf(m_pFile, _T(" width:44pt'>"));
	_ftprintf(m_pFile, _T(" <col width=61 style='mso-width-source:userset;mso-width-alt:2230;width:46pt'>"));
	_ftprintf(m_pFile, _T(" <tr height=27 style='height:20.25pt'>"));
	_ftprintf(m_pFile, _T("  <td height=27 class=xl6510377 width=377 style='height:20.25pt;width:283pt'>CBMS StatSim</td>"));


		if (m_cParE< (int) PROV) {
			sTagVal = m_pRS->SQLFldValue(_T("Region"));
			sTagLabel = _T("Region:");
		}
		else {
			sTagVal = m_pRS->SQLFldValue(_T("Province")) + ", " + m_pRS->SQLFldValue(_T("Region"));
			sTagLabel = _T("Province:");
		}


	_ftprintf(m_pFile, _T("  <td colspan=2 class=xl10110377 width=131 style='width:99pt'>") 
		+ sTagLabel + _T("</td>"));
	_ftprintf(m_pFile, _T("  <td colspan=4 class=xl10210377 width=238 style='width:178pt'>")
		+ sTagVal + _T("</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=21 style='height:15.75pt'>"));

		sTagVal = sTitle;


	_ftprintf(m_pFile, _T("  <td height=21 class=xl6610377 width=377 style='height:15.75pt;width:283pt'>") 
		+ sTagVal + _T("</td>"));

		if (m_cParE< (int) MUN) {
			sTagVal = _T("");
			sTagLabel = _T("");
		}
		else {
			sTagVal = m_pRS->SQLFldValue(_T("Municipality"));
			sTagLabel = _T("City/Municipality:");
		}



	_ftprintf(m_pFile, _T("  <td colspan=2 class=xl10110377>") + sTagLabel + _T("</td>"));
	_ftprintf(m_pFile, _T("  <td colspan=4 class=xl10210377>") + sTagVal +  _T("</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));

			sTagVal = sSubTitle;

	_ftprintf(m_pFile, _T(" <tr height=21 style='height:15.75pt'>"));
	_ftprintf(m_pFile, _T("  <td height=21 class=xl6710377 width=377 style='height:15.75pt;width:283pt'>") 
		+ sTagVal + _T("</td>"));

		if (m_cParE< (int) BRGY) {
			sTagVal = _T("");
			sTagLabel = _T("");
		}
		else if (m_cParE< (int) PUROK) {
			sTagVal = m_pRS->SQLFldValue(_T("Barangay"));
			sTagLabel = _T("Barangay:");
		}
		else {
			sTagVal = m_pRS->SQLFldValue(_T("PurokName")) + ", " + m_pRS->SQLFldValue(_T("Barangay"));
			sTagLabel = _T("Purok:");
		}

	_ftprintf(m_pFile, _T("  <td colspan=2 class=xl10110377>") + sTagLabel + _T("</td>"));
	_ftprintf(m_pFile, _T("  <td colspan=4 class=xl10210377>") + sTagVal + _T("</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=20 style='height:15.0pt'>"));
	_ftprintf(m_pFile, _T("  <td height=20 class=xl6810377 style='height:15.0pt'></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=22 style='height:16.5pt'>"));
	_ftprintf(m_pFile, _T("  <td height=22 class=xl6910377 width=377 style='height:16.5pt;width:283pt'></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=22 style='height:16.5pt'>"));
	_ftprintf(m_pFile, _T("  <td rowspan=3 height=64 class=xl9010377 width=377 style='border-bottom:1.0pt solid black;"));
	_ftprintf(m_pFile, _T("  height:48.0pt;width:283pt'>Group</td>"));
	_ftprintf(m_pFile, _T("  <td colspan=6 class=xl9310377 width=369 style='border-right:1.5pt solid black;"));
	_ftprintf(m_pFile, _T("  border-left:none;width:277pt'>Frequency</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=21 style='height:15.75pt'>"));
	_ftprintf(m_pFile, _T("  <td colspan=3 height=21 class=xl9610377 width=190 style='border-right:1.0pt solid black;"));
	_ftprintf(m_pFile, _T("  height:15.75pt;border-left:none;width:143pt'>Magnitude<span"));
	_ftprintf(m_pFile, _T("  style='mso-spacerun:yes'> </span></td>"));
	_ftprintf(m_pFile, _T("  <td colspan=3 class=xl9910377 width=179 style='border-right:1.5pt solid black;"));
	_ftprintf(m_pFile, _T("  border-left:none;width:134pt'>Proportion</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=21 style='height:15.75pt'>"));
	_ftprintf(m_pFile, _T("  <td height=21 class=xl7010377 width=65 style='height:15.75pt;width:49pt'>Total</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7010377 width=66 style='width:50pt'>Male</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7010377 width=59 style='width:44pt'>Female</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7010377 width=59 style='width:44pt'>Total</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7010377 width=59 style='width:44pt'>Male</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7110377 width=61 style='width:46pt'>Female</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=21 style='height:15.75pt'>"));
	_ftprintf(m_pFile, _T("  <td height=21 class=xl7210377 width=377 style='height:15.75pt;width:283pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl6810377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7310377>&nbsp;</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=22 style='height:16.5pt'>"));
	_ftprintf(m_pFile, _T("  <td height=22 class=xl7410377 width=377 style='height:16.5pt;width:283pt'>HOUSEHOLDS</td>"));

			sField = sLevel + _T("_tothh");
			sTagVal = m_pRS->SQLFldValue(ConstChar(sField));

	_ftprintf(m_pFile, _T("  <td class=xl7510377 width=65 style='width:49pt'>") + _sTagVal _T("</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7610377>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7610377>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7610377>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7610377>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7710377>&nbsp;</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=22 style='height:16.5pt'>"));
	_ftprintf(m_pFile, _T("  <td height=22 class=xl7810377 width=377 style='height:16.5pt;width:283pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=65 style='width:49pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=66 style='width:50pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=59 style='width:44pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=59 style='width:44pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=59 style='width:44pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8010377 width=61 style='width:46pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=21 style='height:15.75pt'>"));
	_ftprintf(m_pFile, _T("  <td height=21 class=xl7810377 width=377 style='height:15.75pt;width:283pt'>Resident population</td>"));

			sField = sLevel + _T("_totmem");
			sTagVal = m_pRS->SQLFldValue(ConstChar(sField));
			in[0] = _ttoi(ConstChar(sTagVal));

	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=65 style='width:49pt'>") + sTagVal + _T("</td>"));

			sField = sLevel + _T("_totmem_male");
			sTagVal = m_pRS->SQLFldValue(ConstChar(sField));
			in[1] = _ttoi(ConstChar(sTagVal));

	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=66 style='width:50pt'>") + sTagVal + _T("</td>"));

			sField = sLevel + _T("_totmem_female");
			sTagVal = m_pRS->SQLFldValue(ConstChar(sField));
			in[2] = _ttoi(ConstChar(sTagVal));

	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=59 style='width:44pt'>") +sTagVal + _T("</td>"));

		if (in[0]!=0) {
			floatval = ((float)in[0]/(float)in[0])*100;
			sTagVal.Format(_T("%.0f"), floatval);
		}
		else {
			sTagVal.Format(_T("-"));
		}

	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=59 style='width:44pt'>") + sTagVal + _T("</td>"));

		if (in[0]!=0) {
			floatval = ((float)in[1]/(float)in[0])*100;
			sTagVal.Format(_T("%.1f"), floatval);
		}
		else {
			sTagVal.Format(_T("-"));
		}

	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=59 style='width:44pt'>") + sTagVal + _T("</td>"));

		if (in[0]!=0) {
			floatval = ((float)in[2]/(float)in[0])*100;
			sTagVal.Format(_T("%.1f"), floatval);
		}
		else {
			sTagVal.Format(_T("-"));
		}

	_ftprintf(m_pFile, _T("  <td class=xl8010377 width=61 style='width:46pt'>") + sTagVal + _T("</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=21 style='height:15.75pt'>"));
	_ftprintf(m_pFile, _T("  <td height=21 class=xl7810377 width=377 style='height:15.75pt;width:283pt'>Overseas Filipino Workers (OFWs)</td>"));
			sField = sLevel + _T("_totmem");
			sTagVal = m_pRS->SQLFldValue(ConstChar(sField));
			out[0] = _ttoi(ConstChar(sTagVal));

	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=65 style='width:49pt'>") + sTagVal + _T("</td>"));

			sField = sLevel + _T("_totmem_male");
			sTagVal = m_pRS->SQLFldValue(ConstChar(sField));
			out[1] = _ttoi(ConstChar(sTagVal));

	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=66 style='width:50pt'>") + sTagVal + _T("</td>"));

			sField = sLevel + _T("_totmem_female");
			sTagVal = m_pRS->SQLFldValue(ConstChar(sField));
			out[2] = _ttoi(ConstChar(sTagVal));

	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=59 style='width:44pt'>") +sTagVal + _T("</td>"));

		if (out[0]!=0) {
			floatval = ((float)out[0]/(float)out[0])*100;
			sTagVal.Format(_T("%.0f"), floatval);
		}
		else {
			sTagVal.Format(_T("-"));
		}

	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=59 style='width:44pt'>") + sTagVal + _T("</td>"));

		if (out[0]!=0) {
			floatval = ((float)out[1]/(float)out[0])*100;
			sTagVal.Format(_T("%.1f"), floatval);
		}
		else {
			sTagVal.Format(_T("-"));
		}

	_ftprintf(m_pFile, _T("  <td class=xl7910377 width=59 style='width:44pt'>") + sTagVal + _T("</td>"));

		if (out[0]!=0) {
			floatval = ((float)out[2]/(float)out[0])*100;
			sTagVal.Format(_T("%.1f"), floatval);
		}
		else {
			sTagVal.Format(_T("-"));
		}

	_ftprintf(m_pFile, _T("  <td class=xl8010377 width=61 style='width:46pt'>") + sTagVal + _T("</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=21 style='height:15.75pt'>"));
	_ftprintf(m_pFile, _T("  <td height=21 class=xl8110377 width=377 style='height:15.75pt;width:283pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8210377 width=65 style='width:49pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8210377 width=66 style='width:50pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8210377 width=59 style='width:44pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8210377 width=59 style='width:44pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8210377 width=59 style='width:44pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8310377 width=61 style='width:46pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=22 style='height:16.5pt'>"));
	_ftprintf(m_pFile, _T("  <td height=22 class=xl7410377 width=377 style='height:16.5pt;width:283pt'>TOTAL POPULATION</td>"));
		
		int inout0 = in[0] + out[0];
		sTagVal.Format(_T("%d"), inout0);

	_ftprintf(m_pFile, _T("  <td class=xl7510377 width=65 style='width:49pt'>") + sTagVal + _T("</td>"));

		int inout1 = in[1] + out[1];
		sTagVal.Format(_T("%d"), inout1);

	_ftprintf(m_pFile, _T("  <td class=xl7510377 width=66 style='width:50pt'>") + sTagVal + _T("</td>"));

		int inout2 = in[2] + out[2];
		sTagVal.Format(_T("%d"), inout2);

	_ftprintf(m_pFile, _T("  <td class=xl7510377 width=59 style='width:44pt'>") + sTagVal + _T("</td>"));

		if (inout0!=0) {
			floatval = ((float)inout0/(float)inout0)*100;
			sTagVal.Format(_T("%.0f"), floatval);
		}
		else {
			sTagVal.Format(_T("-"));
		}

	_ftprintf(m_pFile, _T("  <td class=xl7510377 width=59 style='width:44pt'>") + sTagVal + _T("</td>"));

		if (inout0!=0) {
			floatval = ((float)inout1/(float)inout0)*100;
			sTagVal.Format(_T("%.1f"), floatval);
		}
		else {
			sTagVal.Format(_T("-"));
		}

	_ftprintf(m_pFile, _T("  <td class=xl7510377 width=59 style='width:44pt'>") + sTagVal + _T("</td>"));

		if (inout0!=0) {
			floatval = ((float)inout2/(float)inout0)*100;
			sTagVal.Format(_T("%.1f"), floatval);
		}
		else {
			sTagVal.Format(_T("-"));
		}

	_ftprintf(m_pFile, _T("  <td class=xl8410377 width=61 style='width:46pt'>") + sTagVal + _T("</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=22 style='height:16.5pt'>"));
	_ftprintf(m_pFile, _T("  <td height=22 class=xl8510377 width=377 style='height:16.5pt;width:283pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8610377 width=65 style='width:49pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8610377 width=66 style='width:50pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8610377 width=59 style='width:44pt'>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8710377>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8710377>&nbsp;</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl8810377>&nbsp;</td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=21 style='height:15.75pt'>"));
	_ftprintf(m_pFile, _T("  <td height=21 class=xl8910377 style='height:15.75pt'>Source:<span"));
	_ftprintf(m_pFile, _T("  style='mso-spacerun:yes'>  </span>CBMS Survey ____</td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=20 style='height:15.0pt'>"));
	_ftprintf(m_pFile, _T("  <td height=20 class=xl6810377 style='height:15.0pt'></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=20 style='height:15.0pt'>"));
	_ftprintf(m_pFile, _T("  <td height=20 class=xl6810377 style='height:15.0pt'></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=20 style='height:15.0pt'>"));
	_ftprintf(m_pFile, _T("  <td height=20 class=xl6810377 style='height:15.0pt'></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=20 style='height:15.0pt'>"));
	_ftprintf(m_pFile, _T("  <td height=20 class=xl6810377 style='height:15.0pt'></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <tr height=20 style='height:15.0pt'>"));
	_ftprintf(m_pFile, _T("  <td height=20 class=xl6810377 style='height:15.0pt'></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T("  <td class=xl1510377></td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <![if supportMisalignedColumns]>"));
	_ftprintf(m_pFile, _T(" <tr height=0 style='display:none'>"));
	_ftprintf(m_pFile, _T("  <td width=377 style='width:283pt'></td>"));
	_ftprintf(m_pFile, _T("  <td width=65 style='width:49pt'></td>"));
	_ftprintf(m_pFile, _T("  <td width=66 style='width:50pt'></td>"));
	_ftprintf(m_pFile, _T("  <td width=59 style='width:44pt'></td>"));
	_ftprintf(m_pFile, _T("  <td width=59 style='width:44pt'></td>"));
	_ftprintf(m_pFile, _T("  <td width=59 style='width:44pt'></td>"));
	_ftprintf(m_pFile, _T("  <td width=61 style='width:46pt'></td>"));
	_ftprintf(m_pFile, _T(" </tr>"));
	_ftprintf(m_pFile, _T(" <![endif]>"));
	_ftprintf(m_pFile, _T("</table>"));
	_ftprintf(m_pFile, _T("</div>"));
	_ftprintf(m_pFile, _T("<!----------------------------->"));
	_ftprintf(m_pFile, _T("<!--END OF OUTPUT-->"));
	_ftprintf(m_pFile, _T("<!----------------------------->"));
	_ftprintf(m_pFile, _T("</body>"));
	_ftprintf(m_pFile, _T("</html>"));

			//move recordset
			m_pRS->MoveNext();


		}

		delete [] in;
		delete [] out;
	
	}
	
	catch(_com_error &e) 
	{		
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());
		TRACE ( "Exception thrown for classes generated by #import" );
		TRACE ( "\tCode = %08lx\n", e.Error ());
		TRACE ( "\tCode meaning = %s\n", e.ErrorMessage ());
		TRACE ( "\tSource = %s\n", (LPCTSTR) bstrSource);
		TRACE ( "\tDescription = %s\n", (LPCTSTR) bstrDescription);

		AfxMessageBox ((LPCTSTR) bstrDescription);
	}


}
