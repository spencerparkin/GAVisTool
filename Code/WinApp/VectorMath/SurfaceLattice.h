// SurfaceLattice.h

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
	class SurfaceLatticeDrawCallback
	{
	public:
		virtual void DrawLine( const Vector& vertex0, const Vector& vertex1 ) = 0;
		virtual void DrawTriangle( const Vector& vertex0, const Vector& vertex1, const Vector& vertex2 ) = 0;
	};

	class SurfaceLattice
	{
		friend class SurfaceLatticeRenderer;

	public:

		SurfaceLattice( void );
		virtual ~SurfaceLattice( void );

		class GenerationParameters
		{
		public:
			GenerationParameters( void );

			double latticeWalkLength;
			double epsilon;
			Aabb aabb;
			Vector seed;
			int directionCount;
			int maximumIterationCount;
		};

		// We only generate one path-connected component of the given surface.
		bool Generate( const Surface& surface, const GenerationParameters& genParms );
		void DrawAsWireFrame( SurfaceLatticeDrawCallback& drawCallback );
		void DrawAsTriangleMesh( SurfaceLatticeDrawCallback& drawCallback );

	private:

		class Vertex : public Utilities::List::Item
		{
		public:

			Vertex( const Vector& point, int directionCount );
			virtual ~Vertex( void );

			void CalculateTangentDirections( const Surface& surface, const GenerationParameters& genParms, const Vector* adjacentTangentDir );
			int FindDirectionIndex( Vertex* adjacentVertex );

			struct DirectionData
			{
				Vertex* adjacentVertex;
				Vector tangentDir;
				bool edgeMade;
			};

			Vector point;
			DirectionData* directionData;
			int directionCount;
		};

		class Edge : public Utilities::List::Item
		{
		public:

			Edge( Vertex* vertex0, Vertex* vertex1 );
			virtual ~Edge( void );

			Vertex* vertex[2];
		};

		class Triangle : public Utilities::List::Item
		{
		public:

			Triangle( Vertex* vertex0, Vertex* vertex1, Vertex* vertex2 );
			virtual ~Triangle( void );

			Vertex* vertex[3];
		};

		Utilities::List vertexList;
		Utilities::List edgeList;
		Utilities::List triangleList;

		void DeleteAllLists( void );
		bool GenerateLattice( const Surface& surface, const GenerationParameters& genParms );
		bool GenerateEdges( const GenerationParameters& genParms );
		bool GenerateMesh( const GenerationParameters& genParms );
		bool ExploreSurface( const Surface& surface, const GenerationParameters& genParms, Vertex* vertex, Utilities::List& vertexQueue );
		Vertex* FindNearEnoughVertex( const Vector& point, const GenerationParameters& genParms );
	};
}

// SurfaceLattice.h