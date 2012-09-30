// Vector.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#define PI		3.1415926536

// Provide basic vector math support.
namespace VectorMath
{
	struct Vector
	{
		double x, y, z;
	};

	void Zero( Vector& dst );
	void Copy( Vector& dst, const Vector& src );
	bool Orthogonal( Vector& dst, const Vector& src );
	void Set( Vector& dst, double x, double y, double z );
	void Add( Vector& dst, const Vector& leftSrc, const Vector& rightSrc );
	void Sub( Vector& dst, const Vector& leftSrc, const Vector& rightSrc );
	void AddScale( Vector& dst, const Vector& leftSrc, const Vector& rightSrc, double scalar );
	double Dot( const Vector& leftSrc, const Vector& rightSrc );
	void Cross( Vector& dst, const Vector& leftSrc, const Vector& rightSrc );
	double Length( const Vector& src );
	double Distance( const Vector& leftSrc, const Vector& rightSrc );
	void Scale( Vector& dst, const Vector& src, double scalar );
	void Normalize( Vector& dst, const Vector& src );
	void Rotate( Vector& dst, const Vector& src, const Vector& axis, double angle );
	void Lerp( Vector& dst, const Vector& src0, const Vector& src1, double lerp );
	void Slerp( Vector& dst, const Vector& src0, const Vector& src1, double slerp );
	double RandomNumber( double min, double max );
	void RandomVector( Vector& dst, double min, double max );
	void Project( Vector& dst, const Vector& src, const Vector& unitVector );
	void Reject( Vector& dst, const Vector& src, const Vector& unitVector );
}

// Vector.h