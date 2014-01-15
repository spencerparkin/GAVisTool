// CoordFrame.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Vector.h"

namespace VectorMath
{
	struct CoordFrame
	{
		Vector xAxis;
		Vector yAxis;
		Vector zAxis;
	};

	void Identity( CoordFrame& dst );
	void Transform( Vector& dst, const CoordFrame& coordFrame, const Vector& src );
	void Transform( Vector* dstArray, const CoordFrame& coordFrame, const Vector& origin, const Vector* srcArray, int count );
	void Transform( CoordFrame& dst, const CoordFrame& coordFrame, const CoordFrame& src );
	void TransformNormal( Vector& dst, const CoordFrame& coordFrame, const Vector& src );
	double Determinant( const CoordFrame& coordFrame );
	void MakeRotation( CoordFrame& dst, const Vector& unitAxis, double angle );
	void Scale( CoordFrame& dst, const CoordFrame& src, double scale );
}

// CoordFrame.h