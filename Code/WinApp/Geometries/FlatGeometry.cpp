// FlatGeometry.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "FlatGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ConformalFlatPoint, GAVisToolGeometry );

//=========================================================================================
ConformalFlatPoint::ConformalFlatPoint( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Zero( center );
	weight = 1.0;

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"%s"
		"w = -( fp^ni )*i,"
		"fp = fp / w,"
		"v = ( no . fp )*i,"
		"x = scalar_part( v, e1 ),"
		"y = scalar_part( v, e2 ),"
		"z = scalar_part( v, e3 )"
		")",
		( bindType == NORMAL_FORM ? "fp = fp*I," : "" )
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"do("
		"v = x*e1 + y*e2 + z*e3,"
		"fp = w*( i + ( v*i )^ni )%s"
		")",
		( bindType == NORMAL_FORM ? "*-I" : "" )
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ConformalFlatPoint::~ConformalFlatPoint( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ConformalFlatPoint::Create( BindType bindType )
{
	return new ConformalFlatPoint( bindType );
}

//=========================================================================================
/*virtual*/ void ConformalFlatPoint::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "fp", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "w", *number );
	multivector->AssignTo( weight, gaEnv );
	gaEnv.LookupVariable( "x", *number );
	multivector->AssignTo( center.x, gaEnv );
	gaEnv.LookupVariable( "y", *number );
	multivector->AssignTo( center.y, gaEnv );
	gaEnv.LookupVariable( "z", *number );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalFlatPoint::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	number->AssignFrom( weight, gaEnv );
	gaEnv.StoreVariable( "w", *number );
	number->AssignFrom( center.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	number->AssignFrom( center.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	number->AssignFrom( center.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );

	compositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "fp", *number );
	multivector->AssignTo( element, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalFlatPoint::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a conformal %s.\n"
			"Weight: %f\n"
			"Position: < %f, %f, %f >\n",
			name,
			( bindType == NORMAL_FORM ? "dual flat-point" : "flat-point" ),
			weight,
			center.x, center.y, center.z );
}

//=========================================================================================
/*virtual*/ void ConformalFlatPoint::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Flat-point: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void ConformalFlatPoint::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	// TODO: Draw this as a small box instead so as to differentiate it from round points.
	render.DrawSphere( center, 0.25f, GAVisToolRender::RES_LOW );
}

//=========================================================================================
/*virtual*/ void ConformalFlatPoint::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, this->center );
}

//=========================================================================================
/*virtual*/ void ConformalFlatPoint::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( center, center, delta );
}

//=========================================================================================
/*virtual*/ void ConformalFlatPoint::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	// Flat points don't rotate.
}

//=========================================================================================
/*virtual*/ void ConformalFlatPoint::Scale( float scale )
{
	// Flat points don't scale.
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ConformalLine, GAVisToolGeometry );

//=========================================================================================
ConformalLine::ConformalLine( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Zero( center );
	VectorMath::Set( unitNormal, 0.0, 0.0, 1.0 );
	weight = 1.0;

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"%s"
		"n = grade_part( 1, ( no . ( lin^ni ) )*i ),"
		"w = sqrt( n . n ),"
		"n = n / w,"
		"lin = lin / w,"
		"v = grade_part( 1, -n*( no . lin )*i ),"
		"x = scalar_part( v, e1 ),"
		"y = scalar_part( v, e2 ),"
		"z = scalar_part( v, e3 ),"
		"nx = scalar_part( n, e1 ),"
		"ny = scalar_part( n, e2 ),"
		"nz = scalar_part( n, e3 )"
		")",
		( bindType == NORMAL_FORM ? "lin = lin*I," : "" )
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"do("
		"v = x*e1 + y*e2 + z*e3,"
		"n = nx*e1 + ny*e2 + nz*e3,"
		"lin = w*( n*i - ( v . ( n*i ) )^ni )%s"
		")",
		( bindType == NORMAL_FORM ? "*-I" : "" )
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ConformalLine::~ConformalLine( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ConformalLine::Create( BindType bindType )
{
	return new ConformalLine( bindType );
}

//=========================================================================================
/*virtual*/ void ConformalLine::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "lin", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "w", *number );
	multivector->AssignTo( weight, gaEnv );
	gaEnv.LookupVariable( "x", *number );
	multivector->AssignTo( center.x, gaEnv );
	gaEnv.LookupVariable( "y", *number );
	multivector->AssignTo( center.y, gaEnv );
	gaEnv.LookupVariable( "z", *number );
	multivector->AssignTo( center.z, gaEnv );
	gaEnv.LookupVariable( "nx", *number );
	multivector->AssignTo( unitNormal.x, gaEnv );
	gaEnv.LookupVariable( "ny", *number );
	multivector->AssignTo( unitNormal.y, gaEnv );
	gaEnv.LookupVariable( "nz", *number );
	multivector->AssignTo( unitNormal.z, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalLine::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	number->AssignFrom( weight, gaEnv );
	gaEnv.StoreVariable( "w", *number );
	number->AssignFrom( center.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	number->AssignFrom( center.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	number->AssignFrom( center.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );
	number->AssignFrom( unitNormal.x, gaEnv );
	gaEnv.StoreVariable( "nx", *number );
	number->AssignFrom( unitNormal.y, gaEnv );
	gaEnv.StoreVariable( "ny", *number );
	number->AssignFrom( unitNormal.z, gaEnv );
	gaEnv.StoreVariable( "nz", *number );

	compositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "lin", *number );
	multivector->AssignTo( element, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalLine::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a conformal %s.\n"
			"Weight: %f\n"
			"Position: < %f, %f, %f >\n"
			"Normal: < %f, %f, %f >\n",
			name,
			( bindType == NORMAL_FORM ? "dual line" : "line" ),
			weight,
			center.x, center.y, center.z,
			unitNormal.x, unitNormal.y, unitNormal.z );
}

//=========================================================================================
/*virtual*/ void ConformalLine::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Line: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void ConformalLine::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	VectorMath::Vector point0, point1;
	VectorMath::Vector vec;
	VectorMath::Scale( vec, unitNormal, -20.0 );
	VectorMath::Add( point0, center, vec );
	VectorMath::Scale( vec, vec, -1.0 );
	VectorMath::Add( point1, center, vec );

	render.DrawTube( point0, point1, 0.1f, GAVisToolRender::RES_LOW );

	if( render.GetRenderMode() != GAVisToolRender::RENDER_MODE_SELECTION )
	{
		// TODO: This is terrible.  It creates so many BSP cuts that it blows the tri-heap.  Fix it.
		/*
		int vecCount = 10;
		double dist = VectorMath::Distance( point0, point1 );
		double vecLen = dist / double( vecCount );
		VectorMath::Scale( vec, unitNormal, vecLen );
		for( int count = 0; count < vecCount; count++ )
		{
			render.DrawVector( point0, vec );
			VectorMath::Add( point0, point0, vec );
		}
		*/
	}
}

//=========================================================================================
/*virtual*/ void ConformalLine::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, this->center );
}

