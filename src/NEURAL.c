/*
**
**  File:           NEURAL.C
**  Description:    Executes a back propagation neural network
**  Platform:       Windows
**
**
*/

#include <math.h>
//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// CLANG does not appear to need the malloc.h header
#ifndef __clang__
#include <malloc.h>
#endif

#include "NNDEFS.H"
#include "DATAMAT.H"
#include "PARAMS.H"
#include "NEURAL.H"

	extern union {
		double f;
		long l;
		} MissingUnion;
#define MISSING (MissingUnion.f)

#define CIRCULAR

	double Random(double randz);
	static char FMT_E[] = "%e ";

//	int fgetstr (FILE *fd, LPSTR str, int bufsize);
	int fgetstr (FILE *fd, char *str, int bufsize);
	int fgetint(FILE *fd);
	long fgetlong(FILE *fd);
	double fgetdouble(FILE *fd);

/*
** NCreateNeural
**
** This function is called to create a neural network. Only the shell of the network
** is created. To complete the creation of the network the import network function
** will create the other members.
**
**
** Arguments:
**
**      None
**
** Returns:
**
**      NEURAL		A pointer to the created neural network or a NULL if not created
*/

NEURAL *NCreateNeural(  )
{
	NEURAL *pN;
	pN = (NEURAL*) malloc (sizeof(NEURAL));

	pN->m_version = REVLEVEL;
    pN->m_istate = pN->m_ninputs = pN->m_noutputs = pN->m_nhidden = 0;
    pN->m_params = NULL;
	pN->m_sumerr2 = 0.f;
	pN->m_cnt = 0;
	pN->m_itmax = 2;
	pN->m_dm=NULL;
	pN->m_pinning = 0;

	pN->m_ni	= NULL;
	pN->m_nout	= NULL;
	pN->m_hinputw	= NULL;
	pN->m_houtputv	= NULL;
	pN->m_htheta	= NULL;
	pN->m_oinputw	= NULL;
	pN->m_otheta	= NULL;

#ifdef CIRCULAR
	pN->m_hcircw	= NULL;
	pN->m_ocircw	= NULL;
#endif
	return pN;

}

/*
** NDeleteNeural
**
** This function is called to delete a neural network. All memory used by the network
** is freed.
**
**
** Arguments:
**
**      Neural *pN		A pointer to the neural network to be deleted
**
** Returns:
**
**      Nothing
*/

void NDeleteNeural(NEURAL *pN )
{

	if (pN->m_istate&NN_PARAMSLOADED) {
    	free (pN->m_params);
    }

	if (pN->m_noutputs) {
          free_2d_doubles (pN->m_hinputw, pN->m_nhidden);
          free_2d_doubles (pN->m_oinputw, pN->m_noutputs);
          free_2d_doubles (pN->m_iinputw, pN->m_noutputs);
          free (pN->m_ni);
          pN->m_ni=NULL;
          free (pN->m_houtputv);
          pN->m_houtputv=NULL;
          free (pN->m_htheta);
          pN->m_htheta=NULL;
          free (pN->m_otheta);
          pN->m_otheta=NULL;
          free (pN->m_nout);
          pN->m_nout=NULL;
#ifdef CIRCULAR
          free (pN->m_ocircw);
          pN->m_ocircw=NULL;
          free (pN->m_hcircw);
          pN->m_hcircw=NULL;
#endif
	}
	if (pN->m_istate&NN_DYNAMIC) {
          if(pN->m_noutputs) {
            free_2d_doubles (pN->m_hlastdelta, pN->m_nhidden);
            free_2d_doubles (pN->m_olastdelta,pN->m_noutputs);
            free_2d_doubles (pN->m_ilastdelta,pN->m_noutputs);
            if (pN->m_hlastvar != NULL) {
              free (pN->m_hlastvar);
              pN->m_hlastvar=NULL;
            }
            if (pN->m_hlearn != NULL) {
              free (pN->m_hlearn);
              pN->m_hlearn=NULL;
            }

            free (pN->m_htlearn);
            pN->m_htlearn=NULL;
            free (pN->m_olastvar);
            pN->m_olastvar=NULL;
            free (pN->m_otraining);
            pN->m_otraining=NULL;
            free (pN->m_olearn);
            pN->m_olearn=NULL;
            free (pN->m_otlearn);
            pN->m_otlearn=NULL;
          }
        }
        if (pN->m_istate&NN_DATAMAT) {
        DDeleteDataMat(pN->m_dm);
        }
        if (pN != NULL) {
          free (pN);
          pN=NULL;
        }

    return;
}


/*
** NGetROutput
**
** This function is called to get the re-scaled value of the neural network's output.
** The re-scaled value is the number as the user presented to the network. The network
** scales all numbers to a range of 0.2 to 0.8, where as the inputs and outputs to the
** neural network can be of any range.
**
**
** Arguments:
**
**      NEURAL *pN      	A pointer to the neural network structure
**      const int neuron	The index to the desired output
**
** Returns:
**
**      double		The value of the selected neural network's output
*/

