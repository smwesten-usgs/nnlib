/*
**
**  File:           DATAMAT.C
**  Description:    Contains the data structure and routines used by NEURAL.C
**  Platform:       Windows
**
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
//#include <windows.h>
#include "NNDEFS.H"
#include "DATAMAT.H"

#define ARRAYCHUNK 100

	extern union {
		double f;
		long l;
		} MissingUnion;
#define MISSING (MissingUnion.f)

	void dump (const char *buf, int count);
//	int fgetstr (FILE *fd, LPSTR str, int bufsize);
	int fgetstr (FILE *fd, char *str, int bufsize);
	int fgetint(FILE *fd);
	long fgetlong(FILE *fd);
	double fgetdouble(FILE *fd);
	void ToUpper (char *s);
	int ChkClip (double f, double hi, double lo);

/*
** ZeroAll
**
** This function is called to initialize to a known state the internal members of the
** DATAMAT structure.
**
**
** Arguments:
**
**      DATAMAT *pD		A pointer to the DATAMAT structure to be zapped
**
** Returns:
**
**      None
*/

void ZeroAll(DATAMAT *pD) {

	pD->m_version = REVLEVEL;
	pD->m_numtests = 0 ;
	pD->m_numcols = 0 ;
	pD->m_numrows = 0 ;
	pD->m_ninputs = 0 ;
	pD->m_maxrows = 0;
	pD->m_maxtests = 0 ;
	pD->m_noutputs = 0 ;
	pD->m_maxrows = pD->m_rawcols = pD->m_rawrows = 0;
	pD->m_istate = NFobject_exists;

	pD->m_icrossref = NULL;
	pD->m_ocrossref = NULL;
	pD->m_coldesc = NULL;
	pD->m_rowdesc = NULL;
	pD->m_icoldesc = NULL;
	pD->m_ocoldesc = NULL;
	pD->m_iarray = NULL;
	pD->m_oarray = NULL;
	pD->m_itarray = NULL;
	pD->m_otarray = NULL;
	pD->m_title[0] = pD->m_desc[0] = pD->m_parfname[0] = pD->m_rawfname[0] = 0;
}

/*
** DCreateDataMat
**
** This function is called to create a new DATAMAT structure
**
**
** Arguments:
**
**      None
**
** Returns:
**
**      DATAMAT *		Returns a pointer to the created DATAMAT structure
*/

DATAMAT *DCreateDataMat()
{
	DATAMAT *pD;
	pD = (DATAMAT*) malloc (sizeof(DATAMAT));
	ZeroAll(pD);
	return pD;
}

/*
** ZeroColDesc
**
** This function is called to initialize the column descriptors to a known state
**
**
** Arguments:
**
**      DATAMAT *pD		A pointer to the DATAMAT structure
**      int num			The number of columns to initialize
**      COL_DESC* desc	A pointer to the column descriptor
**
** Returns:
**
**      None
*/

void ZeroColDesc(DATAMAT *pD, int num,COL_DESC* desc)
{
	int i;
	for (i=0;i<num;i++) {
		desc[i].fscale =
		desc[i].foffset = 0.0f;
		desc[i].max = 0.0f;
		desc[i].min = 0.0f;
		desc[i].fieldtype=0;
		desc[i].cliphi = MISSING;
		desc[i].cliplo = MISSING;
		desc[i].colwidth = 90;
		desc[i].flag = 0;
		desc[i].col_usage = 'N';
		strcpy (&desc[i].format[0],"%s");
		desc[i].vlab[0]=0;
		desc[i].units[0]=0;
		if (pD->m_icrossref!=NULL) pD->m_icrossref[i] = pD->m_ocrossref[i] = -1;
	}
}

/*
** DDeleteDataMat
**
** This function is called to destroy a DATAMAT structure
**
**
** Arguments:
**
**      DATAMAT *pD		A pointer to the DATAMAT structure
**
** Returns:
**
**      None
*/

