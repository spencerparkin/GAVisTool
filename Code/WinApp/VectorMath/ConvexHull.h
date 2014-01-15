// ConvexHull.h

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
#include "Plane.h"
#include "Triangle.h"
#include "CoordFrame.h"
#include "Calculator/CalcLib.h"

namespace VectorMath
{
	class ConvexHull
	{
	public:

		ConvexHull( void );
		~ConvexHull( void );

		class Triangle : public Utilities::List::Item
		{
		public:
			int vertexIndex[3];
		};

		class Point : public Utilities::List::Item
		{
		public:
			int vertexIndex;
		};

		bool RegenerateTriangleList( void );
		const Utilities::List& TriangleList( void ) const;

		void AddVertex( double x, double y, double z );
		void AddVertex( const Vector& vertex );
		void RemoveAllVertices( void );
		const VectorMath::Vector& operator[]( int index ) const;
		VectorMath::Vector& operator[]( int index );
		int VertexCount( void ) const;

		void DumpCode( char* code, int codeSize ) const;

	private:

		enum TriangleSide
		{
			NEITHER,
			FRONT,
			BACK,
		};

		void CullPoints( void );
		bool PointIsOnOrInsideHull( const Point* point );
		TriangleSide CalculateTriangleSide( const Triangle* triangle, const Point* point, double epsilon = 1e-7 );
		bool FindFourNonCoplanarPoints( int* tetrahedron );
		double TetrahedronVolume( int* tetrahedron );
		void IntegrateTetrahedron( int* tetrahedron );
		Triangle* FindTriangle( const Triangle* triangle );
		void ExpandHull( const Point* point );
		const Point* ChoosePoint( void );
		bool IsDegenerate( int* tetrahedron );

		typedef Utilities::Array< VectorMath::Vector > VertexArray;

		Utilities::List triangleList;
		Utilities::List pointList;
		VertexArray vertexArray;
	};
}

// ConvexHull.h