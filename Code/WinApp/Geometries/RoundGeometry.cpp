// RoundGeometry.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "RoundGeometry.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ConformalPoint, GAVisToolGeometry );

//=========================================================================================
ConformalPoint::ConformalPoint( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Zero( center );
	weight = 1.0;

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"%s"
		"w = scalar_part( pt, no ),"
		"pt = pt / w,"
		"x = scalar_part( pt, e0 ),"
		"y = scalar_part( pt, e1 ),"
		"z = scalar_part( pt, e2 )"
		")",
		( bindType == NORMAL_FORM ? "pt = pt*I," : "" )
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"do("
		"v = x*e0 + y*e1 + z*e2,"
		"pt = w*(no + v + 0.5*(v . v)*ni)%s"
		")",
		( bindType == NORMAL_FORM ? "*-I" : "" )
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ConformalPoint::~ConformalPoint( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ConformalPoint::Create( BindType bindType )
{
	return new ConformalPoint( bindType );
}

//=========================================================================================
/*virtual*/ void ConformalPoint::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	// Use a locally scoped environment so that we don't pollute the name-space in the user's GA environment.
	CalcLib::GeometricAlgebraEnvironment gaEnv;
	
	// Create a number we can use.
	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	// Stuff the given element into a variable as input to our decomposition script.
	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "pt", *number );

	// Execute the pre-compiled decomposition script.
	decompositionEvaluator->EvaluateResult( *number, gaEnv );
	
	// Collect the output results of the script.
	gaEnv.LookupVariable( "w", *number );
	multivector->AssignTo( weight, gaEnv );
	gaEnv.LookupVariable( "x", *number );
	multivector->AssignTo( center.x, gaEnv );
	gaEnv.LookupVariable( "y", *number );
	multivector->AssignTo( center.y, gaEnv );
	gaEnv.LookupVariable( "z", *number );
	multivector->AssignTo( center.z, gaEnv );

	// We're done with this.
	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalPoint::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	// Use a locally scoped environment so that we don't pollute the name-space in the user's GA environment.
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	// Create a number we can use.
	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	// Stuff our internal representation into the environment as input to our composition script.
	number->AssignFrom( weight, gaEnv );
	gaEnv.StoreVariable( "w", *number );
	number->AssignFrom( center.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	number->AssignFrom( center.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	number->AssignFrom( center.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );

	// Execute the pre-compiled composition script.
	compositionEvaluator->EvaluateResult( *number, gaEnv );

	// Collect the output result of the script.
	gaEnv.LookupVariable( "pt", *number );
	multivector->AssignTo( element, gaEnv );

	// We're done with this.
	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalPoint::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a conformal %s.\n"
			"Weight: %f\n"
			"Position: < %f, %f, %f >\n",
			name,
			( bindType == NORMAL_FORM ? "dual point" : "point" ),
			weight,
			center.x, center.y, center.z );
}

//=========================================================================================
/*virtual*/ void ConformalPoint::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Point: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void ConformalPoint::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	// Points are small spheres so they should always be drawn at low resolution despite user settings.
	render.DrawSphere( center, 0.25f, GAVisToolRender::RES_LOW );
}

//=========================================================================================
/*virtual*/ void ConformalPoint::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, this->center );
}

//=========================================================================================
/*virtual*/ void ConformalPoint::NameCenterOffset( VectorMath::Vector& offsetDelta )
{
	VectorMath::Set( offsetDelta, 1.0, -1.0, 0.0 );
}

//=========================================================================================
/*virtual*/ void ConformalPoint::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( center, center, delta );
}

//=========================================================================================
/*virtual*/ void ConformalPoint::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	// Points don't rotate.
}

//=========================================================================================
/*virtual*/ void ConformalPoint::Scale( float scale )
{
	// Points don't scale.
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ConformalSphere, GAVisToolGeometry );

