// GeometricAlgebraEnv.cpp

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
IMPLEMENT_CALCLIB_CLASS1( GeometricAlgebraEnvironment, Environment );

//=========================================================================================
GeometricAlgebraEnvironment::GeometricAlgebraEnvironment( void )
{
	displayMode = DISPLAY_AS_SUM_OF_BLADES;
}

//=========================================================================================
/*virtual*/ GeometricAlgebraEnvironment::~GeometricAlgebraEnvironment( void )
{
}

//=========================================================================================
/*virtual*/ void GeometricAlgebraEnvironment::PrintEnvironmentInfo( void )
{
	Print(
		"\n"
		"You are in the geometric algebra environment.\n"
		"\n"
	);
}

//=========================================================================================
/*virtual*/ Number* GeometricAlgebraEnvironment::CreateNumber( void )
{
	MultivectorNumber* multivectorNumber = new MultivectorNumber();
	Number* number = multivectorNumber;
	return number;
}

//=========================================================================================
/*virtual*/ FunctionEvaluator* GeometricAlgebraEnvironment::CreateFunction( const char* functionName )
{
	FunctionEvaluator* functionEvaluator = 0;

	if( 0 == strcmp( functionName, "reverse" ) )
		functionEvaluator = new ReverseFunctionEvaluator();
	else if( 0 == strcmp( functionName, "bar" ) )
		functionEvaluator = new BarFunctionEvaluator();
	else if( 0 == strcmp( functionName, "scalar_part" ) )
		functionEvaluator = new ScalarPartFunctionEvaluator();
	else if( 0 == strcmp( functionName, "grade_part" ) )
		functionEvaluator = new GradePartFunctionEvaluator();
	else if( 0 == strcmp( functionName, "inverse" ) )
		functionEvaluator = new InverseFunctionEvaluator();
	else if( 0 == strcmp( functionName, "exp" ) )
		functionEvaluator = new MultivectorMathFunctionEvaluator( MultivectorMathFunctionEvaluator::FUNC_EXP );
	else if( 0 == strcmp( functionName, "log" ) )
		functionEvaluator = new MultivectorMathFunctionEvaluator( MultivectorMathFunctionEvaluator::FUNC_LOG );
	else if( 0 == strcmp( functionName, "pow" ) )
		functionEvaluator = new MultivectorMathFunctionEvaluator( MultivectorMathFunctionEvaluator::FUNC_POW );
	else if( 0 == strcmp( functionName, "sqrt" ) )
		functionEvaluator = new MultivectorMathFunctionEvaluator( MultivectorMathFunctionEvaluator::FUNC_SQRT );
	else if( 0 == strcmp( functionName, "cos" ) )
		functionEvaluator = new MultivectorMathFunctionEvaluator( MultivectorMathFunctionEvaluator::FUNC_COS );
	else if( 0 == strcmp( functionName, "sin" ) )
		functionEvaluator = new MultivectorMathFunctionEvaluator( MultivectorMathFunctionEvaluator::FUNC_SIN );
	else if( 0 == strcmp( functionName, "tan" ) )
		functionEvaluator = new MultivectorMathFunctionEvaluator( MultivectorMathFunctionEvaluator::FUNC_TAN );
	else if( 0 == strcmp( functionName, "abs" ) )
		functionEvaluator = new MultivectorMathFunctionEvaluator( MultivectorMathFunctionEvaluator::FUNC_ABS );
	else if( 0 == strcmp( functionName, "acos" ) )
		functionEvaluator = new MultivectorMathFunctionEvaluator( MultivectorMathFunctionEvaluator::FUNC_ACOS );
	else if( 0 == strcmp( functionName, "asin" ) )
		functionEvaluator = new MultivectorMathFunctionEvaluator( MultivectorMathFunctionEvaluator::FUNC_ASIN );
//	else if( 0 == strcmp( functionName, "conjugate" ) )
//		functionEvaluator = new ConjugateFunctionEvaluator();
	else
		functionEvaluator = Environment::CreateFunction( functionName );
	
	return functionEvaluator;
}

