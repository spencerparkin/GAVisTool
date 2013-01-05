// ConvexHull.h

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
#include "Triangle.h"
#include "CoordFrame.h"
#include "Calculator/CalcLib.h"

namespace VectorMath
{
	//=============================================================================
	class ConvexHull
	{
	public:

		ConvexHull( void );
		virtual ~ConvexHull( void );

		void Reallocate( int pointCloudSize );
		void Deallocate( void );

		Vector& operator[]( int index );
		int CloudSize( void );

		class Triangle : public Utilities::List::Item
		{
		public:
			Triangle( int vertex0, int vertex1, int vertex2 );
			virtual ~Triangle( void );

			int vertex[3];
			Plane plane;
		};

		class Point : public Utilities::List::Item
		{
		public:
			Point( int vertex );
			virtual ~Point( void );

			int vertex;
		};

		Utilities::List& TriangleList( void );

		bool Regenerate( void );

	private:

		// These are used internally.
		void CullPoints( void );
		bool PointIsOnOrInsideHull( Point* point );
		void IntegrateTetrahedron( Point** tetrahedron );
		void ExpandHull( Point* point );
		Point* ChoosePoint( void );
		Triangle* FindTriangle( int vertex0, int vertex1, int vertex2 );
		Point* FindPoint( int vertex );
		bool FindFourNonCoplanarPoints( Point** tetrahedron );

		Utilities::List triangleList;
		Utilities::List pointList;
		Vector* pointCloud;
		int pointCloudSize;
	};
}

// ConvexHull.h