/*
** Function Triangulate
** Creates triangular elements.
**
** In:       tResult Result   = structure containing Results
**           
** Out:      tResult Result   = structure containing Results
**
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>
#include <stdlib.h>

#include "gridgen.h"
#include "loc.h"
#include "triangle.h"

int Triangulate(FILE *log, tResult* Result)
{
	int    ret;

	int    i, j;

	int    triangleNum;
	int    triangleNumWest, triangleNumEast;

	int    nodeSW, nodeSE, nodeNW, nodeNE;

	printf("Triangulating...\n");

	ret = 0;

	/* Set the characteristics in the Result structure */
	Result->numElements     = 2*(Result->im-1)*(Result->jm-1);
	Result->elementType     = etTriangle;
	Result->nodesPerElement = 3;

	/* Allocate memory */
	if (Result->element != NULL)
		free(Result->element);

	Result->element = (tElement*)malloc(Result->numElements*sizeof(tElement));

	/* Fill the triangles array */
	triangleNum = 0;
	for(j=0; j<Result->jm-1; j++)
	{
		for(i=0; i<Result->im-1; i++)
		{
			/* Calculate the number of the current 2 triangles */
			triangleNumWest = triangleNum++;
			triangleNumEast = triangleNum++;

			/* Select the nodes */
			nodeSW = Loc(&(*Result), j, i);
			nodeSE = Loc(&(*Result), j, i+1);
			nodeNW = Loc(&(*Result), j+1, i);
			nodeNE = Loc(&(*Result), j+1, i+1);

			/* The west triangle consists out of nodeSW, nodeSE and nodeNW */
			Result->element[triangleNumWest].triangle.node[0] = nodeSE;
			Result->element[triangleNumWest].triangle.node[1] = nodeSW;
			Result->element[triangleNumWest].triangle.node[2] = nodeNW;

			/* The east triangle consists out of nodeSE, nodeNW, nodeNE */
			Result->element[triangleNumEast].triangle.node[0] = nodeSE;
			Result->element[triangleNumEast].triangle.node[1] = nodeNW;
			Result->element[triangleNumEast].triangle.node[2] = nodeNE;
		}
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION TRIANGULATE *****\n\n");

		fprintf(log, "  N Node1 Node2 Node3\n");

		for(i=0; i<Result->numElements; i++)
		{
			nodeSE = Result->element[i].quadrangle.node[0];
			nodeSW = Result->element[i].quadrangle.node[1];
			nodeNW = Result->element[i].quadrangle.node[2];

			fprintf(log, "%3d   %3d   %3d   %3d\n", i, nodeSE, nodeSW, nodeNW);
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

