/*
** Function SY.
** Solves a tridiagonal system by elmination
**
** In:       int il          = lower boundary value
**           int iu          = upper boundary value
**           double bb       = vector of coefficients behind diagonal
**           double dd       = vector of coefficients on diagonal
**           double aa       = vector of coefficients ahead of diagonal
**           double cc       = constant vector
**
** Out:      double cc       = solution vector
**
** Return:   0 on success, -1 on failure
**
*/

#include <stdio.h>
#include "gridgen.h"
#include "sy.h"

int SY(FILE *log, int il, int iu, double *bb, double *dd, double *aa, double *cc)
{
	int    ret;
	int    i;
	double r;

	/*printf("Solving matrix...\n");*/

	ret = 0;
	
	/* Establish upper triangular matrix */
	for(i=il+1; i<=iu; i++)
	{
		r     = bb[i]/dd[i-1];
		dd[i] = dd[i] - r*aa[i-1];
		cc[i] = cc[i] - r*cc[i-1];
	}

	/* Back substitution */
	cc[iu] = cc[iu]/dd[iu];
	for(i=iu; i>=il+1; i--)
	{
		cc[i] = (cc[i]-aa[i]*cc[i+1])/dd[i];
	}
	
	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION SY *****\n\n");

		fprintf(log, "  RR\n");
		for(i=il+1; i<=iu; i++)
		{
			fprintf(log, "%10.6f\n", cc[i]);
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

