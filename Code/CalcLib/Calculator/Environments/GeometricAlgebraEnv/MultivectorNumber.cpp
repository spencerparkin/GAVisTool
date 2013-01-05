// MultivectorNumber.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "../../CalcLib.h"

using namespace CalcLib;

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( MultivectorNumber, Number );

//=========================================================================================
MultivectorNumber::MultivectorNumber( void )
{
	multivector.AssignZero();
}

//=========================================================================================
MultivectorNumber::MultivectorNumber( const GeometricAlgebra::Vector& vector )
{
	multivector.AssignVector( vector );
}

//=========================================================================================
MultivectorNumber::MultivectorNumber( const GeometricAlgebra::Blade& blade )
{
	this->multivector.AssignBlade( blade );
}

//=========================================================================================
MultivectorNumber::MultivectorNumber( const GeometricAlgebra::SumOfBlades& multivector )
{
	this->multivector.AssignSumOfBlades( multivector );
}

//=========================================================================================
/*virtual*/ MultivectorNumber::~MultivectorNumber( void )
{
}

//=========================================================================================
/*virtual*/ Number* MultivectorNumber::CreateCopy( Environment& environment ) const
{
	return new MultivectorNumber( multivector );
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::Print( char* buffer, int bufferSize, bool printLatex, Environment& environment ) const
{
	ScalarAlgebra::PrintPurpose printPurpose = ScalarAlgebra::PRINT_FOR_READING;
	if( printLatex )
		printPurpose = ScalarAlgebra::PRINT_FOR_LATEX;

	if( !environment.IsTypeOf( GeometricAlgebraEnvironment::ClassName() ) )
	{
		environment.AddError( "Multivector numbers can't be printed in a non-geometric-algebra environment." );
		return false;
	}

	GeometricAlgebraEnvironment* geometricAlgebraEnvironment = ( GeometricAlgebraEnvironment* )&environment;

	if( geometricAlgebraEnvironment->displayMode == GeometricAlgebraEnvironment::DISPLAY_AS_SUM_OF_BLADES )
	{
		if( !multivector.Print( buffer, bufferSize, printPurpose ) )
		{
			environment.AddError( "Failed to print multivector!" );
			return false;
		}
	}
	else if( geometricAlgebraEnvironment->displayMode == GeometricAlgebraEnvironment::DISPLAY_AS_SUM_OF_VERSORS )
	{
		GeometricAlgebra::SumOfPseudoVersors sumOfPseudoVersors;
		if( !sumOfPseudoVersors.AssignSumOfBlades( multivector ) )
			return false;

		if( !sumOfPseudoVersors.Print( buffer, bufferSize, printPurpose ) )
		{
			environment.AddError( "Failed to print sum of versors!" );
			return false;
		}
	}
	else
	{
		environment.AddError( "Multivector number display mode is unknown." );
		return false;
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignFrom( const char* numberString, Environment& environment )
{
	if( *numberString == '$' )
	{
		const char* variableName = numberString + 1;
		if( !multivector.AssignScalar( variableName ) )
		{
			environment.AddError( "Failed to assign scalar \"%s\" to multivector.", variableName );
			return false;
		}
	}
	else
	{
		double scalar = atof( numberString );
		if( !multivector.AssignScalar( scalar ) )
		{
			environment.AddError( "Failed to assign scalar %f to multivector.", scalar );
			return false;
		}
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignFrom( const Number* number, Environment& environment )
{
	if( !number->IsTypeOf( MultivectorNumber::ClassName() ) )
	{
		environment.AddError( "Cannot assign anything but a multivector-type number to such a number." );
		return false;
	}

	MultivectorNumber* multivectorNumber = ( MultivectorNumber* )number;
	if( !multivector.AssignSumOfBlades( multivectorNumber->multivector ) )
	{
		environment.AddError( "Failed to assign one multivector to another!" );
		return false;
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignFrom( double number, Environment& environment )
{
	if( !multivector.AssignScalar( number ) )
	{
		environment.AddError( "Failed to assign scalar %f to multivector.", number );
		return false;
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignFrom( const GeometricAlgebra::Scalar& scalar, Environment& environment )
{
	return multivector.AssignScalar( scalar );
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignFrom( const GeometricAlgebra::SumOfBlades& sumOfBlades, Environment& environment )
{
	if( !multivector.AssignSumOfBlades( sumOfBlades ) )
	{
		environment.AddError( "Failed to assign sum of blades to multivector number." );
		return false;
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignTo( double& number, Environment& environment ) const
{
	if( !multivector.IsHomogeneousOfGrade(0) )
		return false;

	number = 0.0;
	if( multivector.BladeCount() == 0 )
		return true;

	GeometricAlgebra::Blade blade;
	if( !multivector.AssignBladeTo( blade, 0 ) )
		return false;

	GeometricAlgebra::Scalar scalar;
	if( !blade.AssignScalarPartTo( scalar ) )
		return false;

	// This could fail internally.
	number = scalar;
	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignTo( GeometricAlgebra::Blade& blade, Environment& environment ) const
{
	if( multivector.BladeCount() != 1 )
		return false;

	if( !multivector.AssignBladeTo( blade, 0 ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignTo( GeometricAlgebra::SumOfBlades& sumOfBlades, Environment& environment ) const
{
	if( !sumOfBlades.AssignSumOfBlades( multivector ) )
		return false;

	return true;
}

//=========================================================================================
bool MultivectorNumber::CopyOperandsIfNeeded( const Number* left, const Number* right,
												const MultivectorNumber*& leftMultivectorNumber,
												const MultivectorNumber*& rightMultivectorNumber,
												Environment& environment ) const
{
	if( !left->IsTypeOf( MultivectorNumber::ClassName() ) )
		return false;
	if( !right->IsTypeOf( MultivectorNumber::ClassName() ) )
		return false;

	static MultivectorNumber leftCopy;
	static MultivectorNumber rightCopy;

	if( left != this )
		leftMultivectorNumber = ( const MultivectorNumber* )left;
	else
	{
		if( !leftCopy.AssignFrom( left, environment ) )
			return false;
		leftMultivectorNumber = &leftCopy;
	}

	if( right != this )
		rightMultivectorNumber = ( const MultivectorNumber* )right;
	else
	{
		if( !rightCopy.AssignFrom( right, environment ) )
			return false;
		rightMultivectorNumber = &rightCopy;
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignSum( const Number* left, const Number* right, Environment& environment )
{
	const MultivectorNumber* leftMultivector = 0;
	const MultivectorNumber* rightMultivector = 0;
	if( !CopyOperandsIfNeeded( left, right, leftMultivector, rightMultivector, environment ) )
		return false;

	if( !multivector.AssignSum( leftMultivector->multivector, rightMultivector->multivector ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignDifference( const Number* left, const Number* right, Environment& environment )
{
	const MultivectorNumber* leftMultivector = 0;
	const MultivectorNumber* rightMultivector = 0;
	if( !CopyOperandsIfNeeded( left, right, leftMultivector, rightMultivector, environment ) )
		return false;

	MultivectorNumber rightAdditiveInverse;
	if( !rightAdditiveInverse.AssignAdditiveInverse( rightMultivector, environment ) )
		return false;

	if( !multivector.AssignSum( leftMultivector->multivector, rightAdditiveInverse.multivector ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignProduct( const Number* left, const Number* right, Environment& environment )
{
	const MultivectorNumber* leftMultivector = 0;
	const MultivectorNumber* rightMultivector = 0;
	if( !CopyOperandsIfNeeded( left, right, leftMultivector, rightMultivector, environment ) )
		return false;

	if( !multivector.AssignGeometricProduct( leftMultivector->multivector, rightMultivector->multivector ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignQuotient( const Number* left, const Number* right, Environment& environment )
{
	const MultivectorNumber* leftMultivector = 0;
	const MultivectorNumber* rightMultivector = 0;
	if( !CopyOperandsIfNeeded( left, right, leftMultivector, rightMultivector, environment ) )
		return false;

	MultivectorNumber rightMultiplicativeInverse;
	if( !rightMultiplicativeInverse.AssignMultilicativeInverse( rightMultivector, environment ) )
		return false;

	if( !multivector.AssignGeometricProduct( leftMultivector->multivector, rightMultiplicativeInverse.multivector ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignAdditiveInverse( const Number* number, Environment& environment )
{
	if( !number->IsTypeOf( MultivectorNumber::ClassName() ) )
		return false;

	MultivectorNumber* multivectorNumber = ( MultivectorNumber* )number;
	if( !AssignFrom( multivectorNumber, environment ) )
		return false;

	if( !multivector.Scale( -1.0 ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignMultilicativeInverse( const Number* number, Environment& environment )
{
	if( !number->IsTypeOf( MultivectorNumber::ClassName() ) )
		return false;

	GeometricAlgebra::SumOfBlades::InverseResult inverseResult;
	MultivectorNumber* multivectorNumber = ( MultivectorNumber* )number;
	if( !multivector.AssignGeometricInverse( multivectorNumber->multivector, GeometricAlgebra::SumOfBlades::RIGHT_INVERSE, inverseResult ) )
	{
		if( inverseResult == GeometricAlgebra::SumOfBlades::SINGULAR_MULTIVECTOR )
		{
			environment.AddError( "The multivector that you tried to invert is not invertable." );
			return false;
		}
		else
		{
			environment.AddError( "An error occured while trying to calculate the inverse of the multivector." );
			return false;
		}
	}

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignAdditiveIdentity( Environment& environment )
{
	if( !multivector.AssignZero() )
		return false;
	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignMultiplicativeIdentity( Environment& environment )
{
	GeometricAlgebra::Scalar scalar( 1.0 );
	GeometricAlgebra::Blade blade;
	if( !blade.AssignScalar( scalar ) )
		return false;
	if( !multivector.AssignBlade( blade ) )
		return false;
	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignInnerProduct( const Number* left, const Number* right, Environment& environment )
{
	const MultivectorNumber* leftMultivector = 0;
	const MultivectorNumber* rightMultivector = 0;
	if( !CopyOperandsIfNeeded( left, right, leftMultivector, rightMultivector, environment ) )
		return false;

	if( !multivector.AssignInnerProduct( leftMultivector->multivector, rightMultivector->multivector ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignOuterProduct( const Number* left, const Number* right, Environment& environment )
{
	const MultivectorNumber* leftMultivector = 0;
	const MultivectorNumber* rightMultivector = 0;
	if( !CopyOperandsIfNeeded( left, right, leftMultivector, rightMultivector, environment ) )
		return false;

	if( !multivector.AssignOuterProduct( leftMultivector->multivector, rightMultivector->multivector ) )
		return false;

	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::IsAdditiveIdentity( bool& isAdditiveIdentity, Environment& environment ) const
{
	isAdditiveIdentity = false;
	if( multivector.BladeCount() == 0 )
		isAdditiveIdentity = true;
	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::IsMultiplicativeIdentity( bool& isMultiplicativeIdentity, Environment& environment ) const
{
	return false;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::CompareWith( const Number* number, Comparison& comparison, Environment& environment ) const
{
	MultivectorNumber* multivectorNumber = ( MultivectorNumber* )number;

	if( !multivector.IsHomogeneousOfGrade(0) || !multivectorNumber->multivector.IsHomogeneousOfGrade(0) )
	{
		environment.AddError( "How do you compare one multivector to another when one or both of them is/are not homogeneous of grade zero?" );
		return false;
	}

	double thisScalar, givenScalar;
	if( !multivector.AssignScalarTo( thisScalar ) )
		return false;
	if( !multivectorNumber->multivector.AssignScalarTo( givenScalar ) )
		return false;

	if( thisScalar == givenScalar )
		comparison = IS_EQUAL_TO;
	else if( thisScalar < givenScalar )
		comparison = IS_LESS_THAN;
	else if( thisScalar > givenScalar )
		comparison = IS_GREATER_THAN;
	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignDerivative( const Number* number, const char* variableName, Environment& environment )
{
	const MultivectorNumber* multivectorNumber = number->Cast< MultivectorNumber >();
	if( !multivectorNumber )
		return false;
	if( !multivector.AssignDerivative( multivectorNumber->multivector, variableName ) )
		return false;
	return true;
}

//=========================================================================================
/*virtual*/ bool MultivectorNumber::AssignAntiDerivative( const Number* number, const char* variableName, Environment& environment )
{
	const MultivectorNumber* multivectorNumber = number->Cast< MultivectorNumber >();
	if( !multivectorNumber )
		return false;
	if( !multivector.AssignAntiDerivative( multivectorNumber->multivector, variableName ) )
		return false;
	return true;
}

//=========================================================================================
bool MultivectorNumber::Reverse( Environment& environment )
{
	if( !multivector.Reverse() )
		return false;

	return true;
}

//=========================================================================================
bool MultivectorNumber::Bar( Environment& environment )
{
	if( !multivector.Bar() )
		return false;

	return true;
}

// MultivectorNumber.cpp