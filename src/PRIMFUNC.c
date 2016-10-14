/*
**
**  File:           PRIMFUNC.C
**  Description:    Contains general purpose primitive routines
**  Platform:       Windows
**
**
*/


//#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NNDEFS.H"
#include "DATAMAT.H"
#include "PARAMS.H"
#include "NEURAL.H"

#include <ctype.h>
//#include <math.h>
//#include <string.h>
#include <stdarg.h>
//#include <stdlib.h>
//#include "nndefs.h"

// ** These variables are never used in this context...
//	static char msg1[] = "Bad dimensions in alloc_2d_doubles()\n";
//	static char msg2[] = "Can't alloc storage for dimension 1 in alloc_2d_doubles()\n";
//	static char msg3[] = "Can't alloc storage for dimension 2 in alloc_2d_doubles()\n";
//	static char msg4[] = "Bad dimensions in free_2d_double()\n";

	union {
		double f;
		long l;
		} MissingUnion;
#define MISSING (MissingUnion.f)

/*
** fgetstr
**
** This function is called to read in a string from an open file. Any ASCII character
** less than or equal to a blank will terminal the string.
**
**
** Arguments:
**
**      FILE *fd	A pointer to a open file
**      LPSTR str	A pointer to a buffer to put the read in string
**      int size    Integer size of buffer
**
** Returns:
**
**      int			returns a zero if there were no errors
*/
/*
int fgetstr (FILE *fd, LPSTR str){
	int gs;
	LPSTR ps = str;
	gs = fgetc(fd);
	while (gs < 33) {
		if (gs<0) return gs;
		gs = fgetc(fd);
	}
	while (gs > 32) {
		*str=gs;
		str++;
		gs = fgetc(fd);
	}
	*str=0;
	if (gs < 0) return gs;
	else return 0;
}
*/

//int fgetstr (FILE *fd, LPSTR str, int bufsize){
int fgetstr (FILE *fd, char *str, int bufsize){
	char gs;
	char startquote=0;

	// 19991013 DAT
	// Commented line out due to compiler warning
	// warning C4189: 'ps' : local variable is initialized but not referenced
	//
	// LPSTR ps = str;

	gs = fgetc(fd);
	while (gs < 33) {
		if (gs<0) return gs;
		gs = fgetc(fd);
	}
	while ((gs > 32) || startquote) {
		if (gs == '"') startquote ^= 1;
		else
		{
			if (bufsize>1)
			{
				*str=gs;
				str++;
			}
			bufsize--;
		}
		gs = fgetc(fd);
	}
	*str=0;
	if (gs < 0) return gs;
	else return 0;
}

/*
** fgetdouble
**
** This function is called to read in a doubleing point number from an open file.
**
**
** Arguments:
**
**      FILE *fd	A pointer to a open file
**
** Returns:
**
**      double		returns the doubleing point number read in
*/

double fgetdouble (FILE *fd)
{
	static char cdummy[32];
	double f;
	fgetstr(fd,cdummy,32);
	f=(double)atof(cdummy);
	return f;
}

/*
** fgetint
**
** This function is called to read in a integer number from an open file.
**
**
** Arguments:
**
**      FILE *fd	A pointer to a open file
**
** Returns:
**
**      int		returns the integer number read in
*/

int fgetint (FILE *fd)
{
	static char cdummy[32];
	int i;
	fgetstr(fd,cdummy,32);
	i=atoi(cdummy);
	return i;
}

/*
** fgetlong
**
** This function is called to read in a long integer number from an open file.
**
**
** Arguments:
**
**      FILE *fd	A pointer to a open file
**
** Returns:
**
**      long		returns the long integer number read in
*/

long fgetlong (FILE *fd)
{
	static char cdummy[32];
	long l;
	fgetstr(fd,cdummy,32);
	l=atol(cdummy);
	return l;
}

