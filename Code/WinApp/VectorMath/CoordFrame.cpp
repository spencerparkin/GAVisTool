// CoordFrame.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "CoordFrame.h"

//=============================================================================
void VectorMath::Identity( CoordFrame& dst )
{
	Set( dst.xAxis, 1.0, 0.0, 0.0 );
	Set( dst.yAxis, 0.0, 1.0, 0.0 );
	Set( dst.zAxis, 0.0, 0.0, 1.0 );
}

//=============================================================================
void VectorMath::Transform( Vector& dst, const CoordFrame& coordFrame, const Vector& src )
{
	Vector xComp, yComp, zComp;

	Scale( xComp, coordFrame.xAxis, src.x );
	Scale( yComp, coordFrame.yAxis, src.y );
	Scale( zComp, coordFrame.zAxis, src.z );

	Add( dst, xComp, yComp );
	Add( dst, dst, zComp );
}

//=============================================================================
void VectorMath::Transform( Vector* dstArray, const CoordFrame& coordFrame, const Vector& origin, const Vector* srcArray, int count )
{
	for( int index = 0; index < count; index++ )
	{
		Transform( dstArray[ index ], coordFrame, srcArray[ index ] );
		Add( dstArray[ index ], dstArray[ index ], origin );
	}
}

//=============================================================================
void VectorMath::Transform( CoordFrame& dst, const CoordFrame& coordFrame, const CoordFrame& src )
{
	Transform( dst.xAxis, coordFrame, src.xAxis );
	Transform( dst.yAxis, coordFrame, src.yAxis );
	Transform( dst.zAxis, coordFrame, src.zAxis );
}

//=============================================================================
// This should be equivilant to transforming by the inverse transpose.
// It may not be as efficient, but it should be correct.  To understand the
// theory behind this, let M be a matrix and v be a vector.  Then, if a and
// b are vectors such that a x b = v, (their cross product is v), then we
// have the identity (Ma x Mb) = (M^-T)v, where M^-T is the inverse transpose.
// Normals transform this way by the observation that Mv is not the desired
// transformation, but Ma x Mb is the desired transformation.
void VectorMath::TransformNormal( Vector& dst, const CoordFrame& coordFrame, const Vector& src )
{
	Vector vecA, vecB;
	Orthogonal( vecA, src );
	Cross( vecB, src, vecA );
	Transform( vecA, coordFrame, vecA );
	Transform( vecB, coordFrame, vecB );
	Cross( dst, vecA, vecB );
	Normalize( dst, dst );
}

//=============================================================================
// The sign here gives us the handed-ness of the given coordinate frame.
// Negative means that it is left-handed, positive means it is right-handed.
// Zero means that it is a singular (non-invertable) transformation.
// The magnitude of the determinant is the volume of the parallel-piped
// formed by the axes.
double VectorMath::Determinant( const CoordFrame& coordFrame )
{
	// Do a co-fractor expansion along the top row.
	Vector cross;
	Cross( cross, coordFrame.yAxis, coordFrame.zAxis );
	double det = 0.0;
	det += coordFrame.xAxis.x * cross.x;
	det -= coordFrame.xAxis.y * cross.y;
	det += coordFrame.xAxis.z * cross.z;
	return det;
}

//=============================================================================
void VectorMath::MakeRotation( CoordFrame& dst, const Vector& unitAxis, double angle )
{
	Identity( dst );
	Rotate( dst.xAxis, dst.xAxis, unitAxis, angle );
	Rotate( dst.yAxis, dst.yAxis, unitAxis, angle );
	Rotate( dst.zAxis, dst.zAxis, unitAxis, angle );
}

//=============================================================================
void VectorMath::Scale( CoordFrame& dst, const CoordFrame& src, double scale )
{
	Scale( dst.xAxis, src.xAxis, scale );
	Scale( dst.yAxis, src.yAxis, scale );
	Scale( dst.zAxis, src.zAxis, scale );
}

// CoordFrame.cpp