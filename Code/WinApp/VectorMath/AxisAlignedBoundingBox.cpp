// AxisAlignedBoundingBox.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "AxisAlignedBoundingBox.h"

//=============================================================================
bool VectorMath::IsValidAabb( const Aabb& aabb )
{
	if( aabb.min.x > aabb.max.x )
		return false;
	if( aabb.min.y > aabb.max.y )
		return false;
	if( aabb.min.z > aabb.max.z )
		return false;
	return true;
}

//=============================================================================
void VectorMath::MakeAabbMinMax( Aabb& aabb, const Vector& min, const Vector& max )
{
	// We assume the given min and max vectors make a valid AABB.
	Copy( aabb.min, min );
	Copy( aabb.max, max );
}

//=============================================================================
void VectorMath::MakeAabb( Aabb& aabb, const Vector& center, const Vector& delta )
{
	// We assume that all components of the given delta vector are non-negative.
	Sub( aabb.min, center, delta );
	Add( aabb.max, center, delta );
}

//=============================================================================
void VectorMath::CopyAabb( Aabb& dstAabb, const Aabb& srcAabb )
{
	Copy( dstAabb.min, srcAabb.min );
	Copy( dstAabb.max, srcAabb.max );
}

//=============================================================================
void VectorMath::MakeZeroAabb( Aabb& aabb, const Vector& center )
{
	Copy( aabb.min, center );
	Copy( aabb.max, center );
}

//=============================================================================
void VectorMath::ExpandAabb( Aabb& aabb, const Vector& pos )
{
	if( aabb.min.x < pos.x )
		aabb.min.x = pos.x;
	else if( aabb.max.x < pos.x )
		aabb.max.x = pos.x;

	if( aabb.min.y < pos.y )
		aabb.min.y = pos.y;
	else if( aabb.max.y < pos.y )
		aabb.max.y = pos.y;

	if( aabb.min.z < pos.z )
		aabb.min.z = pos.z;
	else if( aabb.max.z < pos.z )
		aabb.max.z = pos.z;
}

//=============================================================================
void VectorMath::IntersectAabb( Aabb& aabb, const Aabb& aabb0, const Aabb& aabb1 )
{
	//...
}

//=============================================================================
// We assume here that the given AABB is valid.  If it is not, then the
// results of this routine are undefined.
VectorMath::Aabb::Side VectorMath::AabbSide( const Aabb& aabb, const Vector& pos, double epsilon /*= 0.0*/ )
{
	if( pos.x < aabb.min.x - epsilon || pos.x > aabb.max.x + epsilon )
		return Aabb::IS_OUTSIDE_BOX;
	if( pos.y < aabb.min.y - epsilon || pos.y > aabb.max.y + epsilon )
		return Aabb::IS_OUTSIDE_BOX;
	if( pos.z < aabb.min.z - epsilon || pos.z > aabb.max.z + epsilon )
		return Aabb::IS_OUTSIDE_BOX;

	if( pos.x <= aabb.min.x + epsilon || pos.x >= aabb.max.x - epsilon )
		return Aabb::IS_ON_BOX;
	if( pos.y <= aabb.min.y + epsilon || pos.y >= aabb.max.y - epsilon )
		return Aabb::IS_ON_BOX;
	if( pos.z <= aabb.min.z + epsilon || pos.z >= aabb.max.z - epsilon )
		return Aabb::IS_ON_BOX;

	return Aabb::IS_INSIDE_BOX;
}

//=============================================================================
// Here indicate if the X, Y or Z axis is the largest dimension along
// which the given AABB extends.  If it is the X axis, we return the
// YZ plane, if the Y axis, the XZ plane, and if the Z axis, the XY plane.
VectorMath::Aabb::Plane VectorMath::LargestExtentDimension( const Aabb& aabb )
{
	Vector delta;
	Sub( delta, aabb.max, aabb.min );
	if( delta.x > delta.y )
	{
		if( delta.x > delta.z )
			return Aabb::YZ_PLANE;
		else
			return Aabb::XY_PLANE;
	}
	if( delta.y > delta.z )
		return Aabb::XZ_PLANE;
	return Aabb::XY_PLANE;
}

//=============================================================================
// We assume here that the given AABB is valid.  If it is not, then the
// results of this routine are undefined.  We assume that the given
// position is inside or on the given box.  The first of the returned
// AABBs will be on the back side of the split plane.  The second will
// be on the front side.  The front of the XY plane, for example is in
// the direction of the positive Z axis.  The split-plane is parallel
// to one of the XY, YZ or XZ planes and contains the given position.
void VectorMath::AabbSplit( const Aabb& aabb, const Vector& pos, Aabb::Plane splitPlane, Aabb& aabb0, Aabb& aabb1 )
{
	Vector delta;
	Sub( delta, aabb.max, aabb.min );

	Copy( aabb0.min, aabb.min );
	Copy( aabb1.max, aabb.max );

	switch( splitPlane )
	{
		case Aabb::XY_PLANE:
		{
			Set( aabb0.max, aabb.max.x, aabb.max.y, pos.z );
			Set( aabb1.min, aabb.min.x, aabb.min.y, pos.z );
			break;
		}
		case Aabb::YZ_PLANE:
		{
			Set( aabb0.max, pos.x, aabb.max.y, aabb.max.z );
			Set( aabb1.min, pos.x, aabb.min.y, aabb.min.z );
			break;
		}
		case Aabb::XZ_PLANE:
		{
			Set( aabb0.max, aabb.max.x, pos.y, aabb.max.z );
			Set( aabb1.min, aabb.min.x, pos.y, aabb.min.z );
			break;
		}
	}
}

//=============================================================================
void VectorMath::CalcCenter( const Aabb& aabb, VectorMath::Vector& center )
{
	Lerp( center, aabb.min, aabb.max, 0.5 );
}

//=============================================================================
double VectorMath::VolumeOfAabb( const Aabb& aabb )
{
	Vector delta;
	Sub( delta, aabb.max, aabb.min );

	// This is non-negative provided we were given a valid AABB.
	return delta.x * delta.y * delta.z;
}

//=============================================================================
bool VectorMath::IsTriangleInsideAabb( const Aabb& aabb, const Triangle& triangle, double epsilon /*= 0.0*/ )
{
	if( Aabb::IS_INSIDE_BOX != AabbSide( aabb, triangle.vertex[0], epsilon ) )
		return false;
	if( Aabb::IS_INSIDE_BOX != AabbSide( aabb, triangle.vertex[1], epsilon ) )
		return false;
	if( Aabb::IS_INSIDE_BOX != AabbSide( aabb, triangle.vertex[2], epsilon ) )
		return false;
	return true;
}

// AxisAlignedBoundingBox.cpp