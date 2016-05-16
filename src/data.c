/*
** Function ReadData.
** Reads data from data-file 'gridgen.in'
**
** In:       -
** Out:      Data = structure containing all data
** Return:   0 on success, -1 on failure
**
** Datafile: gridgen.in or user defines filename
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>
#include <stdlib.h>

#include "gridgen.h"
#include "data.h"
#include "loc.h"

int ReadData(FILE *log, char *dataFileName, tData *Data)
{
	FILE   *dataFile;
	int    ret;
	int    i;

	int    numLines;
	char   *endOfFile;
	char   line[100];

	char   gridType;
	char   distributionType;
	int    numNodes1;
	int    numNodes2;
	int    numNodes3;
	int    numData;
	double chord;
	double alpha;
	double length;
	double height;
	double omegaElliptic, omegaSmooth;
	double *xData = NULL;
	double *yData = NULL;

	printf("Reading data...\n");

	ret = 0;

	dataFile = fopen(dataFileName, "r");
	if (dataFile)
	{
		/* Read number of lines in file */
		numLines = 0;
		endOfFile = fgets(line, 100, dataFile);
		while (endOfFile != NULL)
		{
			numLines++;
			endOfFile = fgets(line, 100, dataFile);
		}
		rewind(dataFile);

		/* Allocate memory for temporary data arrays */
		xData = (double*)malloc(numLines*sizeof(double));
		yData = (double*)malloc(numLines*sizeof(double));

		if (xData == NULL || yData == NULL)
		{
			fprintf(stderr, "ERROR in function ReadData: Could not allocate memory.\n");
			ret = -1;
		}
		else
		{
			/* Read data from file */
			fscanf(dataFile, "%c%c",             &gridType,  &distributionType);
			fscanf(dataFile, "%d %d %d",         &numNodes1, &numNodes2, &numNodes3);
			fscanf(dataFile, "%lf %lf",          &omegaElliptic, &omegaSmooth);
			fscanf(dataFile, "%lf %lf",          &chord, &alpha);
			fscanf(dataFile, "%lf %lf",          &length, &height);

			i = 0;
			while (feof(dataFile) == 0)
			{
				fscanf(dataFile, "%lf %lf", &xData[i], &yData[i]);
				i++;
			}

			i--;
			numData = i;

			/* save data in structure */
			Data->gridType         = gridType;
			Data->distributionType = distributionType;
			Data->numNodes1        = numNodes1;
			Data->numNodes2        = numNodes2;
			Data->numNodes3        = numNodes3;
			Data->chord            = chord;
			Data->height           = height;
			Data->length           = length;
			Data->alpha            = alpha;
			Data->omegaElliptic    = omegaElliptic;
			Data->omegaSmooth      = omegaSmooth;

			/* For the xData and yData the complete aerofoil has to saved */
			Data->numData          = numData*2 - 1;

			Data->xData = (double *)malloc(Data->numData*sizeof(double));
			Data->yData = (double *)malloc(Data->numData*sizeof(double));

			if (Data->xData == NULL || Data->yData == NULL)
			{
				fprintf(stderr, "ERROR in function ReadData: Could not allocate memory.\n");
				ret = -1;
			}
			else
			{
				for (i=0; i<numData; i++)
				{
					/* First save data for upper aerofoil */
					Data->xData[numData - 1 + i] = +xData[i];
					Data->yData[numData - 1 + i] = +yData[i];

					/* Then save data for lower aerofoil */
					Data->xData[numData - 1 - i] = +xData[i];
					Data->yData[numData - 1 - i] = -yData[i];
				}
			}
		}

		/* Free allocated memory used for temporary arrays */
		if (xData)
			free(xData);

		if (yData)
			free(yData);

		fclose(dataFile);

		/* Write report */
		if (log)
		{
			fprintf(log, "\n***** FUNCTION READDATA *****\n\n");
			fprintf(log, "   type of grid                   = %c\n", Data->gridType);
			fprintf(log, "   distribution of nodes          = %c\n", Data->distributionType);
			fprintf(log, "   number of nodes along aerofoil = %d\n", Data->numNodes1);
			fprintf(log, "   number of nodes along cut      = %d\n", Data->numNodes2);
			fprintf(log, "   number of nodes normal         = %d\n", Data->numNodes3);
			fprintf(log, "   chord                          = %f\n", Data->chord);
			fprintf(log, "   height                         = %f\n", Data->height);
			fprintf(log, "   length                         = %f\n", Data->length);
			fprintf(log, "   alpha                          = %f\n", Data->alpha);
			fprintf(log, "   numdata                        = %d\n", Data->numData);
			fprintf(log, "   omegaElliptic                  = %f\n", Data->omegaElliptic);
			fprintf(log, "   omegaSmooth                    = %f\n", Data->omegaSmooth);
			fprintf(log, "\n");
			fprintf(log, "   X          Y\n");

			for (i=0; i<Data->numData; i++)
			{
				fprintf(log, "%10.6f %10.6f\n", Data->xData[i], Data->yData[i]);
			}

			fprintf(log, "\n*****************************\n\n");
		}
	}
	else
	{
		fprintf(stderr, "ERROR in function ReadData: Could not open '%s'.\n", dataFileName);
		ret = -1;
	}

	return ret;
}

