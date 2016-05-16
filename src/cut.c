/*
** C-file for MakeCut
** Makes a cut through the windtunnel to define the KSI-axis.
**
** In:       tData   Data     = structure containing Data
**           tResult Result   = structure containing Results
** Out:      tResult Result   = structure containing Results
** Return:   0 on success, -1 on failure
**
** Assumption: Alpha is small, right end point of cutting line will
**             cut right waal of windtunnel.
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gridgen.h"
#include "cut.h"
#include "distribute.h"

int MakeCut(FILE *log, tData* Data, tResult* Result)
{
	int    ret;
	int    i;

	int    numNodes;

	double xDelta, yDelta;
	double refLength;

	double *v      = NULL;
	double *xDummy = NULL;
	double *yDummy = NULL;

	printf("Building cutting line...\n");

	ret = 0;
	
	/* Define dummy arrays */
	numNodes = Result->im;
	xDummy   = (double*)malloc(numNodes*sizeof(double));
	yDummy   = (double*)malloc(numNodes*sizeof(double));

	if((xDummy == NULL) || (yDummy == NULL))
	{
		fprintf(log, "ERROR in function MakeCut: Could not allocate memory...");
		ret = -1;
	}

	/* Calculate length of cutting line */
	xDelta    = (Data->length - Result->xNode[0]);
	yDelta    = xDelta*tan(-Data->alpha*PI/180);
	refLength = sqrt(xDelta*xDelta + yDelta*yDelta);

	/* Distribute nodes along cutting line 
	**      Use one point more then requested.
	**      First point cutting line == last point aerofoil
	**      This point can then be skipped, resulting in
	**      the number of points as wanted.
	*/
	v = (double*)malloc((1+Data->numNodes2)*sizeof(double));

	if (v == NULL)
	{
		fprintf(log, "ERROR in function MakeCut: Could not allocate enough memory...");
		ret = -1;
	}
	else
	{
		if (Data->distributionType == 'U')
		{
			ret = Distribute1(&(*log), Data->numNodes2 + 1, refLength, v);
		}
		else
		{
			ret = Distribute3(&(*log), Data->numNodes2 + 1, refLength, v);
		}

		/* Fill the dummy arrays */
		/* Copy cutting line data */
		for (i=0; i<Data->numNodes2; i++)
		{
			/* Data->numNodes1 = number of nodes along the aerofoil */
			/* Data->numNodes2 = number of nodes along the cutting line */

			/*
			** Use i+1 for v to skip the point that already exists
			** on the aerofoil.
			*/

			/* Fill points before aerofoil */
			xDummy[Data->numNodes2-1 - i] = Result->xNode[Data->numNodes1-1] + v[i+1]/refLength * xDelta;
			yDummy[Data->numNodes2-1 - i] = Result->yNode[Data->numNodes1-1] + v[i+1]/refLength * yDelta;

			/* Fill points after aerofoil */
			xDummy[Data->numNodes1 + Data->numNodes2 + i] = Result->xNode[0] + v[i+1]/refLength * xDelta;
			yDummy[Data->numNodes1 + Data->numNodes2 + i] = Result->yNode[0] + v[i+1]/refLength * yDelta;
		}

		/* Copy aerofoil data */
		for (i=0; i<Data->numNodes1; i++)
		{
			xDummy[Data->numNodes2 + i] = Result->xNode[i];
			yDummy[Data->numNodes2 + i] = Result->yNode[i];
		}

		/* Now drop the existing Arrays and redirect them to the Dummy arrays */
		free(Result->xNode);
		free(Result->yNode);

		Result->xNode = xDummy;
		Result->yNode = yDummy;

		/* Clear memory */
		free(v);
	}


	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION MAKECUT *****\n\n");

		for(i=0; i<numNodes; i++)
		{
			fprintf(log, "%10.6f %10.6f\n", Result->xNode[i], Result->yNode[i]);
		}
		
		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

