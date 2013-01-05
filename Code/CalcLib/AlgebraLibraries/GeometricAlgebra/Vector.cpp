// Vector.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "GeometricAlgebra.h"

using namespace GeometricAlgebra;

//=========================================================================================
Vector::Vector( void )
{
}

//=========================================================================================
/*virtual*/ Vector::~Vector( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::SortComparison Vector::SortCompare( const Utilities::List::Item* compareWithItem ) const
{
	Vector* vector = ( Vector* )compareWithItem;

	const char* thisVectorName = Name();
	const char* givenVectorName = vector->Name();

	int cmp = strcmp( thisVectorName, givenVectorName );

	if( cmp < 0 )
		return Utilities::List::SORT_COMPARE_LESS_THAN;
	else if( cmp > 0 )
		return Utilities::List::SORT_COMPARE_GREATER_THAN;

	return Utilities::List::SORT_COMPARE_EQUAL_TO;
}

//=========================================================================================
/*virtual*/ bool Vector::IsComposite( void ) const
{
	return false;
}

//=========================================================================================
Vector_e0::Vector_e0( void )
{
}

//=========================================================================================
/*virtual*/ Vector_e0::~Vector_e0( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_e0::MakeCopy( void ) const
{
	return new Vector_e0();
}

//=========================================================================================
/*virtual*/ Vector* Vector_e0::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = 1.0;
	return new Vector_e0_bar();
}

//=========================================================================================
/*virtual*/ double Vector_e0::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "e0" ) )
		return 1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_e0::Name( void ) const
{
	return "e0";
}

//=========================================================================================
/*virtual*/ const char* Vector_e0::LatexName( void ) const
{
	return "e_0";
}

//=========================================================================================
Vector_e1::Vector_e1( void )
{
}

//=========================================================================================
/*virtual*/ Vector_e1::~Vector_e1( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_e1::MakeCopy( void ) const
{
	return new Vector_e1();
}

//=========================================================================================
/*virtual*/ Vector* Vector_e1::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = 1.0;
	return new Vector_e1_bar();
}

//=========================================================================================
/*virtual*/ double Vector_e1::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "e1" ) )
		return 1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_e1::Name( void ) const
{
	return "e1";
}

//=========================================================================================
/*virtual*/ const char* Vector_e1::LatexName( void ) const
{
	return "e_1";
}

//=========================================================================================
Vector_e2::Vector_e2( void )
{
}

//=========================================================================================
/*virtual*/ Vector_e2::~Vector_e2( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_e2::MakeCopy( void ) const
{
	return new Vector_e2();
}

//=========================================================================================
/*virtual*/ Vector* Vector_e2::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = 1.0;
	return new Vector_e2_bar();
}

//=========================================================================================
/*virtual*/ double Vector_e2::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "e2" ) )
		return 1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_e2::Name( void ) const
{
	return "e2";
}

//=========================================================================================
/*virtual*/ const char* Vector_e2::LatexName( void ) const
{
	return "e_2";
}

//=========================================================================================
Vector_e3::Vector_e3( void )
{
}

//=========================================================================================
/*virtual*/ Vector_e3::~Vector_e3( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_e3::MakeCopy( void ) const
{
	return new Vector_e3();
}

//=========================================================================================
/*virtual*/ Vector* Vector_e3::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = 1.0;
	return new Vector_e3_bar();
}

//=========================================================================================
/*virtual*/ double Vector_e3::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "e3" ) )
		return 1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_e3::Name( void ) const
{
	return "e3";
}

//=========================================================================================
/*virtual*/ const char* Vector_e3::LatexName( void ) const
{
	return "e_3";
}

//=========================================================================================
Vector_no::Vector_no( void )
{
}

//=========================================================================================
/*virtual*/ Vector_no::~Vector_no( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_no::MakeCopy( void ) const
{
	return new Vector_no();
}

//=========================================================================================
/*virtual*/ Vector* Vector_no::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = 1.0;
	return new Vector_no_bar();
}

//=========================================================================================
/*virtual*/ double Vector_no::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "ni" ) )
		return -1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_no::Name( void ) const
{
	return "no";
}

//=========================================================================================
/*virtual*/ const char* Vector_no::LatexName( void ) const
{
	return "o";
}

//=========================================================================================
Vector_ni::Vector_ni( void )
{
}

//=========================================================================================
/*virtual*/ Vector_ni::~Vector_ni( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_ni::MakeCopy( void ) const
{
	return new Vector_ni();
}

//=========================================================================================
/*virtual*/ Vector* Vector_ni::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = 1.0;
	return new Vector_ni_bar();
}

//=========================================================================================
/*virtual*/ double Vector_ni::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "no" ) )
		return -1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_ni::Name( void ) const
{
	return "ni";
}

