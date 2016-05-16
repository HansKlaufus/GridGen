/*
** Function Structured
** Creates a structured grid.
**
** In:       tData   Data     = structure containing Data
**           tResult Result   = structure containing Results
**           
** Out:      tResult Result   = structure containing Results
**
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>

#include "gridgen.h"
#include "algebraic.h"
#include "laplace.h"
#include "middlecoff.h"
#include "structured.h"
#include "quadrangle.h"

int Structured(FILE *log, tData *Data, tResult* Result)
{
	int    ret;

	printf("Building structured grid...\n");

	ret = 0;

	/* Algebraic part */
	if (ret != -1)
		ret = Algebraic(&(*log), &(*Result));

	/* Elliptic part */
	if ((ret != -1) && (Data->omegaElliptic > SMALL))
	{
		if (Data->gridType == 'L')
			ret = Laplace(&(*log), &(*Data), &(*Result));
		else if (Data->gridType == 'M' || Data->gridType == 'U')
			ret = Middlecoff(&(*log), &(*Data), &(*Result));
		else if (Data->gridType != 'A')
		{
			fprintf(stderr, "ERROR in function Structured: Unknown GridType.\n");
			ret = -1;
		}
	}

	/* Fill the quadrangles array */
	if (ret != -1)
		ret = Quadrangulate(&(*log), &(*Result));

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION STRUCTURED *****\n\n");

		if (ret != -1)
		{
			fprintf(log, "Succesfully Structured.\n");
		}
		else
		{
			fprintf(log, "NOT succesfully Structured.\n");
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

