// GiftWrapper.h

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
#include "Triangle.h"
#include "Calculator/CalcLib.h"

namespace VectorMath
{
	// This class implements a simple variation of the gift wrapping algorithm.
	// Without a given neighborhood, it is the gift wrapping algorithm, and can
	// find convex hulls of point clouds.  With a given neighborhood, it can find
	// what one might consider the concave hull of a given point cloud.
	class GiftWrapper
	{
	public:

		GiftWrapper( void );
		virtual ~GiftWrapper( void );

		void AddPoint( const Vector& point );
		void RemoveAllPoints( void );

		// If a zero radius is given, we find the convex hull.
		// If a positive radius is given, then we may find a concave hull.
		void SetNeighborhoodRadius( double radius );
		double GetNeighborhoodRadius( void ) const;

		// A generated hull returned by our algorithm will
		// constain a list of triangles with consistent
		// triangle winding orders.  Viewing the hull from
		// the outside, the triangle winding orders will
		// be CCW.  If a neighborhood was given, it's possible
		// that the hull returned is not a closed surface, in
		// which case there is no inside and outside, but the
		// triangle winding orders will still be consistent.
		class Hull
		{
		public:
			Hull( void );
			virtual ~Hull( void );

			VectorMath::Triangle& operator[]( int index );
			int Count( void ) const;

		private:

			VectorMath::Triangle* triangleList;
			int triangleListSize;
		};

		// The assumptoin is made here that no two points
		// in the point cloud are the same.
		Hull* GenerateHull( void );

	private:

		class Point : public Utilities::List::Item
		{
		public:
			Point( const Vector& point );

			Vector point;
		};

		class Triangle : public Utilities::List::Item
		{
		public:
			Triangle( Point* vertex0, Point* vertex1, Point* vertex2 );

			Point* vertex[3];
		};

		class Edge : public Utilities::List::Item
		{
		public:
			Edge( Point* vertex0, Point* vertex1 );
			bool MatchesNewEdge( const Edge& edge ) const;

			Point* vertex[2];
		};

		Triangle* GenerateInitialTriangle( void );
		void CullPointsInTriangle( Triangle* triangle );
		void CreateTriangleEdges( Triangle* triangle, Edge** edges );
		void ExpandEdgeLoop( Edge** edges );

		Utilities::List pointList;		// This is the set of all points making up the cloud.
		Utilities::List edgeList;		// These are the edges, in undefined order, making up the periphery of the partially constructed hull.
		Utilities::List triangleList;	// This is the set of all triangles in the partially or fully constructed hull.

		double neighborhoodRadius;
	};
}

// GiftWrapper.h