/*
** Function WriteData.
** Writes data to data-file 'gridgen.out.1', 'gridgen.out.2' and 'grid.dat'
**
** In:       tResult Result = structure containing all results
**
** Out:      -
**
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

int WriteData(FILE *log, char format, tResult *Result)
{
	int   ret;

	printf("Writing data...\n");

	ret = 0;

	if ((ret!=-1) && (format == 'V' || format == 'B'))
		ret = WriteVigieData(&(*log), &(*Result));

	if ((ret!=-1) && (format == 'G' || format == 'B'))
		ret = WriteGNUData(&(*log), &(*Result));

	if (log)
	{
		fprintf(log, "\n***** FUNCTION WRITEDATA *****\n\n");

		if (ret != -1)
			fprintf(log, "Data succesfully written to file.\n");
		else
			fprintf(log, "Data NOT succesfully written to file.\n");

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}


/*
** Function WriteVigieData.
** Writes data in an ASCII format suitable for the visualisation
** program VIGIE.
**
** In:       tResult Result  = structure containing all results.
**
** Out:      -
**
** Return:   0 on success, -1 on failure
**
** Datfiles: gridgen.out: Datafile containing node-coordinates, node connectivity
**                        and node characteristics.
**           gridgen.des: Description file for gridgen.out.
**
** Author:   J.L. Klaufus
*/

