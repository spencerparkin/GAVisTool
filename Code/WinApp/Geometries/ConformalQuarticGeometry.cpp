// ConformalQuarticGeometry.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "ConformalQuarticGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ConformalQuarticGeometry, SurfaceGeometry );

//=========================================================================================
ConformalQuarticGeometry::ConformalQuarticGeometry( BindType bindType ) : SurfaceGeometry( bindType, &conformalQuartic )
{
	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 2*1024 ];
	strcpy_s( decompositionCode, sizeof( decompositionCode ),
		"do("

		"c0 = scalar_part( quartic, no^nob ),"
		"c1 = scalar_part( quartic, no^e1b ),"
		"c2 = scalar_part( quartic, no^e2b ),"
		"c3 = scalar_part( quartic, no^e3b ),"
		"c4 = scalar_part( quartic, no^nib ),"

		"c5 = scalar_part( quartic, e1^nob ),"
		"c6 = scalar_part( quartic, e1^e1b ),"
		"c7 = scalar_part( quartic, e1^e2b ),"
		"c8 = scalar_part( quartic, e1^e3b ),"
		"c9 = scalar_part( quartic, e1^nib ),"
		
		"c10 = scalar_part( quartic, e2^nob ),"
		"c11 = scalar_part( quartic, e2^e1b ),"
		"c12 = scalar_part( quartic, e2^e2b ),"
		"c13 = scalar_part( quartic, e2^e3b ),"
		"c14 = scalar_part( quartic, e2^nib ),"
		
		"c15 = scalar_part( quartic, e3^nob ),"
		"c16 = scalar_part( quartic, e3^e1b ),"
		"c17 = scalar_part( quartic, e3^e2b ),"
		"c18 = scalar_part( quartic, e3^e3b ),"
		"c19 = scalar_part( quartic, e3^nib ),"
		
		"c20 = scalar_part( quartic, ni^nob ),"
		"c21 = scalar_part( quartic, ni^e1b ),"
		"c22 = scalar_part( quartic, ni^e2b ),"
		"c23 = scalar_part( quartic, ni^e3b ),"
		"c24 = scalar_part( quartic, ni^nib ),"

		")"
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ConformalQuarticGeometry::~ConformalQuarticGeometry( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ConformalQuarticGeometry::Create( BindType bindType )
{
	return new ConformalQuarticGeometry( bindType );
}

//=========================================================================================
/*virtual*/ void ConformalQuarticGeometry::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	SurfaceGeometry::DecomposeFrom( element );

	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "quartic", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "c0", *number );
	multivector->AssignTo( conformalQuartic.c0, gaEnv );
	gaEnv.LookupVariable( "c1", *number );
	multivector->AssignTo( conformalQuartic.c1, gaEnv );
	gaEnv.LookupVariable( "c2", *number );
	multivector->AssignTo( conformalQuartic.c2, gaEnv );
	gaEnv.LookupVariable( "c3", *number );
	multivector->AssignTo( conformalQuartic.c3, gaEnv );
	gaEnv.LookupVariable( "c4", *number );
	multivector->AssignTo( conformalQuartic.c4, gaEnv );
	gaEnv.LookupVariable( "c5", *number );
	multivector->AssignTo( conformalQuartic.c5, gaEnv );
	gaEnv.LookupVariable( "c6", *number );
	multivector->AssignTo( conformalQuartic.c6, gaEnv );
	gaEnv.LookupVariable( "c7", *number );
	multivector->AssignTo( conformalQuartic.c7, gaEnv );
	gaEnv.LookupVariable( "c8", *number );
	multivector->AssignTo( conformalQuartic.c8, gaEnv );
	gaEnv.LookupVariable( "c9", *number );
	multivector->AssignTo( conformalQuartic.c9, gaEnv );
	gaEnv.LookupVariable( "c10", *number );
	multivector->AssignTo( conformalQuartic.c10, gaEnv );
	gaEnv.LookupVariable( "c11", *number );
	multivector->AssignTo( conformalQuartic.c11, gaEnv );
	gaEnv.LookupVariable( "c12", *number );
	multivector->AssignTo( conformalQuartic.c12, gaEnv );
	gaEnv.LookupVariable( "c13", *number );
	multivector->AssignTo( conformalQuartic.c13, gaEnv );
	gaEnv.LookupVariable( "c14", *number );
	multivector->AssignTo( conformalQuartic.c14, gaEnv );
	gaEnv.LookupVariable( "c15", *number );
	multivector->AssignTo( conformalQuartic.c15, gaEnv );
	gaEnv.LookupVariable( "c16", *number );
	multivector->AssignTo( conformalQuartic.c16, gaEnv );
	gaEnv.LookupVariable( "c17", *number );
	multivector->AssignTo( conformalQuartic.c17, gaEnv );
	gaEnv.LookupVariable( "c18", *number );
	multivector->AssignTo( conformalQuartic.c18, gaEnv );
	gaEnv.LookupVariable( "c19", *number );
	multivector->AssignTo( conformalQuartic.c19, gaEnv );
	gaEnv.LookupVariable( "c20", *number );
	multivector->AssignTo( conformalQuartic.c20, gaEnv );
	gaEnv.LookupVariable( "c21", *number );
	multivector->AssignTo( conformalQuartic.c21, gaEnv );
	gaEnv.LookupVariable( "c22", *number );
	multivector->AssignTo( conformalQuartic.c22, gaEnv );
	gaEnv.LookupVariable( "c23", *number );
	multivector->AssignTo( conformalQuartic.c23, gaEnv );
	gaEnv.LookupVariable( "c24", *number );
	multivector->AssignTo( conformalQuartic.c24, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalQuarticGeometry::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize, "The variable \"%s\" is being interpreted as a conformal quartic.\n", name );
}

//=========================================================================================
/*virtual*/ void ConformalQuarticGeometry::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Quartic: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

// ConformalQuarticGeometry.cpp