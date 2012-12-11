// Environment.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Environment.h"
#include "Functions/BindFunction.h"
#include "Functions/ColorFunction.h"
#include "Functions/DumpInfoFunction.h"
#include "Functions/WipeEnvFunction.h"
#include "Functions/FormulatedConstraintFunction.h"
#include "Functions/ReduceBivectorFunction.h"
#include "Functions/VectorToFromBivectorFunction.h"
#include "Functions/PointFunction.h"
#include "Application.h"
#include "Geometries/RoundGeometry.h"
#include "Geometries/FlatGeometry.h"
#include "Geometries/TransformGeometry.h"
#include "Geometries/PositionVectorGeometry.h"
#include "Geometries/ProjectiveGeometry.h"
#include "Geometries/QuadricGeometry.h"
#include "Geometries/ConformalQuarticGeometry.h"
#include "Interfaces/ScalarInterface.h"
#include "VirtualBindTarget.h"
#include "wxAll.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( GAVisToolEnvironment, GeometricAlgebraEnvironment );

//=========================================================================================
GAVisToolEnvironment::GAVisToolEnvironment( void )
{
	wipingEnvironment = false;
}

//=========================================================================================
/*virtual*/ GAVisToolEnvironment::~GAVisToolEnvironment( void )
{
	Wipe( false, false );
}

//=========================================================================================
/*virtual*/ void GAVisToolEnvironment::PrintEnvironmentInfo( void )
{
	Print( "You are in the GAVisTool environment!\n\n" );
}

