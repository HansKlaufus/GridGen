/*
** C-file for Position.
** Constructs the aerofoil and positions it in the windtunnel
**
** In:       tData  Data     = structure with all data
**
** Out:      tData  Data
**
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>
#include <math.h>

#include "gridgen.h"
#include "position.h"

int Position(FILE *log, tData* Data)
{
	int  ret;
	int  i;

	double xAerofoil;
	double yAerofoil;

	printf("Positioning...\n");

	ret = 0;
	
	for(i=0; i<Data->numData; i++)
	{
		xAerofoil = Data->xData[i]*Data->chord/100;
		yAerofoil = Data->yData[i]*Data->chord/100;

		Data->xData[i] = Data->height/2 + xAerofoil*cos(Data->alpha*PI/180) + yAerofoil*sin(Data->alpha*PI/180);
		Data->yData[i] = Data->height/2 - xAerofoil*sin(Data->alpha*PI/180) + yAerofoil*cos(Data->alpha*PI/180);
	}
	
	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION POSITION *****\n\n");
		fprintf(log, "   X          Y\n");

		for (i=0; i<Data->numData; i++)
		{
			fprintf(log, "%10.6f %10.6f\n", Data->xData[i], Data->yData[i]);
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

