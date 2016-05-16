/*
** C-file for Interpolate
** Interpolates all boundary nodes.
**
** In:       tResult Result   = structure containing Results
** Out:      tResult Result   = structure containing Results
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>

#include "gridgen.h"
#include "interpolate.h"
#include "loc.h"

int Interpolate(FILE *log, tResult* Result)
{
	int    ret;
	int    i, j;

	int    loc;
	int    loc_eta_min, loc_eta_max;
	int    loc_ksi_min, loc_ksi_max;
	int    loc_ksi_min_eta_min, loc_ksi_min_eta_max, loc_ksi_max_eta_min, loc_ksi_max_eta_max;

	double x, xEtaMin, xEtaMax, xKsiMin, xKsiMax;
	double y, yEtaMin, yEtaMax, yKsiMin, yKsiMax;
	double xKsiMinEtaMin, xKsiMinEtaMax, xKsiMaxEtaMin, xKsiMaxEtaMax;
	double yKsiMinEtaMin, yKsiMinEtaMax, yKsiMaxEtaMin, yKsiMaxEtaMax;

	double factor1, factor2, factor3, factor4;

	printf("Interpolating...\n");

	ret = 0;

	loc_ksi_min_eta_min = Loc(&(*Result), 0, 0);
	loc_ksi_max_eta_min = Loc(&(*Result), 0, Result->im-1);
	loc_ksi_min_eta_max = Loc(&(*Result), Result->jm-1, 0);
	loc_ksi_max_eta_max = Loc(&(*Result), Result->jm-1, Result->im-1);

	xKsiMinEtaMin = Result->x[loc_ksi_min_eta_min];
	xKsiMaxEtaMin = Result->x[loc_ksi_max_eta_min];
	xKsiMinEtaMax = Result->x[loc_ksi_min_eta_max];
	xKsiMaxEtaMax = Result->x[loc_ksi_max_eta_max];

	yKsiMinEtaMin = Result->y[loc_ksi_min_eta_min];
	yKsiMaxEtaMin = Result->y[loc_ksi_max_eta_min];
	yKsiMinEtaMax = Result->y[loc_ksi_min_eta_max];
	yKsiMaxEtaMax = Result->y[loc_ksi_max_eta_max];

	for(j=1; j<Result->jm-1; j++)
	{
		loc_ksi_min = Loc(&(*Result), j, 0);
		loc_ksi_max = Loc(&(*Result), j, Result->im-1);

		xKsiMin   = Result->x[loc_ksi_min];
		xKsiMax   = Result->x[loc_ksi_max];

		yKsiMin   = Result->y[loc_ksi_min];
		yKsiMax   = Result->y[loc_ksi_max];

		for(i=1; i<Result->im-1; i++)
		{
			loc         = Loc(&(*Result), j, i);
			loc_eta_min = Loc(&(*Result), 0, i);
			loc_eta_max = Loc(&(*Result), Result->jm-1, i);

			xEtaMin   = Result->x[loc_eta_min];
			xEtaMax   = Result->x[loc_eta_max];

			yEtaMin   = Result->y[loc_eta_min];
			yEtaMax   = Result->y[loc_eta_max];

			factor1   = (Result->im-1 - i)/(double)(Result->im-1);
			factor2   = i/(double)(Result->im-1);
			factor3   = (Result->jm-1 - j)/(double)(Result->jm-1);
			factor4   = j/(double)(Result->jm-1);

			x = factor1*xKsiMin               + factor2*xKsiMax +
			    factor3*xEtaMin               + factor4*xEtaMax -
			    factor1*factor3*xKsiMinEtaMin - factor2*factor3*xKsiMaxEtaMin -
			    factor1*factor4*xKsiMinEtaMax - factor2*factor4*xKsiMaxEtaMax;

			y = factor1*yKsiMin               + factor2*yKsiMax +
			    factor3*yEtaMin               + factor4*yEtaMax -
			    factor1*factor3*yKsiMinEtaMin - factor2*factor3*yKsiMaxEtaMin -
			    factor1*factor4*yKsiMinEtaMax - factor2*factor4*yKsiMaxEtaMax;

			Result->x[loc] = x;
			Result->y[loc] = y;
		}
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION INTERPOLATE *****\n\n");

		fprintf(log, "  j   i          x          y\n");

		for(j=0; j<Result->jm; j++)
		{
			for(i=0; i<Result->im; i++)
			{
				loc = Loc(&(*Result), j, i);

				fprintf(log, "%3d %3d %10.6f %10.6f\n", j, i, Result->x[loc], Result->y[loc]);
			}
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