int WriteVigieData(FILE *log, tResult *Result)
{
	FILE   *dataFile1 = NULL;
	FILE   *dataFile2 = NULL;

	int    ret;
	int    i;
	int    numElements;
	int    node1, node2, node3, node4;

	double *area     = NULL;
	double *aspect   = NULL;
	double *angle    = NULL;
	double *skewness = NULL;

	printf("Creating VIGIE datafiles...\n");

	ret = 0;

	dataFile1 = fopen("gridgen.des", "w");
	dataFile2 = fopen("gridgen.vig", "w");
	if (dataFile1 && dataFile2)
	{
		/* 
		** Write description file
		*/
		fprintf(dataFile1, "ascii2d\n");
		fprintf(dataFile1, "./gridgen.vig\n");

		/*
		**  Write data file
		*/

		/* Write point information */
		fprintf(dataFile2, "points %d\n", Result->im*Result->jm);
		for(i=0; i<Result->im*Result->jm; i++)
		{
			fprintf(dataFile2, "\t%10.6f %10.6f\n", Result->x[i], Result->y[i]);
		}

		/* Write connectivity information */
		numElements = Result->numElements;

		if(Result->elementType == etTriangle)
		{
			fprintf(dataFile2, "triangles %d\n", numElements);
			for(i=0; i<numElements; i++)
			{
				node1 = Result->element[i].triangle.node[0];
				node2 = Result->element[i].triangle.node[1];
				node3 = Result->element[i].triangle.node[2];

				fprintf(dataFile2, "\t%d %d %d\n", node1, node2, node3);
			}
		}
		else if (Result->elementType == etQuadrangle)
		{
			fprintf(dataFile2, "quadrangles %d\n", numElements);
			for(i=0; i<numElements; i++)
			{
				node1 = Result->element[i].quadrangle.node[0];
				node2 = Result->element[i].quadrangle.node[1];
				node3 = Result->element[i].quadrangle.node[2];
				node4 = Result->element[i].quadrangle.node[3];

				fprintf(dataFile2, "\t%d %d %d %d\n", node1, node2, node3, node4);
			}
		}
		else
		{
			fprintf(stderr, "ERROR in function WriteVigieData: Unknown elementtype.\n");
			ret = -1;
		}

		/* Write characteristics: EtaX */
		fprintf(dataFile2, "scalars EtaX\n");
		for(i=0; i<(Result->im*Result->jm); i++)
		{
			fprintf(dataFile2, "\t%10.6f\n", Result->etaX[i]);
		}

		/* Write characteristics: EtaY */
		fprintf(dataFile2, "scalars EtaY\n");
		for(i=0; i<(Result->im*Result->jm); i++)
		{
			fprintf(dataFile2, "\t%10.6f\n", Result->etaY[i]);
		}

		/* Write characteristics: KsiX */
		fprintf(dataFile2, "scalars KsiX\n");
		for(i=0; i<(Result->im*Result->jm); i++)
		{
			fprintf(dataFile2, "\t%10.6f\n", Result->ksiX[i]);
		}

		/* Write characteristics: KsiY */
		fprintf(dataFile2, "scalars KsiY\n");
		for(i=0; i<(Result->im*Result->jm); i++)
		{
			fprintf(dataFile2, "\t%10.6f\n", Result->ksiY[i]);
		}

		/* Write characteristics: Jacobian */
		fprintf(dataFile2, "scalars Jacobian\n");
		for(i=0; i<(Result->im*Result->jm); i++)
		{
			fprintf(dataFile2, "\t%10.6f\n", Result->jac[i]);
		}

		/* Set up temporary arrays for nodal characteristics */
		area     = (double*)malloc(Result->im*Result->jm*sizeof(double));
		aspect   = (double*)malloc(Result->im*Result->jm*sizeof(double));
		angle    = (double*)malloc(Result->im*Result->jm*sizeof(double));
		skewness = (double*)malloc(Result->im*Result->jm*sizeof(double));

		if(area == NULL || aspect == NULL || angle == NULL || skewness == NULL)
		{
			fprintf(stderr, "ERROR in funtion WriteStructuredData: Could not allocate memeory.\n");
			ret = -1;
		}
		else
		{
			/* Calculate the elemental characteristics at the nodes */
			if (ret != -1)
				ret = CalcCharAtNodes(&(*log), &(*Result), area, aspect, angle, skewness);

			/* Write the characteristics: ElementalArea */
			fprintf(dataFile2, "scalars ElementalArea\n");
			for(i=0; i<(Result->im*Result->jm); i++)
			{
				fprintf(dataFile2, "%10.6f\n", area[i]);
			}

			/* Write the characteristics: AspectRatio */
			fprintf(dataFile2, "scalars AspectRatio\n");
			for(i=0; i<(Result->im*Result->jm); i++)
			{
				fprintf(dataFile2, "\t%10.6f\n", aspect[i]);
			}

			/* Write the characteristics: MinimumAngle */
			fprintf(dataFile2, "scalars MinimumAngle\n");
			for(i=0; i<(Result->im*Result->jm); i++)
			{
				fprintf(dataFile2, "\t%10.6f\n", angle[i]);
			}

			/* Write the characteristics: Skewness */
			if (Result->elementType != etTriangle)
			{
				fprintf(dataFile2, "scalars Skewness\n");
				for(i=0; i<(Result->im*Result->jm); i++)
				{
					fprintf(dataFile2, "\t%10.6f\n", skewness[i]);
				}
			}
		}

		/* Free the used memory */
		if(area != NULL)
			free(area);

		if(aspect != NULL)
			free(aspect);

		if(angle != NULL)
			free(angle);

		if(skewness != NULL)
			free(skewness);
	}
	else
	{
		ret = -1;
	}

	/* Close the files */
	if (dataFile1)
		fclose(dataFile1);

	if (dataFile2)
		fclose(dataFile2);

	/* Write report */
	if (log)
	{
		fprintf(log, "\n\n***** FUNCTION WRITEVIGIEDATA *****\n\n");

		if(ret != -1)
			fprintf(log, "VIGIE files successfully created.\n");
		else
			fprintf(log, "VIGIE files NOT successfully created.\n");

		fprintf(log, "\n\n***********************************\n\n");
	}

	return ret;
}


