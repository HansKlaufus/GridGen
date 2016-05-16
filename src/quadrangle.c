/*
** Function Quadrangulate
** Creates quadrangular elements.
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
#include "quadrangle.h"

int Quadrangulate(FILE *log, tResult* Result)
{
	int    ret;
	int    i, j;
	int    quadrangleNum;
	int    nodeSW, nodeSE, nodeNW, nodeNE;

	printf("Quadrangulating...\n");

	ret = 0;

	/* Set the characateristics in the Result structure */
	Result->numElements     = (Result->im-1)*(Result->jm-1);
	Result->elementType     = etQuadrangle;
	Result->nodesPerElement = 4;

	/* Allocate memory */
	if (Result->element != NULL)
		free(Result->element);

	Result->element = (tElement*)malloc(Result->numElements*sizeof(tElement));

	/* Fill the quadrangles array */
	quadrangleNum = 0;
	for(j=0; j<Result->jm-1; j++)
	{
		for(i=0; i<Result->im-1; i++)
		{
			/* Select the nodes */
			nodeSE = Loc(&(*Result), j, i+1);
			nodeSW = Loc(&(*Result), j, i);
			nodeNW = Loc(&(*Result), j+1, i);
			nodeNE = Loc(&(*Result), j+1, i+1);

			/* The quadrangle consists out of nodeSW, nodeSE, nodeNW and node NE*/
			Result->element[quadrangleNum].quadrangle.node[0] = nodeSE;
			Result->element[quadrangleNum].quadrangle.node[1] = nodeSW;
			Result->element[quadrangleNum].quadrangle.node[2] = nodeNW;
			Result->element[quadrangleNum].quadrangle.node[3] = nodeNE;

			/* Calculate the number of the current quadrangle */
			quadrangleNum++;
		}
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION QUADRANGULATE *****\n\n");

		fprintf(log, "  N Node1 Node2 Node3 Node4\n");

		for(i=0; i<Result->numElements; i++)
		{
			nodeSE = Result->element[i].quadrangle.node[0];
			nodeSW = Result->element[i].quadrangle.node[1];
			nodeNW = Result->element[i].quadrangle.node[2];
			nodeNE = Result->element[i].quadrangle.node[3];

			fprintf(log, "%3d   %3d   %3d   %3d   %3d\n", i, nodeSE, nodeSW, nodeNW, nodeNE);
		}

		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

