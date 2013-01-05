// ScalarAlgebra.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include <math.h>

//=========================================================================================
#define USE_RATIONAL_EXPRESSIONS_FOR_SCALARS

//=========================================================================================
namespace ScalarAlgebra
{
	enum PrintPurpose
	{
		PRINT_FOR_SORTING,
		PRINT_FOR_READING,
		PRINT_FOR_LATEX,
	};

	class VariableEvaluator;
	class Expression;
	class RationalExpression;
	template< typename ScalarType > class TemplatedScalar;

#ifdef USE_RATIONAL_EXPRESSIONS_FOR_SCALARS
	typedef ScalarAlgebra::TemplatedScalar< ScalarAlgebra::RationalExpression > Scalar;
#else
	typedef ScalarAlgebra::TemplatedScalar< double > Scalar;
#endif
}

#include "../../Utilities/Utilities.h"

#include "Expression.h"
#include "RationalExpression.h"
#include "Scalar.h"
#include "VariableEvaluator.h"

// ScalarAlgebra.h