// Surface.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
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
		friend class SurfaceMeshGenerator;
		friend class SurfaceMesh;

	public:

		Surface( void );
		virtual ~Surface( void );

		class Point : public Utilities::List::Item
		{
		public:
			Point( const Vector& point );
			virtual ~Point( void );

			Vector point;
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
			bool looped;
		};

		struct TraceParameters
		{
			Vector center;
			Vector axis;
			double range;
			double extent;
			int planeCount;
		};

		void GenerateTracesAlongAxis( const TraceParameters& traceParameters, Utilities::List& traceList ) const;
		
		// Derived classes implement this interface.
		virtual double EvaluateAt( const VectorMath::Vector& point ) const = 0;
		virtual double EvaluatePartialX( const VectorMath::Vector& point ) const = 0;
		virtual double EvaluatePartialY( const VectorMath::Vector& point ) const = 0;
		virtual double EvaluatePartialZ( const VectorMath::Vector& point ) const = 0;
		virtual void EvaluateGradientAt( const VectorMath::Vector& point, VectorMath::Vector& gradient ) const;

	private:

		void CalculateTracePlane( const TraceParameters& traceParameters, int index, Plane& plane, Vector& planeCenter ) const;
		void CalculateTracesInPlane( const Plane& plane, const Vector& center, double extent, Utilities::List& traceList ) const;
		Trace* CalculateTraceInPlane( const Plane& plane, const Vector& seed, const Aabb& aabb ) const;
		bool StepTraceInPlane( const Plane& plane, int direction, Vector& point, double traceDelta, double epsilon ) const;
		bool ConvergePointToSurfaceInPlane( const Plane* plane, Vector& point, double epsilon ) const;
	};

	class Quadric : public Surface
	{
	public:

		Quadric( void );
		virtual ~Quadric( void );

		virtual double EvaluateAt( const VectorMath::Vector& point ) const;
		virtual double EvaluatePartialX( const VectorMath::Vector& point ) const;
		virtual double EvaluatePartialY( const VectorMath::Vector& point ) const;
		virtual double EvaluatePartialZ( const VectorMath::Vector& point ) const;

		// A quadric is the set of solutions in <x,y,z>
		// to the following equation.
		//
		// 0 = Ax^2 + By^2 + Cz^2 + Dxy + Exz + Fyz + Gx + Hy + Iz + J
		//
		double A, B, C, D, E, F, G, H, I, J;
	};

	class ConformalQuartic : public Surface
	{
	public:

		ConformalQuartic( void );
		virtual ~ConformalQuartic( void );

		virtual double EvaluateAt( const VectorMath::Vector& point ) const;
		virtual double EvaluatePartialX( const VectorMath::Vector& point ) const;
		virtual double EvaluatePartialY( const VectorMath::Vector& point ) const;
		virtual double EvaluatePartialZ( const VectorMath::Vector& point ) const;

		double c0, c1, c2, c3, c4;
		double c5, c6, c7, c8, c9;
		double c10, c11, c12, c13, c14;
		double c15, c16, c17, c18, c19;
		double c20, c21, c22, c23, c24;
	};
}

// Surface.h