/*
 * $Id: Vector3D.h,v 1.1 2010/07/02 01:57:39 clivewebster Exp $
 * Revision History
 * ================
 * $Log: Vector3D.h,v $
 * Revision 1.1  2010/07/02 01:57:39  clivewebster
 * Added
 *
 * ================
 *
 * Copyright (C) 2010 Clive Webster (webbot@webbot.org.uk)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *        File: Vector3D.h
 *  Created on: 23 Jun 2010
 *      Author: Clive Webster
 *  Perform 3 dimensional Vector algebra
 */

#ifndef VECTOR3D_H_
#define VECTOR3D_H_


#include "../libdefs.h"

typedef struct s_vector3d {
	double	x;
	double	y;
	double	z;
	boolean	normalised;
} VECTOR3D;

#define MAKE_VECTOR3D(x,y,z) { x, y, z, FALSE }

static __inline__ double vector3d_GetX(const VECTOR3D* vector){
	return vector->x;
}

static __inline__ double vector3d_GetY(const VECTOR3D* vector){
	return vector->y;
}

static __inline__ double vector3d_GetZ(const VECTOR3D* vector){
	return vector->z;
}

void vector3d_SetX(VECTOR3D* vector, double x);
void vector3d_SetY(VECTOR3D* vector, double y);
void vector3d_SetZ(VECTOR3D* vector, double z);

static __inline__ void vector3d_Set(VECTOR3D* vector, double x, double y, double z){
	vector3d_SetX(vector,x);
	vector3d_SetY(vector,y);
	vector3d_SetZ(vector,z);
}

// Calculate the distance of the vector. ie sqrt(vector3d_LengthSquared(vector))
double vector3d_Length(const VECTOR3D* vector);

// Calculate the length squared of the vector
double vector3d_LengthSquared(const VECTOR3D* vector);

// Change the vector in 'src' into a unit vector at 'dst' - ie with length = 1
// src and dst can be the same vector
void vector3d_Normalise(VECTOR3D* dst,const VECTOR3D* src);

// Add the src vector to the dst vector
void vector3d_Add(VECTOR3D* dst,const VECTOR3D* src);

// Subtract the src vector from the dst vector
void vector3d_Subtract(VECTOR3D* dst,const VECTOR3D* src);

// Set the dst vector to the src vector
void vector3d_Copy(VECTOR3D* dst,const VECTOR3D* src);


// Scale the vector by a constant value
void vector3d_Scale(VECTOR3D* v,double scale);

// Find the dot product of two vectors
// returns |v1| * |v2| * cos(angle between v1,v2)
double vector3d_DotProduct(const VECTOR3D* v1, const VECTOR3D* v2);

// Return a vector that is at right angles to the other vectors
void vector3d_CrossProduct(VECTOR3D*result, const VECTOR3D* v1, const VECTOR3D* v2);

// Returns the angle in radians between two vectors
// the return value is constrained to the range 0 ... PI

double vector3d_AngleRadians(const VECTOR3D* v1, const VECTOR3D* v2);



#endif /* VECTOR3D_H_ */
