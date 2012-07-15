// Geometry.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Geometry.h"
#include "Application.h"

//=========================================================================================
IMPLEMENT_CALCLIB_CLASS1( GAVisToolGeometry, GAVisToolBindTarget );

//=========================================================================================
GAVisToolGeometry::GAVisToolGeometry( BindType bindType ) : GAVisToolBindTarget( bindType )
{
	decompositionEvaluator = 0;
	compositionEvaluator = 0;

	nameTexture = GL_INVALID_VALUE;

	VectorMath::Set( color, 0.0, 1.0, 1.0 );
	alpha = 0.5;
}

//=========================================================================================
/*virtual*/ GAVisToolGeometry::~GAVisToolGeometry( void )
{
	if( decompositionEvaluator )
		delete decompositionEvaluator;
	if( compositionEvaluator )
		delete compositionEvaluator;

	if( nameTexture != GL_INVALID_VALUE )
		glDeleteTextures( 1, &nameTexture );
}

//=========================================================================================
// Sort by distance to the camera.
/*virtual*/ Utilities::List::SortComparison GAVisToolGeometry::SortCompare( const Utilities::List::Item* compareWithItem ) const
{
	GAVisToolBindTarget* bindTarget = ( GAVisToolBindTarget* )compareWithItem;
	if( !bindTarget->IsTypeOf( GAVisToolGeometry::ClassName() ) )
		return Utilities::List::SORT_COMPARE_GREATER_THAN;
	
	GAVisToolGeometry* geometry = ( GAVisToolGeometry* )compareWithItem;

	VectorMath::Vector thisCenter, givenCenter;
	CalcCenter( thisCenter );
	geometry->CalcCenter( givenCenter );

	const VectorMath::Vector& cameraEye = wxGetApp().canvasFrame->canvas->camera.Eye();
	VectorMath::Vector thisDelta, givenDelta;
	VectorMath::Sub( thisDelta, thisCenter, cameraEye );
	VectorMath::Sub( givenDelta, givenCenter, cameraEye );

	double thisSquareDist = VectorMath::Dot( thisDelta, thisDelta );
	double givenSquareDist = VectorMath::Dot( givenDelta, givenDelta );

	// Pretend that opaque objects are very far away so that they draw
	// before any objects that have some transparency in them.  We want
	// alpha-blend objects to blend in with the opaque objects.
	if( IsOpaque() )
		thisSquareDist = -1000.0;
	if( geometry->IsOpaque() )
		givenSquareDist = -1000.0;

	if( thisSquareDist < givenSquareDist )
		return Utilities::List::SORT_COMPARE_LESS_THAN;
	else if( thisSquareDist > givenSquareDist )
		return Utilities::List::SORT_COMPARE_GREATER_THAN;
	return Utilities::List::SORT_COMPARE_EQUAL_TO;
}

//=========================================================================================
/*static*/ float GAVisToolGeometry::WeightAsColorComponent( double weight )
{
	float w = float( weight );
	float min = -10.f;
	float max = 10.f;
	float c = ( w - min ) / ( max - min );
	if( c < -1.f )
		c = -1.f;
	if( c > 1.f )
		c = 1.f;
	return c;
}

//=========================================================================================
void GAVisToolGeometry::GenerateNameTexture( bool regenerate /*= false*/ )
{
	if( nameTexture != GL_INVALID_VALUE )
	{
		if( regenerate )
			glDeleteTextures( 1, &nameTexture );	// Delete it so we can recreate it.
		else
			return;		// We're done!  It's already generated!
	}

	char latexCode[ 512 ];
	sprintf_s( latexCode, sizeof( latexCode ), "\\Huge\\mbox{%s}", name );
	wxGetApp().CreateLatexTexture( latexCode, nameTexture );
}

//=========================================================================================
/*virtual*/ void GAVisToolGeometry::NameCenterOffset( VectorMath::Vector& offsetDelta )
{
	VectorMath::Zero( offsetDelta );
}

