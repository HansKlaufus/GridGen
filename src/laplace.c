/*
** Function Laplace
**
** In:       tData    Data       = Structure containing read data
**           tResult  Result     = Structure containg results
** Out:      tResult  Result     = Structure containg results
** Return:   0 on success; -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>
#include <math.h>

#include "gridgen.h"
#include "laplace.h"
#include "cursor.h"
#include "loc.h"
#include "metrics.h"

int Laplace(FILE *log, tData *Data, tResult *Result)
{
	int    ret;

	int    i, j;
	int    iter;
	int    diverge;

	int    loc;
	int    loc_ksi_up, loc_ksi_down;
	int    loc_eta_up, loc_eta_down;
	int    loc_ksi_up_eta_up, loc_ksi_up_eta_down;
	int    loc_ksi_down_eta_up, loc_ksi_down_eta_down;

	double resMax, resMaxOld;

	double xKsi, xEta;
	double yKsi, yEta;
	double xKsiKsi, xKsiEta, xEtaEta;
	double yKsiKsi, yKsiEta, yEtaEta;

	double alpha, beta, gamma;

	double resX, resY;
	double x1, x2, x3, x4, x5, x6, x7, x8, x9;
	double y1, y2, y3, y4, y5, y6, y7, y8, y9;
	double omega;

	fprintf(stderr, "Starting Laplace... ");

	ret    = 0;
	resMax = SMALLITER;

	diverge = 0;
	iter    = 0;
	while ((resMax >= SMALLITER) && (diverge == 0))
	{
		iter++;
		resMaxOld = resMax;
		resMax    = 0;

		/* Show cursor animation */
		fprintf(stderr, "\b%c", Cursor(iter));

		for(j=1; j<Result->jm-1; j++)
		{
			for(i=1; i<Result->im-1; i++)
			{
				/* Find all positions in arrays */
				loc                   = Loc(&(*Result), j, i);
				loc_ksi_up            = Loc(&(*Result), j, i+1);
				loc_ksi_down          = Loc(&(*Result), j, i-1);
				loc_eta_up            = Loc(&(*Result), j+1, i);
				loc_eta_down          = Loc(&(*Result), j-1, i);
				loc_ksi_up_eta_up     = Loc(&(*Result), j+1, i+1);
				loc_ksi_up_eta_down   = Loc(&(*Result), j-1, i+1);
				loc_ksi_down_eta_up   = Loc(&(*Result), j+1, i-1);
				loc_ksi_down_eta_down = Loc(&(*Result), j-1, i-1);

				/* Get co-ordinates */
				x1     = Result->x[loc];
				x2     = Result->x[loc_ksi_up];
				x3     = Result->x[loc_ksi_down];
				x4     = Result->x[loc_eta_up];
				x5     = Result->x[loc_eta_down];
				x6     = Result->x[loc_ksi_up_eta_up];
				x7     = Result->x[loc_ksi_up_eta_down];
				x8     = Result->x[loc_ksi_down_eta_up];
				x9     = Result->x[loc_ksi_down_eta_down];

				y1     = Result->y[loc];
				y2     = Result->y[loc_ksi_up];
				y3     = Result->y[loc_ksi_down];
				y4     = Result->y[loc_eta_up];
				y5     = Result->y[loc_eta_down];
				y6     = Result->y[loc_ksi_up_eta_up];
				y7     = Result->y[loc_ksi_up_eta_down];
				y8     = Result->y[loc_ksi_down_eta_up];
				y9     = Result->y[loc_ksi_down_eta_down];

				/* Calculate the metrics */
				xKsi    = (x2-x3)/2; 
				xEta    = (x4-x5)/2;
				xKsiKsi = (x2-2*x1+x3);
				xKsiEta = (x6-x7-x8+x9)/4;
				xEtaEta = (x4-2*x1+x5);

				yKsi    = (y2-y3)/2;
				yEta    = (y4-y5)/2;
				yKsiKsi = (y2-2*y1+y3);
				yKsiEta = (y6-y7-y8+y9)/4;
				yEtaEta = (y4-2*y1+y5);

				/* Calculate the coefficients */
				alpha = xEta*xEta + yEta*yEta;
				beta  = xKsi*xEta + yKsi*yEta;
				gamma = xKsi*xKsi + yKsi*yKsi;

				/* Calculate the residues */
				resX = alpha*xKsiKsi - 2*beta*xKsiEta + gamma*xEtaEta;
				resY = alpha*yKsiKsi - 2*beta*yKsiEta + gamma*yEtaEta;

				resMax = (fabs(resX) > resMax) ? fabs(resX) : resMax;
				resMax = (fabs(resY) > resMax) ? fabs(resY) : resMax;

				/* Rebuild the physical space */
				omega = Data->omegaElliptic;

				x1 = x1 + omega*resX/(2*(alpha + gamma));
				y1 = y1 + omega*resY/(2*(alpha + gamma));

				Result->x[loc] = x1;
				Result->y[loc] = y1;
			}
		}

		/* Check for convergence */
		if ((resMax >= resMaxOld) && (iter>1))
			diverge = 1;
	}

	/* Print some information */
	printf("\b \n");
	if (diverge != 0)
	{
		printf("ERROR in function Laplace: Diverging...\n");
		printf("Aborting operation...\n");
		ret = -1;
	}
	printf("Maximum residue      = %f\n", resMax);
	printf("Number of iterations = %d\n", iter);

	/* Calculate the metrics */
	if (ret != -1)
		ret = CalcMetrics(&(*log), &(*Result));

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION LAPLACE *****\n\n");

		if (ret != -1)
		{
			fprintf(log, "Laplace successfully ended.\n");
			fprintf(log, "Number of iterations: %d\n", iter);
		}
		else
		{
			fprintf(log, "Laplace NOT successfully ended.\n");
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

