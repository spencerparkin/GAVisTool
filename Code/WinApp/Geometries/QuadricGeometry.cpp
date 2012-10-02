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
	CalcLib::Calculator calculator( "geoalg" );

	// ( e0 + x*e1 + y*e2 + z*e3 )^( e4 + x*e5 + y*e6 + z*e7 )
	// = 1*( e0^e4 )
	// + x*( e0^e5 + e1^e4 )
	// + y*( e0^e6 + e2^e4 )
	// + z*( e0^e7 + e3^e4 )
	// + x^2*( e1^e5 )
	// + x*y*( e1^e6 + e2^e5 )
	// + x*z*( e1^e7 + e3^e5 )
	// + y^2*( e2^e6 )
	// + y*z*( e2^e7 + e3^e6 )
	// + z^2*( e3^e7 )

	char decompositionCode[ 512 ];
	strcpy_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"a00 = scalar_part( B, e4^e0 ),"
		"a01 = scalar_part( B, e5^e0 ) + scalar_part( B, e4^e1 ),"
		"a02 = scalar_part( B, e6^e0 ) + scalar_part( B, e4^e2 ),"
		"a03 = scalar_part( B, e7^e0 ) + scalar_part( B, e4^e3 ),"
		"a11 = scalar_part( B, e5^e1 ),"
		"a12 = scalar_part( B, e6^e1 ) + scalar_part( B, e5^e2 ),"
		"a13 = scalar_part( B, e7^e1 ) + scalar_part( B, e5^e3 ),"
		"a22 = scalar_part( B, e6^e2 ),"
		"a23 = scalar_part( B, e7^e2 ) + scalar_part( B, e6^e3 ),"
		"a33 = scalar_part( B, e7^e3 ),"
		")"
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

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
	this->element.AssignSumOfBlades( element );
	traceListValid = false;

	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "B", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "a00", *number );
	multivector->AssignTo( quadric.a00, gaEnv );
	gaEnv.LookupVariable( "a01", *number );
	multivector->AssignTo( quadric.a01, gaEnv );
	gaEnv.LookupVariable( "a02", *number );
	multivector->AssignTo( quadric.a02, gaEnv );
	gaEnv.LookupVariable( "a03", *number );
	multivector->AssignTo( quadric.a03, gaEnv );
	gaEnv.LookupVariable( "a11", *number );
	multivector->AssignTo( quadric.a11, gaEnv );
	gaEnv.LookupVariable( "a12", *number );
	multivector->AssignTo( quadric.a12, gaEnv );
	gaEnv.LookupVariable( "a13", *number );
	multivector->AssignTo( quadric.a13, gaEnv );
	gaEnv.LookupVariable( "a22", *number );
	multivector->AssignTo( quadric.a22, gaEnv );
	gaEnv.LookupVariable( "a23", *number );
	multivector->AssignTo( quadric.a23, gaEnv );
	gaEnv.LookupVariable( "a33", *number );
	multivector->AssignTo( quadric.a33, gaEnv );

	delete number;
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
	// If our trace-list is out of date, go up-date it.  Hopefully this won't take too long.
	if( !traceListValid )
	{
		// TODO: We need to deduce the center of the quadric for the AABB setup to be correct.
		VectorMath::CoordFrame coordFrame;
		VectorMath::Identity( coordFrame );
		double range = 10.0;
		double planeCount = 10.0;
		VectorMath::Vector center, delta;
		VectorMath::Zero( center );
		VectorMath::Set( delta, 5.0, 5.0, 5.0 );
		VectorMath::Aabb aabb;
		MakeAabb( aabb, center, delta );
		traceList.RemoveAll( true );
		quadric.GenerateTracesAlongAxis( coordFrame.xAxis, range, planeCount, aabb, traceList );
		quadric.GenerateTracesAlongAxis( coordFrame.yAxis, range, planeCount, aabb, traceList );
		quadric.GenerateTracesAlongAxis( coordFrame.zAxis, range, planeCount, aabb, traceList );
		traceListValid = true;
	}

	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );

	render.Color( color, alpha );

	VectorMath::Quadric::Trace* trace = ( VectorMath::Quadric::Trace* )traceList.LeftMost();
	while( trace )
	{
		DrawTrace( trace, render );
		trace = ( VectorMath::Quadric::Trace* )trace->Right();
	}
}

//=========================================================================================
void QuadricGeometry::DrawTrace( VectorMath::Quadric::Trace* trace, GAVisToolRender& render )
{
	VectorMath::Quadric::Point* point = 0, *nextPoint = 0;
	VectorMath::Quadric::Point* firstPoint = ( VectorMath::Quadric::Point* )trace->pointList.LeftMost();
	for( point = firstPoint; point; point = nextPoint )
	{
		nextPoint = ( VectorMath::Quadric::Point* )point->Right();
		if( nextPoint )
			render.DrawLine( point->point, nextPoint->point );
		else if( trace->looped )
			render.DrawLine( point->point, firstPoint->point );
	}
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Zero( center );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Translate( const VectorMath::Vector& delta )
{
	// It should be easy to figure out how to translate the quadric.
	// Rotating it, on the other hand, sounds much harder.  Can GA help at all?
	// How do you scale a quadric about it's center?
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