//=========================================================================================
ConformalSphere::ConformalSphere( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Zero( center );
	radius = 1.0;
	weight = 1.0;
	imaginary = false;

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"%s"
		"w = scalar_part( sph, no ),"
		"sph = sph / w,"
		"x = scalar_part( sph, e0 ),"
		"y = scalar_part( sph, e1 ),"
		"z = scalar_part( sph, e2 ),"
		"v = ( sph^no^ni ).( no^ni ),"
		"r2 = 2*( sph^no^i )*I + v . v,"
		"r = sqrt( abs( r2 ) )"
		")",
		( bindType == NORMAL_FORM ? "sph = sph*I," : "" )
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"do("
		"v = x*e0 + y*e1 + z*e2,"
		"sph = w*( no + v + 0.5*(v*v - scale*r*r)*ni )%s"
		")",
		( bindType == NORMAL_FORM ? "*-I" : "" )
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ConformalSphere::~ConformalSphere( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ConformalSphere::Create( BindType bindType )
{
	return new ConformalSphere( bindType );
}

//=========================================================================================
/*virtual*/ void ConformalSphere::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "sph", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "w", *number );
	multivector->AssignTo( weight, gaEnv );
	gaEnv.LookupVariable( "r", *number );
	multivector->AssignTo( radius, gaEnv );
	gaEnv.LookupVariable( "x", *number );
	multivector->AssignTo( center.x, gaEnv );
	gaEnv.LookupVariable( "y", *number );
	multivector->AssignTo( center.y, gaEnv );
	gaEnv.LookupVariable( "z", *number );
	multivector->AssignTo( center.z, gaEnv );

	double r2;
	gaEnv.LookupVariable( "r2", *number );
	multivector->AssignTo( r2, gaEnv );
	if( r2 < 0.0 )
		imaginary = true;
	else
		imaginary = false;

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalSphere::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	double scalar = 1.0;
	if( imaginary )
		scalar = -1.0;

	number->AssignFrom( scalar, gaEnv );
	gaEnv.StoreVariable( "scale", *number );
	number->AssignFrom( weight, gaEnv );
	gaEnv.StoreVariable( "w", *number );
	number->AssignFrom( radius, gaEnv );
	gaEnv.StoreVariable( "r", *number );
	number->AssignFrom( center.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	number->AssignFrom( center.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	number->AssignFrom( center.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );

	compositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "sph", *number );
	multivector->AssignTo( element, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalSphere::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a conformal %s.\n"
			"Weight: %f\n"
			"Position: < %f, %f, %f >\n"
			"Radius: %f\n"
			"Imaginary: %s\n",
			name,
			( bindType == NORMAL_FORM ? "dual sphere" : "sphere" ),
			weight,
			center.x, center.y, center.z,
			radius,
			( imaginary ? "yes" : "no" ) );
}

//=========================================================================================
/*virtual*/ void ConformalSphere::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Sphere: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
// TODO: Draw differently if imaginary.
/*virtual*/ void ConformalSphere::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	render.DrawSphere( center, radius, GAVisToolRender::RES_USER );

	if( render.GetRenderMode() != GAVisToolRender::RENDER_MODE_SELECTION )
	{
		double goldenRatio = ( 1.0 + sqrt( 5.0 ) ) * 0.5;
		VectorMath::Vector unitSpherePoint[6];
		VectorMath::Set( unitSpherePoint[0], 0.0, 0.0, goldenRatio );
		VectorMath::Set( unitSpherePoint[1], 0.0, goldenRatio, 0.0 );
		VectorMath::Set( unitSpherePoint[2], goldenRatio, 0.0, 0.0 );
		VectorMath::Set( unitSpherePoint[3], 0.5, goldenRatio * 0.5, ( 1.0 + goldenRatio ) * 0.5 );
		VectorMath::Set( unitSpherePoint[4], goldenRatio * 0.5, ( 1.0 + goldenRatio ) * 0.5, 0.5 );
		VectorMath::Set( unitSpherePoint[5], ( 1.0 + goldenRatio ) * 0.5, 0.5, goldenRatio * 0.5 );

		for( int index = 0; index < 6; index++ )
			VectorMath::Normalize( unitSpherePoint[ index ], unitSpherePoint[ index ] );

		// Draw the "spikes" of the conformal sphere using the vertices of an icosidodecahedron.
		for( int count = 0; count < 8; count++ )
		{
			for( int index = 0; index < 6; index++ )
			{
				VectorMath::Vector spherePoint;
				VectorMath::Copy( spherePoint, unitSpherePoint[ index ] );
				if( count & 1 )
					spherePoint.x *= -1.0;
				if( count & 2 )
					spherePoint.y *= -1.0;
				if( count & 4 )
					spherePoint.z *= -1.0;

				VectorMath::Vector vec;
				VectorMath::Copy( vec, spherePoint );
				if( weight < 0.0 )
					VectorMath::Scale( vec, vec, -1.0 );
				VectorMath::Scale( spherePoint, spherePoint, radius );
				VectorMath::Add( spherePoint, spherePoint, center );

				// The vectors on a sphere are small so they should always be drawn at low resolution despite the user setting.
				render.DrawVector( spherePoint, vec, GAVisToolRender::RES_LOW );
			}
		}
	}
}

//=========================================================================================
/*virtual*/ void ConformalSphere::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, this->center );
}

