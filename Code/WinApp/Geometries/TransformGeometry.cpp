// TransformGeometry.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "TransformGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ConformalTransformGeometry, GAVisToolGeometry );

//=========================================================================================
ConformalTransformGeometry::ConformalTransformGeometry( BindType bindType ) : GAVisToolGeometry( bindType )
{
	scale = 1.0;
	angle = 0.0;
	VectorMath::Set( unitAxis, 1.0, 0.0, 0.0 );
	VectorMath::Zero( translation );

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"w2 = V * V~,"
		"w2 = grade_part( 0, w2 ),"		// Kill the round-off error.
		"w = sqrt( w2 ),"
		"V = V / w,"
		"R = -no . ( V^ni ),"
		"T = V*R~,"
		"half_theta = acos( grade_part( 0, R ) ),"
		"sin_half_theta = sin( half_theta ),"
		"a = if( sin_half_theta == 0, e0, grade_part( 2, R )*i / sin_half_theta ),"
		"theta = 2*half_theta,"
		"t = 2*( no . ( 1 - T ) ),"
		"ax = scalar_part( a, e1 ),"
		"ay = scalar_part( a, e2 ),"
		"az = scalar_part( a, e3 ),"
		"tx = scalar_part( t, e1 ),"
		"ty = scalar_part( t, e2 ),"
		"tz = scalar_part( t, e3 )"
		")"
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"do("
		"a = ax*e1 + ay*e2 + az*e3,"
		"t = tx*e1 + ty*e2 + tz*e3,"
		"half_theta = 0.5*theta,"
		"V = w*( 1 - 0.5*t*ni )*( cos( half_theta ) - a*i*sin( half_theta ) )"
		")"
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ConformalTransformGeometry::~ConformalTransformGeometry( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ConformalTransformGeometry::Create( BindType bindType )
{
	return new ConformalTransformGeometry( bindType );
}

//=========================================================================================
/*virtual*/ void ConformalTransformGeometry::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "V", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "w", *number );
	multivector->AssignTo( scale, gaEnv );
	gaEnv.LookupVariable( "theta", *number );
	multivector->AssignTo( angle, gaEnv );
	gaEnv.LookupVariable( "ax", *number );
	multivector->AssignTo( unitAxis.x, gaEnv );
	gaEnv.LookupVariable( "ay", *number );
	multivector->AssignTo( unitAxis.y, gaEnv );
	gaEnv.LookupVariable( "az", *number );
	multivector->AssignTo( unitAxis.z, gaEnv );
	gaEnv.LookupVariable( "tx", *number );
	multivector->AssignTo( translation.x, gaEnv );
	gaEnv.LookupVariable( "ty", *number );
	multivector->AssignTo( translation.y, gaEnv );
	gaEnv.LookupVariable( "tz", *number );
	multivector->AssignTo( translation.z, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalTransformGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	number->AssignFrom( scale, gaEnv );
	gaEnv.StoreVariable( "w", *number );
	number->AssignFrom( angle, gaEnv );
	gaEnv.StoreVariable( "theta", *number );
	number->AssignFrom( unitAxis.x, gaEnv );
	gaEnv.StoreVariable( "ax", *number );
	number->AssignFrom( unitAxis.y, gaEnv );
	gaEnv.StoreVariable( "ay", *number );
	number->AssignFrom( unitAxis.z, gaEnv );
	gaEnv.StoreVariable( "az", *number );
	number->AssignFrom( translation.x, gaEnv );
	gaEnv.StoreVariable( "tx", *number );
	number->AssignFrom( translation.y, gaEnv );
	gaEnv.StoreVariable( "ty", *number );
	number->AssignFrom( translation.z, gaEnv );
	gaEnv.StoreVariable( "tz", *number );

	compositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "V", *number );
	multivector->AssignTo( element, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalTransformGeometry::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a conformal transformation.\n"
			"Scale: %f\n"
			"Translation: < %f, %f, %f >\n"
			"Rotation Axis: < %f, %f, %f >\n"
			"Rotation Angle: %f\n",
			name,
			scale,
			translation.x, translation.y, translation.z,
			unitAxis.x, unitAxis.y, unitAxis.z,
			angle );
}

//=========================================================================================
/*virtual*/ void ConformalTransformGeometry::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Transform: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void ConformalTransformGeometry::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	VectorMath::CoordFrame coordFrame;
	VectorMath::MakeRotation( coordFrame, unitAxis, angle );
	VectorMath::Scale( coordFrame, coordFrame, scale );
	
	// TODO: There should be an option to render any geometry we want in place of this coordinate frame.
	render.DrawVector( translation, coordFrame.xAxis, GAVisToolRender::RES_LOW );
	render.DrawVector( translation, coordFrame.yAxis, GAVisToolRender::RES_LOW );
	render.DrawVector( translation, coordFrame.zAxis, GAVisToolRender::RES_LOW );
}

//=========================================================================================
/*virtual*/ void ConformalTransformGeometry::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, translation );
}

//=========================================================================================
/*virtual*/ void ConformalTransformGeometry::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( translation, translation, delta );
}

//=========================================================================================
/*virtual*/ void ConformalTransformGeometry::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	VectorMath::Rotate( this->unitAxis, this->unitAxis, unitAxis, angle );
}

//=========================================================================================
/*virtual*/ void ConformalTransformGeometry::Scale( float scale )
{
	this->scale *= scale;
}

//=========================================================================================
/*virtual*/ void ConformalTransformGeometry::NameCenterOffset( VectorMath::Vector& offsetDelta )
{
	VectorMath::Zero( offsetDelta );
}

// TransformGeometry.cpp