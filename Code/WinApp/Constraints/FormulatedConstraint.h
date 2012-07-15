// FormulatedConstraint.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "../Constraint.h"

//=========================================================================================
// These are mathematical functions, so they have one and only one output.
class FormulatedConstraint : public GAVisToolConstraint
{
	DECLARE_CALCLIB_CLASS( FormulatedConstraint );

public:

	FormulatedConstraint( void );
	virtual ~FormulatedConstraint( void );

	bool SetFormula( const char* formula, GAVisToolEnvironment* visToolEnv );

	virtual void Execute( GAVisToolEnvironment* visToolEnv ) override;

	virtual void AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const override;

private:

	char* formula;
	CalcLib::Evaluator* evaluationTreeRoot;
};

// FormulatedConstraint.h