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

		"a_ = scalar_part( quartic, no^nob ),"
		"b_ = scalar_part( quartic, no^e1b ),"
		"c_ = scalar_part( quartic, no^e2b ),"
		"d_ = scalar_part( quartic, no^e3b ),"
		"e_ = scalar_part( quartic, no^nib ),"

		"f_ = scalar_part( quartic, e1^nob ),"
		"g_ = scalar_part( quartic, e1^e1b ),"
		"h_ = scalar_part( quartic, e1^e2b ),"
		"i_ = scalar_part( quartic, e1^e3b ),"
		"j_ = scalar_part( quartic, e1^nib ),"
		
		"k_ = scalar_part( quartic, e2^nob ),"
		"l_ = scalar_part( quartic, e2^e1b ),"
		"m_ = scalar_part( quartic, e2^e2b ),"
		"n_ = scalar_part( quartic, e2^e3b ),"
		"o_ = scalar_part( quartic, e2^nib ),"
		
		"p_ = scalar_part( quartic, e3^nob ),"
		"q_ = scalar_part( quartic, e3^e1b ),"
		"r_ = scalar_part( quartic, e3^e2b ),"
		"s_ = scalar_part( quartic, e3^e3b ),"
		"t_ = scalar_part( quartic, e3^nib ),"
		
		"u_ = scalar_part( quartic, ni^nob ),"
		"b_ = scalar_part( quartic, ni^e1b ),"
		"w_ = scalar_part( quartic, ni^e2b ),"
		"x_ = scalar_part( quartic, ni^e3b ),"
		"y_ = scalar_part( quartic, ni^nib ),"

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

	gaEnv.LookupVariable( "a_", *number );
	multivector->AssignTo( conformalQuartic.a_, gaEnv );
	gaEnv.LookupVariable( "b_", *number );
	multivector->AssignTo( conformalQuartic.b_, gaEnv );
	gaEnv.LookupVariable( "c_", *number );
	multivector->AssignTo( conformalQuartic.c_, gaEnv );
	gaEnv.LookupVariable( "d_", *number );
	multivector->AssignTo( conformalQuartic.d_, gaEnv );
	gaEnv.LookupVariable( "e_", *number );
	multivector->AssignTo( conformalQuartic.e_, gaEnv );
	gaEnv.LookupVariable( "f_", *number );
	multivector->AssignTo( conformalQuartic.f_, gaEnv );
	gaEnv.LookupVariable( "g_", *number );
	multivector->AssignTo( conformalQuartic.g_, gaEnv );
	gaEnv.LookupVariable( "h_", *number );
	multivector->AssignTo( conformalQuartic.h_, gaEnv );
	gaEnv.LookupVariable( "i_", *number );
	multivector->AssignTo( conformalQuartic.i_, gaEnv );
	gaEnv.LookupVariable( "j_", *number );
	multivector->AssignTo( conformalQuartic.j_, gaEnv );
	gaEnv.LookupVariable( "k_", *number );
	multivector->AssignTo( conformalQuartic.k_, gaEnv );
	gaEnv.LookupVariable( "l_", *number );
	multivector->AssignTo( conformalQuartic.l_, gaEnv );
	gaEnv.LookupVariable( "m_", *number );
	multivector->AssignTo( conformalQuartic.m_, gaEnv );
	gaEnv.LookupVariable( "n_", *number );
	multivector->AssignTo( conformalQuartic.n_, gaEnv );
	gaEnv.LookupVariable( "o_", *number );
	multivector->AssignTo( conformalQuartic.o_, gaEnv );
	gaEnv.LookupVariable( "p_", *number );
	multivector->AssignTo( conformalQuartic.p_, gaEnv );
	gaEnv.LookupVariable( "q_", *number );
	multivector->AssignTo( conformalQuartic.q_, gaEnv );
	gaEnv.LookupVariable( "r_", *number );
	multivector->AssignTo( conformalQuartic.r_, gaEnv );
	gaEnv.LookupVariable( "s_", *number );
	multivector->AssignTo( conformalQuartic.s_, gaEnv );
	gaEnv.LookupVariable( "t_", *number );
	multivector->AssignTo( conformalQuartic.t_, gaEnv );
	gaEnv.LookupVariable( "u_", *number );
	multivector->AssignTo( conformalQuartic.u_, gaEnv );
	gaEnv.LookupVariable( "v_", *number );
	multivector->AssignTo( conformalQuartic.v_, gaEnv );
	gaEnv.LookupVariable( "w_", *number );
	multivector->AssignTo( conformalQuartic.w_, gaEnv );
	gaEnv.LookupVariable( "x_", *number );
	multivector->AssignTo( conformalQuartic.x_, gaEnv );
	gaEnv.LookupVariable( "y_", *number );
	multivector->AssignTo( conformalQuartic.y_, gaEnv );

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