// SurfaceMesh.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Surface.h"

namespace VectorMath
{
	class SurfaceMesh;

	// An instance of this class is used to generate a surface
	// mesh object from the given surface using the given parameters.
	class SurfaceMeshGenerator
	{
	public:

		class Parameters
		{
		public:

			Parameters( void );

			enum { MAX_SEED_POINTS = 4 };
			Vector seedPoint[ MAX_SEED_POINTS ];
			int seedPointCount;
			int maxIterations;
			double epsilon;
			double surfaceWalkDistance;
			Aabb aabb;
		};

		SurfaceMeshGenerator( void );
		virtual ~SurfaceMeshGenerator( void );

		// Go generate all connected components of the given surface as a triangle mesh.
		void Generate( SurfaceMesh& surfaceMesh, const Surface& surface );

		Parameters parms;
	};

	class SurfaceMesh
	{
		friend class SurfaceMeshGenerator;

	public:

		SurfaceMesh( void );
		virtual ~SurfaceMesh( void );

		class Triangle : public Utilities::List::Item
		{
		public:

			Triangle( void );
			virtual ~Triangle( void );

			bool IsComplete( void ) const;

			// These are the vertices of the triangle.  We hope to maintain a
			// consistent winding order.
			VectorMath::Triangle vertices;

			// A triangle is considered complete when all of its edges are complete.
			// An uncompleted edge indicates an edge off of which an additional/adjacent
			// triangle needs to be created or where it needs to be determined that such a
			// triangle cannot be created due to it being out of bounds.
			bool edgeComplete[3];

			Triangle* adjacentTriangle[3];
		};

		void WipeClean( void );
		bool IsPointOnMesh( const Vector& point, double epsilon ) const;
		const Utilities::List& TriangleList( void ) const;

	private:

		void AbsorbAllCompletedTriangles( SurfaceMesh& surfaceMesh );
		bool GenerateInitialTriangle( const Vector& surfacePoint, const Surface& surface, const SurfaceMeshGenerator::Parameters& parms );
		bool CompleteAnEdge( const Surface& surface, const SurfaceMeshGenerator::Parameters& parms );

		Utilities::List completedTriangleList;
		Utilities::List uncompletedTriangleList;
	};
}

// SurfaceMesh.h