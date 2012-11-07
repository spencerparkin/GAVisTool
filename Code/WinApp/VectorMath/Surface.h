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

		// This function traces the manifold in several trace planes.
		void GenerateTracesAlongAxis( const Vector& axis, double range, double planeCount, const Aabb& aabb, Utilities::List& traceList, bool resetList = false );
		Trace* CalculateTraceInPlane( const Plane& plane, const Vector& seed, const Aabb& aabb );
		bool StepTraceInPlane( const Plane& plane, int direction, Vector& point, double traceDelta, double epsilon );
		bool ConvergePointToSurfaceInPlane( const Plane* plane, Vector& point, double epsilon );

		class ManifoldPoint : public Utilities::List::Item
		{
		public:

			ManifoldPoint( const Vector& point );
			virtual ~ManifoldPoint( void );

			Vector point;
			enum { MAX_DEGREE = 5 };
			ManifoldPoint* adjacentPoint[ MAX_DEGREE ];		// These are in CCW order.
			int degree;
		};

		// This function skins the manifold using one or more triangular meshes.
		//void GenerateTriangularMesh(...);
		// THIS STUFF IS __UNTESTED__ AND WILL PROBABLY NEVER WORK
		bool GenerateManifold( const Aabb& aabb, const Vector& seedPoint, Utilities::List& manifoldPointList );
		bool MakeManifoldAboutPoint( const Aabb& aabb, ManifoldPoint* centralManifoldPoint, Utilities::List& manifoldPointList, Utilities::List& pointQueue, double epsilon );
		ManifoldPoint* FindNearestManifoldPoint( const Utilities::List& manifoldPointList, const Vector& point, double& squareDistance );

		// Derived classes implement this interface.
		virtual double EvaluateAt( const VectorMath::Vector& point ) = 0;
		virtual double EvaluatePartialX( const VectorMath::Vector& point ) = 0;
		virtual double EvaluatePartialY( const VectorMath::Vector& point ) = 0;
		virtual double EvaluatePartialZ( const VectorMath::Vector& point ) = 0;
		virtual void EvaluateGradientAt( const VectorMath::Vector& point, VectorMath::Vector& gradient );
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

		virtual double EvaluateAt( const VectorMath::Vector& point );
		virtual double EvaluatePartialX( const VectorMath::Vector& point );
		virtual double EvaluatePartialY( const VectorMath::Vector& point );
		virtual double EvaluatePartialZ( const VectorMath::Vector& point );

		double a_, b_, c_, d_, e_, f_, g_, h_, i_, j_, k_;
		double l_, m_, n_, o_, p_, q_, r_, s_, t_, u_, v_;
		double w_, x_, y_;
	};
}

// Surface.h