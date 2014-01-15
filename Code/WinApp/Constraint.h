// Constraint.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Calculator/CalcLib.h"
#include "Environment.h"

// TODO: Headers need clean-up.
class GAVisToolEnvironment;

//=========================================================================================
// A constraint implements an execution method and defines what bind targets are
// used as input to the constraint and what bind targets are touched as output.
class GAVisToolConstraint : public Utilities::List::Item, public GAVisToolInventoryTree::Item
{
	DECLARE_CALCLIB_CLASS( GAVisToolConstraint );

public:

	GAVisToolConstraint( void );
	virtual ~GAVisToolConstraint( void );

	virtual void Execute( GAVisToolEnvironment* visToolEnv ) = 0;

	virtual Utilities::List::SortComparison SortCompare( const Utilities::List::Item* compareWithItem ) const override;

	void AddInput( const char* bindTargetName );
	void AddOutput( const char* bindTargetName );

	bool IsInput( const char* bindTargetName );
	bool IsOutput( const char* bindTargetName );

	bool NeedsExecution( GAVisToolEnvironment* visToolEnv );
	bool NeedsExecution( Utilities::List& executionList );
	bool DependsUpon( GAVisToolConstraint* constraint );

	void MarkAllInputsAsChanged( GAVisToolEnvironment* visToolEnv );

protected:

	// We keep references to our inputs and output by string.
	// This is so that we don't have to worry about stale pointers
	// in the event that one of our inputs or outputs is deleted
	// without our knowledge of that happening.
	Utilities::Map< bool > inputMap;
	Utilities::Map< bool > outputMap;

	void AddConstraintDependencyTreeItems( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const;
};

// Constraint.h