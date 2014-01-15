// Spline.h

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
	class Spline
	{
	public:

		Spline( void );
		virtual ~Spline( void );

		virtual void Evaluate( double t, Vector& point ) const = 0;
		virtual void EvaluateDerivative( double t, Vector& point ) const = 0;
		
		virtual double ShortestDistanceToSpline( const Vector& point ) const;

		double EvaluateSquareDistanceToPoint( const Vector& point, double t ) const;
		double EvaluateSquareDistanceToPointDerivative( const Vector& point, double t ) const;
	};

	// This is a simple line segment of the form f(t) = c0 + c1*t,
	// where we assume that f(0)=p0 and f(1)=p2.  Here, c0,c1 are
	// the coeficients and p0,p1 are the control points.  Under these
	// assumptions, we get
	//
	//   f(t) = p0 + (p1 - p0)*t.
	//
	// Calculating the derivative of f with respect to t, we get
	//
	//   f'(t) = p1 - p0.
	//
	class LinearSpline : public Spline
	{
	public:

		LinearSpline( void );
		virtual ~LinearSpline( void );

		virtual void Evaluate( double t, Vector& point ) const override;
		virtual void EvaluateDerivative( double t, Vector& point ) const override;

		virtual double ShortestDistanceToSpline( const Vector& point ) const;

		Vector controlPoint[2];
	};

	// This is a simple spline of the form f(t) = c0 + c1*t + c2*t*t,
	// where we assume that f(0)=p0, f(1/2)=p1 and f(1)=p2.  Here,
	// c0,c1,c2 are the coeficients and p0,p1,p2 are the control points.
	// Under these assumptions, we get
	//
	//   f(t) = p0 + (-3*p0 + 4*p1 - p2)*t + (2*p0 -4*p1 + 2*p2)*t*t.
	//
	// Calculating the derivative of f with respect to t, we get
	//
	//   f'(t) = -3*p0 + 4*p1 - p2 + (4*p0 - 8*p1 + 4*p2)*t.
	//
	class QuadraticSpline : public Spline
	{
	public:

		QuadraticSpline( void );
		virtual ~QuadraticSpline( void );

		virtual void Evaluate( double t, Vector& point ) const override;
		virtual void EvaluateDerivative( double t, Vector& point ) const override;

		virtual double ShortestDistanceToSpline( const Vector& point ) const;

		Vector controlPoint[3];
	};

	//class CubicSpline

	//class CubicBezierSpline
}

// Spline.h