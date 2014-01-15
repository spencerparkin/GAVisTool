// Spline.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Spline.h"
#include "BinaryFunctionSearch.h"

//=============================================================================
VectorMath::Spline::Spline( void )
{
}

//=============================================================================
/*virtual*/ VectorMath::Spline::~Spline( void )
{
}

//=============================================================================
/*virtual*/ double VectorMath::Spline::ShortestDistanceToSpline( const Vector& point ) const
{
	// Create a local class that can evaluate our square distance derivative.
	class SquareDistanceToPointDerivativeFunction : public BinaryFunctionSearch::Function
	{
	public:
		virtual double Evaluate( double x ) override
		{
			return spline->EvaluateSquareDistanceToPointDerivative( point, x );
		}

		const Spline* spline;
		Vector point;
	};

	// Setup an instance of the class.
	SquareDistanceToPointDerivativeFunction function;
	Copy( function.point, point );
	function.spline = this;

	// Go search for the index of the closest point on the spline to the given point.
	// If we didn't find it in [0,1], we might go search a different interval, but for now, just fail.
	BinaryFunctionSearch search( &function );
	double t;
	if( !search.FindValue( t, 0.0, 0.0, 1.0 ) )
		return -1.0;

	// Calculate and return the result.
	Vector closestPoint;
	Evaluate( t, closestPoint );
	Vector delta;
	Sub( delta, closestPoint, point );
	double length = Length( delta );
	return length;
}

//=============================================================================
double VectorMath::Spline::EvaluateSquareDistanceToPoint( const Vector& point, double t ) const
{
	// D(t) = (p - f(t)).(p - f(t)) = p^2 - 2*p.f(t) + f(t).f(t)
	Vector splinePoint;
	Evaluate( t, splinePoint );
	Vector delta;
	Sub( delta, point, splinePoint );
	return Dot( delta, delta );
}

//=============================================================================
double VectorMath::Spline::EvaluateSquareDistanceToPointDerivative( const Vector& point, double t ) const
{
	// D(t) = (p - f(t)).(p - f(t)) = p^2 - 2*p.f(t) + f(t).f(t)
	// D'(t) = -2*p.f'(t) + 2*f'(t).f(t) = 2*f'(t).(f(t) - p)
	Vector splinePoint;
	Evaluate( t, splinePoint );
	Vector splinePointDeriv;
	EvaluateDerivative( t, splinePointDeriv );
	Vector delta;
	Sub( delta, splinePoint, point );
	return 2.0 * Dot( splinePointDeriv, delta );
}

//=============================================================================
VectorMath::LinearSpline::LinearSpline( void )
{
	Zero( controlPoint[0] );
	Zero( controlPoint[1] );
}

//=============================================================================
/*virtual*/ VectorMath::LinearSpline::~LinearSpline( void )
{
}

//=============================================================================
/*virtual*/ void VectorMath::LinearSpline::Evaluate( double t, Vector& point ) const
{
	// f(t) = p0 + (p1 - p0)*t.
	Lerp( point, controlPoint[0], controlPoint[1], t );
}

//=============================================================================
/*virtual*/ void VectorMath::LinearSpline::EvaluateDerivative( double t, Vector& point ) const
{
	// f'(t) = p1 - p0.
	Sub( point, controlPoint[1], controlPoint[0] );
}

//=============================================================================
/*virtual*/ double VectorMath::LinearSpline::ShortestDistanceToSpline( const Vector& point ) const
{
	Vector p_p0, p1_p0;
	Sub( p_p0, point, controlPoint[0] );
	Sub( p1_p0, controlPoint[1], controlPoint[0] );
	double t = Dot( p_p0, p1_p0 ) / Dot( p1_p0, p1_p0 );
	Vector vec;
	Evaluate( t, vec );
	return Length( vec );
}

//=============================================================================
VectorMath::QuadraticSpline::QuadraticSpline( void )
{
	Zero( controlPoint[0] );
	Zero( controlPoint[1] );
	Zero( controlPoint[2] );
}

//=============================================================================
/*virtual*/ VectorMath::QuadraticSpline::~QuadraticSpline( void )
{
}

//=============================================================================
void VectorMath::QuadraticSpline::Evaluate( double t, Vector& point ) const
{
	// f(t) = p0 + (-3*p0 + 4*p1 - p2)*t + (2*p0 -4*p1 + 2*p2)*t*t.
	Vector c1, c2;
	LinearCombo( c1, -3.0, controlPoint[0], 4.0, controlPoint[1], -1.0, controlPoint[2] );
	LinearCombo( c2, 2.0, controlPoint[0], -4.0, controlPoint[1], 2.0, controlPoint[2] );
	LinearCombo( point, 1.0, controlPoint[0], t, c1, t*t, c2 );
}

//=============================================================================
void VectorMath::QuadraticSpline::EvaluateDerivative( double t, Vector& point ) const
{
	// f'(t) = -3*p0 + 4*p1 - p2 + (4*p0 - 8*p1 + 4*p2)*t.
	Vector c0, c1;
	LinearCombo( c0, -3.0, controlPoint[0], 4.0, controlPoint[1], -1.0, controlPoint[2] );
	LinearCombo( c1, 4.0, controlPoint[0], -8.0, controlPoint[1], 4.0, controlPoint[2] );
	LinearCombo( point, 1.0, c0, t, c1 );
}

//=============================================================================
/*virtual*/ double VectorMath::QuadraticSpline::ShortestDistanceToSpline( const Vector& point ) const
{
	// I'm sure there is a way to find an explicit formula, but
	// my math on paper is too error-prone.  Go with the numerical method.
	return Spline::ShortestDistanceToSpline( point );
}

// Spline.cpp