//=========================================================================================
/*virtual*/ void ConformalLine::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( center, center, delta );
}

//=========================================================================================
/*virtual*/ void ConformalLine::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	VectorMath::Rotate( unitNormal, unitNormal, unitAxis, angle );
	VectorMath::Normalize( unitNormal, unitNormal );		// Fix round-off error.
}

//=========================================================================================
/*virtual*/ void ConformalLine::Scale( float scale )
{
	// Lines don't scale.
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ConformalPlane, GAVisToolGeometry );

//=========================================================================================
ConformalPlane::ConformalPlane( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Zero( center );
	VectorMath::Set( unitNormal, 0.0, 0.0, 1.0 );
	weight = 1.0;

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"%s"
		"n = no . ( pln ^ ni ),"
		"w = sqrt( n . n ),"
		"n = n / w,"
		"pln = pln / w,"
		"v = -n*( no . pln ),"
		"x = scalar_part( v, e1 ),"
		"y = scalar_part( v, e2 ),"
		"z = scalar_part( v, e3 ),"
		"nx = scalar_part( n, e1 ),"
		"ny = scalar_part( n, e2 ),"
		"nz = scalar_part( n, e3 )"
		")",
		( bindType == NORMAL_FORM ? "pln = pln*I," : "" )
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"do("
		"v = x*e1 + y*e2 + z*e3,"
		"n = nx*e1 + ny*e2 + nz*e3,"
		"pln = w*( n + ( v . n )*ni )%s"
		")",
		( bindType == NORMAL_FORM ? "*-I" : "" )
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ConformalPlane::~ConformalPlane( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ConformalPlane::Create( BindType bindType )
{
	return new ConformalPlane( bindType );
}

//=========================================================================================
/*virtual*/ void ConformalPlane::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "pln", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "w", *number );
	multivector->AssignTo( weight, gaEnv );
	gaEnv.LookupVariable( "x", *number );
	multivector->AssignTo( center.x, gaEnv );
	gaEnv.LookupVariable( "y", *number );
	multivector->AssignTo( center.y, gaEnv );
	gaEnv.LookupVariable( "z", *number );
	multivector->AssignTo( center.z, gaEnv );
	gaEnv.LookupVariable( "nx", *number );
	multivector->AssignTo( unitNormal.x, gaEnv );
	gaEnv.LookupVariable( "ny", *number );
	multivector->AssignTo( unitNormal.y, gaEnv );
	gaEnv.LookupVariable( "nz", *number );
	multivector->AssignTo( unitNormal.z, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalPlane::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	number->AssignFrom( weight, gaEnv );
	gaEnv.StoreVariable( "w", *number );
	number->AssignFrom( center.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	number->AssignFrom( center.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	number->AssignFrom( center.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );
	number->AssignFrom( unitNormal.x, gaEnv );
	gaEnv.StoreVariable( "nx", *number );
	number->AssignFrom( unitNormal.y, gaEnv );
	gaEnv.StoreVariable( "ny", *number );
	number->AssignFrom( unitNormal.z, gaEnv );
	gaEnv.StoreVariable( "nz", *number );

	compositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "pln", *number );
	multivector->AssignTo( element, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalPlane::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a conformal %s.\n"
			"Weight: %f\n"
			"Position: < %f, %f, %f >\n"
			"Normal: < %f, %f, %f >\n",
			name,
			( bindType == NORMAL_FORM ? "dual plane" : "plane" ),
			weight,
			center.x, center.y, center.z,
			unitNormal.x, unitNormal.y, unitNormal.z );
}

//=========================================================================================
/*virtual*/ void ConformalPlane::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Plane: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void ConformalPlane::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	render.DrawDisk( center, unitNormal, 15.0 );

	if( render.GetRenderMode() != GAVisToolRender::RENDER_MODE_SELECTION )
		render.DrawVector( center, unitNormal, GAVisToolRender::RES_LOW );
}

//=========================================================================================
/*virtual*/ void ConformalPlane::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, this->center );
}

//=========================================================================================
/*virtual*/ void ConformalPlane::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( center, center, delta );
}

//=========================================================================================
/*virtual*/ void ConformalPlane::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	VectorMath::Rotate( unitNormal, unitNormal, unitAxis, angle );
	VectorMath::Normalize( unitNormal, unitNormal );		// Fix round-off error.
}

//=========================================================================================
/*virtual*/ void ConformalPlane::Scale( float scale )
{
	// Planes don't scale.
}

// FlatGeometry.cpp