/*
** Function WriteGNUData.
** Writes data in an ASCII format suitable for the visualisation
** package GNUPlot.
**
** In:       tResult Result  = structure containing all results.
**
** Out:      -
**
** Return:   0 on success, -1 on failure
**
** Datfiles: gridgen.gnu.1: Data file containing node coordinates.
**           gridgen.gnu.2: Data file containing node characteristics.
**
** Author:   J.L. Klaufus
*/

int WriteGNUData(FILE *log, tResult *Result)
{
	FILE   *dataFile1 = NULL;
	FILE   *dataFile2 = NULL;

	int    ret;
	int    i, j;
	int    loc;
	int    numElements;
	int    node1, node2, node3, node4;

	printf("Creating GNUPlot datafiles...\n"); 

	ret = 0;

	numElements = Result->numElements;

	/*
	** Write data for GNUPlot
	*/
	dataFile1 = fopen("gridgen.gnu.1", "w");
	dataFile2 = fopen("gridgen.gnu.2", "w");
	if (dataFile1 && dataFile2)
	{
		/* Write the elements */
		fprintf(dataFile1, "#        X          Y\n");
		for(i=0; i<numElements; i++)
		{
			if (Result->elementType == etTriangle)
			{
				node1 = Result->element[i].triangle.node[0];
				node2 = Result->element[i].triangle.node[1];
				node3 = Result->element[i].triangle.node[2];

				fprintf(dataFile1, "%10.6f %10.6f\n", Result->x[node1], Result->y[node1]);
				fprintf(dataFile1, "%10.6f %10.6f\n", Result->x[node2], Result->y[node2]);
				fprintf(dataFile1, "%10.6f %10.6f\n", Result->x[node3], Result->y[node3]);
				fprintf(dataFile1, "%10.6f %10.6f\n", Result->x[node1], Result->y[node1]);

				fprintf(dataFile1, "\n");
			}
			else if (Result->elementType == etQuadrangle)
			{
				node1 = Result->element[i].quadrangle.node[0];
				node2 = Result->element[i].quadrangle.node[1];
				node3 = Result->element[i].quadrangle.node[2];
				node4 = Result->element[i].quadrangle.node[3];

				fprintf(dataFile1, "%10.6f %10.6f\n", Result->x[node1], Result->y[node1]);
				fprintf(dataFile1, "%10.6f %10.6f\n", Result->x[node2], Result->y[node2]);
				fprintf(dataFile1, "%10.6f %10.6f\n", Result->x[node3], Result->y[node3]);
				fprintf(dataFile1, "%10.6f %10.6f\n", Result->x[node4], Result->y[node4]);
				fprintf(dataFile1, "%10.6f %10.6f\n", Result->x[node1], Result->y[node1]);

				fprintf(dataFile1, "\n");
			}
			else
			{
				fprintf(stderr, "ERROR in function WriteGNUData: Unknown elementtype.\n");
				ret = -1;
			}
		}

		/* Write the characteristics */
		fprintf(dataFile2, "#      Ksi        Eta       KsiX       KsiY       EtaX       EtaY        Jac       xKsi       yKsi       xEta       yEta    xKsiKsi    xKsiEta    xEtaEta\n");
		for(j=0; j<Result->jm; j++)
		{
                	for(i=0; i<Result->im; i++)
			{
				loc = Loc(&(*Result), j, i);
				fprintf(dataFile2, "%10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f\n", Result->ksi[loc], Result->eta[loc], Result->ksiX[loc], Result->ksiY[loc], Result->etaX[loc], Result->etaY[loc], Result->jac[loc], Result->xKsi[loc], Result->yKsi[loc], Result->xEta[loc], Result->yEta[loc], Result->xKsiKsi[loc], Result->xKsiEta[loc], Result->xEtaEta[loc]);
			}

			fprintf(dataFile2, "\n");
		}

		for(i=0; i<Result->im; i++)
		{
                	for(j=0; j<Result->jm; j++)
			{
				loc = Loc(&(*Result), j, i);
/*
				fprintf(dataFile2, "%10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f\n", Result->ksi[loc], Result->eta[loc], Result->ksiX[loc], Result->ksiY[loc], Result->etaX[loc], Result->etaY[loc], Result->jac[loc], Result->xKsi[loc], Result->yKsi[loc], Result->xEta[loc], Result->yEta[loc]);
*/
				fprintf(dataFile2, "%10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f %10.6f\n", Result->ksi[loc], Result->eta[loc], Result->ksiX[loc], Result->ksiY[loc], Result->etaX[loc], Result->etaY[loc], Result->jac[loc], Result->xKsi[loc], Result->yKsi[loc], Result->xEta[loc], Result->yEta[loc], Result->xKsiKsi[loc], Result->xKsiEta[loc], Result->xEtaEta[loc]);
			}

			fprintf(dataFile2, "\n");
		}
	}
	else
	{
		ret = -1;
	}

	/* Close the files */
	if (dataFile1)
		fclose(dataFile1);

	if (dataFile2)
		fclose(dataFile2);


	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION WRITEGNUDATA *****\n\n");

		if (ret != -1)
			fprintf(log, "GNU data succesfully written to file.\n");
		else
			fprintf(log, "GNU data NOT succesfully written to file.\n");

		fprintf(log, "\n*********************************\n\n");
	}

	return ret;
}