void DDeleteDataMat(DATAMAT *pD)
{

	if (pD->m_istate & NFnum_col_known) {
		if(pD->m_coldesc!=NULL) {
		    free  (pD->m_coldesc);
            pD->m_coldesc=NULL;
        }

		if(pD->m_icrossref!=NULL) {
		    free (pD->m_icrossref);
            pD->m_icrossref=NULL;
        }

		if(pD->m_ocrossref) {
		    free (pD->m_ocrossref);
            pD->m_ocrossref=NULL;
        }
	}

	if(pD->m_icoldesc!=NULL) {
	    free (pD->m_icoldesc);
        pD->m_icoldesc=NULL;
    }
	if(pD->m_ocoldesc!=NULL) {
	    free (pD->m_ocoldesc);
        pD->m_ocoldesc=NULL;
    }


	if (pD->m_istate&NFtrainmat_loaded) {
		if (pD->m_ninputs)
			free_2d_doubles (pD->m_iarray,pD->m_ninputs);
		if (pD->m_noutputs)
			free_2d_doubles (pD->m_oarray,pD->m_noutputs);
	}
	if (pD->m_istate&NFtestmat_loaded) {
		if (pD->m_ninputs)
			free_2d_doubles (pD->m_itarray,pD->m_ninputs);
		if (pD->m_noutputs)
			free_2d_doubles (pD->m_otarray,pD->m_noutputs);
	}

	if(pD!=NULL) {
	    free (pD);
        pD=NULL;
    }

    return;
}

/*
** DSetInputVal
**
** This function is called to set a cell in the training input matrix
**
**
** Arguments:
**
**      DATAMAT *pD		A pointer to the DATAMAT structure
**      int row			The row number of the cell
**      int col			The column number of the cell
**      double val		The value to set the cell
**
** Returns:
**
**      None
*/

void DSetInputVal(DATAMAT *pD, int row, int col, double val ) {
	pD->m_iarray[col][row] = val;
}

/*
** DSetInputTVal
**
** This function is called to set a cell in the test input matrix
**
**
** Arguments:
**
**      DATAMAT *pD		A pointer to the DATAMAT structure
**      int row			The row number of the cell
**      int col			The column number of the cell
**      double val		The value to set the cell
**
** Returns:
**
**      None
*/

void DSetInputTVal(DATAMAT *pD, int row, int col, double val ) {
	pD->m_itarray[col][row] = val;
}

/*
** DSetOutputVal
**
** This function is called to set a cell in the training output matrix
**
**
** Arguments:
**
**      DATAMAT *pD		A pointer to the DATAMAT structure
**      int row			The row number of the cell
**      int col			The column number of the cell
**      double val		The value to set the cell
**
** Returns:
**
**      None
*/

void DSetOutputVal(DATAMAT *pD, int row, int col, double val ) {
	pD->m_oarray[col][row] = val;
}

/*
** DSetOutputTVal
**
** This function is called to set a cell in the test output matrix
**
**
** Arguments:
**
**      DATAMAT *pD		A pointer to the DATAMAT structure
**      int row			The row number of the cell
**      int col			The column number of the cell
**      double val		The value to set the cell
**
** Returns:
**
**      None
*/

void DSetOutputTVal(DATAMAT *pD, int row, int col, double val ) {
	pD->m_otarray[col][row] = val;
}

/*
** DRescale
**
** This function is called to re-scale the passed value back to user units
**
**
** Arguments:
**
**      DATAMAT *pD		A pointer to the DATAMAT structure
**      double f			The value to re-scale
**      char C			Contains a 'I' for input and 'O' for output
**      int ix			The index to the column descriptor
**
** Returns:
**
**      double			Returns the re-scaled value
*/

double DRescale(DATAMAT *pD, double f,char C,int ix) {
	if (f==MISSING) return f;
	if (C=='I') {
		f /= pD->m_icoldesc[ix].fscale;
		f -= pD->m_icoldesc[ix].foffset;
	}
	if (C=='O') {
		f /= pD->m_ocoldesc[ix].fscale;
		f -= pD->m_ocoldesc[ix].foffset;
	}
	return f;
}

