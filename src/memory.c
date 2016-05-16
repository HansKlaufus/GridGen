/*
** Function Initialise
** Initialises all arrays in structure Result
**
** In:       tData Data = structure containing all data
** Out:      -
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>
#include <stdlib.h>

#include "gridgen.h"
#include "memory.h"

int Initialise(FILE *log, tData *Data, tResult *Result)
{
	int ret;

	printf("Initialising...\n");

	ret = 0;

	Result->im = Data->numNodes1 + 2*Data->numNodes2;
	Result->jm = Data->numNodes3;

	Result->xNode = (double*)malloc(Result->im*sizeof(double));
	Result->yNode = (double*)malloc(Result->im*sizeof(double));

	Result->ksi = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->eta = (double*)malloc((Result->im*Result->jm)*sizeof(double));

	Result->xKsi    = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->xEta    = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->yKsi    = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->yEta    = (double*)malloc((Result->im*Result->jm)*sizeof(double));

	Result->xKsiKsi = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->xEtaEta = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->xKsiEta = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->yKsiKsi = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->yEtaEta = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->yKsiEta = (double*)malloc((Result->im*Result->jm)*sizeof(double));

	Result->ksiX = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->ksiY = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->etaX = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->etaY = (double*)malloc((Result->im*Result->jm)*sizeof(double));

	Result->jac  = (double*)malloc((Result->im*Result->jm)*sizeof(double));

	Result->x = (double*)malloc((Result->im*Result->jm)*sizeof(double));
	Result->y = (double*)malloc((Result->im*Result->jm)*sizeof(double));

	Result->element = NULL; /* Will be allocated later */

	if((Result->xNode == NULL)   || (Result->yNode == NULL)    ||
	   (Result->ksi == NULL)     || (Result->eta == NULL)      ||
	   (Result->xKsi == NULL)    || (Result->xEta == NULL)     ||
	   (Result->yKsi == NULL)    || (Result->yEta == NULL)     ||
	   (Result->xKsiKsi == NULL) || (Result->xEtaEta == NULL)  || (Result->xKsiEta == NULL) ||
	   (Result->yKsiKsi == NULL) || (Result->yEtaEta == NULL)  || (Result->yKsiEta == NULL) ||
	   (Result->ksiX == NULL)    || (Result->ksiY == NULL)     ||
	   (Result->etaX == NULL)    || (Result->etaY == NULL)     ||
	   (Result->jac == NULL)     ||
	   (Result->x == NULL)       || (Result->y == NULL)) 
	{
		if (log)
			fprintf(log, "ERROR in function Prepare: could not allocate memory...\n");

		ret = -1;
	}

	return ret;
}

/*
** Function Finish
** Clears all memory
**
** In:       tData   Data     = structure containing data
**           tResult Result   = structure containing results
**           
** Out:      -
**
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

int Finish(tData *Data, tResult *Result)
{
	int ret = 0;

	printf("Finishing...\n");

	free(Data->xData);
	free(Data->yData);

	free(Result->xNode);
	free(Result->yNode);

	free(Result->ksi);
	free(Result->eta);

	free(Result->xKsi);
	free(Result->xEta);
	free(Result->yKsi);
	free(Result->yEta);
	free(Result->xKsiKsi);
	free(Result->xEtaEta);
	free(Result->xKsiEta);
	free(Result->yKsiKsi);
	free(Result->yEtaEta);
	free(Result->yKsiEta);

	free(Result->ksiX);
	free(Result->ksiY);
	free(Result->etaX);
	free(Result->etaY);

	free(Result->jac);

	free(Result->x);
	free(Result->y);

	free(Result->element);

	return ret;
}

