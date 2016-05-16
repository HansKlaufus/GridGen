/*
** Function Main
** Main routine of program GridGen
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "gridgen.h"
#include "data.h"
#include "geometry.h"
#include "memory.h"
#include "structured.h"
#include "unstructured.h"
#include "quality.h"

int main(int argc, char *argv[])
{
	int    ret;
	int    i;

	int    debug;

	time_t t1, t2;

	FILE   *logFile = NULL;
	char   dataFileName[50];
	char   outputFormat, output[1];

	tData   Data;
	tResult Result;

	printf("\nStarting program GridGen...\n");

	ret        = 0;
	debug      = 0;
	strcpy(dataFileName, "gridgen.in");
	outputFormat = 'B';

	/* get  commandline arguments */
	for(i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "-l") == 0)
		{
			/* Set logging on */
			logFile = fopen("gridgen.log", "w");
			debug   = 1;
		}
		else if (strcmp(argv[i], "-p") == 0)
		{
			/* Define output formats */
			strcpy(output, argv[++i]);
			if (output[0] == 'g' || output[0] == 'G')
				/* GNUPlot only */
				outputFormat = 'G';
			else if (output[0] == 'v' || output[0] == 'V')
				/* VIGIE only */
				outputFormat = 'V';
			else
				/* GNUPLot and VIGIE */
				outputFormat = 'B';
		}
		else if (strcmp(argv[i], "-f") == 0)
		{
			/* Use different datafile */
			strcpy(dataFileName, argv[++i]);
		}
		else
		{
			printf("\nUnknown commandline option: '%s'\n", argv[i]);
			printf("Use : gridgen [-l] [-p G|V|B] [-f FILENAME]\n");
			ret = -1;
		}
	}

	if (logFile || (debug == 0))
	{
		/* Read data from file */
		if (ret != -1)
			ret = ReadData(logFile, dataFileName,  &Data);

		/* Initialise all arrays */
		if (ret != -1)
			ret = Initialise(logFile, &Data, &Result);

		/* Set start time */
		t1 = time(&t1);

		/* Set up the geometry */
		if (ret != -1)
			ret = SetUpGeometry(logFile, &Data, &Result);

		/* Do the structured part */
		if (ret != -1)
			ret = Structured(logFile, &Data, &Result);

		/* Do the unstructured part */
		if (ret != -1 && Data.gridType == 'U')
			ret = Unstructured(logFile, &Data, &Result);

		/* Do the quality checks */
		if (ret != -1)
			ret = Quality(logFile, &Result);

		/* Set end time */
		t2 = time(&t2);
		printf("Calculation time = %d sec.\n", (int) (t2-t1));

		/* Write the data to outputfile */
		if (ret != -1)
			ret = WriteData(logFile, outputFormat, &Result);

		/* Finish program */
		if (ret != -1)
			ret = Finish(&Data, &Result);

		if (logFile)
		{
			fclose(logFile);

			if (ret == -1)
				printf("\nERRORS occurred, please read LOG.\n");

			printf("Log can be found in gridgen.log\n");
		}
	}
	else
	{
		fprintf(stderr, "***** ERROR: Could not open logFile: gridgen.log.\n");
		ret = -1;
	}

	printf("Done.\n\n");

	return ret;
}

