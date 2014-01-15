// AxisAlignedBoundingBox.h

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
#include "Triangle.h"

namespace VectorMath
{
	struct Aabb
	{
		enum Side
		{
			IS_INSIDE_BOX,
			IS_OUTSIDE_BOX,
			IS_ON_BOX,
		};

		enum Plane
		{
			XY_PLANE,
			XZ_PLANE,
			YZ_PLANE,
		};

		Vector min, max;
	};

	bool IsValidAabb( const Aabb& aabb );
	void MakeAabbMinMax( Aabb& aabb, const Vector& min, const Vector& max );
	void MakeAabb( Aabb& aabb, const Vector& center, const Vector& delta );
	void CopyAabb( Aabb& dstAabb, const Aabb& srcAabb );
	void MakeZeroAabb( Aabb& aabb, const Vector& center );
	void ExpandAabb( Aabb& aabb, const Vector& pos );
	void IntersectAabb( Aabb& aabb, const Aabb& aabb0, const Aabb& aabb1 );
	Aabb::Side AabbSide( const Aabb& aabb, const Vector& pos, double epsilon = 0.0 );
	Aabb::Plane LargestExtentDimension( const Aabb& aabb );
	void AabbSplit( const Aabb& aabb, const Vector& pos, Aabb::Plane splitPlane, Aabb& aabb0, Aabb& aabb1 );
	int AabbSplit( const Aabb& aabb, const Vector& pos, Aabb* aabbArray8 );
	void CalcCenter( const Aabb& aabb, VectorMath::Vector& center );
	double VolumeOfAabb( const Aabb& aabb );
	bool IsTriangleInsideAabb( const Aabb& aabb, const Triangle& triangle, double epsilon = 0.0 );
}

// AxisAlignedBoundingBox.h