//=========================================================================================
// Here we assume that texture mapping is turned on.
void GAVisToolGeometry::DrawName( VectorMath::CoordFrame& cameraFrame )
{
	if( nameTexture == GL_INVALID_VALUE )
		return;

	VectorMath::Vector center;
	CalcCenter( center );

	VectorMath::Vector vertex[4];
	VectorMath::Set( vertex[0], -1.0, -1.0, 0.0 );
	VectorMath::Set( vertex[1], 1.0, -1.0, 0.0 );
	VectorMath::Set( vertex[2], 1.0, 1.0, 0.0 );
	VectorMath::Set( vertex[3], -1.0, 1.0, 0.0 );

	VectorMath::CoordFrame coordFrame;
	VectorMath::Copy( coordFrame.xAxis, cameraFrame.xAxis );
	VectorMath::Copy( coordFrame.yAxis, cameraFrame.yAxis );
	VectorMath::Copy( coordFrame.zAxis, cameraFrame.zAxis );

	int nameLen = strlen( name );
	double halfLen = double( nameLen ) * 0.5;
	if( halfLen > 0.0 )
		VectorMath::Scale( coordFrame.xAxis, coordFrame.xAxis, halfLen );

	VectorMath::Vector offsetDelta;
	NameCenterOffset( offsetDelta );
	VectorMath::Transform( offsetDelta, coordFrame, offsetDelta );
	VectorMath::Add( center, center, offsetDelta );

	VectorMath::Transform( vertex, coordFrame, center, vertex, 4 );

	glBindTexture( GL_TEXTURE_2D, nameTexture );

	glColor3f( 0.f, 0.f, 0.f );
	glBegin( GL_QUADS );

	glTexCoord2d( 0.0, 0.0 );
	glVertex3f( vertex[0].x, vertex[0].y, vertex[0].z );
	
	glTexCoord2d( 1.0, 0.0 );
	glVertex3f( vertex[1].x, vertex[1].y, vertex[1].z );

	glTexCoord2d( 1.0, 1.0 );
	glVertex3f( vertex[2].x, vertex[2].y, vertex[2].z );

	glTexCoord2d( 0.0, 1.0 );
	glVertex3f( vertex[3].x, vertex[3].y, vertex[3].z );

	glEnd();
}

//=========================================================================================
void GAVisToolGeometry::SetColor( const VectorMath::Vector& color )
{
	VectorMath::Copy( this->color, color );
}

//=========================================================================================
void GAVisToolGeometry::GetColor( VectorMath::Vector& color )
{
	VectorMath::Copy( color, this->color );
}

//=========================================================================================
// Don't set the alpha.  The color dialog doesn't have an alpha option.
void GAVisToolGeometry::SetColor( const wxColour& color )
{
	double r = double( color.Red() ) / 255.0;
	double g = double( color.Green() ) / 255.0;
	double b = double( color.Blue() ) / 255.0;
	//alpha = double( color.Alpha() ) / 255.0;
	VectorMath::Set( this->color, r, g, b );
}

//=========================================================================================
void GAVisToolGeometry::GetColor( wxColour& color ) const
{
	wxColour::ChannelType r, g, b, a;
	r = unsigned char( this->color.x * 255.0 );
	g = unsigned char( this->color.y * 255.0 );
	b = unsigned char( this->color.z * 255.0 );
	a = unsigned char( alpha * 255.0 );
	color.Set( r, g, b, a );
}

//=========================================================================================
void GAVisToolGeometry::SetAlpha( double alpha )
{
	this->alpha = alpha;
}

//=========================================================================================
double GAVisToolGeometry::GetAlpha( void ) const
{
	return alpha;
}

//=========================================================================================
bool GAVisToolGeometry::IsOpaque( void ) const
{
	if( alpha == 1.0 )
		return true;
	return false;
}

