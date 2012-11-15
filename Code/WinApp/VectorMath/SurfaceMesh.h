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

		class Vertex : public Utilities::List::Item
		{
		public:
			Vector point;
			enum { MAX_DEGREE = 10, };
			Vertex* adjacentVertex[MAX_DEGREE];
			int degree;
		};

		class Edge : public Utilities::List::Item
		{
		public:
			Vertex* connectedVertex[2];
			Vertex* opposingVertex[2];
			Triangle* adjacentTriangle[2];
		};

		class Triangle : public Utilities::List::Item
		{
		public:
			Vertex* vertex[3];
		};

		class PathConnectedComponent : public Utilities::List::Item
		{
		public:
			PathConnectedComponent( void );
			virtual ~PathConnectedComponent( void );

			bool IsPointOnSurface( const Vector& point, double epsilon ) const;

			Utilities::List pointList;
			Utilities::List triangleList;
		};

		struct GenerationParameters
		{
			mutable Vector seed;
		};

		class PathConnectedComponentGenerator
		{
		public:
			PathConnectedComponentGenerator( void );
			virtual ~PathConnectedComponentGenerator( void );

			PathConnectedComponent* Generate( const Surface& surface, const GenerationParameters& genParms );

			Utilities::List growingPointList;
			Utilities::List maturePointList;
			Utilities::List stuntedPointList;
			Utilities::List staticEdgeList;
			Utilities::List dynamicEdgeList;
		};

		bool Generate( const Surface& surface, const GenerationParameters& genParms );

		Utilities::List componentList;
	};
}

// SurfaceMesh.h