#ifndef __NNDEFS_H__
#define __NNDEFS_H__

#define VERBOSE

#define EXPORTVERSION 5
#define REVLEVEL      5

#define UNIPOLAR
#define MAXVARS     256
#define MAXPOINTS 3000
#define NRAVEPTS    64
#define MAXBUF    2400 // maximum record length in input data
#define MAXCSTRING 256

#define MAXVARWIDTH 20
#define MAXFMTSTR   10
#define MAXFORMAT    9
#define MAXSTRING   13
#define MAXFACT		32	// maximum number of factors in a design
#define MAXIMPORTDISP 12 // max # of records displayed in raw load dialog

#define LO_SEARCHCENTER  1
#define LO_SEARCHSTAR    2
#define LO_SEARCHFACTOR  4
#define LO_SEARCHBALANCE 8

#define NFobject_exists         0x1L
#define NFnum_col_known         0x2L
#define NFtrainmat_loaded       0x4L
#define NFreference_loaded      0x8L
#define NFtitle_known           0x10L
#define NFdesc_known            0x20L
#define NFraw_file_known        0x40L
#define NFcol_usage_known       0x80L
#define NFloader_known          0x100L
#define NFunscaled_loaded       0x200L
#define NFavail                 0x400L
#define NFtestmat_loaded        0x800L
#define NFrow_usage_known       0x1000L
#define NFdesigned_matrix       0x2000L
#define NFdisplaying_coded      0x4000L
#define NFi_o_col_known         0x8000L
#define NFedit_training         0x10000L

#define EDIT_LABEL			1
#define EDIT_FORMAT         2
#define EDIT_SCALE_TYPE     4
#define EDIT_SCALE          8
#define EDIT_OFFSET         16
#define EDIT_LSHIFT         32
#define EDIT_MINIMUM        64
#define EDIT_MAXIMUM        128
#define EDIT_COLUMN_USAGE   256

// coldesc.flag bit mask flag
#define SCALE_LOG     1	// scale linear if zero else log
#define EQUATION      2

	typedef union {
		long d;
		double f;
		char c[4];
	} uunion;

	typedef struct Symbol {
		char    *name;
		short   type;
		short   flag;
		short	colnum;
		union {
			double  val;
			double  (*ptr)(double);
			double  (*ptr1)(char *, double, int, void *);
		} u;
		struct Symbol *next;
	} Symbol;

	typedef struct {
		int numfact; // # of factors from design dialog
		int numresp; // # of responces from design dialog
		int dtype; // type of design matrix
		int dsel; // type of design matrix (combo selection)
		int scope; // scope of design
		int phase; // not used yet
		int ncenter;	// number of center points
		int mlevels[MAXFACT+1]; // multilevels
		int mode;
		double scale;
	} DESIGN_DESC;

	typedef struct {
		int rectype;
		double goodness;
		double fields[1];
	} SparseDataRecord;

	typedef struct {
		char rowtype;	// ie. star,simplex,factorial,center
		char rep;		// replicate number
		int seq;		// sequence number
		double goodness;	// sum total of row's goodness number
	} ROW_DESC;

	typedef struct {
		char  vlab[MAXFORMAT];   // variable label
		char  format[MAXFORMAT];   // variable format
	} LAB_DESC;

	typedef struct {
		double upperlimit;
		double lowerlimit;
		double ideal;
	} LEVEL_DESC;

	typedef struct {
		int numlevels;
		LEVEL_DESC* levels;
	} COLUMN_LEVEL;

	typedef struct {
		double fscale;   // scaling multiplier
		double foffset;  // scaling offset
		double max;
		double min;
		double cliphi;	// set to missing if value is above
		double cliplo;	// or below the clip levels
		double dfield;	// equparse data field
//		int len;      // field length
		int flag;     // equation
		int fieldtype; // type of scaling (double,number,etc)
		int colwidth;	// column width in grid
		char  field_type; // type of variable
		char  col_usage; // usage of variable I,O,S
		char  format[MAXFORMAT]; // character convertion format
		char  vlab[MAXFORMAT];   // variable label
		char  units[MAXFORMAT];   // units
		char  convstr[MAXVARWIDTH];   // converted variable
		Symbol* dfieldsym;
// the following are used in DM row matching

// possible other members
//              char description[64];
//              int color; // color of line in graph
//              int linetype; // line type in graph
//              char symbol; // character marker in graph
	} COL_DESC;


	typedef struct {
		double ave;
		double sum;
		double min;
		double max;
		double range;
		double foffset;
		double fscale;
		double count;
		double sumsq;
		int digbefore;
		int digafter;
		int fieldtype;
		int flag;
//		int len;
		unsigned int digsigned : 1;
		unsigned int digstring : 1;
		char fmt[MAXFMTSTR];
		char units[MAXFMTSTR];
		char fieldname[MAXFORMAT];
	} LOAD_DESC ;

	// Grid1 (DDL) is Optimizer I/O
	typedef struct  {
		char iotype;
		char cfreeze;
		char varlabel[10];
		char min[14];
		char max[14];
		char obj[14];
		char pred[14];
	} * LPGRID1BUF;

	// Grid2 (VBX) is input grid for interogate model class
	typedef struct  {
		char label[14];
		char value[14];
		char min[14];
		char max[14];
	} GRID2BUF;

	// Grid3 (VBX) is output grid for interogate model class
	typedef struct  {
		char label[14];
		char value[14];
	} GRID3BUF;

	typedef struct {
		char cfreeze;
		double min;			// constrain min
		double max;			// constrain max
		double obj;			// Objective function
		double pred;			// Prediction
		double limitmin;	// absolute min (from DM)
		double limitmax;	// absolute max (from DM)
		double mincostshft;
		double maxcostshft;
		double OC_limit;
		double OC_last;
		short int useminlim;
		short int usemaxlim;
		short int OC_forward;
		short int OC_missed;
		char format[10];
		} OPTIMSTR;

	typedef struct {
		double *G3darray;
		double *G2darray;
		double *G2dearray;
		double *GCarray;
		double f1,f2,f3,f4;
		double rotinc;
		double axiswidth;
		double camloc[3],xylim[6][2];
		double distance;
		double chrsiz;
		double swait;
		double cosmax;
		double width;
		double zoffset;
		double xorig,xmax;
		double yorig,ymax,zorig,zmax,zmin;
		int nobars;
		int runave;
//		int Xext,Yext,Xorg,Yorg;
//		int ylpstart,xlpstart,ylpinc,xlpinc;
		int graphic3d_flag;
		int graphic_clevels,graphic_curves;
		int Xsel,Ysel,Zsel,Esel;
		int varindx[8];
		long graphic_flag;
		char xformat[32],yformat[32],zformat[32];
#define MAXLEVELS 17
		double conlev[MAXLEVELS];
		char clab[MAXLEVELS][16];
		int ic[MAXLEVELS];
		int icn[MAXLEVELS];
		int linsty[MAXLEVELS];
	} GRAPHSTR;

	typedef struct {
		double rq  ;
		double sumins  ;
		double sumpred ;
		double summeas ;
		double sumres  ;
		double aveins  ;
		double avemeas ;
		double avepred ;
		double averes  ;
		double minins  ;
		double minmeas ;
		double minpred ;
		double minres  ;
		double maxins  ;
		double maxmeas ;
		double maxpred ;
		double maxres  ;
		double stdins  ;
		double stdmeas ;
		double stdpred ;
		double stdres  ;
		} STATS;

	double  **alloc_2d_doubles( int hi1d, int hi2d );
	void free_2d_doubles( double **array, int hi1d);


