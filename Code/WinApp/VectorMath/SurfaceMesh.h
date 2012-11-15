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

#include "Vector.h"
#include "Plane.h"
#include "AxisAlignedBoundingBox.h"
#include "Surface.h"
#include "Calculator/CalcLib.h"

namespace VectorMath
{
	class SurfaceMesh
	{
	public:

		SurfaceMesh( void );
		virtual ~SurfaceMesh( void );

		class PathConnectedComponent : public Utilities::List::Item
		{
		public:
			PathConnectedComponent( void );
			virtual ~PathConnectedComponent( void );

			bool IsPointOnSurface( const Vector& point, double epsilon ) const;

			VectorMath::Triangle* triangleList;
			int triangleListSize;
		};

		class GenerationParameters
		{
		public:

			GenerationParameters( void );
			virtual ~GenerationParameters( void );

			mutable Vector seed;
			double epsilon;
			double edgeLengthThreshold;
			double stuntedDistanceThreshold;
			double growLength;
			Aabb aabb;
		};

		class PathConnectedComponentGenerator
		{
		public:
			PathConnectedComponentGenerator( void );
			virtual ~PathConnectedComponentGenerator( void );

			PathConnectedComponent* Generate( const Surface& surface, const GenerationParameters& genParms );

		private:

			class Vertex : public Utilities::List::Item
			{
			public:

				Vertex( const Vector& point );
				virtual ~Vertex( void );

				bool IsAdjacentTo( const Vertex& vertex ) const;

				Vector point, growDirection;
				enum { MAX_DEGREE = 10, };
				Vertex* adjacentVertex[MAX_DEGREE];
				int degree;
			};

			class Triangle : public Utilities::List::Item
			{
			public:

				Triangle( Vertex* vertex0, Vertex* vertex1, Vertex* vertex2 );
				virtual ~Triangle( void );

				Vertex* vertex[3];
			};

			class Edge : public Utilities::List::Item
			{
			public:

				Edge( Vertex* vertex0, Vertex* vertex1 );
				virtual ~Edge( void );

				double Length( void ) const;
				
				struct Side
				{
					enum { LEFT = 0, RIGHT = 1 };
					Vertex* vertex;
					Triangle* triangle;
					Edge* edge[2];
				};

				enum { FRONT = 0, BACK = 1 };
				Vertex* vertex[2];
				Side* side[2];
			};

			bool GenerateInitialTriangle( const Surface& surface, const GenerationParameters& genParms );
			PathConnectedComponent* BuildComponent( void );
			void DeleteLists( void );

			Edge* FindEdgeAtCriticalLength( const GenerationParameters& genParms );
			void SplitEdge( Edge* edge, const Surface& surface );

			void DetermineStuntedPoints( const GenerationParameters& genParms );
			bool PointIsStunted( Vertex* testVertex, const GenerationParameters& genParms );

			void CalculateGrowDirections( const GenerationParameters& genParms );
			void GrowPoints( const Surface& surface, const GenerationParameters& genParms );

			Utilities::List growingPointList;
			Utilities::List maturePointList;
			Utilities::List stuntedPointList;
			Utilities::List staticEdgeList;
			Utilities::List dynamicEdgeList;
			Utilities::List triangleList;
		};

		bool Generate( const Surface& surface, const GenerationParameters& genParms );

		Utilities::List componentList;
	};
}

// SurfaceMesh.h