//=========================================================================================
/*virtual*/ const char* Vector_ni::LatexName( void ) const
{
	return "\\infty";
}

//=========================================================================================
Vector_e0_bar::Vector_e0_bar( void )
{
}

//=========================================================================================
/*virtual*/ Vector_e0_bar::~Vector_e0_bar( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_e0_bar::MakeCopy( void ) const
{
	return new Vector_e0_bar();
}

//=========================================================================================
/*virtual*/ Vector* Vector_e0_bar::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = 1.0;
	return new Vector_e0();
}

//=========================================================================================
/*virtual*/ double Vector_e0_bar::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "e0b" ) )
		return 1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_e0_bar::Name( void ) const
{
	return "e0b";
}

//=========================================================================================
/*virtual*/ const char* Vector_e0_bar::LatexName( void ) const
{
	return "\\overline{e}_0";
}

//=========================================================================================
Vector_e1_bar::Vector_e1_bar( void )
{
}

//=========================================================================================
/*virtual*/ Vector_e1_bar::~Vector_e1_bar( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_e1_bar::MakeCopy( void ) const
{
	return new Vector_e1_bar();
}

//=========================================================================================
/*virtual*/ Vector* Vector_e1_bar::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = 1.0;
	return new Vector_e1();
}

//=========================================================================================
/*virtual*/ double Vector_e1_bar::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "e1b" ) )
		return 1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_e1_bar::Name( void ) const
{
	return "e1b";
}

//=========================================================================================
/*virtual*/ const char* Vector_e1_bar::LatexName( void ) const
{
	return "\\overline{e}_1";
}

//=========================================================================================
Vector_e2_bar::Vector_e2_bar( void )
{
}

//=========================================================================================
/*virtual*/ Vector_e2_bar::~Vector_e2_bar( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_e2_bar::MakeCopy( void ) const
{
	return new Vector_e2_bar();
}

//=========================================================================================
/*virtual*/ Vector* Vector_e2_bar::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = 1.0;
	return new Vector_e2();
}

//=========================================================================================
/*virtual*/ double Vector_e2_bar::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "e2b" ) )
		return 1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_e2_bar::Name( void ) const
{
	return "e2b";
}

//=========================================================================================
/*virtual*/ const char* Vector_e2_bar::LatexName( void ) const
{
	return "\\overline{e}_2";
}

//=========================================================================================
Vector_e3_bar::Vector_e3_bar( void )
{
}

//=========================================================================================
/*virtual*/ Vector_e3_bar::~Vector_e3_bar( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_e3_bar::MakeCopy( void ) const
{
	return new Vector_e3_bar();
}

//=========================================================================================
/*virtual*/ Vector* Vector_e3_bar::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = 1.0;
	return new Vector_e3();
}

//=========================================================================================
/*virtual*/ double Vector_e3_bar::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "e3b" ) )
		return 1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_e3_bar::Name( void ) const
{
	return "e3b";
}

//=========================================================================================
/*virtual*/ const char* Vector_e3_bar::LatexName( void ) const
{
	return "\\overline{e}_3";
}

//=========================================================================================
Vector_no_bar::Vector_no_bar( void )
{
}

//=========================================================================================
/*virtual*/ Vector_no_bar::~Vector_no_bar( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_no_bar::MakeCopy( void ) const
{
	return new Vector_no_bar();
}

//=========================================================================================
/*virtual*/ Vector* Vector_no_bar::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = -1.0;
	return new Vector_no();
}

//=========================================================================================
/*virtual*/ double Vector_no_bar::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "nib" ) )
		return -1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_no_bar::Name( void ) const
{
	return "nob";
}

//=========================================================================================
/*virtual*/ const char* Vector_no_bar::LatexName( void ) const
{
	return "\\overline{o}";
}

//=========================================================================================
Vector_ni_bar::Vector_ni_bar( void )
{
}

//=========================================================================================
/*virtual*/ Vector_ni_bar::~Vector_ni_bar( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::Item* Vector_ni_bar::MakeCopy( void ) const
{
	return new Vector_ni_bar();
}

//=========================================================================================
/*virtual*/ Vector* Vector_ni_bar::MakeBar( ScalarAlgebra::Scalar& sign ) const
{
	sign = -1.0;
	return new Vector_ni();
}

//=========================================================================================
/*virtual*/ double Vector_ni_bar::InnerProduct( const Vector& right ) const
{
	const char* rightName = right.Name();
	if( 0 == strcmp( rightName, "nob" ) )
		return -1.0;
	return 0.0;
}

//=========================================================================================
/*virtual*/ const char* Vector_ni_bar::Name( void ) const
{
	return "nib";
}

//=========================================================================================
/*virtual*/ const char* Vector_ni_bar::LatexName( void ) const
{
	return "\\overline{\\infty}";
}

// Vector.cpp