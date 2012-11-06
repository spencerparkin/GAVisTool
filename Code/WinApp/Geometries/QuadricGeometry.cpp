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

	// ( no + x*e1 + y*e2 + z*e3 )^( nob + x*e1b + y*e2b + z*e3b )
	// = 1*( no^nob )
	// + x*( no^e1b + e1^nob )
	// + y*( no^e2b + e2^nob )
	// + z*( no^e3b + e3^nob )
	// + x^2*( e1^e1b )
	// + x*y*( e1^e2b + e2^e1b )
	// + x*z*( e1^e3b + e3^e1b )
	// + y^2*( e2^e2b )
	// + y*z*( e2^e3b + e3^e2b )
	// + z^2*( e3^e3b )

	// 0 = A*x^2 + B*y^2 + C*z^2 + D*xy + E*xz + F*yz + G*x + H*y + I*z + J

	char decompositionCode[ 1024 ];
	strcpy_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"quadric_A = -scalar_part( quadric, e1^e1b ),"
		"quadric_B = -scalar_part( quadric, e2^e2b ),"
		"quadric_C = -scalar_part( quadric, e3^e3b ),"
		"quadric_D = -( scalar_part( quadric, e1^e2b ) + scalar_part( quadric, e2^e1b ) ),"
		"quadric_E = -( scalar_part( quadric, e1^e3b ) + scalar_part( quadric, e3^e1b ) ),"
		"quadric_F = -( scalar_part( quadric, e2^e3b ) + scalar_part( quadric, e3^e2b ) ),"
		"quadric_G = ( scalar_part( quadric, ni^e1b ) + scalar_part( quadric, e1^nib ) ),"
		"quadric_H = ( scalar_part( quadric, ni^e2b ) + scalar_part( quadric, e2^nib ) ),"
		"quadric_I = ( scalar_part( quadric, ni^e3b ) + scalar_part( quadric, e3^nib ) ),"
		"quadric_J = -scalar_part( quadric, ni^nib ),"
		")"
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	traceListValid = false;
}

//=========================================================================================
/*virtual*/ QuadricGeometry::~QuadricGeometry( void )
{
	traceList.RemoveAll( true );
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
	gaEnv.StoreVariable( "quadric", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "quadric_A", *number );
	multivector->AssignTo( quadric.A, gaEnv );
	gaEnv.LookupVariable( "quadric_B", *number );
	multivector->AssignTo( quadric.B, gaEnv );
	gaEnv.LookupVariable( "quadric_C", *number );
	multivector->AssignTo( quadric.C, gaEnv );
	gaEnv.LookupVariable( "quadric_D", *number );
	multivector->AssignTo( quadric.D, gaEnv );
	gaEnv.LookupVariable( "quadric_E", *number );
	multivector->AssignTo( quadric.E, gaEnv );
	gaEnv.LookupVariable( "quadric_F", *number );
	multivector->AssignTo( quadric.F, gaEnv );
	gaEnv.LookupVariable( "quadric_G", *number );
	multivector->AssignTo( quadric.G, gaEnv );
	gaEnv.LookupVariable( "quadric_H", *number );
	multivector->AssignTo( quadric.H, gaEnv );
	gaEnv.LookupVariable( "quadric_I", *number );
	multivector->AssignTo( quadric.I, gaEnv );
	gaEnv.LookupVariable( "quadric_J", *number );
	multivector->AssignTo( quadric.J, gaEnv );

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
		double planeCount = 20.0;
		VectorMath::Vector center, delta;
		VectorMath::Zero( center );
		VectorMath::Set( delta, 7.0, 7.0, 7.0 );
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