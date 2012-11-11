// SurfaceGeometry.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "../Geometry.h"
#include "../VectorMath/Surface.h"
#include "../VectorMath/ConvexHull.h"

//=========================================================================================
class SurfaceGeometry : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( SurfaceGeometry );

public:

	SurfaceGeometry( BindType bindType, VectorMath::Surface* surface );
	virtual ~SurfaceGeometry( void );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const override;

	virtual void Draw( GAVisToolRender& render, bool selected );
	virtual void CalcCenter( VectorMath::Vector& center ) const;
	
	virtual void Translate( const VectorMath::Vector& delta );
	virtual void Rotate( const VectorMath::Vector& unitAxis, float angle );
	virtual void Scale( float scale );

protected:

	void RegenerateTraceList( void );
	void DrawTrace( VectorMath::Quadric::Trace* trace, GAVisToolRender& render );

	VectorMath::Surface* surface;
	Utilities::List traceList;
	bool traceListValid;

	GeometricAlgebra::SumOfBlades element;
};

// SurfaceGeometry.h