/*
** alloc_2d_doubles
**
** This function is called to create an 2 dimensional array of doubleing point numbers
**
**
** Arguments:
**
**      int hi1d	The number of columns of numbers
**      int hi2d	The number of rows of numbers
**
** Returns:
**
**      double **	returns a pointer to the array of
*/

double  **alloc_2d_doubles(int hi1d, int hi2d )
{
	int	 i;
	double **m;
	int len;
	/* check dimensions first */
	if( hi1d<1 || hi2d<1 ) {
		return NULL;
	}

	m = (double**) malloc (sizeof(double*) * hi1d);
	if( m == NULL ) {
		return NULL;
	}
	len = hi2d*sizeof(double);
	for( i=0; i<hi1d; ++i ) {
		m[i] = (double *) malloc (sizeof(double)*hi2d) ;
		if( m[i] == NULL ) {
			for( i--; i> 0; --i ) free (m[i]);
			free (m);
			return NULL;
		}
		memset (m[i],0,len);
	}
	return m;

} // end alloc_2d_doubles

/*
** free_2d_doubles
**
** This function is called to destroy the 2d array of doubleing point numbers
**
**
** Arguments:
**
**      int **matrix	The pointer to the 2D array
**      int hi1d		The number of columns of numbers
**
** Returns:
**
**      Nothing
*/

void free_2d_doubles( double **matrix, int hi1d)
{
	int i;

	// check dimensions first
	if( hi1d<1 ) {
		return;
	}
	if( matrix == NULL ) return;
	for( i=0; i<hi1d; i++ ) free (matrix[i]);
	free (matrix);

} // end free_2d_doubles

/*
** ToUpper
**
** This function is called to convert a string to upper case
**
**
** Arguments:
**
**      char *s		A pointer to the character string
**
** Returns:
**
**      Nothing
*/

void ToUpper (char *s) {
//	strupr(s);

  int i;

  for(i = 0; i < strlen(s); i++) {

    if(s[i] >= 'a' && s[i] <= 'z')
      s[i] += ('A' - 'a');

  }

}


/*
** isstring
**
** This function is called to check to see if the field is a string that can be
** converted to a number
** a number is '0123456789+-eE' but not '.'
**
**
** Arguments:
**
**      char *s		A pointer to the character string
**
** Returns:
**
**      int			returns a 1 if the string is not a number
*/

int isstring (const char *s) {
	while (*s!=0) {
		if ((*s>='A') && ((*s != 'e') || (*s != 'E'))) return 1;
		s++;
	}
	return 0;
}

/*
** ChkClip
**
** This function is called to check to see if the number is to be clipped
**
**
** Arguments:
**
**      double f		The value to be cliped
**      double hi	The high clip value
**      double lo	The low clip value
**
** Returns:
**
**      int			returns a 1 if the number is to be clipped
*/

int ChkClip (double f, double hi, double lo)
{
	if (f==MISSING) return 1;
	if ((hi!=MISSING) && (f > hi)) return 1;
	if ((lo!=MISSING) && (f < lo)) return 1;
	return 0;
}

/*
** removeleadingblanks
**
** This function is called to strip a buffer of leading blanks
**
**
** Arguments:
**
**      char *buf	A pointer to the buffer to be striped
**
** Returns:
**
**      Nothing
*/

void removeleadingblanks (char *s) {
	char *p = s;
	while (*p==32) p++;
	strcpy (s,p);
}

/*
**  fixfieldsize
**
** This function is called to blank pad a buffer to a fixed size
**
**
** Arguments:
**
**      char *buf	A pointer to the buffer to be padded
**
** Returns:
**
**      Nothing
*/

void fixfieldsize(char *s)
{
	int i;
	int l = strlen(s);
	if (l >= MAXSTRING) {
		s[MAXSTRING]=0;
		s[MAXSTRING-1]=32;
		return;
	}
	for (i=l; i < MAXSTRING; i++) s[i] = 32;
	s[MAXSTRING]=0;
}

