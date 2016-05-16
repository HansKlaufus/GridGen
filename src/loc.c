/*
** C-file for Loc
** Calculates the position in 1-dimensional array for given j, i
**
** In:       int     j  = ETA-direction
**           int     i  = KSI-direction
** Out:      -
** Return:   position in array; -1 on error
**
** Author:   J.L. Klaufus
*/

#include <stdio.h>

#include "gridgen.h"
#include "loc.h"

int Loc(tResult *Result, int j, int i)
{
	int pos = 0;

	if (j<0 || j>Result->jm-1 || i<0 || i>Result->im-1)
	{
		pos = -1;
	}
	else
	{
		pos = j*Result->im + i;
	}

	return pos;
}

