/*
** Header-file for Quality
*/

#ifndef QUALITY_H
#define QUALITY_H

int Quality(FILE*, tResult*);
int CalcElementalArea(FILE*, tResult*);
int CalcSkewness(FILE*, tResult*);
int CalcAspectRatio(FILE*, tResult*);
int CalcMinimumAngle(FILE*, tResult*);
int CopyNodes(tResult*, int, int*);

#endif
