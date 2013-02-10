/*
 * $Id: Vector2D.h,v 1.1 2010/07/02 01:57:39 clivewebster Exp $
 * Revision History
 * ================
 * $Log: Vector2D.h,v $
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
 *        File: Vector2D.h
 *  Created on: 17 Jun 2010
 *      Author: Clive Webster
 *
 *  Perform 2 dimensional Vector algebra
 */

#ifndef VECTOR2D_H_
#define VECTOR2D_H_

#include "../libdefs.h"

typedef struct s_vector2d {
	double	x;
	double	y;
	boolean	normalised;
} VECTOR2D;

#define MAKE_VECTOR2D(x,y) { x, y, FALSE }

static __inline__ double vector2d_GetX(const VECTOR2D* vector){
	return vector->x;
}

static __inline__ double vector2d_GetY(const VECTOR2D* vector){
	return vector->y;
}

void vector2d_SetX(VECTOR2D* vector, double x);
void vector2d_SetY(VECTOR2D* vector, double y);

static __inline__ void vector2d_Set(VECTOR2D* vector, double x, double y){
	vector2d_SetX(vector,x);
	vector2d_SetY(vector,y);
}

// Calculate the distance of the vector. ie sqrt(vector2d_LengthSquared(vector))
double vector2d_Length(const VECTOR2D* vector);

// Calculate the length squared of the vector
double vector2d_LengthSquared(const VECTOR2D* vector);

// Change the vector in 'src' into a unit vector at 'dst' - ie with length = 1
// src and dst can be the same vector
void vector2d_Normalise(VECTOR2D* dst,const VECTOR2D* src);

// Add the src vector to the dst vector
void vector2d_Add(VECTOR2D* dst,const VECTOR2D* src);

// Subtract the src vector from the dst vector
void vector2d_Subtract(VECTOR2D* dst,const VECTOR2D* src);

// Set the dst vector to the src vector
void vector2d_Copy(VECTOR2D* dst,const VECTOR2D* src);


// Scale the vector by a constant value
void vector2d_Scale(VECTOR2D* v,double scale);

// Find the dot product of two vectors
// returns |v1| * |v2| * cos(angle between v1,v2)
double vector2d_DotProduct(const VECTOR2D* v1, const VECTOR2D* v2);

// Returns the angle in radians between two vectors
// the return value is constrained to the range 0 ... PI

double vector2d_AngleRadians(const VECTOR2D* v1, const VECTOR2D* v2);

#endif /* VECTOR2D_H_ */
