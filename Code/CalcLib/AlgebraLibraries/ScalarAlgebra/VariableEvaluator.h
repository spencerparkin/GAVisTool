// VariableEvaluator.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

//=========================================================================================
// This is more like a resolve.
class ScalarAlgebra::VariableEvaluator
{
public:
	VariableEvaluator( void );
	virtual ~VariableEvaluator( void );
	virtual bool Evaluate( const char* variableName, double& variableValue ) const = 0;
};

// VariableEvaluator.h