#pragma once
#ifndef RAY_MATH
#define RAY_MATH
#include "VectorMath.h"
#include <math.h>
//RAY MATHEMATICS - Math pertinent to ray travel (reflection, transmission)

double snells(double n1, double n2, double theta_i) { //returns angle of transmission
	if (abs((n1 / n2) * sin(theta_i)) > 1) {
		return asin(1);
	}
	return(asin((n1 / n2) * sin(theta_i)));
}

vector3 reflection_angle(vector3 V, vector3 N) {
	//return -V + N * ( 2 * ( vector3::dot(V,N) ) ) ;
	return vector3(2 * N.x * N.z, 2 * N.y * N.z, (2 * N.z * N.z) - 1);
}







#endif GRAPHICS_MATH