/*
** Function CalcCharAtNodes.
** Calculates values for elemental characteristics at the nodes.
** This is done by searching all elements sharing the node, and averaging
** the characteristics.
**
** In:       tResult Result  = structure containing all results.
**
** Out:      double  area    = array containing information about elemental
**                             areas at all nodes.
**           double  aspect  = array containing information about elemental
**                             aspect ratios at all nodes.
**           double  angle   = array containing information about elemental
**                             minimal angles at all node.
**           double  skewness= array containing information about elemental
**                             skewness at all nodes.
**
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

int CalcCharAtNodes(FILE *log, tResult *Result, double *area, double *aspect, double *angle, double *skewness)
{
	int ret;
	int e, ee, n;
	int maxPossible;
	int elementsFound;
	int *elementID = NULL;

	/*printf("Calculating element characteristics at nodes...\n");*/

	ret = 0;

	/* Allocate memory for temporary array */
	if(Result->elementType == etTriangle)
		maxPossible = 6;
	else if(Result->elementType == etQuadrangle)
		maxPossible = 4;
	else
	{
		maxPossible = 0;
		ret         = -1;
	}

	elementID = (int*)malloc(maxPossible*sizeof(int));

	if(elementID == NULL)
	{
		fprintf(stderr, "ERROR in function CalcCharAtNodes: Could not allocate memory.\n");
		ret = -1;
	}
	else
	{
		for(n=0; n<(Result->im*Result->jm); n++)
		{
			/* Find the elements that use the current node */
			if(ret != -1)
				ret = FindElements(&(*log), &(*Result), n, &elementsFound, elementID);

			area[n]     = 0;
			aspect[n]   = 0;
			angle[n]    = 0;
			skewness[n] = 0;

			for(e=0; e<elementsFound; e++)
			{
				ee = elementID[e];

				/* Calculate the average elemental characteristics at the node */
				if(Result->elementType == etTriangle)
				{
					area[n]     += Result->element[ee].triangle.elementalArea/(double)(elementsFound);
					aspect[n]   += Result->element[ee].triangle.aspectRatio/(double)(elementsFound);
					angle[n]    += Result->element[ee].triangle.minimumAngle/(double)(elementsFound);
				}
				else if(Result->elementType == etQuadrangle)
				{
					area[n]     += Result->element[ee].quadrangle.elementalArea/(double)(elementsFound);
					aspect[n]   += Result->element[ee].quadrangle.aspectRatio/(double)(elementsFound);
					angle[n]    += Result->element[ee].quadrangle.minimumAngle/(double)(elementsFound);
					skewness[n] += Result->element[ee].quadrangle.skewness/(double)(elementsFound);
				}
				else
				{
					ret = -1;
				}
			}
		}
	}

	/* Free the memory used */
	if(elementID != NULL)
		free(elementID);

	/* Write report */
	if (log)
	{
		fprintf(log, "\n\n***** FUNCTION CALCCHARATNODES *****\n\n");

		fprintf(log, "Node       Area         AR      Angle    Skewness\n");

		for(n=0; n<Result->im*Result->jm; n++)
		{
			fprintf(log, " %3d %10.6f %10.6f %10.6f %10.6f\n", n, area[n], aspect[n], angle[n], skewness[n]);
		}

		fprintf(log, "\n\n************************************\n\n");
	}

	return ret;
}