#ifdef UNIPOLAR
	// unipolar nerual function (0,1)
	#define dafunc(x) (x - x*x)
	#define afunc(x)  (double)(1.f / (1.f + exp (-x)))
#else
	// bipolar neural function (-1,1.0)
	#define dafunc(x) ( (1.f - x*x)/2.f )
	#define afunc(x) (double) (2.f / (1.f + exp (-x))-1.f)
#endif

// Training flags mask positions
#define TF_STOPONTOL 1	// stop if all TM values are within tolerance
#define TF_STOPONERR 2	// stop if total sum sq err is less then errtol
#define TF_STOPONRSQ 4	// stop if R square is higher then goodrsq
#define TF_NOINTOOUT 8	// use input to output connections
#define TF_CGOPTIM   16	// use conjugate gradient training
#define TF_CGOPTIMSA 32	// use conjugate gradient training + simulated anneal
#define TF_STDBEP    64	// use standard BEP
#define TF_BESTEPOCH 128 // Stop on best epoch
#define TF_BESTONTRAIN 256 // Stop on best epoch use training matrix
#define TF_BESTONTEST  512 // Stop on best epoch use test matrix
#define TF_BESTONBOTH  1024 // Stop on best epoch use both training and test matrix
#define TF_CBPHIDDEN   2048 // Enable circular back prop on hidden layer
#define TF_CBPOUTPUT   4096 // Enable circular back prop on output layer




// 2D & 3D graphs
#define NUMPOINTS  256
#define NUMEFFECTS 10
#define N3DPOINTS 20  // # of divisions on 3d graph

#define DT_TWOLEVEL			1
#define DT_THREELEVEL		2
#define DT_MULTILEVEL		4
#define DT_SIMPLEX			8
#define DT_STARSIMPLEX		16
#define DT_CENTRALCOMP		32
#define DT_MIXTURE			64

// defines for design options m_design.mode
#define DO_ROTATE			1
#define DO_FACECENTERED		2
#define DO_USERSCALED		4
#define DO_AUTOLOAD			8
#define DO_KEEPREJECTS		16
#define DO_USEOLDDATA		32
#define DO_AUTORESCALE		64
#define DO_ADVISOR			128
#define DO_SETFORMAT		256


// defines for print options
#define PO_HORIZ		1
#define PO_VERT			2
#define PO_TITLE		4
#define PO_DATE			8
#define PO_PAGE			16
#define PO_RANDOM		32

// coldesc.fieldtypes
#define FIELDTYPE_double			0
#define FIELDTYPE_STRING		1
#define FIELDTYPE_TIMESTAMP		2
#define FIELDTYPE_RECORDTYPE	3
#define FIELDTYPE_TIMEONLY		4
#define FIELDTYPE_DATEONLY		5

	typedef union  {
			double val;
			Symbol *sym;
		} YYSTYPE;


#endif
