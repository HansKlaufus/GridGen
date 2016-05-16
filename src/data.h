/*
** Header-file for Data
*/

#ifndef DATA_H
#define DATA_H

int ReadData(FILE*, char*, tData*);
int WriteData(FILE*, char, tResult*);
int WriteVigieData(FILE*, tResult*);
int WriteGNUData(FILE*, tResult*);
int CalcCharAtNodes(FILE*, tResult*, double*, double*, double*, double*);
int FindElements(FILE*, tResult*, int, int*, int*);

#endif
