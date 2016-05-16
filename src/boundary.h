/*
** Header-file for BuildBoundaries
*/

#ifndef BOUNDARY_H
#define BOUNDARY_H

int BuildBoundaries(FILE*, tData*, tResult*);
int SideABCBA(FILE*, tResult*);
int SideDEFG(FILE*, tData*, tResult*);
int SideAD(FILE*, tData*, tResult*);
int SideAG(FILE*, tData*, tResult*);
int FindSmallestNodeY(FILE*, tData*,  tResult*, int*);
int FindLargestNodeY(FILE*, tData*, tResult*, int*);
int CalcSlope(FILE*, int, double*, double*, double*);
int CalcPoint(FILE*, tData*, double, double*, double*);

#endif
