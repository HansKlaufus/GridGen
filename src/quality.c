/*
** Function Quality
**
** Calculates quality parameters such as elemental area, aspect ratio,
** skewness and minimum angle for each element.
**
** In:         tResult Result   = structure containing results
** Out:        tResult Result   = structure containing results.
** Return:     0 = success, -1 = failure.
**
** Author:     J.L. Klaufus
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gridgen.h"
#include "quality.h"

int Quality(FILE *log, tResult *Result)
{
	int ret;
	int n;

	double area, aspectRatio, skewness, minAngle;

	printf("Calculating quality parameters...\n");

	ret         = 0;
	area        = 0;
	aspectRatio = 0;
	skewness    = 0;
	minAngle    = 0;

	/* Calculate the elemental areas */
	if (ret != -1)
		ret = CalcElementalArea(&(*log), &(*Result));

	/* Calculate the skewnesses */
	if (ret != -1)
		ret = CalcSkewness(&(*log), &(*Result));

	/* Calculate the aspect ratios */
	if (ret != -1)
		ret = CalcAspectRatio(&(*log), &(*Result));

	/* Calculate the minimum angles */
	if (ret != -1)
		ret = CalcMinimumAngle(&(*log), &(*Result));

	/* Write report */
	if (log)
	{
		fprintf(log, "\n\n***** FUNCTION QUALITY *****\n\n");

		fprintf(log, "  N       Area         AR   Skewness   MinAngle\n");

		for(n=0; n<Result->numElements; n++)
		{
			if (Result->elementType == etTriangle)
			{
				area        = Result->element[n].triangle.elementalArea;
				aspectRatio = Result->element[n].triangle.aspectRatio;
				skewness    = 0;
				minAngle    = Result->element[n].triangle.minimumAngle;
			}
			else if (Result->elementType == etQuadrangle)
			{
				area        = Result->element[n].quadrangle.elementalArea;
				aspectRatio = Result->element[n].quadrangle.aspectRatio;
				skewness    = Result->element[n].quadrangle.skewness;
				minAngle    = Result->element[n].quadrangle.minimumAngle;
			}
			else
			{
				fprintf(stderr, "ERROR in funtion Quality: Unknown element type.\n");
				ret = -1;
			}

			fprintf(log, "%3d %10.6f %10.6f %10.6f %10.6f\n", n, area, aspectRatio, skewness, minAngle);
		}

		fprintf(log, "\n\n*********************************\n\n");
	}

	return ret;
}

/*
** Function CalcElementalArea
** Calculates the area of each element.
**
** In:      tResult Result    =  structure containing results
** Out:     tResult Result    =  structure containing results
** Return:  0 on succes / -1 on failure
**
** Author:     J.L. Klaufus
*/

