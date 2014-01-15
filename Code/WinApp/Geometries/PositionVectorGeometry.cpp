// PositionVectorGeometry.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "PositionVectorGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( PositionVector, GAVisToolGeometry );

//=========================================================================================
PositionVector::PositionVector( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Set( position, 0.0, 0.0, 0.0 );

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"x = scalar_part( v, e1 ),"
		"y = scalar_part( v, e2 ),"
		"z = scalar_part( v, e3 ),"
		")"
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"v = x*e1 + y*e2 + z*e3"
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ PositionVector::~PositionVector( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* PositionVector::Create( BindType bindType )
{
	return new PositionVector( bindType );
}

//=========================================================================================
/*virtual*/ void PositionVector::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "v", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "x", *number );
	multivector->AssignTo( position.x, gaEnv );
	gaEnv.LookupVariable( "y", *number );
	multivector->AssignTo( position.y, gaEnv );
	gaEnv.LookupVariable( "z", *number );
	multivector->AssignTo( position.z, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void PositionVector::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	number->AssignFrom( position.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	number->AssignFrom( position.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	number->AssignFrom( position.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );

	compositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "v", *number );
	multivector->AssignTo( element, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void PositionVector::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a position vector.\n"
			"Position: < %f, %f, %f >\n",
			name,
			position.x, position.y, position.z );
}

//=========================================================================================
/*virtual*/ void PositionVector::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Position vector: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void PositionVector::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	render.DrawSphere( position, 0.25f, GAVisToolRender::RES_LOW );
}

//=========================================================================================
/*virtual*/ void PositionVector::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, position );
}

//=========================================================================================
/*virtual*/ void PositionVector::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( position, position, delta );
}

//=========================================================================================
/*virtual*/ void PositionVector::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	// Points are invariant under rotations.
}

//=========================================================================================
/*virtual*/ void PositionVector::Scale( float scale )
{
	// Points are invariant under scalings.
}

//=========================================================================================
/*virtual*/ void PositionVector::NameCenterOffset( VectorMath::Vector& offsetDelta )
{
	VectorMath::Zero( offsetDelta );
}

// PositionVectorGeometry.cpp