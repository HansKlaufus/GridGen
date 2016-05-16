/*
** Function CalcMetrics
** Calculates all metrics and the Jacobian.
**
** In:       tResult Result   = structure containing Results
** Out:      tResult Result   = structure containing Results
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>

#include "gridgen.h"
#include "metrics.h"
#include "loc.h"

int CalcMetrics(FILE *log, tResult* Result)
{
	int    ret;
	int    i, j;

	int    loc;
	int    loc_ksi_down, loc_ksi_up;
	int    loc_eta_down, loc_eta_up;
	int    loc_ksi_down_eta_down, loc_ksi_down_eta_up;
	int    loc_ksi_up_eta_down, loc_ksi_up_eta_up;

	double ksiDelta, etaDelta;
	double jac;
	double xKsi, xEta;
	double yKsi, yEta;
	double xKsiKsi, xEtaEta, xKsiEta;
	double yKsiKsi, yEtaEta, yKsiEta;
	double ksiX, ksiY;
	double etaX, etaY;

	printf("Calculating metrics...\n");

	ret = 0;

	/* Calculate deltas */
	ksiDelta = Result->ksiDelta;
	etaDelta = Result->etaDelta;

	for(j=0; j<Result->jm; j++)
	{
		for(i=0; i<Result->im; i++)
		{
			/* Calculate location to store values */
			loc                   = Loc(&(*Result), j, i);
			loc_ksi_up            = Loc(&(*Result), j, i+1);
			loc_ksi_down          = Loc(&(*Result), j, i-1);
			loc_eta_up            = Loc(&(*Result), j+1, i);
			loc_eta_down          = Loc(&(*Result), j-1, i);
			loc_ksi_up_eta_down   = Loc(&(*Result), j-1, i+1);
			loc_ksi_up_eta_up     = Loc(&(*Result), j+1, i+1);
			loc_ksi_down_eta_down = Loc(&(*Result), j-1, i-1);
			loc_ksi_down_eta_up   = Loc(&(*Result), j+1, i-1);

			/* Calculate the metrics */
			if (i==0)
			{
				/* Forward differencing */
				xKsi = (Result->x[loc_ksi_up] - Result->x[loc])/ksiDelta;
				yKsi = (Result->y[loc_ksi_up] - Result->y[loc])/ksiDelta;

				xKsiKsi = 0;
				yKsiKsi = 0;
			}
			else if (i==Result->im-1)
			{
				/* Backward differencing */
				xKsi = (Result->x[loc] - Result->x[loc_ksi_down])/ksiDelta;
				yKsi = (Result->y[loc] - Result->y[loc_ksi_down])/ksiDelta;

				xKsiKsi = 0;
				yKsiKsi = 0;
			}
			else
			{
				/* Central differencing */
				xKsi = (Result->x[loc_ksi_up] - Result->x[loc_ksi_down])/(2*ksiDelta);
				yKsi = (Result->y[loc_ksi_up] - Result->y[loc_ksi_down])/(2*ksiDelta);

				xKsiKsi = (Result->x[loc_ksi_up]-2*Result->x[loc]+Result->x[loc_ksi_down])/(ksiDelta*ksiDelta);
				yKsiKsi = (Result->y[loc_ksi_up]-2*Result->y[loc]+Result->y[loc_ksi_down])/(ksiDelta*ksiDelta);
			}

			if (j==0)
			{
				/* Forward differencing */
				xEta = (Result->x[loc_eta_up] - Result->x[loc])/etaDelta;
				yEta = (Result->y[loc_eta_up] - Result->y[loc])/etaDelta;

				xEtaEta = 0;
				yEtaEta = 0;
			}
			else if (j==Result->jm-1)
			{
				/* Backward differencing */
				xEta = (Result->x[loc] - Result->x[loc_eta_down])/etaDelta;
				yEta = (Result->y[loc] - Result->y[loc_eta_down])/etaDelta;
			
				xEtaEta = 0;
				yEtaEta = 0;
			}
			else
			{
				/* Central differencing */
				xEta = (Result->x[loc_eta_up] - Result->x[loc_eta_down])/(2*etaDelta);
				yEta = (Result->y[loc_eta_up] - Result->y[loc_eta_down])/(2*etaDelta);

				xEtaEta = (Result->x[loc_eta_up]-2*Result->x[loc]+Result->x[loc_eta_down])/(etaDelta*etaDelta);
				yEtaEta = (Result->y[loc_eta_up]-2*Result->y[loc]+Result->y[loc_eta_down])/(etaDelta*etaDelta);
			}

			if ((i>0) && (i<Result->im-1) && (j>0) && (j<Result->jm-1))
			{
				xKsiEta = (Result->x[loc_ksi_up_eta_up]+Result->x[loc_ksi_down_eta_down]-Result->x[loc_ksi_down_eta_up]-Result->x[loc_ksi_up_eta_down])/(4*ksiDelta*etaDelta);

				yKsiEta = (Result->y[loc_ksi_up_eta_up]+Result->y[loc_ksi_down_eta_down]-Result->y[loc_ksi_down_eta_up]-Result->y[loc_ksi_up_eta_down])/(4*ksiDelta*etaDelta);
			}
			else
			{
				xKsiEta = 0;
				yKsiEta = 0;
			}

			jac  = xKsi*yEta - yKsi*xEta;
			ksiX = +yEta/jac;
			ksiY = -xEta/jac;
			etaX = -yKsi/jac;
			etaY = +xKsi/jac;

			/* Save in arrays */
			Result->xKsi[loc] = xKsi;
			Result->xEta[loc] = xEta;
			Result->yKsi[loc] = yKsi;
			Result->yEta[loc] = yEta;

			Result->xKsiKsi[loc] = xKsiKsi;
			Result->xEtaEta[loc] = xEtaEta;
			Result->xKsiEta[loc] = xKsiEta;
			Result->yKsiKsi[loc] = yKsiKsi;
			Result->yEtaEta[loc] = yEtaEta;
			Result->yKsiEta[loc] = yKsiEta;

			Result->ksiX[loc] = ksiX;
			Result->ksiY[loc] = ksiY;
			Result->etaX[loc] = etaX;
			Result->etaY[loc] = etaY;

			Result->jac[loc]  = jac;
		}
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION CALCMETRICS *****\n\n");

		fprintf(log, "  j   i       ksiX       ksiY       etaX       etaY        jac       xKsi       xEta       yKsi       yEta    xKsiKsi    xEtaEta    xKsiEta    yKsiKsi    yEtaEta    yKsiEta\n");

		for(j=0; j<Result->jm; j++)
		{
			for(i=0; i<Result->im; i++)
			{
				loc      = Loc(&(*Result), j, i);

				fprintf(log, "%3d %3d %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f\n", j, i, Result->ksiX[loc], Result->ksiY[loc], Result->etaX[loc], Result->etaY[loc], Result->jac[loc], Result->xKsi[loc], Result->xEta[loc], Result->yKsi[loc], Result->yEta[loc], Result->xKsiKsi[loc], Result->xEtaEta[loc], Result->xKsiEta[loc], Result->yKsiKsi[loc], Result->yEtaEta[loc], Result->yKsiEta[loc]);
			}
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

