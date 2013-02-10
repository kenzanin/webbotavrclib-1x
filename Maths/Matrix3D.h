/*
 * $Id: Matrix3D.h,v 1.2 2011/10/03 13:41:49 clivewebster Exp $
 * Revision History
 * ================
 * $Log: Matrix3D.h,v $
 * Revision 1.2  2011/10/03 13:41:49  clivewebster
 * *** empty log message ***
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
 *        File: Matrix3D.h
 *  Created on: 23 Jun 2010
 *      Author: Clive Webster
 */

#ifndef MATRIX3D_H_
#define MATRIX3D_H_

#include "../libdefs.h"
#include "Vector3D.h"

typedef struct s_matrix3d {
	double	m00,m01,m02;	// top row
	double	m10,m11,m12;	// middle row
	double	m20,m21,m22;	// bottom row
} MATRIX3D;

// Construct matrix with given values
#define MAKE_MATRIX3D(m00,m01,m02, m10,m11,m12, m20,m21,m22) {m00,m01,m02, m10,m11,m12, m20,m21,m22}
// Construct matrix with all values as zero
#define MAKE_ZERO_MATRIX3D() MAKE_MATRIX3D{0,0,0, 0,0,0, 0,0,0}
// Construct identity matrix
#define MAKE_IDENTITY_MATRIX3D() MAKE_MATRIX3D{1,0,0, 0,1,0, 0,0,1}

// Set a matrix to the given values
void matrix3d_Set(MATRIX3D* matrix,
				  double m00,double m01,double m02,
				  double m10,double m11,double m12,
				  double m20,double m21,double m22);

// Copy one matrix to another
void matrix3d_Copy(MATRIX3D* dst, const MATRIX3D* src);

// Calculate the determinant of the matrix
double matrix3d_Determinant(const MATRIX3D* matrix);

// Set the matrix to a rotation around the X axis
void matrix3d_SetRotateX(MATRIX3D* matrix, double radians);
// Set the matrix to a rotation around the Y axis
void matrix3d_SetRotateY(MATRIX3D* matrix, double radians);
// Set the matrix to a rotation around the Z axis
void matrix3d_SetRotateZ(MATRIX3D* matrix, double radians);

// Set the matrix to a scale factor
void matrix3d_SetScale(MATRIX3D* matrix, double scale);

// Dst = Dst * Src
void matrix3d_Multiply(MATRIX3D* dst, const MATRIX3D* src);

// Dst = Dst + Src
void matrix3d_Add(MATRIX3D* dst, const MATRIX3D* src);

// dst = src x matrix
void matrix3d_Transform(VECTOR3D* dst, const VECTOR3D* src, const MATRIX3D* matrix);

// Set to the zero matrix
void matrix3d_Zero(MATRIX3D* matrix);

// Set to the unity matrix
void matrix3d_Unity(MATRIX3D* matrix);

#endif /* MATRIX3D_H_ */
