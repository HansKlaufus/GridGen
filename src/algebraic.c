/*
** C-file for Algebraic
**
** In:       tResult  Result     = Structure containg results
** Out:      tResult  Result     = Structure containg results
** Return:   0 on success; -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>

#include "gridgen.h"
#include "algebraic.h"
#include "compspace.h"
#include "interpolate.h"
#include "metrics.h"

int Algebraic(FILE *logFile, tResult *Result)
{
	int    ret;

	printf("\nStarting algebraic part...\n");

	ret = 0;

	/* Build computational plane */
	if (ret != -1)
		ret = BuildComputationalSpace(&(*logFile), &(*Result));

	/* Interpolate all nodes */
	if (ret != -1)
		ret = Interpolate(&(*logFile), &(*Result));

	/* Calculate metrics */
	if (ret != -1)
		ret = CalcMetrics(&(*logFile), &(*Result));

	/* Write report */
	if (logFile)
	{
		fprintf(logFile, "\n***** FUNCTION ALGEBRAIC *****\n\n");

		if (ret != -1)
		{
			fprintf(logFile, "Algebraic part successfully ended.\n");
		}
		else
		{
			fprintf(logFile, "Algebraic part NOT successfully ended.\n");
		}

		fprintf(logFile, "\n******************************\n\n");
	}

	return ret;
}

