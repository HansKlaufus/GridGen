/*
** Function Unstructured
** Creates an unstructured grid from a structured one by dividing
** quadrilaterals into two triangles.
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

#include "gridgen.h"
#include "metrics.h"
#include "unstructured.h"
#include "smooth.h"
#include "triangle.h"

int Unstructured(FILE *log, tData *Data, tResult *Result)
{
	int    ret;

	printf("Destructuring grid...\n");

	ret = 0;

	/* Triangulate */
	if (ret != -1)
		ret = Triangulate(&(*log), &(*Result));

	/* Apply smoothing */
	if (ret != -1)
		ret = Smooth(&(*log), &(*Data), &(*Result));

	/* Recalculate all metrics */
	if (ret != -1)
		ret = CalcMetrics(&(*log), &(*Result));

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION UNSTRUCTURED *****\n\n");

		if (ret != -1)
		{
			fprintf(log, "Succesfully destructured.\n");
		}
		else
		{
			fprintf(log, "NOT succesfully destructured.\n");
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

