// 
// 
// 

#include "geometry.h"

CoordinatePolar CartesianToPolar(int x, int y)
{
	double rho = sqrt(sq(x) + sq(y));
	double theta = 2 * atan(y / (x + rho));
	return CoordinatePolar(rho, theta);
}
