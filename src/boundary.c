/*
** C-file for BuildBoundaries
** Calculates the Boundaries
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
#include <stdlib.h>
#include <math.h>

#include "gridgen.h"
#include "boundary.h"
#include "distribute.h"
#include "loc.h"

int BuildBoundaries(FILE *log, tData* Data, tResult* Result)
{
	int    ret;

	printf("Building boundaries...\n");

	ret = 0;

	if (ret != -1)
		ret = SideABCBA(&(*log), &(*Result));

	if (ret != -1)
		ret = SideDEFG(&(*log), &(*Data), &(*Result));

	if (ret != -1)
		ret = SideAD(&(*log), &(*Data), &(*Result));

	if (ret != -1)
		ret = SideAG(&(*log), &(*Data), &(*Result));

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION BUILDBOUNDARIES *****\n\n");

		if (ret == 0)
		{
			fprintf(log, "Sides were succesfully created.\n");
		}
		else
		{
			fprintf(log, "Sides were NOT succesfully created.\n");
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int SideABCBA(FILE *log, tResult *Result)
{
	/*
	** This routine constructs the ETA = 0 boundary.
	*/

	int ret;
	int i;

	int loc_eta_min;

	printf("Building SideABCBA...\n");

	ret = 0;

	/* Copy the boundary ETA = 0; already constructed */
	for(i=0; i<Result->im; i++)
	{
		loc_eta_min = Loc(&(*Result), 0, i);
		Result->x[loc_eta_min] = Result->xNode[i];
		Result->y[loc_eta_min] = Result->yNode[i];
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION SIDEABCBA *****\n\n");

		fprintf(log, " X          Y\n");

		for(i=0; i<Result->im; i++)
		{
			loc_eta_min = Loc(&(*Result), 0, i);
			fprintf(log, "%10.6f %10.6f\n", Result->x[loc_eta_min], Result->y[loc_eta_min]);
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int SideDEFG(FILE *log, tData *Data, tResult *Result)
{
	/*
	** This routine constructs the ETA = max boundary.
	*/

	int    ret, subRet;
	int    i;

	int    loc_eta_max;
	int    lowerNode, higherNode;
	int    cornerNodeE, cornerNodeF;
	int    nodeDelta;

	double length, deltaLength;

	printf("Building SideDEFG...\n");

	ret         = 0;
	cornerNodeE = 0;
	cornerNodeF = 0;

	/* Find node with smallest y value */
	subRet = FindSmallestNodeY(&(*log), &(*Data), &(*Result), &lowerNode);

	/* Find node with largest y value */
	subRet = FindLargestNodeY(&(*log), &(*Data), &(*Result), &higherNode);

	if (lowerNode >= higherNode)
	{
		fprintf(log, "ERROR in function SideDEFG: higherNode < lowerNode\n");
		ret = -1;
	}
	else
	{
		/* Length to be divided between lowerNode and higherNode */
		length      = Result->xNode[lowerNode] + Data->height + Result->xNode[higherNode];
		deltaLength = length/(higherNode - lowerNode);

		/* Find node closest to corner E */
		nodeDelta   = (int) (Result->xNode[lowerNode]/deltaLength + 0.5);
		cornerNodeE = lowerNode + nodeDelta;

		/* Find node closest to corner F */
		nodeDelta   = (int) (Result->xNode[higherNode]/deltaLength + 0.5);
		cornerNodeF = higherNode - nodeDelta;

		/* Distribute nodes 0 to lowerNode */
		for(i=0; i<=lowerNode; i++)
		{
			loc_eta_max = Loc(&(*Result), Result->jm-1, i);
			Result->x[loc_eta_max] = Result->xNode[i];
			Result->y[loc_eta_max] = 0;
		}

		/* Distribute lowerNode+1 to cornerNodeE */
		deltaLength = Result->xNode[lowerNode]/(int)(cornerNodeE - lowerNode);
		for(i=lowerNode+1; i<=cornerNodeE; i++)
		{
			loc_eta_max = Loc(&(*Result), Result->jm-1, i);
			Result->x[loc_eta_max] = Result->xNode[lowerNode] - (i-lowerNode)*deltaLength;
			Result->y[loc_eta_max] = 0;
		}

		/* Distribute cornerNodeE+1 to cornerNodeF */
		deltaLength = Data->height/(int)(cornerNodeF - cornerNodeE);
		for(i=cornerNodeE+1; i<=cornerNodeF; i++)
		{
			loc_eta_max = Loc(&(*Result), Result->jm-1, i);
			Result->x[loc_eta_max] = 0;
			Result->y[loc_eta_max] = (i - cornerNodeE)*deltaLength;
		}

		/* Distribute cornerNodeF+1 to higherNode */
		deltaLength = Result->xNode[higherNode]/(int)(higherNode - cornerNodeF);
		for(i=cornerNodeF+1; i<=higherNode; i++)
		{
			loc_eta_max = Loc(&(*Result), Result->jm-1, i);
			Result->x[loc_eta_max] = (i - cornerNodeF)*deltaLength;
			Result->y[loc_eta_max] = Data->height;
		}

		/* Distribute nodes higherNode+1 to im-1 */
		for(i=higherNode+1; i<Result->im; i++)
		{
			loc_eta_max = Loc(&(*Result), Result->jm-1, i);
			Result->x[loc_eta_max] = Result->xNode[i];
			Result->y[loc_eta_max] = Data->height;
		}
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION SIDEDEFG *****\n\n");

		fprintf(log, "lowerNode  : %d\n", lowerNode);
		fprintf(log, "higherNode : %d\n", higherNode);
		fprintf(log, "cornerNodeE: %d\n", cornerNodeE);
		fprintf(log, "cornerNodeF: %d\n", cornerNodeF);

		fprintf(log, "\n X          Y\n");

		if (ret == 0)
		{
			for(i=0; i<Result->im; i++)
			{
				loc_eta_max = Loc(&(*Result), Result->jm-1, i);
				fprintf(log, "%10.6f %10.6f\n", Result->x[loc_eta_max], Result->y[loc_eta_max]);
			}
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int SideAD(FILE *log, tData *Data, tResult *Result)
{
	/*
	** This routine constructs the KSI = 0 boundary.
	*/

	int    ret;
	int    j;

	int loc_ksi_min;

	double *v = NULL;
	double refLength;

	printf("Building SideAD...\n");

	ret = 0;

	/* Allocate memory */
	v = (double*)malloc(Result->jm*sizeof(double));

	if (v==NULL)
	{
		fprintf(log, "ERROR in function SIDEAD: Could not allocate memory...\n");
		ret = -1;
	}
	else
	{
		refLength = Result->yNode[0];

		/* Create a distribution */
		if (Data->distributionType == 'U')
		{
			ret = Distribute1(&(*log), Result->jm, refLength, v);
		}
		else
		{
			ret = Distribute3(&(*log), Result->jm, refLength, v);
		}

		for(j=0; j<Result->jm; j++)
		{
			loc_ksi_min = Loc(&(*Result), j, 0);
			Result->x[loc_ksi_min] = Data->length;
			Result->y[loc_ksi_min] = Result->yNode[0] - v[j];
		}

		/* Free allocated memory */
		free(v);
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION SIDEAD *****\n\n");

		fprintf(log, " X          Y\n");

		for(j=0; j<Result->jm; j++)
		{
			loc_ksi_min = Loc(&(*Result), j, 0);
			fprintf(log, "%10.6f %10.6f\n", Result->x[loc_ksi_min], Result->y[loc_ksi_min]);
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int SideAG(FILE *log, tData *Data, tResult *Result)
{
	/*
	** This routine constructs the KSI = max boundary.
	*/

	int    ret;
	int    j;

	int    loc_ksi_max;

	double *v = NULL;
	double refLength;

	printf("Building SideAG...\n");

	ret = 0;

	/* Allocate memory */
	v = (double*)malloc(Result->jm*sizeof(double));

	if (v==NULL)
	{
		fprintf(log, "ERROR in function SIDEAG: Could not allocate memory...\n");
		ret = -1;
	}
	else
	{
		refLength = Data->height - Result->yNode[Result->im-1];

		/* Create a distribution */
		if (Data->distributionType == 'U')
		{
			ret = Distribute1(&(*log), Result->jm, refLength, v);
		}
		else
		{
			ret = Distribute3(&(*log), Result->jm, refLength, v);
		}

		for(j=0; j<Result->jm; j++)
		{
			loc_ksi_max = Loc(&(*Result), j, Result->im-1);
			Result->x[loc_ksi_max] = Data->length;
			Result->y[loc_ksi_max] = Result->yNode[Result->im-1] + v[j];
		}

		/* Free allocated memory */
		free(v);
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION SIDEAG *****\n\n");

		fprintf(log, " X          Y\n");

		for(j=0; j<Result->jm; j++)
		{
			loc_ksi_max = Loc(&(*Result), j, Result->im-1);
			fprintf(log, "%10.6f %10.6f\n", Result->x[loc_ksi_max], Result->y[loc_ksi_max]);
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int FindSmallestNodeY(FILE *log, tData *Data, tResult *Result, int *node)
{
	int    ret;
	int    i;

	int    yStart, yEnd;

	double ySmall;

	/*printf("Searching for smallest node in y\n");*/

	ret = 0;

	yStart = Data->numNodes2 + 1;
	yEnd   = Data->numNodes2 + Data->numNodes1/2;

	*node  = yStart;
	ySmall = Result->yNode[yStart];
	for(i=yStart; i<yEnd; i++)
	{
		if (Result->yNode[i] < ySmall)
		{
			ySmall = Result->yNode[i];
			*node  = i;
		}
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION FINDSMALLESTNODEY *****\n");

		fprintf(log, "node = %d\n", *node);

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int FindLargestNodeY(FILE *log, tData *Data, tResult *Result, int *node)
{
	int    ret;
	int    i;

	int    yStart, yEnd;

	double yBig;

	/*printf("Searching for largest node in y\n");*/

	ret = 0;

	yStart = Data->numNodes2 + Data->numNodes1/2;
	yEnd   = Data->numNodes2 + Data->numNodes1;

	*node  = yStart;
	yBig   = Result->yNode[yStart];
	for(i=yStart; i<yEnd; i++)
	{
		if (Result->yNode[i] > yBig)
		{
			yBig  = Result->yNode[i];
			*node = i;
		}
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION FINDLARGESTNODEY *****\n");

		fprintf(log, "node = %d\n", *node);

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int CalcSlope(FILE *log, int i, double *slope, double *xArray, double *yArray)
{
	int    ret;

	double xDelta, yDelta;

	/*printf("Calculating slope...\n");*/

	ret = 0;

	xDelta = xArray[i+1] - xArray[i-1];
	yDelta = yArray[i+1] - yArray[i-1];

	if (fabs(xDelta) < SMALL && fabs(yDelta) < SMALL)
	{
		/* Slope can not be calculated */
		ret = -1;
	}
	else if (fabs(xDelta) < SMALL)
	{
		/* Tangent slope vertical; normal slope horizontal */
		*slope = 0;
		ret = 0;
	}
	else if (fabs(yDelta) < SMALL)
	{
		/* Tangent slope horizontal; normal slope vertical */
		*slope = 10000;
		ret = 1;
	}
	else
	{
		/* Calculate slope */
		*slope = -xDelta/yDelta;
		ret = 0;
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION CALCSLOPE *****\n");

		fprintf(log, "slope = %10.6f\n", *slope);

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int CalcPoint(FILE *log, tData *Data, double slope, double *x, double *y)
{
	int    ret;

	double xDummy, yDummy;
	double xDelta, yDelta;

	/*printf("Calculating point...\n");*/

	ret = 0;

	/* Try to solve y for x=0 */
	xDummy = 0;
	xDelta = xDummy - *x;
	yDelta = xDelta*slope;
	yDummy = *y + yDelta;

	if (yDummy < 0)
	{
		/* Solve x for y=0 */
		yDummy = 0;
		yDelta = yDummy - *y;
		xDelta = yDelta/slope;
		xDummy = *x + xDelta;
	}
	else if (yDummy > Data->height)
	{
		/* Solve x for y=height */
		yDummy = Data->height;
		yDelta = yDummy - *y;
		xDelta = yDelta/slope;
		xDummy = *x + xDelta;
	}
	else
	{
		ret = -1;
	}

	*x = xDummy;
	*y = yDummy;

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION CALCPOINT *****\n");

		fprintf(log, " x          y\n");
		fprintf(log, "%10.6f %10.6f", *x, *y);

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

