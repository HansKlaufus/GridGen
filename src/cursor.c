/*
** C-file for Cursor
** Returns the appropiate character for the animation
**
** In:       int     count = counter for current state
** Out:      -
** Return:   char    ret   = character containing the cursor.
**
** Author:   J.L. Klaufus
*/

#include "stdio.h"
#include "cursor.h"

char Cursor(int count)
{
	char ret;
	int  remainder;

	remainder = count - 4*(count/4);

	switch (remainder)
	{
		case 0:
			ret = '-';
			break;
		case 1:
			ret = '\\';
			break;
		case 2:
			ret = '|';
			break;
		case 3:
			ret = '/';
			break;
		default:
			ret = '-';
			break;
	}

	return ret;
}