int CalcElementalArea(FILE *log, tResult *Result)
{
	int ret;
	int e;
	int nodePrev, nodeCur, nodeNext;

	double vecA[2], vecB[2];
	double lengthA, lengthB;
	double angle;
	double area;

	/*printf("Calculating elemental areas...\n");*/

	ret = 0;

	for(e=0; e<Result->numElements; e++)
	{
		if(Result->elementType == etTriangle)
		{
			/* Set up two vectors intersecting at node 1 */
			nodePrev = Result->element[e].triangle.node[0];
			nodeCur  = Result->element[e].triangle.node[1];
			nodeNext = Result->element[e].triangle.node[2];

			vecA[0] = Result->x[nodeCur] - Result->x[nodePrev];
			vecA[1] = Result->y[nodeCur] - Result->y[nodePrev];
			vecB[0] = Result->x[nodeCur] - Result->x[nodeNext];
			vecB[1] = Result->y[nodeCur] - Result->y[nodeNext];

			lengthA = sqrt(vecA[0]*vecA[0] + vecA[1]*vecA[1]);
			lengthB = sqrt(vecB[0]*vecB[0] + vecB[1]*vecB[1]);

			angle = acos((vecA[0]*vecB[0] + vecA[1]*vecB[1])/(lengthA*lengthB));
			area  = 0.5*lengthA*lengthB*sin(angle);

			Result->element[e].triangle.elementalArea = area;
		}
		else if (Result->elementType == etQuadrangle)
		{
			/* Set up two vectors intersecting at node 1 */
			nodePrev = Result->element[e].quadrangle.node[0];
			nodeCur  = Result->element[e].quadrangle.node[1];
			nodeNext = Result->element[e].quadrangle.node[2];

			vecA[0] = Result->x[nodeCur] - Result->x[nodePrev];
			vecA[1] = Result->y[nodeCur] - Result->y[nodePrev];
			vecB[0] = Result->x[nodeCur] - Result->x[nodeNext];
			vecB[1] = Result->y[nodeCur] - Result->y[nodeNext];

			lengthA = sqrt(vecA[0]*vecA[0] + vecA[1]*vecA[1]);
			lengthB = sqrt(vecB[0]*vecB[0] + vecB[1]*vecB[1]);

			angle = acos((vecA[0]*vecB[0] + vecA[1]*vecB[1])/(lengthA*lengthB));
			area  = lengthA*lengthB*sin(angle);

			Result->element[e].quadrangle.elementalArea = area;
		}
		else
		{
			fprintf(stderr, "ERROR in funtion CalcSkewness: Unknown element type\n");
			ret = -1;
		}
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n\n***** FUNCTION CALCELEMENTALAREA *****\n\n");

		if (ret != -1)
			fprintf(log, "Function CalcElementalArea successfully ended.\n");
		else
			fprintf(log, "Function CalcElementalArea NOT successfully ended.\n");

		fprintf(log, "\n\n**************************************\n\n");
	}

	return ret;
}

/*
** Function CalcSkewness
** Calculates the skewness of each element.
**
** Skewness = (minimum diagonal) / (maximum diagonal)
**
** In:      tResult Result    =  structure containing results
** Out:     tResult Result    =  structure containing results
** Return:  0 on succes / -1 on failure
**
** Author:     J.L. Klaufus
*/

int CalcSkewness(FILE *log, tResult *Result)
{
	int ret;
	int e;
	int nodeSE, nodeSW, nodeNW, nodeNE;

	double diagNWSE, diagSWNE;
	double skewness;

	/*printf("Calculating skewnesses...\n");*/

	ret = 0;

	if (Result->elementType == etTriangle)
	{
		fprintf(stderr, "\nWARNING in funtion CalcSkewness: Cannot calculate skewness of triangular elements\n");
		fprintf(stderr, "Skipping...\n\n");
	}
	else if (Result->elementType == etQuadrangle)
	{
		for (e = 0; e < Result->numElements; e++)
		{
			nodeSE = Result->element[e].quadrangle.node[0];
			nodeSW = Result->element[e].quadrangle.node[1];
			nodeNW = Result->element[e].quadrangle.node[2];
			nodeNE = Result->element[e].quadrangle.node[3];

			diagNWSE = sqrt(pow((Result->x[nodeNW] - Result->x[nodeSE]), 2) +
		                	pow((Result->y[nodeNW] - Result->y[nodeSE]), 2));
			diagSWNE = sqrt(pow((Result->x[nodeSW] - Result->x[nodeNE]), 2) +
		                	pow((Result->y[nodeSW] - Result->y[nodeNE]), 2));

			skewness = (diagNWSE > diagSWNE ? (diagSWNE/diagNWSE) : (diagNWSE/diagSWNE));

			Result->element[e].quadrangle.skewness = skewness;
		}
	}
	else
	{
		fprintf(stderr, "ERROR in funtion CalcSkewness: Unknown element type\n");
		ret = -1;
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n\n***** FUNCTION CALCSKEWNESS *****\n\n");

		if (ret != -1)
			fprintf(log, "Function CalcSkewness successfully ended.\n");
		else
			fprintf(log, "Function CalcSkewness NOT successfully ended.\n");

		fprintf(log, "\n\n*********************************\n\n");
	}

	return ret;
}

