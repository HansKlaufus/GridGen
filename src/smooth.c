/*
** Function Smooth
** Smoothes all inner gridpoints.
**
** In:       tData   Data    = structure containing all data
**           tResult Result  = structure containing all results
**          
** Out:      tResult Result  = structure containing all results
**
** Return:   0 on success; -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>
#include <math.h>

#include "gridgen.h"
#include "cursor.h"
#include "smooth.h"
#include "loc.h"

int Smooth(FILE *log, tData *Data, tResult *Result)
{
	int    ret;
	int    loc;
	int    i, j;
	int    iter;
	int    diverge;
	int    node, nodeS, nodeW, nodeNW, nodeN, nodeE, nodeSE;

	double omega;
	double resX, resY, resMaxX, resMaxY, resMax, resMaxOld;

	fprintf(stderr, "Smoothing... ");


	ret         = 0;
	diverge     = 0;
	iter        = 0;
	resMax      = SMALLITER;
	omega       = Data->omegaSmooth;

	if (omega < SMALL)
	{
		fprintf(stderr, "ERROR in function Smooth: omegaSmooth not defined...\n");
		ret = -1;
	}

	while((resMax >= SMALLITER) && (ret != -1) && (diverge == 0))
	{
		iter++;

		resMaxX = 0;
		resMaxY = 0;

		/* Show cursor animation */
		fprintf(stderr, "\b%c", Cursor(iter));

		for (j=1; j<Result->jm-1; j++)
		{
			for (i=1; i<Result->im-1; i++)
			{
				if (Result->elementType == etTriangle)
				{
					/* Triangles */
					node   = Loc(&(*Result), j, i);
					nodeS  = Loc(&(*Result), j-1, i);
					nodeW  = Loc(&(*Result), j, i-1);
					nodeNW = Loc(&(*Result), j+1, i-1);
					nodeN  = Loc(&(*Result), j+1, i);
					nodeE  = Loc(&(*Result), j, i+1);
					nodeSE = Loc(&(*Result), j-1, i+1);

					resX =  (Result->x[nodeS]  +
						 Result->x[nodeW]  +
						 Result->x[nodeNW] +
						 Result->x[nodeN]  +
						 Result->x[nodeE]  +
						 Result->x[nodeSE] -
					         Result->x[node]*6)/6;

					resY =  (Result->y[nodeS]  +
						 Result->y[nodeW]  +
						 Result->y[nodeNW] +
						 Result->y[nodeN]  +
						 Result->y[nodeE]  +
						 Result->y[nodeSE] -
					         Result->y[node]*6)/6;

					resMaxX = (fabs(resX) > resMaxX ? fabs(resX) : resMaxX);
					resMaxY = (fabs(resY) > resMaxY ? fabs(resY) : resMaxY);

					Result->x[node] = Result->x[node] + omega*resX;
					Result->y[node] = Result->y[node] + omega*resY;
				}
				else if (Result->elementType == etQuadrangle)
				{
					/* Quadrangles */
					node   = Loc(&(*Result), j, i);
					nodeS  = Loc(&(*Result), j-1, i);
					nodeW  = Loc(&(*Result), j, i-1);
					nodeN  = Loc(&(*Result), j+1, i);
					nodeE  = Loc(&(*Result), j, i+1);

					resX =  (Result->x[nodeS]  +
						 Result->x[nodeW]  +
						 Result->x[nodeN]  +
						 Result->x[nodeE]  -
					         Result->x[node]*4)/4;

					resY =  (Result->y[nodeS]  +
						 Result->y[nodeW]  +
						 Result->y[nodeN]  +
						 Result->y[nodeE]  -
					         Result->y[node]*4)/4;

					resMaxX = (fabs(resX) > resMaxX ? fabs(resX) : resMaxX);
					resMaxY = (fabs(resY) > resMaxY ? fabs(resY) : resMaxY);

					Result->x[node] = Result->x[node] + omega*resX;
					Result->y[node] = Result->y[node] + omega*resY;
				}
				else
				{
					printf("ERROR in function Smooth: Unknown element type.\n");
					ret = -1;
				}
			}
		}

		resMaxOld = resMax;
		resMax = (resMaxX > resMaxY ? resMaxX : resMaxY);

		if ((resMax >= resMaxOld) && (iter>1))
			diverge = 1;
	}

	fprintf(stderr, "\b \n");
	if (diverge != 0)
	{
		fprintf(stderr, "ERROR in function Smooth: diverging...\n");
		fprintf(stderr, "Aborting operation...\n");
		ret = -1;
	}
	printf("\nNumber of iterations = %d\n\n", iter);

	/* Write report */
	if (log)
	{
		fprintf(log, "\n\n***** FUNCTION SMOOTH *****\n\n");

		fprintf(log, "  j   i          x          y\n");

		for (j=1; j<Result->jm-1; j++)
		{
			for (i=1; i<Result->im-1; i++)
			{
				loc = Loc(&(*Result), j, i);
				fprintf(log, "%3d %3d %10.6f %10.6f\n", j, i, Result->x[loc], Result->y[loc]);
			}
		}

		fprintf(log, "\n\n***************************\n\n");
	}

	return ret;
}

