/*
 * $Id: Matrix3D.c,v 1.1 2010/07/02 01:57:39 clivewebster Exp $
 * Revision History
 * ================
 * $Log: Matrix3D.c,v $
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
 *        File: Matrix3D.c
 *  Created on: 23 Jun 2010
 *      Author: Clive Webster
 */
#include "Matrix3D.h"
#include <Math.h>
#include <string.h>

// Set a matrix to the given values
void matrix3d_Set(MATRIX3D* matrix,
				  double m00,double m01,double m02,
				  double m10,double m11,double m12,
				  double m20,double m21,double m22){
	matrix->m00 = m00;
	matrix->m01 = m01;
	matrix->m02 = m02;
	matrix->m10 = m10;
	matrix->m11 = m11;
	matrix->m12 = m12;
	matrix->m20 = m20;
	matrix->m21 = m21;
	matrix->m22 = m22;
}

// Copy one matrix to another
void matrix3d_Copy(MATRIX3D* dst, const MATRIX3D* src){
	if(src!=dst){
		memcpy(dst,src,sizeof(MATRIX3D));
	}
}

// Calculate the determinant of the matrix
double matrix3d_Determinant(const MATRIX3D* matrix){
	double total;

   total =  matrix->m00*(matrix->m11*matrix->m22 - matrix->m12*matrix->m21)
		  + matrix->m01*(matrix->m12*matrix->m20 - matrix->m10*matrix->m22)
		  + matrix->m02*(matrix->m10*matrix->m21 - matrix->m11*matrix->m20);
   return total;
}

// Set the matrix to a rotation around the X axis
void matrix3d_SetRotateX(MATRIX3D* matrix, double radians){
	double	sinAngle, cosAngle;

	sinAngle = sin(radians);
	cosAngle = cos(radians);

	matrix->m00 = 1.0;
	matrix->m01 = 0.0;
	matrix->m02 = 0.0;

	matrix->m10 = 0.0;
	matrix->m11 = cosAngle;
	matrix->m12 = -sinAngle;

	matrix->m20 = 0.0;
	matrix->m21 = sinAngle;
	matrix->m22 = cosAngle;

}

// Set the matrix to a rotation around the Y axis
void matrix3d_SetRotateY(MATRIX3D* matrix, double radians){
	double	sinAngle, cosAngle;

	sinAngle = sin(radians);
	cosAngle = cos(radians);

	matrix->m00 = cosAngle;
	matrix->m01 = 0.0;
	matrix->m02 = sinAngle;

	matrix->m10 = 0.0;
	matrix->m11 = 1.0;
	matrix->m12 = 0.0;

	matrix->m20 = -sinAngle;
	matrix->m21 = 0.0;
	matrix->m22 = cosAngle;

}
// Set the matrix to a rotation around the Z axis
void matrix3d_SetRotateZ(MATRIX3D* matrix, double radians){
	double	sinAngle, cosAngle;

	sinAngle = sin(radians);
	cosAngle = cos(radians);

	matrix->m00 = cosAngle;
	matrix->m01 = -sinAngle;
	matrix->m02 = 0.0;

	matrix->m10 = sinAngle;
	matrix->m11 = cosAngle;
	matrix->m12 = 0.0;

	matrix->m20 = 0.0;
	matrix->m21 = 0.0;
	matrix->m22 = 1.0;

}

// Dst = Dst * Src
void matrix3d_Multiply(MATRIX3D* dst, const MATRIX3D* src){
	MATRIX3D result;

		result.m00 = dst->m00*src->m00 + dst->m01*src->m10 + dst->m02*src->m20;
		result.m01 = dst->m00*src->m01 + dst->m01*src->m11 + dst->m02*src->m21;
		result.m02 = dst->m00*src->m02 + dst->m01*src->m12 + dst->m02*src->m22;

		result.m10 = dst->m10*src->m00 + dst->m11*src->m10 + dst->m12*src->m20;
		result.m11 = dst->m10*src->m01 + dst->m11*src->m11 + dst->m12*src->m21;
		result.m12 = dst->m10*src->m02 + dst->m11*src->m12 + dst->m12*src->m22;

		result.m20 = dst->m20*src->m00 + dst->m21*src->m10 + dst->m22*src->m20;
		result.m21 = dst->m20*src->m01 + dst->m21*src->m11 + dst->m22*src->m21;
		result.m22 = dst->m20*src->m02 + dst->m21*src->m12 + dst->m22*src->m22;

		matrix3d_Copy(dst,&result);
}


// dst = src x matrix
void matrix3d_Transform(VECTOR3D* dst, const VECTOR3D* src, const MATRIX3D* matrix ){
	double x,y,z;
	x = matrix->m00 * src->x + matrix->m01 * src->y + matrix->m02 * src->z;
	y = matrix->m10 * src->x + matrix->m11 * src->y + matrix->m12 * src->z;
	z = matrix->m20 * src->x + matrix->m21 * src->y + matrix->m22 * src->z;
	vector3d_Set(dst,x,y,z);
}

// Set the matrix to a scale factor
void matrix3d_SetScale(MATRIX3D* matrix, double scale){
	matrix3d_Set(matrix, scale,0,0, 0,scale,0, 0,0,scale);
}
