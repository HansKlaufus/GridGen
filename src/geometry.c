/*
** C-file for SetUpGeometry
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>

#include "gridgen.h"
#include "geometry.h"
#include "boundary.h"
#include "cut.h"
#include "position.h"
#include "spline.h"

int SetUpGeometry(FILE *log, tData *Data, tResult *Result)
{
	int    ret;

	printf("\nSetting up geometry...\n");

	ret = 0;

	/* Position control points in field */
	if (ret != -1)
		ret = Position(&(*log), &(*Data));

	/* Spline data and return nodes */
	if (ret != -1)
		ret = Spline(&(*log), &(*Data), &(*Result));

	/* Make a clear cut to form ETA = 0 line */
	if (ret != -1)
		ret = MakeCut(&(*log), &(*Data), &(*Result));

	/* Build Boundaries */
	if (ret != -1)
		ret = BuildBoundaries(&(*log), &(*Data), &(*Result));

	if (log)
	{
		fprintf(log, "\n\n***** FUNCTION SETUPGEOMETRY *****\n\n");

		if (ret != -1)
		{
			fprintf(log, "Function SetUpGeometry successfully ended\n");
		}
		else
		{
			fprintf(log, "Function SetUpGeometry NOT successfully ended\n");
		}

		fprintf(log, "\n\n**********************************\n\n");
	}

	return ret;
}

