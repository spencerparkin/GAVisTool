// Surface.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Vector.h"
#include "Plane.h"
#include "AxisAlignedBoundingBox.h"
#include "CoordFrame.h"
#include "Calculator/CalcLib.h"

namespace VectorMath
{
	class Surface
	{
	public:

		Surface( void );
		virtual ~Surface( void );

		class Point : public Utilities::List::Item
		{
		public:
			Point( const VectorMath::Vector& point );
			virtual ~Point( void );

			VectorMath::Vector point;
		};

		// The trace of a surface in a plane is the crossection or intersection
		// of the surface in that plane.  It may consist of zero, one or two curves
		// in the case that it is a quadric.
		class Trace : public Utilities::List::Item
		{
		public:
			Trace( void );
			virtual ~Trace( void );

			bool IsPointOnTrace( const Vector& givenPoint, double epsilon ) const;

			Utilities::List pointList;
		};

		void GenerateTracesAlongAxis( const Vector& axis, double range, double density, Utilities::List& traceList, bool resetList = false );
		Trace* CalculateTraceInPlane( const Plane& plane, const Vector& seed, const Aabb& aabb );
		bool StepTraceInPlane( const Plane& plane, int direction, Vector& point, double traceDelta, double epsilon );
		bool ConvergePointToSurfaceInPlane( const Plane& plane, Vector& point, double epsilon );

		virtual double EvaluateAt( const VectorMath::Vector& point ) = 0;
		virtual double EvaluatePartialX( const VectorMath::Vector& point ) = 0;
		virtual double EvaluatePartialY( const VectorMath::Vector& point ) = 0;
		virtual double EvaluatePartialZ( const VectorMath::Vector& point ) = 0;
		void EvaluateGradientAt( const VectorMath::Vector& point, VectorMath::Vector& gradient );

		
	};

	class Quadric : public Surface
	{
	public:

		Quadric( void );
		virtual ~Quadric( void );

		virtual double EvaluateAt( const VectorMath::Vector& point );
		virtual double EvaluatePartialX( const VectorMath::Vector& point );
		virtual double EvaluatePartialY( const VectorMath::Vector& point );
		virtual double EvaluatePartialZ( const VectorMath::Vector& point );

		// A quadric is the set of solution in <x,y,z>
		// to the following equation.
		// a00 + 2*( a01*x + a02*y + a03*z ) +
		// a11*x^2 + 2*( a12*x*y + a13*x*z ) +
		// a22*y^2 + 2*( a23*y*z ) +
		// a33*z^2 = 0
		double a00, a01, a02, a03;
		double a11, a12, a13;
		double a22, a23;
		double a33;
	};
}

// Surface.h