/*
** Function CalcAspectratio
** Calculates the aspect ratio of each element.
**
** Aspect Ratio = (minimum side length) / (maximum side length)
**
** In:      tResult Result    =  structure containing results
** Out:     tResult Result    =  structure containing results
** Return:  0 on succes / -1 on failure
**
** Author:     J.L. Klaufus
*/

int CalcAspectRatio(FILE *log, tResult *Result)
{
	int    ret;
	int    e;
	int    n, nn;
	int    *node;

	double side, minSide, maxSide;

	/*printf("Calculating aspect ratios...\n");*/

	ret = 0;

	/* Allocate memory */
	node = (int*)malloc(Result->nodesPerElement*sizeof(int));
	if (node == NULL)
	{
		ret = -1;
		printf("ERROR in function CalcMinimumAngle: Could not allocate memory.\n");
	}
	else
	{
		for(e=0; e<Result->numElements; e++)
		{
			/*
			** Copy all nodes.
			*/
			if (ret != -1)
				ret = CopyNodes(&(*Result), e, node);

			if (ret != -1)
			{
				/*
				** Calculate the side lengths and find the minimum and maximum sidelengths
				*/

				/* First the first side to set minSide and maxSide */
				side = sqrt(pow(Result->x[node[0]] - Result->x[node[1]], 2) +
					        pow(Result->y[node[0]] - Result->y[node[1]], 2));
				maxSide  = side;
				minSide  = side;

				/* Now the rest of the sides to update minSide and maxSide */
				for(n=1; n<Result->nodesPerElement; n++)
				{
					/* Find next node */
					nn = (n == Result->nodesPerElement-1 ? 0 : n+1);

					/* calculate side length */
					side = sqrt(pow(Result->x[node[n]] - Result->x[node[nn]], 2) +
						        pow(Result->y[node[n]] - Result->y[node[nn]], 2));

					/* Update maxSide and minSide */
					maxSide = (side > maxSide ? side : maxSide);
					minSide = (side < minSide ? side : minSide);
				}

				/*
				** Calculate the aspect ratio of the element
				*/
				if (Result->elementType == etTriangle)
					Result->element[e].triangle.aspectRatio = minSide / maxSide;
				else if (Result->elementType == etQuadrangle)
					Result->element[e].quadrangle.aspectRatio = minSide / maxSide;
			}
		}
	}

	/* Free allocated memory */
	if(node)
		free(node);

	/* Write report */
	if (log)
	{
		fprintf(log, "\n\n***** FUNCTION CALCASPECTRATIO *****\n\n");

		if (ret != -1)
			fprintf(log, "Function CalcAspectRatio successfully ended.\n");
		else
			fprintf(log, "Function CalcAspectRatio NOT successfully ended.\n");

		fprintf(log, "\n\n************************************\n\n");
	}

	return ret;
}

/*
** Function CalcMinimumAngle
** Calculates the minimum angle of each element.
**
** In:      tResult Result    =  structure containing results
** Out:     tResult Result    =  structure containing results
** Return:  0 on succes / -1 on failure
**
** Author:     J.L. Klaufus
*/

