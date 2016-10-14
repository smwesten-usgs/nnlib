/*
**
**  File:           PARAMS.C
**  Description:    Loaded and holds the parameters of a neural network
**  Platform:       Windows
**
**
*/
//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NNDEFS.H"
#include "PARAMS.H"

	void Logit(const char* fmt, ...);
//	int fgetstr (FILE *fd, LPSTR str, int bufsize);
	int fgetstr (FILE *fd, char *str, int bufsize);
	int fgetint(FILE *fd);
	long fgetlong(FILE *fd);
	double fgetdouble(FILE *fd);

/*
** CreateParams
**
** This function is called to create a new params structure and initialize it to the
** default settings
**
**
** Arguments:
**
**      None
**
** Returns:
**
**      PARAMS			returns a pointer to the newly created PARAMS structure
*/

PARAMS *CreateParams( )
{
	PARAMS *pM;
	pM = (PARAMS *) malloc (sizeof(PARAMS));

	pM->m_TrainFlags=0;
	pM->m_AImaxhid=4;
	pM->m_goodness=3;
	pM->m_autosave = 1000;
	pM->m_seed = 15;
	pM->m_eon = 100;

	pM->m_cnt_max = 1000;

	pM->m_hiddegrad=0.75f;
	pM->m_errtol = 0.001f;
	pM->m_goodrsq = .9f;
	pM->m_signinc = .05f;
	pM->m_nosigninc = .005f;
	pM->m_alpha = .8f;
	pM->m_theta = .5f;
	pM->m_randz = .5f;
	pM->m_inrandzdiv = 0.f;
	pM->m_tol = .05f;
	pM->m_learning_rate = .75f;
	pM->m_Hlearning_rate = 1.5f;
	pM->m_tlearning_rate = .75f;
	pM->m_inoutlearn = .1f;

	return pM;
}

/*
** ImportParams
**
** This function is called to read in a params structure from an ENN file
**
**
** Arguments:
**
**      FILE *fd  		A pointer to the open ENN file
**      PARAMS *pM		A pointer to the blank params sructure
**      int *ver		A pointer to put the ENN version number
**
** Returns:
**
**      int			returns a zero if okay otherwise a -1
*/

int ImportParams (FILE *fd, PARAMS *pM, int *ExportVersion)
{
	static char cdummy[80];
	int ex;

	fgetstr(fd,cdummy,80);
	if (strncmp("P00",cdummy,3)!=0) goto errorexit;
	ex = fgetint(fd);
	*ExportVersion = ex;
//	if (ex != EXPORTVERSION)

	fgetstr(fd,cdummy,80);
	if (strncmp("P01",cdummy,3)!=0) goto errorexit;
	pM->m_TrainFlags = fgetint(fd);
	pM->m_AImaxhid   = fgetint(fd);
	pM->m_goodness   = fgetint(fd);
	pM->m_autosave   = fgetint(fd);
	pM->m_seed       = fgetint(fd);
	pM->m_eon        = fgetint(fd);

	fgetstr(fd,cdummy,80);
	if (strncmp("P02",cdummy,3)!=0) goto errorexit;
	pM->m_cnt_max = fgetlong(fd);
	pM->m_hiddegrad = fgetdouble(fd);
	pM->m_errtol    = fgetdouble(fd);
	pM->m_goodrsq  = fgetdouble(fd);
	pM->m_signinc   = fgetdouble(fd);
	pM->m_nosigninc  = fgetdouble(fd);
	pM->m_alpha      = fgetdouble(fd);

	fgetstr(fd,cdummy,80);
	if (strncmp("P03",cdummy,3)!=0) goto errorexit;
	pM->m_theta     = fgetdouble(fd);
	pM->m_randz     = fgetdouble(fd);
	pM->m_inrandzdiv = fgetdouble(fd);
	pM->m_tol        = fgetdouble(fd);
	pM->m_learning_rate = fgetdouble(fd);
	pM->m_Hlearning_rate = fgetdouble(fd);
	pM->m_tlearning_rate = fgetdouble(fd);
	pM->m_inoutlearn = fgetdouble(fd);
	return 0;

errorexit:
	return -1;

}
