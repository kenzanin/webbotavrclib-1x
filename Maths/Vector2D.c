/*
 * $Id: Vector2D.c,v 1.3 2011/10/01 12:45:44 clivewebster Exp $
 * Revision History
 * ================
 * $Log: Vector2D.c,v $
 * Revision 1.3  2011/10/01 12:45:44  clivewebster
 * Remove use of 'square' function as its not C standard
 *
 * Revision 1.2  2011/09/04 15:26:32  clivewebster
 * Fix LengthSquared (thanks to Gertlex for spotting it)
 *
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
 *        File: Vector2D.c
 *  Created on: 17 Jun 2010
 *      Author: Clive Webster
 */
#include "Vector2D.h"
#include <Math.h>
#include <string.h>


void vector2d_Copy(VECTOR2D* dst,const VECTOR2D* src){
	if(src!=dst){
		memcpy(dst,src,sizeof(VECTOR2D));
	}
}

double vector2d_LengthSquared(const VECTOR2D* vector){
	if(vector->normalised)
		return 1.0;
	return (vector->x*vector->x) + (vector->y*vector->y);
}

double vector2d_Length(const VECTOR2D* vector){
	if(vector->normalised)
		return 1.0;
	return sqrt( vector2d_LengthSquared(vector) );
}

// Change the vector in 'src' into a unit vector at 'dst' - ie with length = 1
// src and dst can be the same vector
void vector2d_Normalise(VECTOR2D* dst,const VECTOR2D* src){
	vector2d_Copy(dst,src);
	if(!dst->normalised){
		double dist = vector2d_Length(dst);
		dst->x /= dist;
		dst->y /= dist;
		dst->normalised = TRUE;
	}
}

void vector2d_Add(VECTOR2D* dst,const VECTOR2D* src){
	dst->x += src->x;
	dst->y += src->y;
	dst->normalised = FALSE;
}
// Subtract the src vector from the dst vector
void vector2d_Subtract(VECTOR2D* dst,const VECTOR2D* src){
	dst->x -= src->x;
	dst->y -= src->y;
	dst->normalised = FALSE;
}

void vector2d_Scale(VECTOR2D* v, double scale){
	v->x *= scale;
	v->y *= scale;
	v->normalised = (v->normalised && scale==1.0) ? TRUE : FALSE;
}

// Find the dot product of two vectors
double vector2d_DotProduct(const VECTOR2D* v1, const VECTOR2D* v2){
	return (v1->x * v2->x) + (v1->y * v2->y);

}

double vector2d_AngleRadians(const VECTOR2D* v1, const VECTOR2D* v2){
	double vDot = vector2d_DotProduct(v1,v2) / ( vector2d_Length(v1) * vector2d_Length(v2) );
	if( vDot < -1.0){
		vDot = -1.0;
	}else if( vDot >  1.0){
		vDot =  1.0;
	}
	return acos( vDot );
}

void vector2d_SetX(VECTOR2D* vector, double x){
	if(x != vector->x){
		vector->x = x;
		vector->normalised=FALSE;
	}
}

void vector2d_SetY(VECTOR2D* vector, double y){
	if(y != vector->y){
		vector->y = y;
		vector->normalised=FALSE;
	}
}