//=========================================================================================
TestGeometry::TestGeometry( int nameIndex ) : GAVisToolGeometry( NORMAL_FORM )
{
	char geoName[64];
	sprintf_s( geoName, sizeof( geoName ), "TestGeometry%d", nameIndex );
	SetName( geoName );

	if( nameIndex == 0 )
	{
		VectorMath::Set( triangle.vertex[0], 4.0, 5.0, 0.0 );
		VectorMath::Set( triangle.vertex[1], 4.0, -3.0, 4.0 );
		VectorMath::Set( triangle.vertex[2], 4.0, -3.0, -4.0 );
	}
	else
	{
		VectorMath::Set( triangle.vertex[0], -5.0, 0.0, -2.0 );
		VectorMath::Set( triangle.vertex[1], 5.0, 0.0, -2.0 );
		VectorMath::Set( triangle.vertex[2], 0.0, 0.0, 5.0 );
	}
}

//=========================================================================================
/*virtual*/ TestGeometry::~TestGeometry( void )
{
}

//=========================================================================================
/*virtual*/ void TestGeometry::DecomposeFrom( const GeometricAlgebra::SumOfBlades& element )
{
}

//=========================================================================================
/*virtual*/ void TestGeometry::ComposeTo( GeometricAlgebra::SumOfBlades& element ) const
{
}

//=========================================================================================
/*virtual*/ void TestGeometry::DumpInfo( char* printBuffer, int printBufferSize ) const
{
	strcpy_s( printBuffer, printBufferSize, "This is a test." );
}

//=========================================================================================
/*virtual*/ void TestGeometry::AddInventoryTreeItem( wxTreeCtrl* treeCtrl, wxTreeItemId parentItem ) const
{
}

//=========================================================================================
/*virtual*/ void TestGeometry::Draw( GAVisToolRender& render, bool selected )
{
	if( selected )
		render.Color( 0.5, 0.5, 0.5, 0.5 );
	else
		render.Color( 1.0, 0.0, 0.0, 0.5 );
	render.DrawTriangle( triangle );
}

//=========================================================================================
/*virtual*/ void TestGeometry::CalcCenter( VectorMath::Vector& center ) const
{
	VectorMath::CalcCenter( triangle, center );
}

//=========================================================================================
/*virtual*/ void TestGeometry::Translate( const VectorMath::Vector& delta )
{
	VectorMath::Add( triangle.vertex[0], triangle.vertex[0], delta );
	VectorMath::Add( triangle.vertex[1], triangle.vertex[1], delta );
	VectorMath::Add( triangle.vertex[2], triangle.vertex[2], delta );
}

//=========================================================================================
/*virtual*/ void TestGeometry::Rotate( const VectorMath::Vector& unitAxis, float angle )
{
	VectorMath::Rotate( triangle.vertex[0], triangle.vertex[0], unitAxis, angle );
	VectorMath::Rotate( triangle.vertex[1], triangle.vertex[1], unitAxis, angle );
	VectorMath::Rotate( triangle.vertex[2], triangle.vertex[2], unitAxis, angle );
}

//=========================================================================================
/*virtual*/ void TestGeometry::Scale( float scale )
{
	VectorMath::Vector center;
	VectorMath::CalcCenter( triangle, center );

	VectorMath::Vector vec[3];

	VectorMath::Sub( vec[0], triangle.vertex[0], center );
	VectorMath::Sub( vec[1], triangle.vertex[1], center );
	VectorMath::Sub( vec[2], triangle.vertex[2], center );

	VectorMath::Scale( vec[0], vec[0], scale );
	VectorMath::Scale( vec[1], vec[1], scale );
	VectorMath::Scale( vec[2], vec[2], scale );

	VectorMath::Add( triangle.vertex[0], center, vec[0] );
	VectorMath::Add( triangle.vertex[1], center, vec[1] );
	VectorMath::Add( triangle.vertex[2], center, vec[2] );
}

// Geometry.cpp