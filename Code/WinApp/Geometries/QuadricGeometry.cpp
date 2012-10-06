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

	// The 1/2 factors here are needed by the internal low-level quadric representation.
	char decompositionCode[ 512 ];
	strcpy_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"a00 = scalar_part( B, e0^e4 ),"
		"a01 = 0.5*( scalar_part( B, e0^e5 ) - scalar_part( B, e4^e1 ) ),"
		"a02 = 0.5*( scalar_part( B, e0^e6 ) - scalar_part( B, e4^e2 ) ),"
		"a03 = 0.5*( scalar_part( B, e0^e7 ) - scalar_part( B, e4^e3 ) ),"
		"a11 = scalar_part( B, e1^e5 ),"
		"a12 = 0.5*( scalar_part( B, e1^e6 ) - scalar_part( B, e5^e2 ) ),"
		"a13 = 0.5*( scalar_part( B, e1^e7 ) - scalar_part( B, e5^e3 ) ),"
		"a22 = scalar_part( B, e2^e6 ),"
		"a23 = 0.5*( scalar_part( B, e2^e7 ) - scalar_part( B, e6^e3 ) ),"
		"a33 = scalar_part( B, e3^e7 ),"
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

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( QuadricGeometryPoint, GAVisToolGeometry );

//=========================================================================================
QuadricGeometryPoint::QuadricGeometryPoint( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Zero( point );
	weight = 1.0;

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	decompositionCode[0] = '\0';
	if( bindType == NORMAL_FORM )
	{
		sprintf_s( decompositionCode, sizeof( decompositionCode ),
			"do("
			"w = pt . e0,"
			"pt = pt / w,"
			"x = scalar_part( pt, e1 ),"
			"y = scalar_part( pt, e2 ),"
			"z = scalar_part( pt, e3 ),"
			")"
		);
	}
	else if( bindType == ROTATED_FORM )
	{
		sprintf_s( decompositionCode, sizeof( decompositionCode ),
			"do("
			"w = pt . e4,"
			"pt = pt / w,"
			"x = scalar_part( pt, e5 ),"
			"y = scalar_part( pt, e6 ),"
			"z = scalar_part( pt, e7 ),"
			")"
		);
	}
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	compositionCode[0] = '\0';
	if( bindType == NORMAL_FORM )
	{
		sprintf_s( compositionCode, sizeof( compositionCode ),
			"do("
			"c = x*e1 + y*e2 + z*e3,"
			"pt = w*(e0 + c),"
			")"
		);
	}
	else if( bindType == ROTATED_FORM )
	{
		sprintf_s( compositionCode, sizeof( compositionCode ),
			"do("
			"c = x*e5 + y*e6 + z*e7,"
			"pt = w*(e4 + c),"
			")"
		);
	}
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ QuadricGeometryPoint::~QuadricGeometryPoint( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* QuadricGeometryPoint::Create( BindType bindType )
{
	return new QuadricGeometryPoint( bindType );
}

//=========================================================================================
/*virtual*/ void QuadricGeometryPoint::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;
	
	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = number->Cast< CalcLib::MultivectorNumber >();

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "pt", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );
	
	gaEnv.LookupVariable( "w", *number );
	multivector->AssignTo( weight, gaEnv );
	gaEnv.LookupVariable( "x", *number );
	multivector->AssignTo( point.x, gaEnv );
	gaEnv.LookupVariable( "y", *number );
	multivector->AssignTo( point.y, gaEnv );
	gaEnv.LookupVariable( "z", *number );
	multivector->AssignTo( point.z, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void QuadricGeometryPoint::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = number->Cast< CalcLib::MultivectorNumber >();

	number->AssignFrom( weight, gaEnv );
	gaEnv.StoreVariable( "w", *number );
	number->AssignFrom( point.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	number->AssignFrom( point.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	number->AssignFrom( point.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );

	compositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "pt", *number );
	multivector->AssignTo( element, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void QuadricGeometryPoint::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a projective point for the quadric model.\n"
			"Weight: %f\n"
			"Position: < %f, %f, %f >\n",
			name,
			weight,
			point.x, point.y, point.z );
}

//=========================================================================================
/*virtual*/ void QuadricGeometryPoint::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Quad-Proj-Point: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void QuadricGeometryPoint::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	// Points are small spheres so they should always be drawn at low resolution despite user settings.
	render.DrawSphere( point, 0.25f, GAVisToolRender::RES_LOW );
}

//=========================================================================================
/*virtual*/ void QuadricGeometryPoint::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, point );
}

//=========================================================================================
/*virtual*/ void QuadricGeometryPoint::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( point, point, delta );
}

//=========================================================================================
/*virtual*/ void QuadricGeometryPoint::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	// Projective points don't rotate.
}

//=========================================================================================
/*virtual*/ void QuadricGeometryPoint::Scale( float scale )
{
	// Projective points don't scale.
}

//=========================================================================================
/*virtual*/ void QuadricGeometryPoint::NameCenterOffset( VectorMath::Vector& offsetDelta )
{
	VectorMath::Zero( offsetDelta );
}

// QuadricGeoemtry.cpp