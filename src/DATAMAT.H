#ifndef __DATAMAT_H__
#define __DATAMAT_H__

	struct _datamat {

	// Member varibles that hold data for DataMat
		long m_istate;   // state of initialization
		long m_version;   // version
		int m_numcols;  // number of columns
		int m_numrows;  // number of rows
		int m_numtests;  // number of test rows
		int m_ninputs;   // # of inputs to network
		int m_noutputs;  // # of output from network
		int m_rawrows;  // # of records in raw data file
		int m_rawcols;  // # of columns in raw data file
		int m_maxrows;	// # of rows m_array allocated
		int m_maxtests;	// # of test rows m_itarray & m_otarry allocated
		int m_sep;		// field separator
	
		unsigned m_total; // # number of vals stored in ioarray
		char m_title[MAXCSTRING];  // short doc & comments
		char m_desc[MAXCSTRING];  // comments
		char m_rawfname[MAXCSTRING];  // raw ascii data filename
		char m_parfname[MAXCSTRING];  // parent DataMat object filename
		int *m_icrossref;  // cross-ref table from raw file
		int *m_ocrossref;  // cross-ref table from raw file
		char *m_rowdesc ;  // row design mat descriptor
		COL_DESC *m_coldesc ;  // column design mat descriptor
		COL_DESC *m_icoldesc ;  // column neural in descriptor
		COL_DESC *m_ocoldesc ;  // column neural out descriptor

		double **m_iarray;  // input training matrix
		double **m_oarray;  // output training matrix
		double **m_itarray;  // input test matrix
		double **m_otarray;  // output test matrix

		int m_cols;  // temps used by loadstr
		int maxncols;
		char m_buf[MAXBUF];
	};

	typedef struct _datamat DATAMAT;

	DATAMAT *DCreateDataMat( );
	void DDeleteDataMat(DATAMAT *pD);
	void DumpDataMat(DATAMAT *pD,FILE *fd);
    int DImportDataMat (DATAMAT *pD, FILE *fd, int ExportVersion);
	double DRescale( DATAMAT *pD, double f,char C,int ix);
	double DScale( DATAMAT *pD, double f,char C,int ix);
	double DGetOutputVal( DATAMAT *pD, int row, int col );
	void DZeroAll(DATAMAT *pD);
	double DGetInputVal(DATAMAT *pD,  int row, int col );
	double DGetOutputVal(DATAMAT *pD,  int row, int col );
	double DGetInputTVal(DATAMAT *pD,  int row, int col );
	double DGetOutputTVal(DATAMAT *pD,  int row, int col );
	void DSetInputVal(DATAMAT *pD,  int row, int col, double val );
	void DSetInputTVal(DATAMAT *pD,  int row, int col, double val );
	void DSetOutputVal(DATAMAT *pD,  int row, int col, double val );
	void DSetOutputTVal(DATAMAT *pD,  int row, int col, double val );
//	int DCalc(DATAMAT *pD, );
//	int DReCalc(DATAMAT *pD, );
	void DReCalcScalingFactor(COL_DESC *cold, int cols);
	void ZeroColDesc(DATAMAT *pD,int num,COL_DESC* desc);
	char * RescaleFmt(DATAMAT *pD, double f,char C,int ix);

/*
	char * GetFmtStr(char C, int ix);
	char * RescaleFmt(double f,int ix);
	char * GetFmtStr(int ix);

*/


#endif // __CDATAMAT_H__
