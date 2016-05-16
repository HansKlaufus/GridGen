/*
** C-file for Distribute
** Distributes nodes on reference length
**
** In:       int    numNodes  = number of nodes
**           double refLength = reference length
**           
** Out:      double xArray    = array of node x-coordinates
**
** Return:   0 on success, -1 on failure
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>
#include <math.h>

#include "gridgen.h"
#include "distribute.h"

int Distribute1(FILE *log, int numNodes, double refLength, double *xArray)
{
	/*
	** Creates a uniform distribution
	*/

	int    ret;
	int    i;

	double xDelta;

	/*printf("Distributing 1...\n");*/

	ret = 0;

	xDelta = refLength / (numNodes - 1);

	/* Fill xArray */
	for(i=0; i<numNodes; i++)
	{
		xArray[i] = i*xDelta;
	}

	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION DISTRIBUTE1 *****\n\n");

		for(i=0; i<numNodes; i++)
		{
			fprintf(log, "x = %f\n", xArray[i]);
		}
		
		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int Distribute2(FILE *log, int numNodes, double refLength, double *xArray)
{
	/*
	** Makes a distribution with more densed areas at beginning, in middle and at end
	** of refLength.
	** Good for aerofoil.
	*/

	int    ret;
	int    i;

	double xDelta;
	double sum;
	double correction;

	/*printf("Distributing 2...\n");*/

	ret = 0;
	
	/* Use sine distribution */
	sum = 0;
	xArray[0] = 0;
	for(i=1; i<numNodes; i++)
	{
		/*
		** Calculate stepsize for each step
		**    Add a sine to it so xDelta=0 will only occur at beginning and end;
		**    NOT at pi/2. This would cause two points to overlap near the leading
		**    edge.
		*/
		xDelta = fabs(sin((i*2*PI)/numNodes)) + 0.1;
		/*xDelta = fabs(20*pow(sin((i*2*PI)/numNodes), 2)) + sin(i*PI/numNodes);*/
		sum = sum + xDelta;

		/* Store temporarily in xArray */
		xArray[i] = xDelta;
	}

	/* Calculate correction factor for stepsizes */
	correction = refLength/sum;

	/* Now distances in xArray using the correction on the stepsizes */
	for(i=1; i<numNodes; i++)
	{
		xArray[i] = xArray[i-1] + correction*xArray[i];
	}
	
	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION DISTRIBUTE2 *****\n\n");

		for(i=0; i<numNodes; i++)
		{
			fprintf(log, "x = %f\n", xArray[i]);
		}
		
		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int Distribute3(FILE *log, int numNodes, double refLength, double *xArray)
{
	/*
	** Make a distribution with more densed area only at beginning of refLength.
	** Good for cutting line.
	*/

	int    ret;
	int    i;

	double xDelta;
	double sum;
	double correction;

	/*printf("Distributing 3...\n");*/

	ret = 0;
	
	/* Use e distribution */
	sum = 0;
	xArray[0] = 0;
	for(i=1; i<numNodes; i++)
	{
		/* Calculate stepsize for each step */
		/*xDelta = exp(0.2*(double)(i));*/
		xDelta = 1 - exp((double)(-i)/numNodes);
		sum = sum + xDelta;

		/* Store temporarily in xArray */
		xArray[i] = xDelta;
	}

	/* Calculate correction factor for stepsizes */
	correction = refLength/sum;

	/* Store distances in xArray using the correction on the stepsizes */
	for(i=1; i<numNodes; i++)
	{
		xArray[i] = xArray[i-1] + correction*xArray[i];
	}
	
	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION DISTRIBUTE3 *****\n\n");

		for(i=0; i<numNodes; i++)
		{
			fprintf(log, "x = %f\n", xArray[i]);
		}
		
		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

int Distribute4(FILE *log, int numNodes, double refLength, double *xArray)
{
	/*
	** Make a distribution with more densed area only at beginning of refLength.
	** Good for cutting line.
	*/

	int    ret;
	int    i;

	double xDelta;
	double sum;
	double correction;

	/*printf("Distributing 4...\n");*/

	ret = 0;
	
	/* Use sine distribution */
	sum = 0;
	xArray[0] = 0;
	for(i=1; i<numNodes; i++)
	{
		/* Calculate stepsize for each step */
		xDelta = fabs(sin((i*0.5*PI)/numNodes));
		sum = sum + xDelta;

		/* Store temporarily in xArray */
		xArray[i] = xDelta;
	}

	/* Calculate correction factor for stepsizes */
	correction = refLength/sum;

	/* Now distances in xArray using the correction on the stepsizes */
	for(i=1; i<numNodes; i++)
	{
		xArray[i] = xArray[i-1] + correction*xArray[i];
	}
	
	/* Write report */
	if (log)
	{
		fprintf(log, "\n***** FUNCTION DISTRIBUTE4 *****\n\n");

		for(i=0; i<numNodes; i++)
		{
			fprintf(log, "x = %f\n", xArray[i]);
		}
		
		fprintf(log, "\n*****************************\n\n");
	}

	return ret;
}