//=========================================================================================
/*virtual*/ void ConformalSphere::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( center, center, delta );
}

//=========================================================================================
/*virtual*/ void ConformalSphere::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	// Sphere's don't rotate.
}

//=========================================================================================
/*virtual*/ void ConformalSphere::Scale( float scale )
{
	radius *= scale;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ConformalCircle, GAVisToolGeometry );

//=========================================================================================
ConformalCircle::ConformalCircle( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Zero( center );
	VectorMath::Set( unitNormal, 0.0, 0.0, 1.0 );
	radius = 1.0;
	weight = 1.0;
	imaginary = false;

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"%s"
		"n = grade_part( 1, ( no^ni ) . ( cir^ni ) ),"
		"w = sqrt( n . n ),"
		"n = n / w,"
		"cir = cir / w,"
		"v = grade_part( 1, n*( ( no^ni ) . ( cir^( no*ni ) ) ) ),"
		"r2 = grade_part( 0, v . v - 2*n*( ( v . n )*v - ( no^ni ) . ( no^cir ) ) ),"
		"r = sqrt( abs( r2 ) ),"
		"x = scalar_part( v, e0 ),"
		"y = scalar_part( v, e1 ),"
		"z = scalar_part( v, e2 ),"
		"nx = scalar_part( n, e0 ),"
		"ny = scalar_part( n, e1 ),"
		"nz = scalar_part( n, e2 )"
		")",
		( bindType == NORMAL_FORM ? "cir = cir*I," : "" )
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"do("
		"n = nx*e0 + ny*e1 + nz*e2,"
		"v = x*e0 + y*e1 + z*e2,"
		"cir = w*( ( n + ( v . n )*ni )^( no + v + 0.5*( v . v - scale*r*r )*ni ) )%s"
		")",
		( bindType == NORMAL_FORM ? "*-I" : "" )
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ConformalCircle::~ConformalCircle( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ConformalCircle::Create( BindType bindType )
{
	return new ConformalCircle( bindType );
}

//=========================================================================================
/*virtual*/ void ConformalCircle::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "cir", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "w", *number );
	multivector->AssignTo( weight, gaEnv );
	gaEnv.LookupVariable( "r", *number );
	multivector->AssignTo( radius, gaEnv );
	gaEnv.LookupVariable( "x", *number );
	multivector->AssignTo( center.x, gaEnv );
	gaEnv.LookupVariable( "y", *number );
	multivector->AssignTo( center.y, gaEnv );
	gaEnv.LookupVariable( "z", *number );
	multivector->AssignTo( center.z, gaEnv );
	gaEnv.LookupVariable( "nx", *number );
	multivector->AssignTo( unitNormal.x, gaEnv );
	gaEnv.LookupVariable( "ny", *number );
	multivector->AssignTo( unitNormal.y, gaEnv );
	gaEnv.LookupVariable( "nz", *number );
	multivector->AssignTo( unitNormal.z, gaEnv );

	double r2;
	gaEnv.LookupVariable( "r2", *number );
	multivector->AssignTo( r2, gaEnv );
	if( r2 < 0.0 )
		imaginary = true;
	else
		imaginary = false;

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalCircle::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	double scalar = 1.0;
	if( imaginary )
		scalar = -1.0;

	number->AssignFrom( scalar, gaEnv );
	gaEnv.StoreVariable( "scale", *number );
	number->AssignFrom( weight, gaEnv );
	gaEnv.StoreVariable( "w", *number );
	number->AssignFrom( radius, gaEnv );
	gaEnv.StoreVariable( "r", *number );
	number->AssignFrom( center.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	number->AssignFrom( center.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	number->AssignFrom( center.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );
	number->AssignFrom( unitNormal.x, gaEnv );
	gaEnv.StoreVariable( "nx", *number );
	number->AssignFrom( unitNormal.y, gaEnv );
	gaEnv.StoreVariable( "ny", *number );
	number->AssignFrom( unitNormal.z, gaEnv );
	gaEnv.StoreVariable( "nz", *number );

	compositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "cir", *number );
	multivector->AssignTo( element, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalCircle::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a conformal %s.\n"
			"Weight: %f\n"
			"Position: < %f, %f, %f >\n"
			"Radius: %f\n"
			"Normal: < %f, %f, %f >\n"
			"Imaginary: %s\n"
			"Tangent point: %s\n",
			name,
			( bindType == NORMAL_FORM ? "dual circle" : "circle" ),
			weight,
			center.x, center.y, center.z,
			radius,
			unitNormal.x, unitNormal.y, unitNormal.z,
			( imaginary ? "yes" : "no" ),
			( radius == 0.0 ? "yes" : "no" ) );
}

//=========================================================================================
/*virtual*/ void ConformalCircle::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Circle: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
// TODO: Draw differently if imaginary.
/*virtual*/ void ConformalCircle::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	// Draw as a tangent point when the radius is zero.
	if( radius > 0.0 )
		render.DrawTorus( center, unitNormal, radius );
	else
		render.DrawSphere( center, 0.25, GAVisToolRender::RES_LOW );

	if( render.GetRenderMode() != GAVisToolRender::RENDER_MODE_SELECTION )
		render.DrawVector( center, unitNormal );
}

