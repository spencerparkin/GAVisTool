// QuadricGeometry.h

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
class QuadricGeometry : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( QuadricGeometry );

public:

	QuadricGeometry( BindType bindType );
	virtual ~QuadricGeometry( void );

	static GAVisToolBindTarget* Create( BindType bindType );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const override;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

	virtual void Draw( GAVisToolRender& render, bool selected );
	virtual void CalcCenter( VectorMath::Vector& center ) const;
	
	virtual void Translate( const VectorMath::Vector& delta );
	virtual void Rotate( const VectorMath::Vector& unitAxis, float angle );
	virtual void Scale( float scale );

private:

	void DrawTrace( VectorMath::Quadric::Trace* trace, GAVisToolRender& render );

	VectorMath::Quadric quadric;
	Utilities::List traceList;
	bool traceListValid;

	GeometricAlgebra::SumOfBlades element;
};

//=========================================================================================
// It's important to realize that this class does not interpret
// a bivector as a quadric surface of one point, which does exist.
// It represents a projective point that is used in the definition
// of how bivectors can represent quadric surfaces.
class QuadricGeometryPoint : public GAVisToolGeometry
{
	DECLARE_CALCLIB_CLASS( QuadricGeometryPoint );

public:

	QuadricGeometryPoint( BindType bindType );
	virtual ~QuadricGeometryPoint( void );

	static GAVisToolBindTarget* Create( BindType bindType );

	virtual void DecomposeFrom( const GeometricAlgebra::SumOfBlades& element ) override;
	virtual void ComposeTo( GeometricAlgebra::SumOfBlades& element ) const override;

	virtual void DumpInfo( char* printBuffer, int printBufferSize ) const override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

	virtual void Draw( GAVisToolRender& render, bool selected );
	virtual void CalcCenter( VectorMath::Vector& center ) const;
	
	virtual void Translate( const VectorMath::Vector& delta );
	virtual void Rotate( const VectorMath::Vector& unitAxis, float angle );
	virtual void Scale( float scale );

	virtual void NameCenterOffset( VectorMath::Vector& offsetDelta ) override;

private:

	VectorMath::Vector point;
	double weight;
};

// QuadricGeometry.h