//=========================================================================================
/*virtual*/ CalcLib::FunctionEvaluator* GAVisToolEnvironment::CreateFunction( const char* functionName )
{
	if( 0 == strcmp( functionName, "unbind" ) )
		return new GAVisToolBindFunctionEvaluator( 0, GAVisToolBindTarget::DOESNT_MATTER );
	else if( 0 == strcmp( functionName, "bind_virtual" ) )
		return new GAVisToolBindFunctionEvaluator( &GAVisToolVirtualBindTarget::Create, GAVisToolBindTarget::DOESNT_MATTER );
	else if( 0 == strcmp( functionName, "bind_posvec" ) )
		return new GAVisToolBindFunctionEvaluator( &PositionVector::Create, GAVisToolBindTarget::DOESNT_MATTER );
	else if( 0 == strcmp( functionName, "bind_point" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalPoint::Create, GAVisToolBindTarget::NORMAL_FORM );
	else if( 0 == strcmp( functionName, "bind_dual_point" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalPoint::Create, GAVisToolBindTarget::DUAL_FORM );
	else if( 0 == strcmp( functionName, "bind_sphere" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalSphere::Create, GAVisToolBindTarget::NORMAL_FORM );
	else if( 0 == strcmp( functionName, "bind_dual_sphere" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalSphere::Create, GAVisToolBindTarget::DUAL_FORM );
	else if( 0 == strcmp( functionName, "bind_circle" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalCircle::Create, GAVisToolBindTarget::NORMAL_FORM );
	else if( 0 == strcmp( functionName, "bind_dual_circle" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalCircle::Create, GAVisToolBindTarget::DUAL_FORM );
	else if( 0 == strcmp( functionName, "bind_pointpair" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalPointPair::Create, GAVisToolBindTarget::NORMAL_FORM );
	else if( 0 == strcmp( functionName, "bind_dual_pointpair" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalPointPair::Create, GAVisToolBindTarget::DUAL_FORM );
	else if( 0 == strcmp( functionName, "bind_flatpoint" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalFlatPoint::Create, GAVisToolBindTarget::NORMAL_FORM );
	else if( 0 == strcmp( functionName, "bind_dual_flatpoint" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalFlatPoint::Create, GAVisToolBindTarget::DUAL_FORM );
	else if( 0 == strcmp( functionName, "bind_line" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalLine::Create, GAVisToolBindTarget::NORMAL_FORM );
	else if( 0 == strcmp( functionName, "bind_dual_line" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalLine::Create, GAVisToolBindTarget::DUAL_FORM );
	else if( 0 == strcmp( functionName, "bind_plane" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalPlane::Create, GAVisToolBindTarget::NORMAL_FORM );
	else if( 0 == strcmp( functionName, "bind_dual_plane" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalPlane::Create, GAVisToolBindTarget::DUAL_FORM );
	else if( 0 == strcmp( functionName, "bind_transform" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalTransformGeometry::Create, GAVisToolBindTarget::DOESNT_MATTER );
	else if( 0 == strcmp( functionName, "bind_scalar_iface" ) )
		return new GAVisToolBindFunctionEvaluator( &ScalarInterface::Create, GAVisToolBindTarget::DOESNT_MATTER );
	else if( 0 == strcmp( functionName, "geo_color" ) )
		return new GAVisToolColorFunctionEvaluator();
	else if( 0 == strcmp( functionName, "formula_constraint" ) )
		return new GAVisToolFormulatedConstraintFunctionEvaluator();
	else if( 0 == strcmp( functionName, "dump_info" ) )
		return new GAVisToolDumpInfoFunctionEvaluator();
	else if( 0 == strcmp( functionName, "wipe_env" ) )
		return new GAVisToolWipeEnvFunctionEvaluator();
	else if( 0 == strcmp( functionName, "bind_proj_point" ) )
		return new GAVisToolBindFunctionEvaluator( &ProjectivePoint::Create, GAVisToolBindTarget::DOESNT_MATTER );
	else if( 0 == strcmp( functionName, "bind_proj_line" ) )
		return new GAVisToolBindFunctionEvaluator( &ProjectiveLine::Create, GAVisToolBindTarget::DOESNT_MATTER );
	else if( 0 == strcmp( functionName, "bind_proj_plane" ) )
		return new GAVisToolBindFunctionEvaluator( &ProjectivePlane::Create, GAVisToolBindTarget::DOESNT_MATTER );
	else if( 0 == strcmp( functionName, "bind_quadric" ) )
		return new GAVisToolBindFunctionEvaluator( &QuadricGeometry::Create, GAVisToolBindTarget::DOESNT_MATTER );
	else if( 0 == strcmp( functionName, "bind_conformal_quartic" ) )
		return new GAVisToolBindFunctionEvaluator( &ConformalQuarticGeometry::Create, GAVisToolBindTarget::DOESNT_MATTER );
	else if( 0 == strcmp( functionName, "reduce_bivector" ) )
		return new GAVisToolReduceBivectorFunctionEvaluator();
	else if( 0 == strcmp( functionName, "as_vector" ) )
		return new GAVisToolVectorToFromBivectorFunctionEvaluator( GAVisToolVectorToFromBivectorFunctionEvaluator::CONVERT_VECTOR_FROM_BIVECTOR );
	else if( 0 == strcmp( functionName, "as_bivector" ) )
		return new GAVisToolVectorToFromBivectorFunctionEvaluator( GAVisToolVectorToFromBivectorFunctionEvaluator::CONVERT_VECTOR_TO_BIVECTOR );
	else if( 0 == strcmp( functionName, "point" ) )
		return new GAVisToolPointFunctionEvaluator();

	return GeometricAlgebraEnvironment::CreateFunction( functionName );
}

//=========================================================================================
/*virtual*/ bool GAVisToolEnvironment::LookupVariable( const char* variableName, CalcLib::Number& variableValue )
{
	if( variableValue.IsTypeOf( CalcLib::MultivectorNumber::ClassName() ) )
	{
		// If the variable is bound to a target, then pull the value of the variable from the target.
		// (The target may have changed outside the environment.)
		GAVisToolBindTarget* bindTarget = LookupBindTargetByName( variableName );
		if( bindTarget )
		{
			GeometricAlgebra::SumOfBlades multivector;
			bindTarget->ComposeTo( multivector );
			CalcLib::MultivectorNumber* multivectorNumber = ( CalcLib::MultivectorNumber* )&variableValue;
			if( !multivectorNumber->AssignFrom( multivector, *this ) )
				return false;

			return true;
		}
	}

	bool success = GeometricAlgebraEnvironment::LookupVariable( variableName, variableValue );
	return success;
}

//=========================================================================================
/*virtual*/ bool GAVisToolEnvironment::StoreVariable( const char* variableName, const CalcLib::Number& variableValue )
{
	if( variableValue.IsTypeOf( CalcLib::MultivectorNumber::ClassName() ) )
	{
		// If the variable is bound to a target, then push the value of the variable to the target.
		GAVisToolBindTarget* bindTarget = LookupBindTargetByName( variableName );
		if( bindTarget )
		{
			CalcLib::MultivectorNumber* multivectorNumber = ( CalcLib::MultivectorNumber* )&variableValue;
			GeometricAlgebra::SumOfBlades multivector;
			if( !multivectorNumber->AssignTo( multivector, *this ) )
				return false;
			bindTarget->DecomposeFrom( multivector );
			bindTarget->HasChanged( true );
			return true;
		}
	}

	bool success = GeometricAlgebraEnvironment::StoreVariable( variableName, variableValue );
	return success;
}

//=========================================================================================
GAVisToolBindTarget* GAVisToolEnvironment::LookupBindTargetByIndex( int index )
{
	if( index < 0 || index >= listOfBindTargets.Count() )
		return 0;

	return( ( GAVisToolBindTarget* )listOfBindTargets[ index ] );
}

//=========================================================================================
GAVisToolBindTarget* GAVisToolEnvironment::LookupBindTargetByName( const char* name )
{
	// Do a no-brainer linear search for a target by the given name.
	GAVisToolBindTarget* foundTarget = 0;
	for( GAVisToolBindTarget* bindTarget = ( GAVisToolBindTarget* )listOfBindTargets.LeftMost(); bindTarget && !foundTarget; bindTarget = ( GAVisToolBindTarget* )bindTarget->Right() )
		if( 0 == strcmp( bindTarget->GetName(), name ) )
			foundTarget = bindTarget;
	return foundTarget;
}

//=========================================================================================
GAVisToolGeometry* GAVisToolEnvironment::LookupGeometryByIndex( int index )
{
	GAVisToolBindTarget* bindTarget = LookupBindTargetByIndex( index );
	if( bindTarget && !bindTarget->IsTypeOf( GAVisToolGeometry::ClassName() ) )
		bindTarget = 0;
	return( ( GAVisToolGeometry* )bindTarget );
}

//=========================================================================================
GAVisToolGeometry* GAVisToolEnvironment::LookupGeometryByName( const char* name )
{
	GAVisToolBindTarget* bindTarget = LookupBindTargetByName( name );
	if( bindTarget && !bindTarget->IsTypeOf( GAVisToolGeometry::ClassName() ) )
		bindTarget = 0;
	return( ( GAVisToolGeometry* )bindTarget );
}

//=========================================================================================
GAVisToolInterface* GAVisToolEnvironment::LookupInterfaceByName( const char* name )
{
	GAVisToolBindTarget* bindTarget = LookupBindTargetByName( name );
	if( bindTarget && !bindTarget->IsTypeOf( GAVisToolInterface::ClassName() ) )
		bindTarget = 0;
	return( ( GAVisToolInterface* )bindTarget );
}

//=========================================================================================
bool GAVisToolEnvironment::AddBindTarget( GAVisToolBindTarget* bindTarget )
{
	if( LookupBindTargetByName( bindTarget->GetName() ) )
		return false;

	if( !listOfBindTargets.InsertLeftOf( listOfBindTargets.LeftMost(), bindTarget ) )
		return false;

	bindTarget->Initialize();

	wxGetApp().canvasFrame->inventoryTree->RegenerationNeeded();

	return true;
}

//=========================================================================================
bool GAVisToolEnvironment::RemoveBindTarget( GAVisToolBindTarget* bindTarget )
{
	if( !LookupBindTargetByName( bindTarget->GetName() ) )
		return false;

	if( !listOfBindTargets.Remove( bindTarget, false ) )
		return false;

	bindTarget->Finalize();

	wxGetApp().canvasFrame->inventoryTree->RegenerationNeeded();

	return true;
}

//=========================================================================================
const Utilities::List& GAVisToolEnvironment::BindTargetList( void ) const
{
	return listOfBindTargets;
}

//=========================================================================================
const Utilities::List& GAVisToolEnvironment::ConstraintList( void ) const
{
	return listOfConstraints;
}

//=========================================================================================
void GAVisToolEnvironment::Wipe( bool finalizeBindTargets, bool regenInventoryTree )
{
	wipingEnvironment = true;

	while( listOfBindTargets.Count() > 0 )
	{
		GAVisToolBindTarget* bindTarget = ( GAVisToolBindTarget* )listOfBindTargets.LeftMost();
		listOfBindTargets.Remove( bindTarget, false );
		if( finalizeBindTargets )
			bindTarget->Finalize();
		delete bindTarget;
	}

	listOfConstraints.RemoveAll( true );

	if( regenInventoryTree )
		wxGetApp().canvasFrame->inventoryTree->RegenerationNeeded();

	wipingEnvironment = false;
}

//=========================================================================================
void GAVisToolEnvironment::Draw( GAVisToolRender& render, const char* selectedGeometry )
{
	// Do alpha sorting so that alpha blending works better.
	// True alpha sorting would require us to sort all the individual polygons,
	// but we can do a little better if we sort all the individual geometries.
	listOfBindTargets.Sort( Utilities::List::SORT_ORDER_DESCENDING );

	// Now go draw all the geometries in the correct order.
	GLint selectionName = 0;
	for( GAVisToolBindTarget* bindTarget = ( GAVisToolBindTarget* )listOfBindTargets.LeftMost(); bindTarget; bindTarget = ( GAVisToolBindTarget* )bindTarget->Right() )
	{
		if( bindTarget->IsTypeOf( GAVisToolGeometry::ClassName() ) )
		{
			GAVisToolGeometry* geometry = ( GAVisToolGeometry* )bindTarget;

			bool selected = false;
			if( render.GetRenderMode() != GAVisToolRender::RENDER_MODE_SELECTION && selectedGeometry && 0 == strcmp( geometry->GetName(), selectedGeometry ) )
				selected = true;

			if( render.GetRenderMode() == GAVisToolRender::RENDER_MODE_SELECTION )
				glPushName( selectionName );

			geometry->Draw( render, selected );

			if( render.GetRenderMode() == GAVisToolRender::RENDER_MODE_SELECTION )
				glPopName();
		}

		// The name should be an index into the bind targets list.
		selectionName++;
	}
}

//=========================================================================================
void GAVisToolEnvironment::DrawNames( void )
{
	VectorMath::CoordFrame cameraFrame;
	wxGetApp().canvasFrame->canvas->camera.CameraFrame( cameraFrame );

	glEnable( GL_TEXTURE_2D );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );

	for( GAVisToolBindTarget* bindTarget = ( GAVisToolBindTarget* )listOfBindTargets.LeftMost(); bindTarget; bindTarget = ( GAVisToolBindTarget* )bindTarget->Right() )
	{
		if( bindTarget->IsTypeOf( GAVisToolGeometry::ClassName() ) )
		{
			GAVisToolGeometry* geometry = ( GAVisToolGeometry* )bindTarget;
			geometry->DrawName( cameraFrame );
		}
	}

	glDisable( GL_TEXTURE_2D );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
}

//=========================================================================================
void GAVisToolEnvironment::GenerateGeometryNameTextures( void )
{
	for( GAVisToolBindTarget* bindTarget = ( GAVisToolBindTarget* )listOfBindTargets.LeftMost(); bindTarget; bindTarget = ( GAVisToolBindTarget* )bindTarget->Right() )
	{
		if( bindTarget->IsTypeOf( GAVisToolGeometry::ClassName() ) )
		{
			GAVisToolGeometry* geometry = ( GAVisToolGeometry* )bindTarget;
			geometry->GenerateNameTexture();
		}
	}
}

//=========================================================================================
// Here, if the given constraint can be schedualed, it is put in the proper place within
// the given execution list.  If it cannot be schedualed, then it is put on the other
// given list.  A constraint cannot be schedualed when the addition of that constraint
// creates a circular dependency.  One might wonder why we would even allow the user to
// create a constraint that creates a circular dependency in the graph.  Well, I believe
// that even when such things exist, the graph is still useful.  For example, we may want
// two bind targets to depend upon one another in different ways.  This is a circular
// dependency, but we should still be able to accomodate both constraints at individual
// times when individual bind targets are changed.  It should also be noted here that
// the ability to schedual one constraint over another may depend on the order in which
// we try to schedual the constraints.  Therefore, for consistency, it might be a good
// idea to sort the list of constraints before we begin the entire schedualling algorithm.
void GAVisToolEnvironment::SchedualConstraint( GAVisToolConstraint* constraint, Utilities::List& executionList, Utilities::List& cannotExecuteList )
{
	// The given constraint must be inserted anywhere after the last constraint to be executed that it depends upon.
	GAVisToolConstraint* insertAfter = ( GAVisToolConstraint* )executionList.RightMost();
	while( insertAfter )
	{
		if( constraint->DependsUpon( insertAfter ) )
			break;
		insertAfter = ( GAVisToolConstraint* )insertAfter->Left();
	}

	// The given constraint must be inserted anywhere before the first constraint to be executed that depends upon it.
	GAVisToolConstraint* insertBefore = ( GAVisToolConstraint* )executionList.LeftMost();
	while( insertBefore )
	{
		if( insertBefore->DependsUpon( insertBefore ) )
			break;
		insertBefore = ( GAVisToolConstraint* )insertBefore->Right();
	}

	// Arbitrarily insert the constraint if it can be schedualed.
	if( insertAfter && insertBefore )
	{
		if( insertBefore->IsLeftOf( insertAfter ) || insertAfter == insertBefore )
			cannotExecuteList.InsertRightOf( cannotExecuteList.RightMost(), constraint );
		else
			executionList.InsertRightOf( insertAfter, constraint );
	}
	else if( insertAfter )
		executionList.InsertRightOf( insertAfter, constraint );
	else if( insertBefore )
		executionList.InsertLeftOf( insertBefore, constraint );
	else
		executionList.InsertRightOf( executionList.RightMost(), constraint );
}

//=========================================================================================
bool GAVisToolEnvironment::SatisfyConstraints( void )
{
	// I'm not sure if this is the cleanest solution, but don't
	// try to satisfy any constraints if we're in the middle of
	// wiping the environment.  This type of re-entrancy can happen
	// due to all the windows messaging that goes on when we finalize
	// certain interfaces.
	if( wipingEnvironment )
		return true;

	Utilities::List executionList, cannotExecuteList;

	// Sort our list first so that our schedualing algorithm is consistent in the cases
	// where there may be circular dependencies in the constraint graph.
	listOfConstraints.Sort( Utilities::List::SORT_ORDER_ASCENDING );

	// Build up the initial list of constraints to execute based on which bind targets have been touched.
	GAVisToolConstraint* constraint = ( GAVisToolConstraint* )listOfConstraints.LeftMost();
	GAVisToolConstraint* nextConstraint = 0;
	while( constraint )
	{
		nextConstraint = ( GAVisToolConstraint* )constraint->Right();
		if( constraint->NeedsExecution( this ) )
		{
			listOfConstraints.Remove( constraint, false );
			SchedualConstraint( constraint, executionList, cannotExecuteList );
		}
		constraint = nextConstraint;
	}

	// Complete the execution list with additional constraints that must execute in turn.
	if( executionList.Count() > 0 )
	{
		bool schedualingMade = false;
		do
		{
			schedualingMade = false;
			constraint = ( GAVisToolConstraint* )listOfConstraints.LeftMost();
			nextConstraint = 0;
			while( constraint )
			{
				nextConstraint = ( GAVisToolConstraint* )constraint->Right();
				if( constraint->NeedsExecution( executionList ) )
				{
					listOfConstraints.Remove( constraint, false );
					SchedualConstraint( constraint, executionList, cannotExecuteList );
					schedualingMade = true;
				}
				constraint = nextConstraint;
			}
		}
		while( schedualingMade );
	}

	// Now go execute the schedualed constraints in the proper order from left to right.
	constraint = ( GAVisToolConstraint* )executionList.LeftMost();
	while( constraint )
	{
		constraint->Execute( this );
		constraint = ( GAVisToolConstraint* )constraint->Right();
	}

	// Dump the constraints on each of these lists back into the list of constraints.
	executionList.EmptyIntoOnRight( listOfConstraints );
	cannotExecuteList.EmptyIntoOnRight( listOfConstraints );

	// Lastly, reset the changed status flag of all bind targets.  Notice that here we may not
	// be reseting just the flags that triggered constraints to fire in the first place, but may
	// also be reseting flags that were set during the constraint execution process.
	for( GAVisToolBindTarget* bindTarget = ( GAVisToolBindTarget* )listOfBindTargets.LeftMost(); bindTarget; bindTarget = ( GAVisToolBindTarget* )bindTarget->Right() )
		bindTarget->HasChanged( false );

	return true;
}

//=========================================================================================
bool GAVisToolEnvironment::AddConstraint( GAVisToolConstraint* constraint )
{
	wxGetApp().canvasFrame->inventoryTree->RegenerationNeeded();

	return listOfConstraints.InsertRightOf( listOfConstraints.RightMost(), constraint );
}

//=========================================================================================
bool GAVisToolEnvironment::RemoveConstraint( GAVisToolConstraint* constraint )
{
	wxGetApp().canvasFrame->inventoryTree->RegenerationNeeded();

	return listOfConstraints.Remove( constraint, false );
}

//=========================================================================================
// Do a linear search for it.  There should never be so many constraints
// in the system that a linear search would be too slow.
GAVisToolConstraint* GAVisToolEnvironment::LookupConstraintByID( int id )
{
	GAVisToolConstraint* constraint = ( GAVisToolConstraint* )listOfConstraints.LeftMost();
	while( constraint )
	{
		if( constraint->ID() == id )
			return constraint;
		constraint = ( GAVisToolConstraint* )constraint->Right();
	}
	return 0;
}

//=========================================================================================
GAVisToolBindTarget* GAVisToolEnvironment::LookupBindTargetByID( int id )
{
	GAVisToolBindTarget* bindTarget = ( GAVisToolBindTarget* )listOfBindTargets.LeftMost();
	while( bindTarget )
	{
		if( bindTarget->ID() == id )
			return bindTarget;
		bindTarget = ( GAVisToolBindTarget* )bindTarget->Right();
	}
	return 0;
}

//=========================================================================================
GAVisToolInventoryTree::Item* GAVisToolEnvironment::LookupInventoryItemByID( int id )
{
	GAVisToolInventoryTree::Item* item = LookupBindTargetByID( id );
	if( !item )
		item = LookupConstraintByID( id );
	return item;
}

// Environment.cpp