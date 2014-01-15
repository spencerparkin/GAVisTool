// PointSampledGeometry.cpp

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
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
}

//=========================================================================================
/*virtual*/ PointSampledGeometry::~PointSampledGeometry( void )
{
	pointList.RemoveAll( true );
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* PointSampledGeometry::Create( BindType bindType )
{
	return new PointSampledGeometry( bindType );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	surface.element.AssignSumOfBlades( element );

	pointList.RemoveAll( true );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	element.AssignSumOfBlades( surface.element );
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
	RegeneratePointListIfNeeded();

	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );

	render.Color( color, alpha );

	VectorMath::Vector normal;
	VectorMath::Set( normal, 0.0, 0.0, 1.0 );
	for( Point* point = ( Point* )pointList.LeftMost(); point; point = ( Point* )point->Right() )
		render.DrawPoint( point->point, normal );
}

//=========================================================================================
/*virtual*/ void PointSampledGeometry::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Zero( center );
	for( Point* point = ( Point* )pointList.LeftMost(); point; point = ( Point* )point->Right() )
		VectorMath::Add( center, center, point->point );
	if( pointList.Count() > 0 )
		VectorMath::Scale( center, center, 1.0 / double( pointList.Count() ) );
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

//=========================================================================================
void PointSampledGeometry::RegeneratePointListIfNeeded( void )
{
	if( pointList.Count() > 0 )
		return;

	double angleStepCount = 40.0;
	double deltaTheta = 2.0 * M_PI / angleStepCount;
	double deltaPhi = M_PI / angleStepCount;
	double radius = 5.0;

	for( double theta = 0.0; theta < 2.0 * M_PI; theta += deltaTheta )
	{
		double cosTheta = cos( theta );
		double sinTheta = sin( theta );

		for( double phi = 0.0; phi < M_PI; phi += deltaPhi )
		{
			double cosPhi = cos( phi );
			double sinPhi = sin( phi );

			Point* point = new Point();

			VectorMath::Set( point->point, sinPhi * cosTheta, sinPhi * sinTheta, cosPhi );
			VectorMath::Scale( point->point, point->point, radius );

			pointList.InsertRightOf( pointList.RightMost(), point );
		}
	}

	// TODO: Throw up progress dialog box?
	double epsilon = 1e-7;
	Point* rightPoint = 0;
	for( Point* point = ( Point* )pointList.LeftMost(); point; point = rightPoint )
	{
		rightPoint = ( Point* )point->Right();
		if( !surface.ConvergePointToSurfaceInPlane( 0, point->point, epsilon ) )
			pointList.Remove( point, true );
	}
}

