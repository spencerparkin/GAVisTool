// QuadricGeometry.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "QuadricGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( QuadricGeometry, GAVisToolGeometry );

//=========================================================================================
bool QuadricGeometry::setupSamplingParameters = true;
VectorMath::Aabb QuadricGeometry::samplingRegion;
VectorMath::Vector QuadricGeometry::samplingResolution;

//=========================================================================================
QuadricGeometry::QuadricGeometry( BindType bindType ) : GAVisToolGeometry( bindType )
{
	samplePointCloudDataValid = false;

	if( setupSamplingParameters )
	{
		VectorMath::Set( samplingRegion.min, -3.0, -3.0, -3.0 );
		VectorMath::Set( samplingRegion.max, 3.0, 3.0, 3.0 );
		VectorMath::Set( samplingResolution, 0.2, 0.2, 0.2 );
		setupSamplingParameters = false;
	}

	CalcLib::Calculator calculator( "geoalg" );

	char samplePointTestCode[ 512 ];
	strcpy_s( samplePointTestCode, sizeof( samplePointTestCode ),
		"do("
		"p = e0 + x*e1 + y*e2 + z*e3,"
		"q = e4 + x*e5 + y*e6 + z*e7,"
		"result = ( p ^ q ) . B,"
		")"
	);

	samplePointTestEvaluator = calculator.CompileEvaluator( samplePointTestCode );
	wxASSERT( samplePointTestEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ QuadricGeometry::~QuadricGeometry( void )
{
	samplePointCloudData.RemoveAll( true );
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* QuadricGeometry::Create( BindType bindType )
{
	return new QuadricGeometry( bindType );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	this->element.AssignSumOfBlades( element );

	// Invalidate the current point cloud data.
	samplePointCloudDataValid = false;
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	element.AssignSumOfBlades( this->element );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize, "The variable \"%s\" is being interpreted as a quadric.\n", name );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Quadric: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Draw( GAVisToolRender& render, bool selected )
{
	if( !samplePointCloudDataValid )
	{
		RegeneratePointCloudData();
		samplePointCloudDataValid = true;
	}

	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );
	
	// Draw the triangular mesh.
	/*
	Utilities::List& triangleList = convexHull.TriangleList();
	VectorMath::ConvexHull::Triangle* triangle = ( VectorMath::ConvexHull::Triangle* )triangleList.LeftMost();
	while( triangle )
	{
		VectorMath::Triangle triangleGeometry;
		VectorMath::Copy( triangleGeometry.vertex[0], convexHull[ triangle->vertex[0] ] );
		VectorMath::Copy( triangleGeometry.vertex[1], convexHull[ triangle->vertex[1] ] );
		VectorMath::Copy( triangleGeometry.vertex[2], convexHull[ triangle->vertex[2] ] );

		render.DrawTriangle( triangleGeometry );

		triangle = ( VectorMath::ConvexHull::Triangle* )triangle->Right();
	}
	*/

	// Draw the point-cloud data.
	SamplePointData* samplePointData = ( SamplePointData* )samplePointCloudData.LeftMost();
	while( samplePointData )
	{
		static double threshold = 10.0;
		if( samplePointData->scalar < threshold )
			render.DrawSphere( samplePointData->point, 0.1f, GAVisToolRender::RES_LOW );

		samplePointData = ( SamplePointData* )samplePointData->Right();
	}
}

//=========================================================================================
void QuadricGeometry::RegeneratePointCloudData( void )
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;
	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivectorNumber = number->Cast< CalcLib::MultivectorNumber >();
	
	multivectorNumber->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "B", *number );

	// Sample the entire sampling region at the desired resolution.  This is where we'll be spending most of the time!
	samplePointCloudData.RemoveAll( true );
	VectorMath::Vector samplePoint;
	for( samplePoint.x = samplingRegion.min.x; samplePoint.x < samplingRegion.max.x; samplePoint.x += samplingResolution.x )
		for( samplePoint.y = samplingRegion.min.y; samplePoint.y < samplingRegion.max.y; samplePoint.y += samplingResolution.y )
			for( samplePoint.z = samplingRegion.min.z; samplePoint.z < samplingRegion.max.z; samplePoint.z += samplingResolution.z )
				samplePointCloudData.InsertRightOf( samplePointCloudData.RightMost(), TestSamplePoint( samplePoint, gaEnv ) );

	// Clean up our mess.
	delete number;
}

//=========================================================================================
QuadricGeometry::SamplePointData* QuadricGeometry::TestSamplePoint( const VectorMath::Vector& samplePoint, CalcLib::GeometricAlgebraEnvironment& gaEnv )
{
	CalcLib::Number* number = gaEnv.CreateNumber();
	
	number->AssignFrom( samplePoint.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	number->AssignFrom( samplePoint.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	number->AssignFrom( samplePoint.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );

	samplePointTestEvaluator->EvaluateResult( *number, gaEnv );

	GeometricAlgebra::SumOfBlades sumOfBlades;
	CalcLib::MultivectorNumber* multivectorNumber = number->Cast< CalcLib::MultivectorNumber >();
	multivectorNumber->AssignTo( sumOfBlades, gaEnv );
	delete number;

	ScalarAlgebra::Scalar squareMagnitude;
	sumOfBlades.AssignSquareMagnitudeTo( squareMagnitude );

	double scalar = sqrt( squareMagnitude );
	return new SamplePointData( samplePoint, scalar );
}

//=========================================================================================
QuadricGeometry::SamplePointData::SamplePointData( const VectorMath::Vector& point, double scalar )
{
	VectorMath::Copy( this->point, point );
	this->scalar = scalar;
}

//=========================================================================================
/*virtual*/ QuadricGeometry::SamplePointData::~SamplePointData( void )
{
}

//=========================================================================================
/*virtual*/ Utilities::List::SortComparison QuadricGeometry::SamplePointData::SortCompare( const Utilities::List::Item* compareWithItem ) const
{
	SamplePointData* samplePointData = ( SamplePointData* )compareWithItem;
	if( scalar < samplePointData->scalar )
		return Utilities::List::SORT_COMPARE_LESS_THAN;
	else if( scalar > samplePointData->scalar )
		return Utilities::List::SORT_COMPARE_GREATER_THAN;
	return Utilities::List::SORT_COMPARE_EQUAL_TO;
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Zero( center );
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Translate( const VectorMath::Vector& delta )
{
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
}

//=========================================================================================
/*virtual*/ void QuadricGeometry::Scale( float scale )
{
}

// QuadricGeoemtry.cpp