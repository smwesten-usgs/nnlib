#ifndef __CNEURAL_H__
#define __CNEURAL_H__

#define NN_DYNAMIC		1
#define NN_DATAMAT		2
#define NN_PARAMSLOADED	4
#define NN_INPUTCHANGED	8
#define NN_NETINVALID	16

#define NN_STHRU	// Straight through connections

	struct _neural {
		long m_version;   // version
		int m_istate;
		int m_ninputs;
		int m_nhidden;
		int m_noutputs;
		int m_pinning;

		double *m_ni;  /* declare input neurons */
		double *m_nout;    // output of output layer
		double *m_ocircw;
		double *m_olastoutputv;   // output of output

// declare hidden layer neurons
		double **m_hinputw;   // weight of input to hidden
		double *m_houtputv;   // output of hidden
		double *m_hlastoutputv;   // output of hidden
		double *m_htheta;
		double *m_hcircw;
// declare output neurons
		double **m_oinputw;   // weight of hidden to output
		double *m_otheta;

#ifdef NN_STHRU
		double **m_iinputw;   // weight of input to output
#endif

// Now for the dynamic variables

		PARAMS *m_params;
		DATAMAT *m_dm;

// declare hidden layer neurons
		double **m_hlastdelta; // last delta weight of input/hidden
		double *m_hlastvar;    // last VAR
		double *m_hlearn;
		double *m_htlearn;
// declare output neurons
		double **m_olastdelta; // last delta weight of hidden/output
#ifdef NN_STHRU
		double **m_ilastdelta; // last delta weight of input/output
#endif
		double *m_olastvar;    // last var weight of output
		double *m_otraining;
		double *m_olearn;
		double *m_otlearn;

		double *m_startp;
		int m_NDIM;
		double *m_pcom;
		double *m_xicom;
		double *m_xt;
		double *m_g,*m_h,*m_xi;
		double m_sumerr2;
		long m_cnt;
		double m_stats[7];
		int m_itmax; // # of iterations in CG optim
	};
	typedef struct _neural NEURAL;

	NEURAL *NCreateNeural();
	void NDeleteNeural(NEURAL *pN );
	int NImportNetwork (NEURAL *pN, FILE *fd);
	NEURAL *LoadNetwork (char *filename);
	void DumpNeural(NEURAL *pN, FILE *fd);
	void NSetPinning(NEURAL *pN, int val);

	void NAddHidden(NEURAL *pN);
	void NFeedForward(NEURAL *pN);
	int NBackProp1(NEURAL *pN,int cnt);
	void NClearDelta(NEURAL *pN);
	void NInitializeNetwork(NEURAL *pN);
	void NNewTrainingSet(NEURAL *pN, int t,int flag);
	int NAI(NEURAL *pN, int flag,int a);
	double NCalcRsquare(NEURAL *pN);
	double NCheckTol(NEURAL *pN,double *min, double *max, int *nmin, int *nmax);
	int NQuickTrain(NEURAL *pN,int mode,int increment);

// Conj Grad routines
	void Nfrprmn(NEURAL *pN,double *p, double ftol, int *iter, double *fret);
	double Nbrent(NEURAL *pN,double ax, double bx, double cx, double tol,double *xmin);
	void Nmnbrak(NEURAL *pN,double *ax, double *bx, double *cx, double *fa, double *fb,double *fc);
	double Nf1dim(NEURAL *pN,double x);
	void Nlinmin(NEURAL *pN,double *p, double *xi, double *fret);
	void NforwardDIffGradient (NEURAL *pN,double *x0, double *g);
	double NErrorFunction(NEURAL *pN,double* x);
	void Nrerror(char *error_text);
	void NCGTrain(NEURAL *pN);
	void NSetDM( NEURAL *pN, DATAMAT *a);
	void NInterrogate(NEURAL *pN,double *Ivec,double *Ovec);

/*
	void SetInput(const int neuron, double f);
	void SetRInput(const int neuron, double f);
	double GetInput(const int neuron);
	double GetOutput(const int neuron);
	double GetTrain(const int neuron);
	double GetRInput(const int neuron);
	double GetROutput(const int neuron);
	double GetRTrain(const int neuron);

*/

#endif // __CNEURAL_H__


