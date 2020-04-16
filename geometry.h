

#ifndef _GEOMETRY_h
#define _GEOMETRY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct Coordinates {
	double array[2];
};

class CoordinatePolar {
	double rho;
	double theta;

public:
	CoordinatePolar(double rho, double theta) {
		this->rho = rho;
		this->theta = theta;
	}

	double GetRho() {
		return rho;
	}

	double GetTheta() {
		return theta;
	}
};

Coordinates CartesianToPolar(int x, int y);

struct Transform
{
	double array[2];
};
Transform PolarToSteps(int rho, int theta);
#endif

