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
		};

		class Triangle : public Utilities::List::Item
		{
		public:
			Vertex* vertex[3];
		};

		// The generation of path connected components
		// will be based upon growth and cell division.
		// We start as a single triangle (cell), and then
		// we grow the triangle until it reaches a point
		// where it needs to be divided in two.  Whenever
		// a triangle is divided, we choose a vertex of
		// the triangle with minimal degree in the mesh
		// to divide the triangle.  If all points have the
		// same degree, it's best to choose a vertex that
		// is one that has stopped growing, so that we create
		// a new points that grows outward from the mesh.  Through the course
		// of the algorithm, we keep track of which points
		// are interior to the growing mesh, and which are
		// on the periphery.  Only those points on the
		// periphery need grow.  They always grow in a direction
		// that is away from the average of the adjacent stationary points.
		// All of this growth is sliding the points of the mesh
		// along the surface.  The trickiest part will be
		// stopping growth when the mesh meets itself.
		// Stopping when we hit the edge of an AABB is trivial.
		class PathConnectedComponent : public Utilities::List::Item
		{
		public:
			PathConnectedComponent( void );
			virtual ~PathConnectedComponent( void );

			Utilities::List stationaryPoints;
			Utilities::List growingPoints;
			Utilities::List triangleList;
		};

		Utilities::List componentList;
	};
}

// SurfaceMesh.h