// QuadricGeometry.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "QuadricGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( QuadricGeometry, GAVisToolGeometry );

//=========================================================================================
QuadricGeometry::QuadricGeometry( BindType bindType ) : GAVisToolGeometry( bindType )
{
	// Make composition and decomposition code for the quadric...

	// Test code...
	// a00 + 2*( a01*x + a02*y + a03*z ) +
	// a11*x^2 + 2*( a12*x*y + a13*x*z ) +
	// a22*y^2 + 2*( a23*y*z ) +
	// a33*z^2 = 0
	// A sphere of radius 2...
	quadric.a00 = -4.0;
	quadric.a11 = 1.0;
	quadric.a22 = 1.0;
	quadric.a33 = 1.0;

	traceListValid = false;
}

//=========================================================================================
/*virtual*/ QuadricGeometry::~QuadricGeometry( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* QuadricGeometry::Create( BindType bindType )
{
	return new QuadricGeometry( bindType );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize, "The variable \"%s\" is being interpreted as a quadric.\n", name );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Quadric: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Draw( GAVisToolRender& render, bool selected )
{
	// If our trace-list is out of date, go up-date it.  Hopefully this won't take too long.
	if( !traceListValid )
	{
		VectorMath::CoordFrame coordFrame;
		VectorMath::Identity( coordFrame );
		double range = 4.0;
		double density = 10.0;
		quadric.GenerateTracesAlongAxis( coordFrame.xAxis, range, density, traceList );
		quadric.GenerateTracesAlongAxis( coordFrame.yAxis, range, density, traceList );
		quadric.GenerateTracesAlongAxis( coordFrame.zAxis, range, density, traceList );
		traceListValid = true;
	}

	// Go render the trace list here...
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Zero( center );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Translate( const VectorMath::Vector& delta )
{
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Scale( float scale )
{
}

// QuadricGeoemtry.cpp