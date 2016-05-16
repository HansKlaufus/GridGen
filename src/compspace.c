/*
** Function BuildComputationalSpace
** Builds the Computational Space
**
** In:       tResult Result   = structure containing Results
** Out:      tResult Result   = structure containing Results
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>

#include "gridgen.h"
#include "compspace.h"
#include "loc.h"

int BuildComputationalSpace(FILE *log, tResult* Result)
{
	int    ret;
	int    i, j;

	int    loc;

	double ksiDelta, etaDelta;

	printf("Building computational space...\n");

	ret = 0;

	ksiDelta = 1;
	etaDelta = 1;

	Result->ksiDelta = ksiDelta;
	Result->etaDelta = etaDelta;

	for(j=0; j<Result->jm; j++)
	{
		for(i=0; i<Result->im; i++)
		{
			loc = Loc(&(*Result), j, i);
			Result->ksi[loc] = i*ksiDelta;
			Result->eta[loc] = j*etaDelta;
		}
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION BUILDCOMPUTATIONALSPACE *****\n\n");

		fprintf(log, " im       = %d\n", Result->im);
		fprintf(log, " jm       = %d\n", Result->jm);
		fprintf(log, " ksiDelta = %10.6f\n", ksiDelta);
		fprintf(log, " etaDelta = %10.6f\n", etaDelta);

		fprintf(log, "\n   ksi        eta\n");
		for(j=0; j<Result->jm; j++)
		{
			for(i=0; i<Result->im; i++)
			{
				loc = Loc(&(*Result), j, i);
				fprintf(log, " %10.6f %10.6f\n", Result->ksi[loc], Result->eta[loc]);
			}
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