//=========================================================================================
/*virtual*/ Evaluator* GeometricAlgebraEnvironment::CreateVariable( const char* variableName )
{
	ConstantEvaluator* evaluator = 0;
	MultivectorNumber* multivectorNumber = 0;
	GeometricAlgebra::Vector* vector = 0;
	GeometricAlgebra::Blade* blade = 0;
	GeometricAlgebra::SumOfBlades* sumOfBlades = 0;

	if( 0 == strcmp( variableName, "e0" ) )
		vector = new GeometricAlgebra::Vector_e0();
	else if( 0 == strcmp( variableName, "e1" ) )
		vector = new GeometricAlgebra::Vector_e1();
	else if( 0 == strcmp( variableName, "e2" ) )
		vector = new GeometricAlgebra::Vector_e2();
	else if( 0 == strcmp( variableName, "e3" ) )
		vector = new GeometricAlgebra::Vector_e3();
	else if( 0 == strcmp( variableName, "e0b" ) )
		vector = new GeometricAlgebra::Vector_e0_bar();
	else if( 0 == strcmp( variableName, "e1b" ) )
		vector = new GeometricAlgebra::Vector_e1_bar();
	else if( 0 == strcmp( variableName, "e2b" ) )
		vector = new GeometricAlgebra::Vector_e2_bar();
	else if( 0 == strcmp( variableName, "e3b" ) )
		vector = new GeometricAlgebra::Vector_e3_bar();
	else if( 0 == strcmp( variableName, "no" ) )
		vector = new GeometricAlgebra::Vector_no();
	else if( 0 == strcmp( variableName, "ni" ) )
		vector = new GeometricAlgebra::Vector_ni();
	else if( 0 == strcmp( variableName, "nob" ) )
		vector = new GeometricAlgebra::Vector_no_bar();
	else if( 0 == strcmp( variableName, "nib" ) )
		vector = new GeometricAlgebra::Vector_ni_bar();
	else if( 0 == strcmp( variableName, "i" ) )
	{
		blade = new GeometricAlgebra::Blade();
		Utilities::List vectorProduct;
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e1() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e2() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e3() );
		blade->AssignBlade( vectorProduct, 1.0 );
	}
	else if( 0 == strcmp( variableName, "I" ) )
	{
		blade = new GeometricAlgebra::Blade();
		Utilities::List vectorProduct;
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e1() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e2() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e3() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_ni() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_no() );
		blade->AssignBlade( vectorProduct, 1.0 );
	}
	else if( 0 == strcmp( variableName, "E2" ) )
	{
		blade = new GeometricAlgebra::Blade();
		Utilities::List vectorProduct;
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e3() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e1() );
		blade->AssignBlade( vectorProduct, 1.0 );
	}
	else if( 0 == strcmp( variableName, "E1" ) )
	{
		blade = new GeometricAlgebra::Blade();
		Utilities::List vectorProduct;
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e2() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e0() );
		blade->AssignBlade( vectorProduct, 1.0 );
	}
	else if( 0 == strcmp( variableName, "E3" ) )
	{
		blade = new GeometricAlgebra::Blade();
		Utilities::List vectorProduct;
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e1() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e2() );
		blade->AssignBlade( vectorProduct, 1.0 );
	}
	else if( 0 == strcmp( variableName, "Omega" ) )
	{
		sumOfBlades = new GeometricAlgebra::SumOfBlades();

		Utilities::List vectorProduct;

		GeometricAlgebra::Blade blade_e1e5;
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e1() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e1_bar() );
		blade_e1e5.AssignBlade( vectorProduct, 1.0 );

		GeometricAlgebra::Blade blade_e2e6;
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e2() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e2_bar() );
		blade_e2e6.AssignBlade( vectorProduct, 1.0 );

		GeometricAlgebra::Blade blade_e3e7;
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e3() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_e3_bar() );
		blade_e3e7.AssignBlade( vectorProduct, 1.0 );

		sumOfBlades->Accumulate( blade_e1e5 );
		sumOfBlades->Accumulate( blade_e2e6 );
		sumOfBlades->Accumulate( blade_e3e7 );
	}
	else if( 0 == strcmp( variableName, "Alpha" ) )
	{
		blade = new GeometricAlgebra::Blade();
		Utilities::List vectorProduct;
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_ni() );
		vectorProduct.InsertRightOf( vectorProduct.RightMost(), new GeometricAlgebra::Vector_ni_bar() );
		blade->AssignBlade( vectorProduct, 1.0 );
	}
	// Composites with e0...
	else if( 0 == strcmp( variableName, "e0_e0b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e0(), new GeometricAlgebra::Vector_e0_bar() );
	else if( 0 == strcmp( variableName, "e0_e1b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e0(), new GeometricAlgebra::Vector_e1_bar() );
	else if( 0 == strcmp( variableName, "e0_e2b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e0(), new GeometricAlgebra::Vector_e2_bar() );
	else if( 0 == strcmp( variableName, "e0_e3b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e0(), new GeometricAlgebra::Vector_e3_bar() );
	else if( 0 == strcmp( variableName, "e0_nob" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e0(), new GeometricAlgebra::Vector_no_bar() );
	else if( 0 == strcmp( variableName, "e0_nib" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e0(), new GeometricAlgebra::Vector_ni_bar() );
	// Composites with e1...
	else if( 0 == strcmp( variableName, "e1_e0b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e1(), new GeometricAlgebra::Vector_e0_bar() );
	else if( 0 == strcmp( variableName, "e1_e1b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e1(), new GeometricAlgebra::Vector_e1_bar() );
	else if( 0 == strcmp( variableName, "e1_e2b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e1(), new GeometricAlgebra::Vector_e2_bar() );
	else if( 0 == strcmp( variableName, "e1_e3b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e1(), new GeometricAlgebra::Vector_e3_bar() );
	else if( 0 == strcmp( variableName, "e1_nob" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e1(), new GeometricAlgebra::Vector_no_bar() );
	else if( 0 == strcmp( variableName, "e1_nib" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e1(), new GeometricAlgebra::Vector_ni_bar() );
	// Composites with e2...
	else if( 0 == strcmp( variableName, "e2_e0b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e2(), new GeometricAlgebra::Vector_e0_bar() );
	else if( 0 == strcmp( variableName, "e2_e1b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e2(), new GeometricAlgebra::Vector_e1_bar() );
	else if( 0 == strcmp( variableName, "e2_e2b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e2(), new GeometricAlgebra::Vector_e2_bar() );
	else if( 0 == strcmp( variableName, "e2_e3b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e2(), new GeometricAlgebra::Vector_e3_bar() );
	else if( 0 == strcmp( variableName, "e2_nob" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e2(), new GeometricAlgebra::Vector_no_bar() );
	else if( 0 == strcmp( variableName, "e2_nib" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e2(), new GeometricAlgebra::Vector_ni_bar() );
	// Composites with e3...
	else if( 0 == strcmp( variableName, "e3_e0b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e3(), new GeometricAlgebra::Vector_e0_bar() );
	else if( 0 == strcmp( variableName, "e3_e1b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e3(), new GeometricAlgebra::Vector_e1_bar() );
	else if( 0 == strcmp( variableName, "e3_e2b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e3(), new GeometricAlgebra::Vector_e2_bar() );
	else if( 0 == strcmp( variableName, "e3_e3b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e3(), new GeometricAlgebra::Vector_e3_bar() );
	else if( 0 == strcmp( variableName, "e3_nob" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e3(), new GeometricAlgebra::Vector_no_bar() );
	else if( 0 == strcmp( variableName, "e3_nib" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_e3(), new GeometricAlgebra::Vector_ni_bar() );
	// Composites with no...
	else if( 0 == strcmp( variableName, "no_e0b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_no(), new GeometricAlgebra::Vector_e0_bar() );
	else if( 0 == strcmp( variableName, "no_e1b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_no(), new GeometricAlgebra::Vector_e1_bar() );
	else if( 0 == strcmp( variableName, "no_e2b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_no(), new GeometricAlgebra::Vector_e2_bar() );
	else if( 0 == strcmp( variableName, "no_e3b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_no(), new GeometricAlgebra::Vector_e3_bar() );
	else if( 0 == strcmp( variableName, "no_nob" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_no(), new GeometricAlgebra::Vector_no_bar() );
	else if( 0 == strcmp( variableName, "no_nib" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_no(), new GeometricAlgebra::Vector_ni_bar() );
	// Composites with ni...
	else if( 0 == strcmp( variableName, "ni_e0b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_ni(), new GeometricAlgebra::Vector_e0_bar() );
	else if( 0 == strcmp( variableName, "ni_e1b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_ni(), new GeometricAlgebra::Vector_e1_bar() );
	else if( 0 == strcmp( variableName, "ni_e2b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_ni(), new GeometricAlgebra::Vector_e2_bar() );
	else if( 0 == strcmp( variableName, "ni_e3b" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_ni(), new GeometricAlgebra::Vector_e3_bar() );
	else if( 0 == strcmp( variableName, "ni_nob" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_ni(), new GeometricAlgebra::Vector_no_bar() );
	else if( 0 == strcmp( variableName, "ni_nib" ) )
		vector = new GeometricAlgebra::CompositeVector( new GeometricAlgebra::Vector_ni(), new GeometricAlgebra::Vector_ni_bar() );

	if( vector )
		multivectorNumber = new MultivectorNumber( *vector );
	else if( blade )
		multivectorNumber = new MultivectorNumber( *blade );
	else if( sumOfBlades )
		multivectorNumber = new MultivectorNumber( *sumOfBlades );

	delete vector;
	delete blade;
	delete sumOfBlades;

	if( multivectorNumber )
		evaluator = new ConstantEvaluator( multivectorNumber );
	
	return evaluator;
}

//=========================================================================================
/*virtual*/ Evaluator* GeometricAlgebraEnvironment::CreateBinaryOperator( const char* operatorName, Evaluator* leftOperand, Evaluator* rightOperand, bool& isBinaryOperationEvaluator )
{
	BinaryArithmeticOperationEvaluator* evaluator = 0;

	// The geometric product is handled through normal multiplication,
	// but the inner and outer products are a special case.
	if( 0 == strcmp( operatorName, "^" ) )
	{
		evaluator = new BinaryArithmeticOperationEvaluator( BinaryArithmeticOperationEvaluator::ARITHMETIC_OPERATION_OUTER_PRODUCT );
		isBinaryOperationEvaluator = true;
	}
	else if( 0 == strcmp( operatorName, "." ) )
	{
		evaluator = new BinaryArithmeticOperationEvaluator( BinaryArithmeticOperationEvaluator::ARITHMETIC_OPERATION_INNER_PRODUCT );
		isBinaryOperationEvaluator = true;
	}

	return evaluator;
}

//=========================================================================================
/*virtual*/ Evaluator* GeometricAlgebraEnvironment::CreateUnaryOperator( const char* operatorName, Evaluator* operand, bool& isUnaryOperationEvaluator )
{
	Evaluator* evaluator = 0;

	if( 0 == strcmp( operatorName, "~" ) )
	{
		ReverseFunctionEvaluator* reverseFunctionEvaluator = new ReverseFunctionEvaluator();
		FunctionArgumentEvaluator* argument = new FunctionArgumentEvaluator();
		argument->Argument( operand );
		reverseFunctionEvaluator->AddArgument( argument, FunctionEvaluator::APPEND_TO_LIST_OF_ARGS );
		evaluator = reverseFunctionEvaluator;
		isUnaryOperationEvaluator = false;
	}

	return evaluator;
}

// GeometricAlgebraEnv.cpp