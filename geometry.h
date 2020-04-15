

#ifndef _GEOMETRY_h
#define _GEOMETRY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

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

class Person {
	String nom;
	String prenom;

public:
	Person(String name, String prenom) { 
		nom = name;
		this->prenom = prenom;
	}

	Person() {
		nom = "Toto";
		prenom = "Zéro";
	}

	String GetNom() {
		return nom;
	}

	String GetPrenom() {
		return prenom;
	}

	String SetNom(String nom) {
		this->nom = nom;
	}

	String ToString() {
		return nom + " " + prenom;
	}

};

CoordinatePolar CartesianToPolar(int x, int y);

#endif