/*
** DScale
**
** This function is called to scale the passed value from user units to neural units
**
**
** Arguments:
**
**      DATAMAT *pD		A pointer to the DATAMAT structure
**      double f			The value to scale
**      char C			Contains a 'I' for input and 'O' for output
**      int ix			The index to the column descriptor
**
** Returns:
**
**      double			Returns the scaled value
*/

double DScale(DATAMAT *pD, double f,char C,int ix) {
	if (C=='I') {
		f += pD->m_icoldesc[ix].foffset;
		f *= pD->m_icoldesc[ix].fscale;
	}
	if (C=='O') {
		f += pD->m_ocoldesc[ix].foffset;
		f *= pD->m_ocoldesc[ix].fscale;
	}
	return f;
}

/*
** DRescaleFmt
**
** This function is called to re-scale the passed value from neural units to users units.
** The function also builds a formatted string. The format is determined by the format
** string stored in the column descriptor
**
**
** Arguments:
**
**      DATAMAT *pD		A pointer to the DATAMAT structure
**      double f			The value to scale
**      char C			Contains a 'I' for input and 'O' for output
**      int ix			The index to the column descriptor
**
** Returns:
**
**      char *			Returns a pointer to the formatted string
*/

char * DRescaleFmt(DATAMAT *pD,double f,char C,int ix) {
	double val;
	static char buf[40];

	if (f==MISSING) return " . ";
	val = DRescale(pD,f,C,ix);
	if (C=='O') {
		sprintf (buf,&pD->m_ocoldesc[ix].format[0],val);
		strncpy (&pD->m_ocoldesc[ix].convstr[0],buf,20);
		pD->m_ocoldesc[ix].convstr[19]=0;
		return &pD->m_ocoldesc[ix].convstr[0];
	}
	if (C=='I') {
		sprintf (buf,&pD->m_icoldesc[ix].format[0],val);
		strncpy (&pD->m_icoldesc[ix].convstr[0],buf,20);
		pD->m_icoldesc[ix].convstr[19]=0;
		return &pD->m_icoldesc[ix].convstr[0];
	}
	return "BadC";
}

/*
** dtransl
**
** This function is used internally by the import data matrix function.
**
**
** Arguments:
**
**      char *cdummy      	A pointer to a character string
**
** Returns:
**
**      int		returns the tranlated string or a -1 if the string doesn't start with 'D'
*/

int dtransl(char *cdummy)
{
	int val=0;
	if (cdummy[0] == 'D') {
		val = atoi(&cdummy[1]);
		if (val > 8) return -1;
		return val;
	}
	if (cdummy[0] == 'M') return 0;
	if (cdummy[0] == 'T') return 1000;
	return -1;
}

/*
** DImportDataMat
**
** This function is called to import a data matrix from an ENN file.
**
**
** Arguments:
**
**      DATAMAT *pD     A pointer to the datamat structure to load
**      FILE *fd		A pointer to the open ENN file
**      int ver			Version ENN file being imported
**
** Returns:
**
**      int		returns a zero is the datamat is imported without error
*/