//=========================================================================================
/*virtual*/ void ConformalCircle::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, this->center );
}

//=========================================================================================
/*virtual*/ void ConformalCircle::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( center, center, delta );
}

//=========================================================================================
/*virtual*/ void ConformalCircle::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	VectorMath::Rotate( unitNormal, unitNormal, unitAxis, angle );
	VectorMath::Normalize( unitNormal, unitNormal );		// Re-normalize to fix round-off error.
}

//=========================================================================================
/*virtual*/ void ConformalCircle::Scale( float scale )
{
	radius *= scale;
}

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( ConformalPointPair, GAVisToolGeometry );

//=========================================================================================
ConformalPointPair::ConformalPointPair( BindType bindType ) : GAVisToolGeometry( bindType )
{
	VectorMath::Zero( center );
	VectorMath::Set( unitNormal, 0.0, 0.0, 1.0 );
	radius = 1.0;
	weight = 1.0;
	imaginary = false;

	CalcLib::Calculator calculator( "geoalg" );

	char decompositionCode[ 512 ];
	sprintf_s( decompositionCode, sizeof( decompositionCode ),
		"do("
		"%s"
		"n = -( ( no^ni ) . ( ppr^ni ) )*i,"
		"w = sqrt( n . n ),"
		"n = n / w,"
		"ppr = ppr / w,"
		"v = -n*( ( no^ni ) . ( ppr^( no*ni ) ) )*i,"
		"r2 = -v . v + 2*n*( ( v . n )*v + ( ( no^ni ) . ( no^ppr ) )*i ),"
		"r2 = grade_part( 0, r2 ),"		// Kill any round-off error.
		"r = sqrt( abs( r2 ) ),"
		"x = scalar_part( v, e0 ),"
		"y = scalar_part( v, e1 ),"
		"z = scalar_part( v, e2 ),"
		"nx = scalar_part( n, e0 ),"
		"ny = scalar_part( n, e1 ),"
		"nz = scalar_part( n, e2 )"
		")",
		( bindType == NORMAL_FORM ? "ppr = ppr*I," : "" )
	);
	decompositionEvaluator = calculator.CompileEvaluator( decompositionCode );
	wxASSERT( decompositionEvaluator != 0 );

	char compositionCode[ 512 ];
	sprintf_s( compositionCode, sizeof( compositionCode ),
		"do("
		"n = nx*e0 + ny*e1 + nz*e2,"
		"v = x*e0 + y*e1 + z*e2,"
		"ppr = w*( ( n*i - ( v . ( n*i ) )*ni ) ^ ( no + v + 0.5*( v*v - scale*r*r )*ni ) )%s"
		")",
		( bindType == NORMAL_FORM ? "*-I" : "" )
	);
	compositionEvaluator = calculator.CompileEvaluator( compositionCode );
	wxASSERT( compositionEvaluator != 0 );
}

//=========================================================================================
/*virtual*/ ConformalPointPair::~ConformalPointPair( void )
{
}

//=========================================================================================
/*static*/ GAVisToolBindTarget* ConformalPointPair::Create( BindType bindType )
{
	return new ConformalPointPair( bindType );
}

