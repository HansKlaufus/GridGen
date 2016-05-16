/*
** Header-file for GridGen
*/

#ifndef GRIDGEN_H
#define GRIDGEN_H

#define PI 3.1415926535
#define SMALL 1e-7
#define SMALLANGLE 5
#define SMALLITER 1e-6

typedef struct
{
	char   gridType;
	char   distributionType;
	int    numNodes1;
	int    numNodes2;
	int    numNodes3;

	double chord;
	double height;
	double length;
	double alpha;

	double omegaElliptic;
	double omegaSmooth;

	int    numData;
	double *xData;
	double *yData;
} tData;

typedef enum
{
	etTriangle,
	etQuadrangle
} tElementType;

typedef struct
{
	int    node[3];

	double elementalArea;
	double aspectRatio;
	double minimumAngle;
} tTriangle;

typedef struct
{
	int    node[4];

	double elementalArea;
	double aspectRatio;
	double skewness;
	double minimumAngle;
} tQuadrangle;

typedef union
{
	tTriangle   triangle;
	tQuadrangle quadrangle;
} tElement;

typedef struct
{
	int      im, jm;

	double   *xNode;
	double   *yNode;

	double   *x;
	double   *y;

	double    ksiDelta;
	double    etaDelta;

	double    *ksi;
	double    *eta;

	double    *xKsi, *xEta;
	double    *yKsi, *yEta;

	double    *xKsiKsi, *xEtaEta, *xKsiEta;
	double    *yKsiKsi, *yEtaEta, *yKsiEta;

	double    *ksiX, *ksiY;
	double    *etaX, *etaY;

	double    *jac;

	int          numElements;
	int          nodesPerElement;
	tElementType elementType;
	tElement     *element;
} tResult;

#endif