//=========================================================================================
PointSampledGeometry::Surface::Surface( void ) : calculator( "geoalg" )
{
	char code[512];
	
	sprintf_s( code, sizeof( code ),
		"do"
		"("
			"dot = "
			"("
				"e0 + "
				"x*e1 + "
				"y*e2 + "
				"z*e3 + "
				"y*z*e4 + "
				"z*x*e5 + "
				"x*y*e6 + "
				"x*x*e7 + "
				"y*y*e8 + "
				"z*z*e9"
			") . E,"
			"result = scalar_part( dot * dot ),"
		")" );
	functionEvaluator = calculator.CompileEvaluator( code );
	wxASSERT( functionEvaluator != 0 );

	sprintf_s( code, sizeof( code ),
		"do"
		"("
			"dot = "
			"("
				"e0 + "
				"x*e1 + "
				"y*e2 + "
				"z*e3 + "
				"y*z*e4 + "
				"z*x*e5 + "
				"x*y*e6 + "
				"x*x*e7 + "
				"y*y*e8 + "
				"z*z*e9"
			") . E,"
			"partial_dot = "
			"("
				"e1 + "
				"z*e5 + "
				"y*e6 + "
				"2*x*e7"
			") . E,"
			"result = 2 * scalar_part( dot * partial_dot ),"
		")" );
	functionPartialXEvaluator = calculator.CompileEvaluator( code );
	wxASSERT( functionPartialXEvaluator != 0 ); 

	sprintf_s( code, sizeof( code ),
		"do"
		"("
			"dot = "
			"("
				"e0 + "
				"x*e1 + "
				"y*e2 + "
				"z*e3 + "
				"y*z*e4 + "
				"z*x*e5 + "
				"x*y*e6 + "
				"x*x*e7 + "
				"y*y*e8 + "
				"z*z*e9"
			") . E,"
			"partial_dot = "
			"("
				"e2 + "
				"z*e4 + "
				"x*e6 + "
				"2*y*e8"
			") . E,"
			"result = 2 * scalar_part( dot * partial_dot ),"
		")" );
	functionPartialYEvaluator = calculator.CompileEvaluator( code );
	wxASSERT( functionPartialYEvaluator != 0 );

	sprintf_s( code, sizeof( code ),
		"do"
		"("
			"dot = "
			"("
				"e0 + "
				"x*e1 + "
				"y*e2 + "
				"z*e3 + "
				"y*z*e4 + "
				"z*x*e5 + "
				"x*y*e6 + "
				"x*x*e7 + "
				"y*y*e8 + "
				"z*z*e9"
			") . E,"
			"partial_dot = "
			"("
				"e3 + "
				"y*e4 + "
				"x*e5 + "
				"2*z*e9"
			") . E,"
			"result = 2 * scalar_part( dot * partial_dot ),"
		")" );
	functionPartialZEvaluator = calculator.CompileEvaluator( code );
	wxASSERT( functionPartialZEvaluator != 0 );

	CalcLib::GeometricAlgebraEnvironment* gaEnv = ( CalcLib::GeometricAlgebraEnvironment* )calculator.GetEnvironment();
	number = gaEnv->CreateNumber();
}

//=========================================================================================
/*virtual*/ PointSampledGeometry::Surface::~Surface( void )
{
	delete functionEvaluator;
	delete functionPartialXEvaluator;
	delete functionPartialYEvaluator;
	delete functionPartialZEvaluator;
	delete number;
}

//=========================================================================================
/*virtual*/ double PointSampledGeometry::Surface::EvaluateAt( const VectorMath::Vector& point ) const
{
	return EvaluateWith( functionEvaluator, point );
}

//=========================================================================================
/*virtual*/ double PointSampledGeometry::Surface::EvaluatePartialX( const VectorMath::Vector& point ) const
{
	return EvaluateWith( functionPartialXEvaluator, point );
}

//=========================================================================================
/*virtual*/ double PointSampledGeometry::Surface::EvaluatePartialY( const VectorMath::Vector& point ) const
{
	return EvaluateWith( functionPartialYEvaluator, point );
}

//=========================================================================================
/*virtual*/ double PointSampledGeometry::Surface::EvaluatePartialZ( const VectorMath::Vector& point ) const
{
	return EvaluateWith( functionPartialZEvaluator, point );
}

//=========================================================================================
double PointSampledGeometry::Surface::EvaluateWith( CalcLib::Evaluator* evaluator, const VectorMath::Vector& point ) const
{
	CalcLib::GeometricAlgebraEnvironment* gaEnv = ( CalcLib::GeometricAlgebraEnvironment* )calculator.GetEnvironment();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, *gaEnv );
	gaEnv->StoreVariable( "E", *number );
	multivector->AssignFrom( point.x, *gaEnv );
	gaEnv->StoreVariable( "x", *number );
	multivector->AssignFrom( point.y, *gaEnv );
	gaEnv->StoreVariable( "y", *number );
	multivector->AssignFrom( point.z, *gaEnv );
	gaEnv->StoreVariable( "z", *number );

	evaluator->EvaluateResult( *number, *gaEnv );
	
	double result;
	gaEnv->LookupVariable( "result", *number );
	multivector->AssignTo( result, *gaEnv );
	return result;
}

// PointSampledGeometry.cpp