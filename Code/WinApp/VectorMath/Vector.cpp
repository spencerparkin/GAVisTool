// Vector.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Vector.h"
#include <math.h>
#include <stdlib.h>

//=============================================================================
void VectorMath::Zero( Vector& dst )
{
	dst.x = 0.f;
	dst.y = 0.f;
	dst.z = 0.f;
}

//=============================================================================
void VectorMath::Copy( Vector& dst, const Vector& src )
{
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

//=============================================================================
// Return a vector that is the same length as and orthogonal to the given vector.
bool VectorMath::Orthogonal( Vector& dst, const Vector& src )
{
	double srcLength = Length( src );
	if( srcLength == 0.f )
		return false;

	if( src.y < src.z )
	{
		if( src.x < src.y )
			Set( dst, 0.f, src.z, -src.y );
		else
			Set( dst, -src.z, 0.f, src.x );
	}
	else
	{
		if( src.x < src.z )
			Set( dst, 0.f, src.z, -src.y );
		else
			Set( dst, src.y, -src.x, 0.f );
	}
	
	double dstLength = Length( dst );
	if( dstLength == 0.f )
	{
		Set( dst, src.z, src.x, src.y );
		dstLength = Length( dst );
	}
	
	Scale( dst, dst, srcLength / dstLength );
	return true;
}

//=============================================================================
void VectorMath::Set( Vector& dst, double x, double y, double z )
{
	dst.x = x;
	dst.y = y;
	dst.z = z;
}

//=============================================================================
void VectorMath::Add( Vector& dst, const Vector& leftSrc, const Vector& rightSrc )
{
	dst.x = leftSrc.x + rightSrc.x;
	dst.y = leftSrc.y + rightSrc.y;
	dst.z = leftSrc.z + rightSrc.z;
}

//=============================================================================
void VectorMath::Sub( Vector& dst, const Vector& leftSrc, const Vector& rightSrc )
{
	dst.x = leftSrc.x - rightSrc.x;
	dst.y = leftSrc.y - rightSrc.y;
	dst.z = leftSrc.z - rightSrc.z;
}

//=============================================================================
void VectorMath::AddScale( Vector& dst, const Vector& leftSrc, const Vector& rightSrc, double scalar )
{
	dst.x = leftSrc.x + rightSrc.x * scalar;
	dst.y = leftSrc.y + rightSrc.y * scalar;
	dst.z = leftSrc.z + rightSrc.z * scalar;
}

//=============================================================================
double VectorMath::Dot( const Vector& leftSrc, const Vector& rightSrc )
{
	double dot =
			leftSrc.x * rightSrc.x +
			leftSrc.y * rightSrc.y +
			leftSrc.z * rightSrc.z;
	return dot;
}

//=============================================================================
// We assume here that neither the source vector is the same as the destination.
void VectorMath::Cross( Vector& dst, const Vector& leftSrc, const Vector& rightSrc )
{
	dst.x = leftSrc.y * rightSrc.z - leftSrc.z * rightSrc.y;
	dst.y = leftSrc.z * rightSrc.x - leftSrc.x * rightSrc.z;
	dst.z = leftSrc.x * rightSrc.y - leftSrc.y * rightSrc.x;
}

//=============================================================================
double VectorMath::Length( const Vector& src )
{
	return sqrt( Dot( src, src ) );
}

//=============================================================================
double VectorMath::Distance( const Vector& leftSrc, const Vector& rightSrc )
{
	Vector vec;
	Sub( vec, leftSrc, rightSrc );
	return Length( vec );
}

//=============================================================================
void VectorMath::Scale( Vector& dst, const Vector& src, double scalar )
{
	dst.x = src.x * scalar;
	dst.y = src.y * scalar;
	dst.z = src.z * scalar;
}

//=============================================================================
void VectorMath::Normalize( Vector& dst, const Vector& src )
{
	double len = Length( src );
	Scale( dst, src, 1.f / len );
}

//=============================================================================
// We assume here that the given axis is a unit-length vector.
//
// Let R be the given rotor, and v be the vector we wish to rotate/scale
// with this rotor.  Let s and B be a scalar and bivector, respectively,
// such that R = s + B.  We wish to compute RvR~.  To that end...
//
//   RvR~ = (s + B)v(s + B)~
//        = (s + B)v(s - B).
//
// Continuing, we get...
//
//   (s + B)v(s - B) = (sv + Bv)(s - B)
//                   = (s^2)v - svB + sBv - BvB
//                   = (s^2)v - s(v.B + v^B) + s(B.v + B^v) - (B.v + B^v)B
//                   = (s^2)v - sv.B - sv^B - sv.B + sv^B - (B.v)B - (B^v)B
//                   = (s^2)v - 2sv.B + (v.B)B - (v^B)B
//                   = (s^2)v - 2sv.B + (v.B)B + (v^B)B - 2(v^B)B
//                   = (s^2)v - 2sv.B + (v.B + v^B)B - 2(v^B)B
//                   = (s^2)v - 2sv.B + (B^2)v - 2(v^B)B
//                   = (s^2 + B^2)v - 2(sv.B + (v^B)B)
//                   = (s^2 - |B|^2)v - 2(s(1/2)(vB - Bv) + (1/2)(vB + Bv)B)
//                   = (s^2 - |B|^2)v - s(vB - Bv) - (vB + Bv)B.
//
// Loosely speaking, this reduces the operation to the sum of a vector scale,
// a vector dot product, and a vector cross product.  To see how, let B =
// aE0 + bE0 + cE0, and v = xe0 + ye0 + ze0, where E0 = e1e2, E1=e2e0, and
// E2=e0e1.  Letting the notation <x,y,z> be short-hand for v in terms of x,
// y, and z, we see that B = I<a,b,c>, where I=e0e1e2 is the unit psuedo-scalar.
// We then see that...
//
//   s^2 - |B|^2 = s^2 - a^2 - b^2 - c^2,
//   vB - Bv = 2< bz-cy, cx-az, ay-bx >, and
//   vB + Bv = 2(ax + by + cz)I.
//
// Our final formula then comes to...
//
//   RvR~ = (s^2-a^2-b^2-c^2)<x,y,z> - 2( s< bz-cy, cx-az, ay-bx > - (ax+by+cz)<a,b,c> ).
//
void VectorMath::Rotate( Vector& dst, const Vector& src, const Vector& axis, double angle )
{
	angle *= -0.5f;
	double sinHalfAngle, cosHalfAngle;
	sinHalfAngle = sin( angle );
	cosHalfAngle = cos( angle );

	double s = cosHalfAngle;
	Vector B;
	Scale( B, axis, sinHalfAngle );

	Vector vec;
	Cross( vec, B, src );
	Scale( vec, vec, s );

	Vector tmp;
	Scale( tmp, B, Dot( B, src ) );

	Sub( vec, vec, tmp );
	Scale( vec, vec, 2.f );

	Scale( tmp, src, s * s - Dot( B, B ) );
	Sub( dst, tmp, vec );
}

//=============================================================================
void VectorMath::Lerp( Vector& dst, const Vector& src0, const Vector& src1, double lerp )
{
	Vector delta;
	Sub( delta, src1, src0 );
	Scale( delta, delta, lerp );
	Add( dst, src0, delta );
}

//=============================================================================
// We assume here that the given source vectors are of unit length.
void VectorMath::Slerp( Vector& dst, const Vector& src0, const Vector& src1, double slerp )
{
	double theta = acos( Dot( src0, src1 ) );
	double sinTheta = sin( theta );
	if( sinTheta == 0.0 )
		Copy( dst, src0 );
	else if( fabs( sinTheta ) < 0.0001 )
	{
		VectorMath::Lerp( dst, src0, src1, slerp );
		VectorMath::Normalize( dst, dst );
	}
	else
	{
		Vector vec0, vec1;
		VectorMath::Scale( vec0, src0, sin( ( 1.0 - slerp ) * theta ) );
		VectorMath::Scale( vec1, src1, sin( slerp * theta ) );
		VectorMath::Add( dst, vec0, vec1 );
		VectorMath::Scale( dst, dst, 1.0 / sinTheta );
	}
}

//=============================================================================
double VectorMath::RandomNumber( double min, double max )
{
	double t = double( rand() ) / double( RAND_MAX );
	double r = min + t * ( max - min );
	return r;
}

//=============================================================================
void VectorMath::RandomVector( Vector& dst, double min, double max )
{
	dst.x = RandomNumber( min, max );
	dst.y = RandomNumber( min, max );
	dst.z = RandomNumber( min, max );
}

//=============================================================================
void VectorMath::Project( Vector& dst, const Vector& src, const Vector& unitVector )
{
	double dot = Dot( src, unitVector );
	Scale( dst, unitVector, dot );
}

//=============================================================================
void VectorMath::Reject( Vector& dst, const Vector& src, const Vector& unitVector )
{
	Project( dst, src, unitVector );
	Sub( dst, src, dst );
}

//=============================================================================
void VectorMath::LinearCombo( Vector& dst, double scalar0, const Vector& src0, double scalar1, const Vector& src1 )
{
	dst.x = scalar0 * src0.x + scalar1 * src1.x;
	dst.y = scalar0 * src0.y + scalar1 * src1.y;
	dst.z = scalar0 * src0.z + scalar1 * src1.z;
}

//=============================================================================
void VectorMath::LinearCombo( Vector& dst, double scalar0, const Vector& src0, double scalar1, const Vector& src1, double scalar2, const Vector& src2 )
{
	dst.x = scalar0 * src0.x + scalar1 * src1.x + scalar2 * src2.x;
	dst.y = scalar0 * src0.y + scalar1 * src1.y + scalar2 * src2.y;
	dst.z = scalar0 * src0.z + scalar1 * src1.z + scalar2 * src2.z;
}

// Vector.cpp