double NGetROutput(NEURAL *pN, const int neuron)
{
    if (pN->m_istate & NN_NETINVALID) return MISSING;
	return (DRescale(pN->m_dm, pN->m_nout[neuron],'O',neuron));
}

/*
** NGetRInput
**
** This function is called to get the re-scaled value of the neural network's input.
** The re-scaled value is the number as the user presented to the network. The network
** scales all numbers to a range of 0.2 to 0.8, where as the inputs and outputs to the
** neural network can be of any range.
**
**
** Arguments:
**
**      NEURAL *pN      	A pointer to the neural network structure
**      const int neuron	The index to the desired input
**
** Returns:
**
**      double		The value of the selected neural network's input
*/

double NGetRInput(NEURAL *pN, const int neuron)
{
	return (DRescale(pN->m_dm, pN->m_ni[neuron],'I',neuron));
}

/*
** NSetRInput
**
** This function is called to set the value of the neural network's input.
** The value is scaled to the range that is optimal for this network.
**
**
** Arguments:
**
**      NEURAL *pN      	A pointer to the neural network structure
**      const int neuron	The index to the desired input
**
** Returns:
**
**      Nothing
*/
/*    If PINNING is set to 0 then the network ignores the input validation step and predicts regardless
**    If PINNING is set to 1 then if any input is outside of the observed range then the network
**               will be set to INVALID
**    If PINNING is set to 2 then the invalid inputs are pinned to the max or min observation
**    If PINNING is set to 3 then the invalid inputs are set to 0.5 or neutral
*/

void NSetRInput(NEURAL *pN, const int neuron, double inval)
{
	pN->m_istate |= NN_INPUTCHANGED;

	if (inval == MISSING) {
		pN->m_ni[neuron] = 0.5f;
		return;
	}
	switch (pN->m_pinning) {
		default:
		case 0:
			break;
		case 1:
			if (inval > pN->m_dm->m_icoldesc[neuron].max) goto inputinvalid;
			if (inval < pN->m_dm->m_icoldesc[neuron].min) goto inputinvalid;
			break;
		case 2:
			if (inval > pN->m_dm->m_icoldesc[neuron].max) inval = pN->m_dm->m_icoldesc[neuron].max;
			if (inval < pN->m_dm->m_icoldesc[neuron].min) inval = pN->m_dm->m_icoldesc[neuron].min;
			break;
		case 3:
			if (inval > pN->m_dm->m_icoldesc[neuron].max) inval =  0.5f;
			if (inval < pN->m_dm->m_icoldesc[neuron].min) inval =  0.5f;
			break;
	} // end of switch
	pN->m_ni[neuron] = DScale(pN->m_dm,inval,'I',neuron);
	return;
inputinvalid:
	pN->m_ni[neuron] = 0.5f;
	pN->m_istate |= NN_NETINVALID;
}

/*
** NFeedForward
**
** This function is called to evaluate the selected neural network. The network inputs
** must of been already loaded. After calling this function the network's outputs can
** be retrived.
**
**
** Arguments:
**
**      NEURAL *pN      	A pointer to the neural network structure
**
** Returns:
**
**      Nothing
*/

void NFeedForward(NEURAL *pN)
{
	int i,j;
	double sum1;
#ifdef CIRCULAR
	double csum;
#endif

	pN->m_istate &= ~NN_INPUTCHANGED;
	if (pN->m_istate & NN_NETINVALID) {
	    pN->m_istate &= ~NN_NETINVALID;
		return;
	}
    pN->m_istate &= ~NN_NETINVALID;
// calculate the outputs of the hidden layer

	for (i=0; i < pN->m_nhidden; i++) {
		for (sum1=0.f,j=0; j < pN->m_ninputs; j++)
			sum1 += (pN->m_ni[j] * pN->m_hinputw[i][j] );
		sum1 += pN->m_htheta[i];
#ifdef CIRCULAR
		if (pN->m_params->m_TrainFlags&TF_CBPHIDDEN) {
			for (csum=0.f,j=0; j < pN->m_ninputs; j++) {
				csum += (pN->m_ni[j] * pN->m_ni[j] );
			}
			sum1 += (csum * pN->m_hcircw[i]);
		}
#endif
		pN->m_houtputv[i] = afunc (sum1);
	}

// connect hidden layer 1 to output neurons

	for (j=0; j < pN->m_noutputs; j++) {
		for (sum1=0.f,i=0; i < pN->m_nhidden; i++)
			sum1 += (pN->m_houtputv[i] * pN->m_oinputw[j][i]);
		if (pN->m_params->m_TrainFlags & TF_NOINTOOUT) {
			for (i=0; i < pN->m_ninputs; i++)
				sum1 += (pN->m_ni[i] * pN->m_iinputw[j][i]);
		}
		sum1 += pN->m_otheta[j];
#ifdef CIRCULAR
		if (pN->m_params->m_TrainFlags&TF_CBPOUTPUT) {
			for (csum=0.f,i=0; i < pN->m_ninputs; i++) {
				csum += (pN->m_ni[i] * pN->m_ni[i] );
//				TRACE ("hi=%e csum=%e ocircw=%e\n",m_houtputv[i],csum,m_ocircw[j]);
			}
			sum1 += (csum * pN->m_ocircw[j]);
		}
#endif
		pN->m_nout[j] = afunc (sum1);
	}
}

