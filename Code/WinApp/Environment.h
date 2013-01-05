// Environment.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Calculator/CalcLib.h"
#include "Geometry.h"
#include "Interface.h"
#include "Constraint.h"
#include "Render.h"

// TODO: Headers need clean-up.
class GAVisToolConstraint;

//=========================================================================================
class GAVisToolEnvironment : public CalcLib::GeometricAlgebraEnvironment
{
	DECLARE_CALCLIB_CLASS( GAVisToolEnvironment );

public:

	GAVisToolEnvironment( void );
	virtual ~GAVisToolEnvironment( void );

	virtual void PrintEnvironmentInfo( void );
	virtual CalcLib::FunctionEvaluator* CreateFunction( const char* functionName );

	virtual bool LookupVariable( const char* variableName, CalcLib::Number& variableValue ) override;
	virtual bool StoreVariable( const char* variableName, const CalcLib::Number& variableValue ) override;

	GAVisToolBindTarget* LookupBindTargetByID( int id );
	GAVisToolBindTarget* LookupBindTargetByIndex( int index );
	GAVisToolBindTarget* LookupBindTargetByName( const char* name );
	GAVisToolGeometry* LookupGeometryByIndex( int index );
	GAVisToolGeometry* LookupGeometryByName( const char* name );
	GAVisToolInterface* LookupInterfaceByName( const char* name );
	bool AddBindTarget( GAVisToolBindTarget* bindTarget );
	bool RemoveBindTarget( GAVisToolBindTarget* bindTarget );

	void Draw( GAVisToolRender& render, const char* selectedGeometry );
	void DrawNames( void );

	void GenerateGeometryNameTextures( void );

	bool SatisfyConstraints( void );
	bool AddConstraint( GAVisToolConstraint* constraint );
	bool RemoveConstraint( GAVisToolConstraint* constraint );
	GAVisToolConstraint* LookupConstraintByID( int id );

	GAVisToolInventoryTree::Item* LookupInventoryItemByID( int id );

	const Utilities::List& BindTargetList( void ) const;
	const Utilities::List& ConstraintList( void ) const;

	// This wipes out everything, cleaning/resetting the environment.
	void Wipe( bool finalizeBindTargets, bool regenInventoryTree );

private:

	void SchedualConstraint( GAVisToolConstraint* constraint, Utilities::List& executionList, Utilities::List& cannotExecuteList );

	Utilities::List listOfBindTargets;
	Utilities::List listOfConstraints;

	bool wipingEnvironment;
};

// Environment.h