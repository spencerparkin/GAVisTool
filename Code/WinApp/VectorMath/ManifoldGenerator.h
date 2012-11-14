// ManifoldGenerator.h

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

namespace VectorMath
{
	class Manifold
	{
	public:

		Manifold( void );
		virtual ~Manifold( void );

		bool IsPointOnManifold( const Vector& point, double epsilon );
	};

	class ManifoldGenerator
	{
	public:

		ManifoldGenerator( void );
		virtual ~ManifoldGenerator( void );

		bool GenerateManifold( const Surface& surface, const Vector& seed, Manifold& manifold );
	};
}

// ManifoldGenerator.h