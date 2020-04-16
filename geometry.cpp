//
//
//

#include "geometry.h"

int scaleX = 5;
int scaleY = 5;

Coordinates CartesianToPolar(int x, int y)
{
	x = x * scaleX;
	y = y * scaleY;

	double rho = sqrt(sq(x) + sq(y));
	double theta = 2 * atan(y / (x + rho));

	Coordinates c;
	c.array[0] = rho;
	c.array[1] = theta;

	return c;
}

Transform PolarToSteps(int rho, int theta)
{
	rho = rho ;
	theta = theta;

	Transform s;
	s.array[0] = rho;
	s.array[1] = theta;

	return s;
}

