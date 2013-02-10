/*
 * $Id: Vector3D.c,v 1.3 2011/10/01 12:45:44 clivewebster Exp $
 * Revision History
 * ================
 * $Log: Vector3D.c,v $
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
 *        File: Vector3D.c
 *  Created on: 23 Jun 2010
 *      Author: Clive Webster
 */

#include "Vector3D.h"
#include <Math.h>
#include <string.h>


void vector3d_Copy(VECTOR3D* dst,const VECTOR3D* src){
	if(src!=dst){
		memcpy(dst,src,sizeof(VECTOR3D));
	}
}

double vector3d_LengthSquared(const VECTOR3D* vector){
	if(vector->normalised)
		return 1.0;
	return (vector->x*vector->x) + (vector->y*vector->y) + (vector->z*vector->z);
}

double vector3d_Length(const VECTOR3D* vector){
	if(vector->normalised)
		return 1.0;
	return sqrt( vector3d_LengthSquared(vector) );
}

// Change the vector in 'src' into a unit vector at 'dst' - ie with length = 1
// src and dst can be the same vector
void vector3d_Normalise(VECTOR3D* dst,const VECTOR3D* src){
	vector3d_Copy(dst,src);
	if(!dst->normalised){
		double dist = vector3d_Length(dst);
		dst->x /= dist;
		dst->y /= dist;
		dst->z /= dist;
		dst->normalised = TRUE;
	}
}

void vector3d_Add(VECTOR3D* dst,const VECTOR3D* src){
	dst->x += src->x;
	dst->y += src->y;
	dst->z += src->z;
	dst->normalised = FALSE;
}
// Subtract the src vector from the dst vector
void vector3d_Subtract(VECTOR3D* dst,const VECTOR3D* src){
	dst->x -= src->x;
	dst->y -= src->y;
	dst->z -= src->z;
	dst->normalised = FALSE;
}

void vector3d_Scale(VECTOR3D* v, double scale){
	v->x *= scale;
	v->y *= scale;
	v->z *= scale;
	v->normalised = (v->normalised && scale==1.0) ? TRUE : FALSE;
}

// Find the dot product of two vectors
double vector3d_DotProduct(const VECTOR3D* v1, const VECTOR3D* v2){
	return (v1->x * v2->x) + (v1->y * v2->y) + (v1->z * v2->z);

}

double vector3d_AngleRadians(const VECTOR3D* v1, const VECTOR3D* v2){
	double vDot = vector3d_DotProduct(v1,v2) / ( vector3d_Length(v1) * vector3d_Length(v2) );
	if( vDot < -1.0){
		vDot = -1.0;
	}else if( vDot >  1.0){
		vDot =  1.0;
	}
	return acos( vDot );
}

void vector3d_SetX(VECTOR3D* vector, double x){
	if(x != vector->x){
		vector->x = x;
		vector->normalised=FALSE;
	}
}

void vector3d_SetY(VECTOR3D* vector, double y){
	if(y != vector->y){
		vector->y = y;
		vector->normalised=FALSE;
	}
}

void vector3d_SetZ(VECTOR3D* vector, double z){
	if(z != vector->z){
		vector->z = z;
		vector->normalised=FALSE;
	}
}

void vector3d_CrossProduct(VECTOR3D*result, const VECTOR3D* v1, const VECTOR3D* v2){

	VECTOR3D rtn;

	rtn.x = v1->y * v2->z - v1->z * v2->y;
	rtn.y = v2->x * v1->z - v2->z * v1->x;
	rtn.z = v1->x * v2->y - v1->y * v2->x;

	vector3d_Copy(result,&rtn);
}




