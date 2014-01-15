// FormulatedConstraint.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "FormulatedConstraint.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( FormulatedConstraint, GAVisToolConstraint );

//=========================================================================================
FormulatedConstraint::FormulatedConstraint( void )
{
	evaluationTreeRoot = 0;
	formula = 0;
}

//=========================================================================================
/*virtual*/ FormulatedConstraint::~FormulatedConstraint( void )
{
	if( evaluationTreeRoot )
	{
		delete evaluationTreeRoot;
		evaluationTreeRoot = 0;
	}

	if( formula )
	{
		delete formula;
		formula = 0;
	}
}

//=========================================================================================
bool FormulatedConstraint::SetFormula( const char* formula, GAVisToolEnvironment* visToolEnv )
{
	bool success = false;
	CalcLib::Tokenizer tokenizer;
	CalcLib::Parser parser;
	Utilities::List listOfTokens;

	do
	{
		// We could call a CalcLib routine to compile the given formula for us, but we're
		// going to do the steps ourselves, because we want to analyze the tokenized formula
		// for our own purposes.
		if( !tokenizer.Tokenize( formula, listOfTokens, *visToolEnv ) )
			break;

		// We expect the first token to be the output variable.
		CalcLib::Token* outputToken = ( CalcLib::Token* )listOfTokens.LeftMost();
		if( !outputToken || outputToken->type != CalcLib::Token::TYPE_NAME )
		{
			if( !visToolEnv->LookupBindTargetByName( outputToken->string ) )
			{
				visToolEnv->AddError(
						"No output token found taht is a bind target in the given formula for"
						"a formulated constraint.  Your formula must be an explicit function, not implicit." );
				break;
			}
		}

		// Register our output.
		AddOutput( outputToken->string );

		// We expect the next token to be an assignment operator.
		CalcLib::Token* assignmentToken = ( CalcLib::Token* )outputToken->Right();
		if( !assignmentToken || assignmentToken->type != CalcLib::Token::TYPE_OPERATOR || 0 != strcmp( assignmentToken->string, "=" ) )
		{
			visToolEnv->AddError( "Formulas for the formulated constraint must be of the form \"y=f(x)\" or \"z=f(x,y)\", etc., where \"f\" is some expression in some number of variables." );
			break;
		}

		// Go find all the input tokens and register our inputs.
		CalcLib::Token* inputToken = ( CalcLib::Token* )assignmentToken->Right();
		while( inputToken )
		{
			CalcLib::Token* nextToken = ( CalcLib::Token* )inputToken->Right();
			if( inputToken->type == CalcLib::Token::TYPE_NAME && ( !nextToken || nextToken->type != CalcLib::Token::TYPE_LEFT_PARAN ) )
				if( visToolEnv->LookupBindTargetByName( inputToken->string ) )
					AddInput( inputToken->string );
			inputToken = nextToken;
		}

		// Now let the parser create our evaluation tree.
		if( !parser.Parse( listOfTokens, evaluationTreeRoot, *visToolEnv ) )
			break;

		// We made it through the gauntlet!  Hurray!
		success = true;
	}
	while( false );

	listOfTokens.RemoveAll( true );

	if( success )
	{
		// Keep our own copy of the formula for future reference.
		int len = strlen( formula ) + 1;
		this->formula = new char[ len ];
		strcpy_s( this->formula, len, formula );
	}

	return success;
}

//=========================================================================================
/*virtual*/ void FormulatedConstraint::Execute( GAVisToolEnvironment* visToolEnv )
{
	if( evaluationTreeRoot )
	{
		CalcLib::Environment* environment = visToolEnv;
		CalcLib::Number* result = environment->CreateNumber( evaluationTreeRoot );
		evaluationTreeRoot->EvaluateResult( *result, *visToolEnv );
		delete result;
	}
}

//=========================================================================================
/*virtual*/ void FormulatedConstraint::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxTreeItemId constraintItem = treeCtrl->AppendItem( parentItem, wxString::Format( wxT( "Formula: %s" ), formula ), -1, -1, new GAVisToolInventoryTree::Data( id ) );
	AddConstraintDependencyTreeItems( treeCtrl, constraintItem );
}

// FormulatedConstraint.cpp