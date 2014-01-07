// PointSampledGeometry.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "PointSampledGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( PointSampledGeometry, GAVisToolGeometry );

//=========================================================================================
PointSampledGeometry::PointSampledGeometry( BindType bindType ) : GAVisToolGeometry( bindType )
{
	CalcLib::Calculator calculator( "geoalg" );

	char testCode[ 512 ];
	sprintf_s( testCode, sizeof( testCode ),
				"do( "
					"dot = ( "
					"e0 + "
					"x*e1 + "
					"y*e2 + "
					"z*e3 + "
					"y*z*e4 + "
					"z*x*e5 + "
					"x*y*e6 + "
					"x*x*e7 + "
					"y*y*e8 + "
					"z*z*e9 ) . element,"
					"test = sqrt( dot . dot ),"
				")" );
	testEvaluator = calculator.CompileEvaluator( testCode );
	wxASSERT( testEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ PointSampledGeometry::~PointSampledGeometry( void )
{
	WipePointCache();

	delete testEvaluator;
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* PointSampledGeometry::Create( BindType bindType )
{
	return new PointSampledGeometry( bindType );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	this->element.AssignSumOfBlades( element );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	element.AssignSumOfBlades( this->element );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::DumpInfo( char* printBuffer, int printBufferSize ) const
{
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::Draw( GAVisToolRender& render, bool selected )
{
	GeneratePointCache();

	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );

	render.Color( color, alpha );

	VectorMath::Vector normal;
	VectorMath::Set( normal, 0.0, 0.0, 1.0 );
	for( const Point* point = ( const Point* )pointCache.LeftMost(); point; point = ( const Point* )point->Right() )
		render.DrawPoint( point->point, normal );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::CalcCenter( VectorMath::Vector& center ) const
{
	// Calculate point average here...
}

//=========================================================================================
void PointSampledGeometry::GeneratePointCache( void )
{
	if( pointCache.Count() > 0 )
		return;

	negativeBoxList.RemoveAll( true );
	positiveBoxList.RemoveAll( true );
	
	// TODO: Throw up a progress dialog here?

	// The initial sample volume must encumpus whatever we'd want to visualize.
	// The initial sample resolution/density must be fine enough that we don't miss anything.
	VectorMath::Vector center, delta, halfDelta;
	VectorMath::Set( delta, 0.2, 0.2, 0.2 );
	VectorMath::Scale( halfDelta, delta, 0.5 );
	for( center.x = -5.0; center.x < 5.0; center.x += delta.x )
	{
		for( center.y = -5.0; center.y < 5.0; center.y += delta.y )
		{
			for( center.z = -5.0; center.z < 5.0; center.z += delta.z )
			{
				VectorMath::Aabb aabb;
				VectorMath::MakeAabb( aabb, center, halfDelta );
				SortNewBox( aabb );
			}
		}
	}

#if 0

	// This algorithm needs a lot of work.  It doesn't work very well at all.
	splitPlane = VectorMath::Aabb::XY_PLANE;
	while( pointCache.Count() < 500 )
	{
		if( negativeBoxList.Count() == 0 && positiveBoxList.Count() == 0 )
			break;

		if( negativeBoxList.Count() > 500 )
			CullBoxList( negativeBoxList );
		if( positiveBoxList.Count() > 500 )
			CullBoxList( positiveBoxList );

		ProcessBoxList( negativeBoxList );
		ProcessBoxList( positiveBoxList );

		RefineBoxList( negativeBoxList );
		RefineBoxList( positiveBoxList );

		splitPlane = VectorMath::Aabb::Plane( ( int( splitPlane ) + 1 ) % 3 );
	}

#else

	ProcessBoxList( negativeBoxList );
	ProcessBoxList( positiveBoxList );

#endif

	negativeBoxList.RemoveAll( true );
	positiveBoxList.RemoveAll( true );
}

//=========================================================================================
void PointSampledGeometry::SortNewBox( const VectorMath::Aabb& aabb )
{
	Box* box = new Box();
	VectorMath::CopyAabb( box->aabb, aabb );
	VectorMath::Vector center;
	VectorMath::CalcCenter( aabb, center );
	box->test = TestPoint( center );
	if( box->test < 0.0 )
		negativeBoxList.InsertRightOf( negativeBoxList.RightMost(), box );
	else
		positiveBoxList.InsertRightOf( positiveBoxList.RightMost(), box );
}

//=========================================================================================
void PointSampledGeometry::CullBoxList( Utilities::List& boxList )
{
	// Here we are betting that the geometry is contained in
	// just the first half of the sorted box list.
	boxList.Sort( Utilities::List::SORT_ORDER_ASCENDING );
	int keepCount = boxList.Count() / 2;
	while( boxList.Count() > keepCount )
	{
		Box* box = ( Box* )boxList.RightMost();
		boxList.Remove( box, true );
	}
}

//=========================================================================================
void PointSampledGeometry::ProcessBoxList( const Utilities::List& boxList )
{
	double epsilon = 1e-4;
	for( const Box* box = ( const Box* )boxList.LeftMost(); box; box = ( const Box* )box->Right() )
	{
		if( fabs( box->test ) < epsilon )
		{
			Point* point = new Point();
			VectorMath::CalcCenter( box->aabb, point->point );
			pointCache.InsertRightOf( pointCache.RightMost(), point );
		}
	}
}

//=========================================================================================
void PointSampledGeometry::RefineBoxList( Utilities::List& boxList )
{
	// We must scan right to left for this algorithm to be correct.
	Box* leftBox = 0;
	for( Box* box = ( Box* )boxList.RightMost(); box; box = leftBox )
	{
		leftBox = ( Box* )box->Left();
		VectorMath::Aabb aabbArray[8];
		VectorMath::Vector center;
		VectorMath::CalcCenter( box->aabb, center );
		int arraySize = 2;
		VectorMath::AabbSplit( box->aabb, center, splitPlane, aabbArray[0], aabbArray[1] );
		for( int index = 0; index < arraySize; index++ )
			SortNewBox( aabbArray[ index ] );
		boxList.Remove( box, true );
	}
}

//=========================================================================================
/*virtual*/ Utilities::List::SortComparison PointSampledGeometry::Box::SortCompare( const Utilities::List::Item* compareWithItem ) const
{
	const Box* box = ( const Box* )compareWithItem;
	if( test < box->test )
		return Utilities::List::SORT_COMPARE_LESS_THAN;
	else if( test > box->test )
		return Utilities::List::SORT_COMPARE_GREATER_THAN;
	return Utilities::List::SORT_COMPARE_EQUAL_TO;
}

//=========================================================================================
double PointSampledGeometry::TestPoint( const VectorMath::Vector& point )
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "element", *number );

	multivector->AssignFrom( point.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	multivector->AssignFrom( point.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	multivector->AssignFrom( point.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );

	testEvaluator->EvaluateResult( *number, gaEnv );

	double test;
	gaEnv.LookupVariable( "test", *number );
	multivector->AssignTo( test, gaEnv );

	return test;
}

//=========================================================================================
void PointSampledGeometry::WipePointCache( void )
{
	pointCache.RemoveAll( true );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::NameCenterOffset( VectorMath::Vector& offsetDelta )
{
	VectorMath::Set( offsetDelta, 0.0, 0.0, 0.0 );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::Translate( const VectorMath::Vector& delta )
{
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::Scale( float scale )
{
}

// PointSampledGeometry.cpp