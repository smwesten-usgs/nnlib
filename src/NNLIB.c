// NNLIB.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <R.h>
#include <Rdefines.h>
#include "NNDEFS.H"
#include "DATAMAT.H"
#include "PARAMS.H"
#include "NEURAL.H"

//	NEURAL *tneural;
//	void Logit(const char* fmt, ...);
//  FILE *fd;
//    double *Ivec,*Ovec;
//  char buf[128], fname[128], m1[16],m2[16];
//	int i;

/*int main(int argc, char* argv[])*/

void nnpredict(char **mod_name, double *in_vec, double *out_vec, int *echo)
{
    NEURAL *tneural;
    int i;

    if( *echo > 0 ) {
                Rprintf ("mod_name[0]: %s \n",mod_name[0]);
        }

    tneural = LoadNetwork(mod_name[0]);

    if (tneural == NULL) {
   		Rprintf ("Error loading file with name %s",mod_name[0]);
   		return;
   	}

//    if (tneural->m_ninputs != *num_in) {
//   		Rprintf ("Error loading model %s: ",mod_name[0]);
//        Rprintf ("  Model file specifies %i input nodes.",tneural->m_ninputs);
//        Rprintf ("  You specified %i input nodes.",*num_in);
//   		return;
//   	}

   	if( *echo > 0 ) {
            Rprintf("  Line 51, nnlib.c: about to echo print the input vector...\n");
   	    for (i=0;i<tneural->m_ninputs;i++) {
            Rprintf("  input: %20s = \t%12.3f\n",tneural->m_dm->m_icoldesc[i].vlab,
                in_vec[i]);
        }
    }

    //fd = fopen("itest.dmp","w");
    //DumpNeural(tneural,fd);
    //fclose (fd);

/*	Ivec = (double*) malloc (sizeof(double)*tneural->m_ninputs);
	Ovec = (double*) malloc (sizeof(double)*tneural->m_noutputs);

    Ivec = in_vec;
    Ovec= out_vec;*/

/*	for (i=0;i<tneural->m_ninputs;i++) {
		sprintf (m1,tneural->m_dm->m_icoldesc[i].format,tneural->m_dm->m_icoldesc[i].min);
		sprintf (m2,tneural->m_dm->m_icoldesc[i].format,tneural->m_dm->m_icoldesc[i].max);
		printf ("Enter %8s (range %10s to %10s ) = ",tneural->m_dm->m_icoldesc[i].vlab,m1,m2);
		gets (buf);
		if ((buf[0]=='E') || (buf[0]=='e')) goto getout;
		sscanf (buf,"%f",&Ivec[i]);
	}*/

	NInterrogate(tneural, in_vec, out_vec);
/*	for (i=0;i<tneural->m_noutputs;i++) {
		sprintf (m1,tneural->m_dm->m_ocoldesc[i].format,Ovec[i]);
		printf ("\n%8s = %10s",tneural->m_dm->m_ocoldesc[i].vlab,m1);
	}
	printf ("\n\n");*/

    NDeleteNeural(tneural);

	return;
}

void nninfo(char** mod_name, int* num_in, int* num_out, int* num_hidden)
{
    NEURAL *tneural;
    int i;

    tneural = LoadNetwork(mod_name[0]);

    if (tneural == NULL) {
   		Rprintf ("Error loading file with name %s",mod_name[0]);
   		return;
   	}

    *num_in = tneural->m_ninputs;
    *num_out = tneural->m_noutputs;
    *num_hidden = tneural->m_nhidden;

    NDeleteNeural(tneural);

    return;
}


void nnpredictTS(char** mod_name, int* num_dyn_fields, int* num_dyn_recs,   \
             double* in_dyn_vec, int* num_static_fields,                    \
             double* in_static_vec, double* out_ts_vec, int* echo,          \
			       int* order)
{

    NEURAL *tneural;
//	FILE *fd;
    int fieldno, recno, in_offset, out_offset, n;
    int num_in;
    double* out_vec;
    double* in_vec;
	double* temp_vec;
//    double sum = 0, average = 0;

    num_in = *num_dyn_fields + *num_static_fields;

    if( *echo > 0 ) {
	    Rprintf ("mod_name[0]: %s \n",mod_name[0]);
        }

    tneural = LoadNetwork(mod_name[0]);

    if (tneural == NULL) {
   		Rprintf ("Error loading file with name %s",mod_name[0]);
   		return;
   	}

    if (tneural->m_ninputs != num_in) {
   	Rprintf ("Error loading model %s: ",mod_name[0]);
        Rprintf ("  Model file specifies %i input nodes.",tneural->m_ninputs);
        Rprintf ("  You specified %i input nodes.",num_in);
   	return;
    }

  out_vec = (double*) malloc (sizeof(double)*tneural->m_noutputs);
  in_vec = (double*) malloc (sizeof(double)*tneural->m_ninputs);
  temp_vec = (double*) malloc (sizeof(double)*tneural->m_ninputs);

    for(recno=0; recno < *num_dyn_recs; recno++) {
        in_offset= *num_dyn_fields * recno;

        // populate the input static variables
        for(fieldno=0; fieldno < *num_static_fields; fieldno++) {

            temp_vec[fieldno] =  in_static_vec[fieldno];

        }

        // tack on the dynamic inputs to the ANN
        for( fieldno=0; fieldno < *num_dyn_fields; fieldno++ ) {

            temp_vec[fieldno + *num_static_fields] = in_dyn_vec[in_offset + fieldno];

        }

		// now allow for reordering of the columns of input data
		for ( fieldno=0; fieldno < tneural->m_ninputs; fieldno++) {

		    in_vec[fieldno] = temp_vec[order[fieldno]];

		}

        NInterrogate(tneural,in_vec,out_vec);

        for ( fieldno=0; fieldno < tneural->m_noutputs; fieldno++ ) {
            out_offset = tneural->m_noutputs * recno;
            out_ts_vec[out_offset + fieldno]= out_vec[fieldno];
//            n++;
//            sum+=out_ts_vec[out_offset+j];
		}

            if( *echo > 0 ) {

              Rprintf ("\ntime series record %i of %i:\n",recno+1, *num_dyn_recs);

              for (fieldno=0; fieldno < tneural->m_noutputs; fieldno++) {

                out_offset= tneural->m_noutputs * recno;
                Rprintf ("\ttneural->m_noutputs: %i \n",tneural->m_noutputs);
                Rprintf ("\trecno: %i \t fieldno: %i\n",recno, fieldno);
                Rprintf ("\tout_offset = \t%i\n",out_offset);
                Rprintf ("\tout_vec:: \t%8s = %16.3f\n",tneural->m_dm->m_ocoldesc[fieldno].vlab,\
                         out_vec[fieldno]);
                Rprintf ("\tout_ts_vec:: \t%8s = %16.3f\n",tneural->m_dm->m_ocoldesc[fieldno].vlab,\
                         out_ts_vec[out_offset + fieldno]);
                Rprintf ("----------------------------------------------------------------------\n");

              }

              Rprintf ("\n\n");

            }

        }

        NDeleteNeural(tneural);

        free(in_vec);
        free(out_vec);
//    free(out_ts_vec);

	return;
}