int CalcMinimumAngle(FILE *log, tResult *Result)
{
	int    ret;
	int    e;
	int    np, n, nn;
	int    *node = NULL;

	double vecA[2], vecB[2];
	double lengthA, lengthB;
	double angle, minAngle;

	/*printf("Calculating minimum angles...\n");*/

	ret = 0;

	/* Allocate memory */
	node = (int*)malloc(Result->nodesPerElement*sizeof(int));
	if (node == NULL)
	{
		ret = -1;
		printf("ERROR in function CalcMinimumAngle: Could not allocate memory.\n");
	}
	else
	{
		for(e=0; e<Result->numElements; e++)
		{
			/*
			** Copy all nodes.
			*/
			if (ret != -1)
				ret = CopyNodes(&(*Result), e, node);

			if (ret != -1)
			{
				/*
				** Create two vectors intersecting at this node, calculate the angle,
				** and update minAngle.
				*/

				/* First the first angle to set minAngle */
				vecA[0] = Result->x[node[Result->nodesPerElement-1]] - Result->x[node[0]];
				vecA[1] = Result->y[node[Result->nodesPerElement-1]] - Result->y[node[0]];
				vecB[0] = Result->x[node[1]] - Result->x[node[0]];
				vecB[1] = Result->y[node[1]] - Result->y[node[0]];

				lengthA = sqrt(vecA[0]*vecA[0] + vecA[1]*vecA[1]);
				lengthB = sqrt(vecB[0]*vecB[0] + vecB[1]*vecB[1]);

				angle = acos((vecA[0]*vecB[0] + vecA[1]*vecB[1])/(lengthA*lengthB))*180/PI;
				minAngle = angle;
		
				/* Now the rest to update minAngle */
				for (n=1; n<Result->nodesPerElement; n++)
				{
					/* Find previous and next node */
					np = (n == 0 ? Result->nodesPerElement-1 : n-1);
					nn = (n == Result->nodesPerElement-1 ? 0 : n+1);

					vecA[0] = Result->x[node[np]] - Result->x[node[n]];
					vecA[1] = Result->y[node[np]] - Result->y[node[n]];
					vecB[0] = Result->x[node[nn]] - Result->x[node[n]];
					vecB[1] = Result->y[node[nn]] - Result->y[node[n]];

					lengthA = sqrt(vecA[0]*vecA[0] + vecA[1]*vecA[1]);
					lengthB = sqrt(vecB[0]*vecB[0] + vecB[1]*vecB[1]);

					angle = acos((vecA[0]*vecB[0] + vecA[1]*vecB[1])/(lengthA*lengthB))*180/PI;
					minAngle = (angle < minAngle ? angle : minAngle);
				}

				/* Store minimum angle */
				if (Result->elementType == etTriangle)
					Result->element[e].triangle.minimumAngle = minAngle;
				else if (Result->elementType == etQuadrangle)
					Result->element[e].quadrangle.minimumAngle = minAngle;
			}
		}
	}

	/* Free allocated memory */
	if(node)
		free(node);

	/* Write report */
	if (log)
	{
		fprintf(log, "\n\n***** FUNCTION CALCMINIMUMANGLE *****\n\n");

		if (ret != -1)
			fprintf(log, "Function CalcMinimumAngle successfully ended.\n");
		else
			fprintf(log, "Function CalcMinimumAngle NOT successfully ended.\n");

		fprintf(log, "\n\n*************************************\n\n");
	}

	return ret;
}

/*
** Function CopyNodes
** Copies all nodes for element e to a temporary array.
**
** In:      tResult Result     = structure containing all results.
**          int     element    = number identifying current element.
** Out:     int     *nodes     = array with nodes
** Return:  0 on success / -1 on failure
**
** Author:  J.L. Klaufus
*/

int CopyNodes(tResult *Result, int element, int *node)
{
	int ret;
	int n;

	/*printf("copying nodes...\n");*/

	ret = 0;

	/*
	** Copy all nodes.
	*/
	if(Result->elementType == etTriangle)
	{
		/* Fill the nodes array */
		for(n=0; n<Result->nodesPerElement; n++)
			node[n] = Result->element[element].triangle.node[n];
	}
	else if(Result->elementType == etQuadrangle)
	{
		/* Fill the nodes array */
		for(n=0; n<Result->nodesPerElement; n++)
			node[n] = Result->element[element].quadrangle.node[n];
	}
	else
	{
		fprintf(stderr, "ERROR in function CopyNodes: Unknown element type.\n");
		ret = -1;
	}

	return ret;
}