/*
** ntransl
**
** This function is used internally by the import neural network function.
**
**
** Arguments:
**
**      char *cdummy      	A pointer to a character string
**
** Returns:
**
**      int		returns the tranlated string or a -1 if the string doesn't start with 'N'
*/

int ntransl(char *cdummy)
{
	int val=0;

	if (cdummy[0] != 'N') return -1;
	val = atoi(&cdummy[1]);
	return val;
}

/*
** NImportNetwork
**
** This function is called to import a neural network from an ENN file. This file is
** an ASCII exported file from NNMODEL.
**
**
** Arguments:
**
**      NEURAL *pN      	A pointer to the already created network
**      FILE *fd			A pointer to a opened ENN file
**
** Returns:
**
**      int		returns a zero if the network was imported without error. A -1 is return on error.
*/

int NImportNetwork (NEURAL *pN, FILE *fd)
{
	int i,j,sel,stat,h,o;
	static char cdummy[80];
	int ExportVersion;

	stat=ImportParams(fd,pN->m_params, &ExportVersion);
	if (stat) return stat;
	pN->m_sumerr2 = 0.f;

top:
	stat = fgetstr(fd,cdummy,80);
	if (stat==EOF) goto errorexit;


	sel = ntransl(cdummy);
	switch (sel) {
	case 99:
		pN->m_dm = DCreateDataMat();
		stat=DImportDataMat(pN->m_dm,fd,ExportVersion);
		return stat;

		break;
	default:
		goto errorexit;
		break;
	case 1:
		pN->m_istate = fgetint(fd);
		pN->m_ninputs = fgetint(fd);
		pN->m_nhidden = fgetint(fd);
		pN->m_noutputs = fgetint(fd);
		pN->m_cnt = fgetlong(fd);
		pN->m_istate |= NN_PARAMSLOADED;
		pN->m_ni = (double*) malloc (sizeof(double)*pN->m_ninputs);
		pN->m_houtputv = (double*) malloc (sizeof(double)*pN->m_nhidden);
		pN->m_htheta = (double*) malloc (sizeof(double)*pN->m_nhidden);
		pN->m_otheta = (double*) malloc (sizeof(double)*pN->m_noutputs);
		pN->m_nout = (double*) malloc (sizeof(double)*pN->m_noutputs);
		pN->m_hinputw = alloc_2d_doubles (pN->m_nhidden,pN->m_ninputs);
		pN->m_oinputw = alloc_2d_doubles (pN->m_noutputs,pN->m_nhidden);
		pN->m_iinputw = alloc_2d_doubles (pN->m_noutputs,pN->m_ninputs);
		pN->m_hcircw = (double*) malloc (sizeof(double)*pN->m_nhidden);
		pN->m_ocircw = (double*) malloc (sizeof(double)*pN->m_noutputs);
		for (i=0;i<pN->m_nhidden;i++) pN->m_hcircw[i]=0;
		for (i=0;i<pN->m_noutputs;i++) pN->m_ocircw[i]=0;

		if (pN->m_istate&NN_DYNAMIC) {
			pN->m_hlastvar      = (double*) malloc (sizeof(double)*pN->m_nhidden);
			pN->m_hlearn        = (double*) malloc (sizeof(double)*pN->m_nhidden);
			pN->m_htlearn       = (double*) malloc (sizeof(double)*pN->m_nhidden);
			pN->m_olastvar      = (double*) malloc (sizeof(double)*pN->m_noutputs);
			pN->m_otraining     = (double*) malloc (sizeof(double)*pN->m_noutputs);
			pN->m_olearn        = (double*) malloc (sizeof(double)*pN->m_noutputs);
			pN->m_otlearn       = (double*) malloc (sizeof(double)*pN->m_noutputs);
			pN->m_hlastdelta    = alloc_2d_doubles (pN->m_nhidden,pN->m_ninputs);
			pN->m_olastdelta    = alloc_2d_doubles (pN->m_noutputs,pN->m_nhidden);
			pN->m_ilastdelta    = alloc_2d_doubles (pN->m_noutputs,pN->m_ninputs);

		}

		for (i=0;i<pN->m_ninputs;i++) pN->m_ni[i] = 0.f;
		for (h=0;h<pN->m_nhidden;h++) {
			for (i=0;i<pN->m_ninputs;i++) pN->m_hinputw[h][i] = 0.f;
			pN->m_houtputv[h] = pN->m_htheta[h] = 0.f;
		}
		for (o=0;o<pN->m_noutputs;o++) {
			for (h=0;h<pN->m_nhidden;h++) pN->m_oinputw[o][h] = 0.f;
			for (i=0;i<pN->m_ninputs;i++) pN->m_iinputw[o][i] = 0.f;
			pN->m_otheta[o] = pN->m_nout[o] = 0.f;
		}
		break;
	case 2:
		for (i=0;i<pN->m_ninputs;i++)
			for (j=0;j<pN->m_nhidden;j++) pN->m_hinputw[j][i]=fgetdouble(fd);
		break;
	case 3:
		for (i=0;i<pN->m_nhidden;i++) pN->m_htheta[i]=fgetdouble(fd);
		break;
	case 4:
		for (i=0;i<pN->m_noutputs;i++)
		for (j=0;j<pN->m_nhidden;j++) pN->m_oinputw[i][j]=fgetdouble(fd);
		break;
	case 5:
		for (i=0;i<pN->m_noutputs;i++) pN->m_otheta[i]=fgetdouble(fd);
		break;
	// Now load data for the dynamic part of CNeural class
	case 6:
		for (i=0;i<pN->m_nhidden;i++) pN->m_hlastvar[i]=fgetdouble(fd);
		break;
	case 7:
		for (i=0;i<pN->m_nhidden;i++) pN->m_hlearn[i]=fgetdouble(fd);
		break;
	case 8:
		for (i=0;i<pN->m_nhidden;i++) pN->m_htlearn[i]=fgetdouble(fd);
		break;
	case 9:
		for (i=0;i<pN->m_noutputs;i++) pN->m_olastvar[i]=fgetdouble(fd);
		break;
	case 10:
		for (i=0;i<pN->m_noutputs;i++) pN->m_olearn[i]=fgetdouble(fd);
		break;
	case 11:
		for (i=0;i<pN->m_noutputs;i++) pN->m_otlearn[i]=fgetdouble(fd);
		break;
	case 12:
		for (i=0;i<pN->m_noutputs;i++)
			for (j=0;j<pN->m_ninputs;j++) pN->m_iinputw[i][j]=fgetdouble(fd);
		break;
#ifdef CIRCULAR
	case 13:
		for (i=0;i<pN->m_nhidden;i++) pN->m_hcircw[i] = fgetdouble(fd);
		for (i=0;i<pN->m_noutputs;i++) pN->m_ocircw[i] = fgetdouble(fd);
		break;
#endif
	}
	goto top;

errorexit:
	return -1;
}