//=========================================================================================
/*virtual*/ void ConformalPointPair::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	multivector->AssignFrom( element, gaEnv );
	gaEnv.StoreVariable( "ppr", *number );

	decompositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "w", *number );
	multivector->AssignTo( weight, gaEnv );
	gaEnv.LookupVariable( "r", *number );
	multivector->AssignTo( radius, gaEnv );
	gaEnv.LookupVariable( "x", *number );
	multivector->AssignTo( center.x, gaEnv );
	gaEnv.LookupVariable( "y", *number );
	multivector->AssignTo( center.y, gaEnv );
	gaEnv.LookupVariable( "z", *number );
	multivector->AssignTo( center.z, gaEnv );
	gaEnv.LookupVariable( "nx", *number );
	multivector->AssignTo( unitNormal.x, gaEnv );
	gaEnv.LookupVariable( "ny", *number );
	multivector->AssignTo( unitNormal.y, gaEnv );
	gaEnv.LookupVariable( "nz", *number );
	multivector->AssignTo( unitNormal.z, gaEnv );

	double r2;
	gaEnv.LookupVariable( "r2", *number );
	multivector->AssignTo( r2, gaEnv );
	if( r2 < 0.0 )
		imaginary = true;
	else
		imaginary = false;

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalPointPair::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
	CalcLib::GeometricAlgebraEnvironment gaEnv;

	CalcLib::Number* number = gaEnv.CreateNumber();
	CalcLib::MultivectorNumber* multivector = ( CalcLib::MultivectorNumber* )number;

	double scalar = 1.0;
	if( imaginary )
		scalar = -1.0;

	number->AssignFrom( scalar, gaEnv );
	gaEnv.StoreVariable( "scale", *number );
	number->AssignFrom( weight, gaEnv );
	gaEnv.StoreVariable( "w", *number );
	number->AssignFrom( radius, gaEnv );
	gaEnv.StoreVariable( "r", *number );
	number->AssignFrom( center.x, gaEnv );
	gaEnv.StoreVariable( "x", *number );
	number->AssignFrom( center.y, gaEnv );
	gaEnv.StoreVariable( "y", *number );
	number->AssignFrom( center.z, gaEnv );
	gaEnv.StoreVariable( "z", *number );
	number->AssignFrom( unitNormal.x, gaEnv );
	gaEnv.StoreVariable( "nx", *number );
	number->AssignFrom( unitNormal.y, gaEnv );
	gaEnv.StoreVariable( "ny", *number );
	number->AssignFrom( unitNormal.z, gaEnv );
	gaEnv.StoreVariable( "nz", *number );

	compositionEvaluator->EvaluateResult( *number, gaEnv );

	gaEnv.LookupVariable( "ppr", *number );
	multivector->AssignTo( element, gaEnv );

	delete number;
}

//=========================================================================================
/*virtual*/ void ConformalPointPair::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	sprintf_s( printBuffer, printBufferSize,
			"The variable \"%s\" is being interpreted as a conformal %s.\n"
			"Weight: %f\n"
			"Position: < %f, %f, %f >\n"
			"Radius: %f\n"
			"Normal: < %f, %f, %f >\n"
			"Imaginary: %s\n"
			"Tangent point: %s\n",
			name,
			( bindType == NORMAL_FORM ? "dual point-pair" : "point-pair" ),
			weight,
			center.x, center.y, center.z,
			radius,
			unitNormal.x, unitNormal.y, unitNormal.z,
			( imaginary ? "yes" : "no" ),
			( radius == 0.0 ? "yes" : "no" ) );
}

//=========================================================================================
/*virtual*/ void ConformalPointPair::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
	wxString itemName = wxString::Format( wxT( "Point-pair: %s" ), name );
	treeCtrl->AppendItem( parentItem, itemName, -1, -1, new GAVisToolInventoryTree::Data( id ) );
}

//=========================================================================================
/*virtual*/ void ConformalPointPair::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Highlight( GAVisToolRender::NORMAL_HIGHLIGHTING );
	else
		render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );
	render.Color( color, alpha );

	VectorMath::Vector point[2];
	VectorMath::Vector vec;
	VectorMath::Scale( vec, unitNormal, radius );
	VectorMath::Add( point[0], center, vec );
	VectorMath::Scale( vec, vec, -1.0 );
	VectorMath::Add( point[1], center, vec );

	render.DrawSphere( point[0], 0.25f, GAVisToolRender::RES_LOW );
	if( radius > 0.0 )
		render.DrawSphere( point[1], 0.25f, GAVisToolRender::RES_LOW );

	if( render.GetRenderMode() != GAVisToolRender::RENDER_MODE_SELECTION )
	{
		// Draw as a tangent point when the radius is zero.
		render.DrawVector( point[0], unitNormal );
		if( radius > 0.0 )
			render.DrawVector( point[1], unitNormal );
	}
}

//=========================================================================================
/*virtual*/ void ConformalPointPair::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::Copy( center, this->center );
}

//=========================================================================================
/*virtual*/ void ConformalPointPair::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( center, center, delta );
}

//=========================================================================================
/*virtual*/ void ConformalPointPair::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	VectorMath::Rotate( unitNormal, unitNormal, unitAxis, angle );
	VectorMath::Normalize( unitNormal, unitNormal );		// Fix any round-off error.
}

//=========================================================================================
/*virtual*/ void ConformalPointPair::Scale( float scale )
{
	radius *= scale;
}

// RoundGeometry.cpp