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
	convexHullValid = true;
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
	this->element.AssignSumOfBlades( element );

	convexHullValid = false;
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	element.AssignSumOfBlades( this->element );
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
	if( !convexHullValid )
	{
		RegenerateConvexHull();
		convexHullValid = true;
	}

	/*
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );
	*/
	
	//...
}

//=========================================================================================
void QuadricGeometry::RegenerateConvexHull( void )
{
	//...

	// all p s.t. pRpR~ . G = 0.
	// don't actually calculate RpR~ -- that would be slow.
	// just iterate over ((e0 + x*e1 + y*e2 + z*e3)^(e4 + x*e5 + y*e6 + z*e7)).G=0.
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