/*
** LoadNetwork
**
** This function is called to create and import a neural network from an ENN file.
** This file is an ASCII exported file from NNMODEL.
**
**
** Arguments:
**
**      char *filename     	A pointer to a characer string containing the file name
**
** Returns:
**
**      NEURAL		returns a pointer to the imported network or a NULL if tere was an error
*/

NEURAL *LoadNetwork (char *filename) {
	FILE *fd;
	int stat;
	PARAMS *tparams;
	NEURAL *tneural;

	fd = fopen(filename,"r");
	if (fd == NULL) {
		return NULL;
	}

	tparams = (PARAMS*) malloc (sizeof(PARAMS));
	tneural = (NEURAL*) malloc (sizeof(NEURAL));
	tneural->m_params = tparams;
	tneural->m_istate |= NN_PARAMSLOADED;
	stat = NImportNetwork(tneural,fd);
	fclose(fd);


	if (stat) {
		NDeleteNeural (tneural);
        	return NULL;
	}
	return tneural;
}

/*
** NInterrogate
**
** This function is called to load a neural network's inputs, evaluate the network
** and then retrieves the network's outputs
**
**
** Arguments:
**
**      NEURAL *pN      	A pointer to the already created network
**      double *Ivec			A pointer to an array of input values
**      double *Ovec			A pointer to an array of output values
**
** Returns:
**
**      Nothing
*/

void NInterrogate(NEURAL *pN, double *Ivec, double *Ovec)
{
	int i;

	for (i=0; i < pN->m_ninputs; i++) pN->m_ni[i] = DScale(pN->m_dm,Ivec[i],'I',i);
	NFeedForward(pN);
    for (i=0; i < pN->m_noutputs; i++) Ovec[i] = DRescale(pN->m_dm,pN->m_nout[i],'O',i);
}

void NSetPinning(NEURAL *pN, int val)
{
	pN->m_pinning = val;
}
