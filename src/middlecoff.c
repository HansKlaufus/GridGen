/*
** Function Middlecoff
**
** In:       tData    Data       = Structure containing read data
**           tResult  Result     = Structure containg results
** Out:      tResult  Result     = Structure containg results
** Return:   0 on success; -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gridgen.h"
#include "middlecoff.h"
#include "cursor.h"
#include "metrics.h"
#include "loc.h"

int Middlecoff(FILE *log, tData *Data, tResult *Result)
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

	double x1, x2, x3, x4, x5, x6, x7, x8, x9;
	double y1, y2, y3, y4, y5, y6, y7, y8, y9;

	double xKsi, xEta, xKsiKsi, xKsiEta, xEtaEta;
	double yKsi, yEta, yKsiKsi, yKsiEta, yEtaEta;

	double alpha, beta, gamma;

	double resX, resY;
	double resMax, resMaxOld;

	double omega;

	double *phi = NULL;
	double *psi = NULL;

	fprintf(stderr, "Solving Middlecoff... ");

	ret         = 0;
	diverge     = 0;
	iter        = 0;
	resMax      = SMALLITER;

	/* Allocate memory */
	phi = (double*)malloc(Result->im*Result->jm*sizeof(double));
	psi = (double*)malloc(Result->im*Result->jm*sizeof(double));

	if (phi==NULL || psi==NULL)
	{
		printf("\nERROR in function Middlecoff: could not allocate memory.\n");
		ret = -1;
	}
	else
	{
		/* Calculate phi and psi */
		if (ret != -1)
			ret = CalcPhiPsi(&(*log), &(*Result), phi, psi);

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
					resX = alpha*(xKsiKsi+phi[loc]*xKsi) - 2*beta*xKsiEta + gamma*(xEtaEta+psi[loc]*xEta);
					resY = alpha*(yKsiKsi+phi[loc]*yKsi) - 2*beta*yKsiEta + gamma*(yEtaEta+psi[loc]*yEta);

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
			printf("ERROR in function Middlecoff: Diverging...\n");
			printf("Aborting operation...\n");
			//ret = -1;
		}
		printf("Maximum residue      = %f\n", resMax);
		printf("Number of iterations = %d\n", iter);

		/* Calculate metrics */
		if (ret != -1)
			CalcMetrics(&(*log), &(*Result));

		/* Free allocated memory */
		if(phi)
			free(phi);

		if(psi)
			free(psi);
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION MIDDLECOFF *****\n\n");

		if (ret != -1)
		{
			fprintf(log, "Middlecoff successfully ended.\n");
			fprintf(log, "Number of iterations: %d\n", iter);
		}
		else
		{
			fprintf(log, "Middlecoff NOT successfully ended.\n");
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int CalcPhiPsi(FILE *log, tResult *Result, double *phi, double *psi)
{
	int ret;
	int i, j;

	int loc, loc_eta_min, loc_eta_max, loc_ksi_min, loc_ksi_max;

	double xKsi, yKsi, xEta, yEta;
	double xKsiKsi, yKsiKsi, xEtaEta, yEtaEta;
	double jFactor1, jFactor2;
	double iFactor1, iFactor2;

	/*printf("Calculating source terms phi and psi...\n");*/

	ret = 0;

	/*
	** Calculate term phi.
	*/
	for(i=1; i<Result->im-1; i++)
	{
		/* ETA = 0 boundary */
		loc_eta_min = Loc(&(*Result), 0, i);

		xKsi = Result->xKsi[loc_eta_min];
		yKsi = Result->yKsi[loc_eta_min];

		xKsiKsi = Result->xKsiKsi[loc_eta_min];
		yKsiKsi = Result->yKsiKsi[loc_eta_min];

		phi[loc_eta_min] = -(xKsi*xKsiKsi + yKsi*yKsiKsi)/(xKsi*xKsi + yKsi*yKsi);

		/* ETA = MAX boundary */
		loc_eta_max = Loc(&(*Result), Result->jm-1, i);

		xKsi = Result->xKsi[loc_eta_max];
		yKsi = Result->yKsi[loc_eta_max];

		xKsiKsi = Result->xKsiKsi[loc_eta_max];
		yKsiKsi = Result->yKsiKsi[loc_eta_max];

		phi[loc_eta_max] = -(xKsi*xKsiKsi + yKsi*yKsiKsi)/(xKsi*xKsi + yKsi*yKsi);
	 }

	/*
	** Calculate term psi.
	*/
	for(j=1; j<Result->jm-1; j++)
	{
		/* KSI = 0 boundary */
		loc_ksi_min = Loc(&(*Result), j, 0);

		xEta = Result->xEta[loc_ksi_min];
		yEta = Result->yEta[loc_ksi_min];

		xEtaEta = Result->xEtaEta[loc_ksi_min];
		yEtaEta = Result->yEtaEta[loc_ksi_min];

		psi[loc_ksi_min] = -(xEta*xEtaEta + yEta*yEtaEta)/(xEta*xEta + yEta*yEta);

		/* KSI = MAX boundary */
		loc_ksi_max = Loc(&(*Result), j, Result->im-1);

		xEta = Result->xEta[loc_ksi_max];
		yEta = Result->yEta[loc_ksi_max];

		xEtaEta = Result->xEtaEta[loc_ksi_max];
		yEtaEta = Result->yEtaEta[loc_ksi_max];

		psi[loc_ksi_max] = -(xEta*xEtaEta + yEta*yEtaEta)/(xEta*xEta + yEta*yEta);
	}

	/* Interpolate between boundaries */
	for(j=1; j<Result->jm-1; j++)
	{
		loc_ksi_min = Loc(&(*Result), j, 0);
		loc_ksi_max = Loc(&(*Result), j, Result->im-1);

		jFactor1 = (double)(Result->jm-1 - j)/(double)(Result->jm-1);
		jFactor2 = (double)(j)/(double)(Result->jm-1);

		for(i=1; i<Result->im-1; i++)
		{
			loc         = Loc(&(*Result), j, i);
			loc_eta_min = Loc(&(*Result), 0, i);
			loc_eta_max = Loc(&(*Result), Result->jm-1, i);

			iFactor1 = (double)(Result->im-1 - i)/(double)(Result->im-1);
			iFactor2 = (double)(i)/(double)(Result->im-1);

			phi[loc] = jFactor1*phi[loc_eta_min] + jFactor2*phi[loc_eta_max];
			psi[loc] = iFactor1*psi[loc_ksi_min] + iFactor2*psi[loc_ksi_max];
		}
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION CALCPHIPSI *****\n\n");

		fprintf(log, "  j   i        phi        psi\n");

		for(j=1; j<Result->jm-1; j++)
		{
			for(i=1; i<Result->im-1; i++)
			{
				loc = Loc(&(*Result), j, i);
				fprintf(log, "%3d %3d %10.6f %10.6f\n", j, i, phi[loc], psi[loc]);
			}
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

