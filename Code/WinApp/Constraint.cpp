// Constraint.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Constraint.h"
#include "Application.h"

IMPLEMENT_CALCLIB_CLASS1( GAVisToolConstraint, GAVisToolInventoryTree::Item );

//=========================================================================================
GAVisToolConstraint::GAVisToolConstraint( void )
{
}

//=========================================================================================
/*virtual*/ GAVisToolConstraint::~GAVisToolConstraint( void )
{
}

//=========================================================================================
void GAVisToolConstraint::AddInput( const char* bindTargetName )
{
	inputMap.Insert( bindTargetName, true );
}

//=========================================================================================
void GAVisToolConstraint::AddOutput( const char* bindTargetName )
{
	outputMap.Insert( bindTargetName, true );
}

//=========================================================================================
bool GAVisToolConstraint::IsInput( const char* bindTargetName )
{
	bool entryValue = false;
	bool entryFound = inputMap.Lookup( bindTargetName, &entryValue );
	return( entryFound && entryValue );
}

//=========================================================================================
bool GAVisToolConstraint::IsOutput( const char* bindTargetName )
{
	bool entryValue = false;
	bool entryFound = outputMap.Lookup( bindTargetName, &entryValue );
	return( entryFound && entryValue );
}

//=========================================================================================
void GAVisToolConstraint::MarkAllInputsAsChanged( GAVisToolEnvironment* visToolEnv )
{
	Utilities::Map< bool >::Iterator inputMapIter( &inputMap );
	while( !inputMapIter.Finished() )
	{
		const char* bindTargetName = 0;
		inputMapIter.CurrentEntry( &bindTargetName );
		GAVisToolBindTarget* bindTarget = visToolEnv->LookupBindTargetByName( bindTargetName );
		if( bindTarget )
			bindTarget->HasChanged( true );
		inputMapIter.Next();
	}
}

//=========================================================================================
bool GAVisToolConstraint::NeedsExecution( GAVisToolEnvironment* visToolEnv )
{
	Utilities::Map< bool >::Iterator inputMapIter( &inputMap );
	while( !inputMapIter.Finished() )
	{
		const char* bindTargetName = 0;
		inputMapIter.CurrentEntry( &bindTargetName );
		GAVisToolBindTarget* bindTarget = visToolEnv->LookupBindTargetByName( bindTargetName );
		if( bindTarget && bindTarget->HasChanged() )
			return true;
		inputMapIter.Next();
	}
	return false;
}

//=========================================================================================
bool GAVisToolConstraint::NeedsExecution( Utilities::List& executionList )
{
	for( GAVisToolConstraint* constraint = ( GAVisToolConstraint* )executionList.LeftMost(); constraint; constraint = ( GAVisToolConstraint* )constraint->Right() )
		if( this->DependsUpon( constraint ) )
			return true;
	return false;
}

//=========================================================================================
bool GAVisToolConstraint::DependsUpon( GAVisToolConstraint* constraint )
{
	Utilities::Map< bool >::Iterator inputMapIter( &inputMap );
	while( !inputMapIter.Finished() )
	{
		const char* bindTargetName = 0;
		inputMapIter.CurrentEntry( &bindTargetName );
		if( constraint->IsOutput( bindTargetName ) )
			return true;
		inputMapIter.Next();
	}
	return false;
}

//=========================================================================================
/*virtual*/ Utilities::List::SortComparison GAVisToolConstraint::SortCompare( const Utilities::List::Item* compareWithItem ) const
{
	// This sort compare is non-sensical, and its only purpose is to provide
	// a consistent ordering for the list of constraints at the start of the
	// constraint schedualing algorithm.
	int thisAddr = int( this );
	int givenAddr = int( compareWithItem );
	if( thisAddr < givenAddr )
		return Utilities::List::SORT_COMPARE_LESS_THAN;
	if( thisAddr > givenAddr )
		return Utilities::List::SORT_COMPARE_GREATER_THAN;
	return Utilities::List::SORT_COMPARE_EQUAL_TO;
}

//=========================================================================================
void GAVisToolConstraint::AddConstraintDependencyTreeItems( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxTreeItemId inputsItem = treeCtrl->AppendItem( parentItem, wxT( "Inputs" ) );
	wxTreeItemId outputsItem = treeCtrl->AppendItem( parentItem, wxT( "Outputs" ) );

	GAVisToolEnvironment* visToolEnv = wxGetApp().environment;

	Utilities::Map< bool >::Iterator inputMapIter( &inputMap );
	while( !inputMapIter.Finished() )
	{
		const char* bindTargetName = 0;
		inputMapIter.CurrentEntry( &bindTargetName );
		GAVisToolBindTarget* bindTarget = visToolEnv->LookupBindTargetByName( bindTargetName );
		if( bindTarget )
			bindTarget->AddInventoryTreeItem( treeCtrl, inputsItem );
		else
			treeCtrl->AppendItem( inputsItem, wxString::Format( wxT( "Unknown: %s" ), bindTargetName ) );
		inputMapIter.Next();
	}

	Utilities::Map< bool >::Iterator outputMapIter( &outputMap );
	while( !outputMapIter.Finished() )
	{
		const char* bindTargetName = 0;
		outputMapIter.CurrentEntry( &bindTargetName );
		GAVisToolBindTarget* bindTarget = visToolEnv->LookupBindTargetByName( bindTargetName );
		if( bindTarget )
			bindTarget->AddInventoryTreeItem( treeCtrl, outputsItem );
		else
			treeCtrl->AppendItem( inputsItem, wxString::Format( wxT( "Unknown: %s" ), bindTargetName ) );
		outputMapIter.Next();
	}
}

// Constraint.cpp