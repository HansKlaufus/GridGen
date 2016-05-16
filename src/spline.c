/*
** C-file for Spline
** Constructs a spline through the control points of the aerofoil
**
** In:       tData  Data     = structure containing all data
**
** Out:      tResult Result  = structure containing all results
**
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gridgen.h"
#include "spline.h"
#include "sy.h"
#include "distribute.h"

int Spline(FILE *log, tData* Data, tResult *Result)
{
	int    ret;
	int    i, j;

	double *u=NULL;
	double *v=NULL;
	double *bx=NULL, *dx=NULL, *ax=NULL, *cx=NULL;
	double *by=NULL, *dy=NULL, *ay=NULL, *cy=NULL;

	double A, B, C, D;

	printf("Splining...\n");

	ret = 0;

	/* Allocate memory */
	u = (double *)malloc(Data->numData*sizeof(double));
	v  = (double *)malloc(Data->numNodes1*sizeof(double));

	bx = (double *)malloc(Data->numData*sizeof(double));
	dx = (double *)malloc(Data->numData*sizeof(double));
	ax = (double *)malloc(Data->numData*sizeof(double));
	cx = (double *)malloc(Data->numData*sizeof(double));

	by = (double *)malloc(Data->numData*sizeof(double));
	dy = (double *)malloc(Data->numData*sizeof(double));
	ay = (double *)malloc(Data->numData*sizeof(double));
	cy = (double *)malloc(Data->numData*sizeof(double));

	if ((u == NULL) || (v == NULL) ||
	    (bx == NULL) || (dx == NULL) || (ax == NULL) || (cx == NULL) ||
	    (by == NULL) || (dy == NULL) || (ay == NULL) || (cy == NULL))
	{
		fprintf(log, "ERROR in function Spline: Could not allocate memory...");
		ret = -1;
	}
	else
	{
		/* Parametrize all control points on distance */
		u[0] = 0;
		for(i=1; i<Data->numData; i++)
		{
			u[i] = u[i-1] + sqrt(Data->xData[i]*Data->xData[i] + Data->yData[i]*Data->yData[i]);
		}

		/* Now set up 3 diagonal vectors and solution vector */
		bx[0] = dx[0] = ax[0] = cx[0] = 0;
		by[0] = dy[0] = ay[0] = cy[0] = 0;
		for(i=1; i<Data->numData-1; i++)
		{
			/* Set up diagonal vectors */
			bx[i] = u[i] - u[i-1];
			dx[i] = 2*(u[i+1] - u[i-1]);
			ax[i] = u[i+1] - u[i];

			by[i] = u[i] - u[i-1];
			dy[i] = 2*(u[i+1] - u[i-1]);
			ay[i] = u[i+1] - u[i];

			/* Set up solution vectors */
			cx[i] = 6*((Data->xData[i+1] - Data->xData[i])/(u[i+1] - u[i]) - (Data->xData[i] - Data->xData[i-1])/(u[i] - u[i-1]));
			cy[i] = 6*((Data->yData[i+1] - Data->yData[i])/(u[i+1] - u[i]) - (Data->yData[i] - Data->yData[i-1])/(u[i] - u[i-1]));
		}
		bx[i] = dx[i] = ax[i] = cx[i] = 0;
		by[i] = dy[i] = ay[i] = cy[i] = 0;

		/* Solve for second derivatives */
		if (ret != -1)
			ret = SY(&(*log), 1, Data->numData-2, bx, dx, ax, cx);

		if (ret != -1)
			ret = SY(&(*log), 1, Data->numData-2, by, dy, ay, cy);

		/* Free some memory */
		if (ax)
			free(ax);
		if (bx)
			free(bx);
		if (dx)
			free(dx);
		if (ay)
			free(ay);
		if (by)
			free(by);
		if (dy)
			free(dy);

		/* Now generate a nice distribution for v */
		if(Data->distributionType == 'U')
		{
			if (ret != -1)
				ret = Distribute1(log, Data->numNodes1, u[Data->numData-1], v);
		}
		else
		{
			if (ret != -1)
				ret = Distribute2(log, Data->numNodes1, u[Data->numData-1], v);
		}

		/* For this distribution in v, solve x and y */
		for(i=0; i<Data->numNodes1; i++)
		{
			j = 0;
			while((v[i] - u[j+1] > SMALL) && (j < Data->numData-1))
				j++;

			/* Now v lies between u(j) and u(j+1) */
			/* Solve for x and y */
			if (j < Data->numData-1)
			{
				A = (u[j+1]-v[i])/(u[j+1]-u[j]);
				B = 1 - A;
				C = (A*A*A-A)/6 * (u[j+1]-u[j])*(u[j+1]-u[j]);
				D = (B*B*B-B)/6 * (u[j+1]-u[j])*(u[j+1]-u[j]);

				Result->xNode[i] = A*Data->xData[j] + B*Data->xData[j+1] + C*cx[j] + D*cx[j+1];
				Result->yNode[i] = A*Data->yData[j] + B*Data->yData[j+1] + C*cy[j] + D*cy[j+1];
			}
		}

		/* Free  memory */
		if (u)
			free(u);
		if (v)
			free(v);
		if (cx)
			free(cx);
		if (cy)
			free(cy);
	}


	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION SPLINE *****\n\n");

		fprintf(log, "  X          Y\n");
		for(i=0; i<Data->numNodes1; i++)
		{
			fprintf(log, "%10.6f %10.6f\n", Result->xNode[i], Result->yNode[i]);
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

