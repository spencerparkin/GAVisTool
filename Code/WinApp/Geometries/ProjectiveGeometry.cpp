// ProjectiveGeometry.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "ProjectiveGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ProjectivePoint, GAVisToolGeometry );

//=========================================================================================
ProjectivePoint::ProjectivePoint( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Zero( center );
	weight = 1.0;

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"w = pt . e0,"
		"pt = pt / w,"
		"x = scalar_part( pt, e1 ),"
		"y = scalar_part( pt, e2 ),"
		"z = scalar_part( pt, e3 ),"
		")"
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"do("
		"c = x*e1 + y*e2 + z*e3,"
		"pt = w*(c + e0),"
		")"
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ProjectivePoint::~ProjectivePoint( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ProjectivePoint::Create( BindType bindType )
{
	return new ProjectivePoint( bindType );
}

//=========================================================================================
/*virtual*/ void ProjectivePoint::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
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
	multivector->AssignTo( center.x, gaEnv );
	gaEnv.LookupVariable( "y", *number );
	multivector->AssignTo( center.y, gaEnv );
	gaEnv.LookupVariable( "z", *number );
	multivector->AssignTo( center.z, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ProjectivePoint::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = number->Cast< CalcLib::MultivectorNumber >();

	number->AssignFrom( weight, gaEnv );
	gaEnv.StoreVariable( "w", *number );
	number->AssignFrom( center.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	number->AssignFrom( center.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	number->AssignFrom( center.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );

	compositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "pt", *number );
	multivector->AssignTo( element, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ProjectivePoint::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a projective point.\n"
			"Weight: %f\n"
			"Position: < %f, %f, %f >\n",
			name,
			weight,
			center.x, center.y, center.z );
}

//=========================================================================================
/*virtual*/ void ProjectivePoint::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Proj-Point: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void ProjectivePoint::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	// Points are small spheres so they should always be drawn at low resolution despite user settings.
	render.DrawSphere( center, 0.25f, GAVisToolRender::RES_LOW );
}

//=========================================================================================
/*virtual*/ void ProjectivePoint::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, this->center );
}

//=========================================================================================
/*virtual*/ void ProjectivePoint::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( center, center, delta );
}

//=========================================================================================
/*virtual*/ void ProjectivePoint::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	// Projective points don't rotate.
}

//=========================================================================================
/*virtual*/ void ProjectivePoint::Scale( float scale )
{
	// Projective points don't scale.
}

//=========================================================================================
/*virtual*/ void ProjectivePoint::NameCenterOffset( VectorMath::Vector& offsetDelta )
{
	VectorMath::Zero( offsetDelta );
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ProjectiveLine, GAVisToolGeometry );

//=========================================================================================
ProjectiveLine::ProjectiveLine( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Zero( center );
	VectorMath::Set( unitNormal, 0.0, 0.0, 1.0 );
	weight = 1.0;

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"n = e0 . lin,"
		"w = sqrt( n . n ),"
		"n = n / w,"
		"lin = lin / w,"
		"c = ( e0 . ( e0^lin ) )*n,"
		"x = scalar_part( c, e1 ),"
		"y = scalar_part( c, e2 ),"
		"z = scalar_part( c, e3 ),"
		"nx = scalar_part( n, e1 ),"
		"ny = scalar_part( n, e2 ),"
		"nz = scalar_part( n, e3 ),"
		")"
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"do("
		"c = x*e1 + y*e2 + z*e3,"
		"n = nx*e1 + ny*e2 + nz*e3,"
		"lin = w*(c + e0)^n,"
		")"
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ProjectiveLine::~ProjectiveLine( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ProjectiveLine::Create( BindType bindType )
{
	return new ProjectiveLine( bindType );
}

//=========================================================================================
/*virtual*/ void ProjectiveLine::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
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
/*virtual*/ void ProjectiveLine::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
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
/*virtual*/ void ProjectiveLine::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a projective line.\n"
			"Weight: %f\n"
			"Position: < %f, %f, %f >\n"
			"Normal: < %f, %f, %f >\n",
			name,
			weight,
			center.x, center.y, center.z,
			unitNormal.x, unitNormal.y, unitNormal.z );
}

//=========================================================================================
/*virtual*/ void ProjectiveLine::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Proj-Line: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void ProjectiveLine::Draw( GAVisToolRender& render, bool selected )
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
}

//=========================================================================================
/*virtual*/ void ProjectiveLine::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, this->center );
}

//=========================================================================================
/*virtual*/ void ProjectiveLine::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( center, center, delta );
}

//=========================================================================================
/*virtual*/ void ProjectiveLine::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	VectorMath::Rotate( unitNormal, unitNormal, unitAxis, angle );
}

//=========================================================================================
/*virtual*/ void ProjectiveLine::Scale( float scale )
{
	// Projective lines don't scale.
}

//=========================================================================================
/*virtual*/ void ProjectiveLine::NameCenterOffset( VectorMath::Vector& offsetDelta )
{
	VectorMath::Zero( offsetDelta );
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ProjectivePlane, GAVisToolGeometry );

//=========================================================================================
ProjectivePlane::ProjectivePlane( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Zero( center );
	VectorMath::Set( unitNormal, 0.0, 0.0, 1.0 );
	weight = 1.0;

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"n = ( e0 . pln )*( e1^e2^e3 ),"
		"w = sqrt( n . n ),"
		"n = n / w,"
		"pln = pln / w,"
		"c = ( e0 . ( e0^pln ) )*( e1^e2^e3 )*n,"
		"x = scalar_part( c, e1 ),"
		"y = scalar_part( c, e2 ),"
		"z = scalar_part( c, e3 ),"
		"nx = scalar_part( n, e1 ),"
		"ny = scalar_part( n, e2 ),"
		"nz = scalar_part( n, e3 ),"
		")"
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"do("
		"c = x*e1 + y*e2 + z*e3,"
		"n = nx*e1 + ny*e2 + nz*e3,"
		"pln = w*(c + e0)^( -n*( e1^e2^e3 ) ),"
		")"
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ProjectivePlane::~ProjectivePlane( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ProjectivePlane::Create( BindType bindType )
{
	return new ProjectivePlane( bindType );
}

//=========================================================================================
/*virtual*/ void ProjectivePlane::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
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
/*virtual*/ void ProjectivePlane::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
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
/*virtual*/ void ProjectivePlane::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a projective plane.\n"
			"Weight: %f\n"
			"Position: < %f, %f, %f >\n"
			"Normal: < %f, %f, %f >\n",
			name,
			weight,
			center.x, center.y, center.z,
			unitNormal.x, unitNormal.y, unitNormal.z );
}

//=========================================================================================
/*virtual*/ void ProjectivePlane::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Proj-Plane: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void ProjectivePlane::Draw( GAVisToolRender& render, bool selected )
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
/*virtual*/ void ProjectivePlane::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, this->center );
}

//=========================================================================================
/*virtual*/ void ProjectivePlane::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( center, center, delta );
}

//=========================================================================================
/*virtual*/ void ProjectivePlane::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	VectorMath::Rotate( unitNormal, unitNormal, unitAxis, angle );
}

//=========================================================================================
/*virtual*/ void ProjectivePlane::Scale( float scale )
{
	// Projective planes don't scale.
}

//=========================================================================================
/*virtual*/ void ProjectivePlane::NameCenterOffset( VectorMath::Vector& offsetDelta )
{
	VectorMath::Zero( offsetDelta );
}

// ProjectiveGeometry.cpp