int DImportDataMat(DATAMAT *pD, FILE *fd, int ExportVersion) {
	int i,numtrain,numtest;
	static char cdummy[2048];
	int sel,stat,lastsel;
	double f;
	int x,y;
	time_t ttime;
	sel=0;

	numtrain = numtest = 0;
top:
	stat= fgetstr(fd,cdummy,32);
	if (stat==EOF) {
		return 0;
	}
	lastsel = sel;
	sel = dtransl(cdummy);
	switch (sel) {
	default: //error
		goto errorexit;
		break;
	case 0: //training data
		for(i=0;i<pD->m_ninputs;i++) {
			f=fgetdouble(fd);
			DSetInputVal(pD,numtrain,i,DScale(pD,f,'I',i));
		}
		for(i=0;i<pD->m_noutputs;i++) {
			f=fgetdouble(fd);
			DSetOutputVal(pD,numtrain,i,DScale(pD,f,'O',i));
		}
		numtrain++;
		break;
	case 1000: // test data
		for(i=0;i<pD->m_ninputs;i++) {
			f=fgetdouble(fd);
			DSetInputTVal(pD,numtest,i,DScale(pD,f,'I',i));
		}
		for(i=0;i<pD->m_noutputs;i++) {
			f=fgetdouble(fd);
			DSetOutputTVal(pD,numtest,i,DScale(pD,f,'O',i));
		}
		numtest++;
		break;
	case 1:
		pD->m_istate = fgetlong(fd);
		pD->m_numcols = fgetint(fd);
		pD->m_numrows = fgetint(fd);
		pD->m_ninputs = fgetint(fd);
		pD->m_noutputs = fgetint(fd);
		pD->m_rawrows = fgetint(fd);
		pD->m_rawcols = fgetint(fd);
		pD->m_total = fgetint(fd);

		if (pD->m_istate&NFnum_col_known) {
			pD->m_coldesc = (COL_DESC*) malloc (sizeof(COL_DESC)*pD->m_numcols);
			pD->m_icrossref = (int*) malloc (sizeof(int)*pD->m_numcols);
			pD->m_ocrossref = (int*) malloc (sizeof(int)*pD->m_numcols);
			for (i=0;i<pD->m_numcols;i++) {
				pD->m_icrossref[i]=0;
				pD->m_ocrossref[i]=0;
			}
			for (i=0;i<pD->m_numcols;i++) {
				pD->m_coldesc[i].fscale = 0.f;
				pD->m_coldesc[i].foffset = 0.f;
				pD->m_coldesc[i].cliphi = pD->m_coldesc[i].cliplo = MISSING;
				pD->m_coldesc[i].max = 0.f;
				pD->m_coldesc[i].min = 0.f;
				pD->m_coldesc[i].flag = 0;
				pD->m_coldesc[i].fieldtype = 0;
				pD->m_coldesc[i].col_usage = 'U';
				pD->m_coldesc[i].format[0] = 0;
				pD->m_coldesc[i].vlab[0] = 0;
				pD->m_coldesc[i].units[0] = 0;
				pD->m_coldesc[i].colwidth = 90;
			}

		}
		pD->m_istate = NFobject_exists | NFnum_col_known |
					NFtrainmat_loaded | NFi_o_col_known;
		if (pD->m_istate&NFtrainmat_loaded) {
			pD->m_icoldesc = (COL_DESC*) malloc (sizeof(COL_DESC)*pD->m_ninputs);
			pD->m_ocoldesc = (COL_DESC*) malloc (sizeof(COL_DESC)*pD->m_noutputs);
			if (pD->m_numrows) {
				pD->m_iarray = alloc_2d_doubles (pD->m_ninputs,pD->m_numrows);
				pD->m_oarray = alloc_2d_doubles (pD->m_noutputs,pD->m_numrows);
			} else pD->m_istate &= ~NFtrainmat_loaded;
		}
		break;
	case 2:
		fgetstr(fd,cdummy,MAXCSTRING);
		strcpy (pD->m_title,cdummy);
		fgetstr(fd,cdummy,MAXCSTRING);
		strcpy (pD->m_desc,cdummy);
		fgetstr(fd,cdummy,MAXCSTRING); // was previously hard-wired to 2400
		strcpy (pD->m_rawfname,cdummy); // changed by SMW on 25 April 2006
		fgetstr(fd,cdummy,MAXCSTRING);
		strcpy (pD->m_parfname,cdummy);
		ttime = fgetlong(fd);

		pD->m_istate |= (NFtitle_known    | NFdesc_known |
					 NFraw_file_known );
		break;
	case 3:
		i=fgetint(fd);
		if (ExportVersion==4) //old format
		{
			pD->m_coldesc[i].flag = fgetint(fd);
		} else {
			x = fgetint(fd);
			y = fgetint(fd);
			pD->m_coldesc[i].flag = (y<<8) | x;
		}

		pD->m_coldesc[i].fieldtype = fgetint(fd);
		pD->m_coldesc[i].fscale = fgetdouble(fd);
		pD->m_coldesc[i].foffset = fgetdouble(fd);
		pD->m_coldesc[i].max = fgetdouble(fd);
		pD->m_coldesc[i].min = fgetdouble(fd);
		fgetstr(fd,cdummy,32);
		pD->m_coldesc[i].col_usage=cdummy[0];
		fgetstr(fd,pD->m_coldesc[i].format,MAXFORMAT);
		fgetstr(fd,pD->m_coldesc[i].vlab,MAXFORMAT);

		pD->m_coldesc[i].units[0] = 0;
		pD->m_coldesc[i].cliphi = pD->m_coldesc[i].cliplo = MISSING;
		pD->m_coldesc[i].colwidth = 90;
		pD->m_istate |= NFcol_usage_known;
		break;
	case 4:
		for (i=0;i<pD->m_numcols;i++) pD->m_icrossref[i]=fgetint(fd);
		break;
	case 5:
		for (i=0;i<pD->m_numcols;i++) pD->m_ocrossref[i]=fgetint(fd);
		break;
	case 6:
		i=fgetint(fd);
		if (ExportVersion==4) //old format
		{
			pD->m_icoldesc[i].flag = fgetint(fd);
		} else {
			x = fgetint(fd);
			y = fgetint(fd);
			pD->m_icoldesc[i].flag = (y<<8) | x;
		}
		pD->m_icoldesc[i].fieldtype = fgetint(fd);
		pD->m_icoldesc[i].fscale = fgetdouble(fd);
		pD->m_icoldesc[i].foffset = fgetdouble(fd);
		pD->m_icoldesc[i].max = fgetdouble(fd);
		pD->m_icoldesc[i].min = fgetdouble(fd);
		fgetstr(fd,cdummy,32);
		pD->m_icoldesc[i].col_usage=cdummy[0];
		fgetstr(fd,pD->m_icoldesc[i].format,MAXFORMAT);
		fgetstr(fd,pD->m_icoldesc[i].vlab,MAXFORMAT);
		pD->m_icoldesc[i].units[0] = 0;
		pD->m_icoldesc[i].colwidth = 90;
		pD->m_icoldesc[i].cliphi = pD->m_icoldesc[i].cliplo = MISSING;
		pD->m_istate |= NFcol_usage_known | NFi_o_col_known;
		break;
	case 7:
		i=fgetint(fd);
		if (ExportVersion==4) //old format
		{
			pD->m_ocoldesc[i].flag = fgetint(fd);
		} else {
			x = fgetint(fd);
			y = fgetint(fd);
			pD->m_ocoldesc[i].flag = (y<<8) | x;
		}
		pD->m_ocoldesc[i].fieldtype = fgetint(fd);
		pD->m_ocoldesc[i].fscale = fgetdouble(fd);
		pD->m_ocoldesc[i].foffset = fgetdouble(fd);
		pD->m_ocoldesc[i].max = fgetdouble(fd);
		pD->m_ocoldesc[i].min = fgetdouble(fd);
		fgetstr(fd,cdummy,32);
		pD->m_ocoldesc[i].col_usage=cdummy[0];
		fgetstr(fd,pD->m_ocoldesc[i].format,MAXFORMAT);
		fgetstr(fd,pD->m_ocoldesc[i].vlab,MAXFORMAT);
		pD->m_ocoldesc[i].units[0] = 0;
		pD->m_ocoldesc[i].colwidth = 90;
		pD->m_ocoldesc[i].cliphi = pD->m_ocoldesc[i].cliplo = MISSING;
		pD->m_istate |= NFcol_usage_known | NFi_o_col_known;
		break;
	case 8: //NUMBER OF TEST ROWS
		pD->m_numtests = fgetint(fd);
		if (pD->m_numtests) {
			pD->m_itarray = alloc_2d_doubles (pD->m_ninputs,pD->m_numtests);
			pD->m_otarray = alloc_2d_doubles (pD->m_noutputs,pD->m_numtests);
			pD->m_istate |= NFtestmat_loaded;
		}
		break;
	}
	goto top;

errorexit:
	return -1;
}
