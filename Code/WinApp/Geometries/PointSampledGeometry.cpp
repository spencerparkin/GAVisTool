// PointSampledGeometry.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "PointSampledGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( PointSampledGeometry, GAVisToolGeometry );

//=========================================================================================
PointSampledGeometry::PointSampledGeometry( BindType bindType ) : GAVisToolGeometry( bindType )
{
}

//=========================================================================================
/*virtual*/ PointSampledGeometry::~PointSampledGeometry( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* PointSampledGeometry::Create( BindType bindType )
{
	return new PointSampledGeometry( bindType );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	this->element.AssignSumOfBlades( element );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	element.AssignSumOfBlades( this->element );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::DumpInfo( char* printBuffer, int printBufferSize ) const
{
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );

	render.Color( color, alpha );

	//...
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::CalcCenter( VectorMath::Vector& center ) const
{
	// Calculate point average here...
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::NameCenterOffset( VectorMath::Vector& offsetDelta )
{
	VectorMath::Set( offsetDelta, 0.0, 0.0, 0.0 );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::Translate( const VectorMath::Vector& delta )
{
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::Scale( float scale )
{
}

// PointSampledGeometry.cpp