/*
** Function FindElements.
** Find all elements sharing a specified node.
**
** In:       tResult Result  = structure containing all results.
**           int     node    = node identifier
**
** Out:      int     elementsFound = number of elements sharing the specified node
**           int     elementID     = array containing the element identifiers shring
**                                   the specified node.
**
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

int FindElements(FILE *log, tResult *Result, int node, int *elementsFound, int *elementID)
{
	int ret;
	int e, n;
	int maxPossible;
	int found;

	/*printf("Searching elements...\n");*/

	ret = 0;

	if(Result->elementType == etTriangle)
		maxPossible = 6;
	else if(Result->elementType == etQuadrangle)
		maxPossible = 4;
	else
	{
		maxPossible = 0;
		ret         = -1;
	}

	e              = 0;
	*elementsFound = 0;
	while((e < Result->numElements) && (*elementsFound < maxPossible) && (ret != -1))
	{
		found = 0;
		n = 0;
		while (n<Result->nodesPerElement && found == 0 && ret != -1)
		{
			if (Result->elementType == etTriangle)
				found = (node == Result->element[e].triangle.node[n] ? 1 : 0);
			else if (Result->elementType == etQuadrangle)
				found = (node == Result->element[e].quadrangle.node[n] ? 1 : 0);
			else
				ret = -1;

			n++;
		}

		if(found == 1)
			elementID[(*elementsFound)++] = e;

		e++;
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n\n***** FUNTION FINDELEMENTS *****\n\n");

		fprintf(log, "Node ElementID\n");

		for(e = 0; e < *elementsFound; e++)
		{
			fprintf(log, " %3d       %3d\n", node, elementID[e]);
		}

		fprintf(log, "\n\n********************************\n\n");
	}

	return ret;
}

