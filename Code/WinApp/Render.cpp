// Render.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Render.h"
#include "Application.h"
#include "wxAll.h"

//=============================================================================
GAVisToolRender::GAVisToolRender( void ) :
			selectionPrimitiveCache( 1024, 1024, 128, 0 ),
			noAlphaBlendingPrimitiveCache( 1024 * 8, 1024, 128, 0 ),
			alphaBlendingPrimitiveCache( 1024 * 16, 1024 * 4, 128, 1024 * 8 )
{
	SetRenderMode( RENDER_MODE_NO_ALPHA_SORTING );
	userResolution = RES_MEDIUM;
	shading = SHADE_SMOOTH;
	draw3DVectors = false;
	doLighting = true;
	showBspDetail = false;
	bspTreeCreationMethod = RANDOM_TRIANGLE_INSERTION;
	currentHighlightMethod = NO_HIGHLIGHTING;

	for( int index = 0; index < NUM_RES_TYPES; index++ )
	{
		tubeGeometry[ index ] = 0;
		sphereGeometry[ index ] = 0;
		torusGeometry[ index ] = 0;
		diskGeometry[ index ] = 0;
		vectorGeometry[ index ] = 0;
	}
}

//=============================================================================
/*virtual*/ GAVisToolRender::~GAVisToolRender( void )
{
	for( int index = 0; index < NUM_RES_TYPES; index++ )
	{
		if( tubeGeometry[ index ] )
			delete tubeGeometry[ index ];
		if( sphereGeometry[ index ] )
			delete sphereGeometry[ index ];
		if( torusGeometry[ index ] )
			delete torusGeometry[ index ];
		if( diskGeometry[ index ] )
			delete diskGeometry[ index ];
		if( vectorGeometry[ index ] )
			delete vectorGeometry[ index ];
	}
}

//=============================================================================
bool GAVisToolRender::ShowBspDetail( void )
{
	return showBspDetail;
}

//=============================================================================
void GAVisToolRender::ShowBspDetail( bool showBspDetail )
{
	this->showBspDetail = showBspDetail;
}

//=============================================================================
void GAVisToolRender::SetRenderMode( RenderMode renderMode )
{
	this->renderMode = renderMode;

	switch( renderMode )
	{
		case RENDER_MODE_SELECTION:
		{
			activePrimitiveCache = &selectionPrimitiveCache;
			break;
		}
		case RENDER_MODE_NO_ALPHA_SORTING:
		{
			activePrimitiveCache = &noAlphaBlendingPrimitiveCache;
			break;
		}
		case RENDER_MODE_ALPHA_SORTING:
		{
			activePrimitiveCache = &alphaBlendingPrimitiveCache;
			break;
		}
		default:
		{
			activePrimitiveCache = 0;
			break;
		}
	}
}

//=============================================================================
GAVisToolRender::RenderMode GAVisToolRender::GetRenderMode( void )
{
	return renderMode;
}

//=============================================================================
void GAVisToolRender::SetGeometryMode( GeometryMode geometryMode )
{
	this->geometryMode = geometryMode;
}

//=============================================================================
GAVisToolRender::GeometryMode GAVisToolRender::GetGeometryMode( void )
{
	return geometryMode;
}

//=============================================================================
void GAVisToolRender::SetResolution( Resolution resolution )
{
	userResolution = resolution;
}

//=============================================================================
GAVisToolRender::Resolution GAVisToolRender::GetResolution( void )
{
	return userResolution;
}

//=============================================================================
void GAVisToolRender::SetShading( Shading shading )
{
	this->shading = shading;
}

//=============================================================================
GAVisToolRender::Shading GAVisToolRender::GetShading( void )
{
	return shading;
}

//=============================================================================
void GAVisToolRender::SetDraw3DVectors( bool draw3DVectors )
{
	this->draw3DVectors = draw3DVectors;
}

//=============================================================================
bool GAVisToolRender::GetDraw3DVectors( void )
{
	return draw3DVectors;
}

//=============================================================================
void GAVisToolRender::SetDoLighting( bool doLighting )
{
	this->doLighting = doLighting;
}

//=============================================================================
bool GAVisToolRender::GetDoLighting( void )
{
	return doLighting;
}

//=============================================================================
GAVisToolRender::BspTreeCreationMethod GAVisToolRender::GetBspTreeCreationMethod( void )
{
	return bspTreeCreationMethod;
}

//=============================================================================
void GAVisToolRender::SetBspTreeCreationMethod( BspTreeCreationMethod bspTreeCreationMethod )
{
	this->bspTreeCreationMethod = bspTreeCreationMethod;
}

//=============================================================================
void GAVisToolRender::Draw( Drawer& drawer )
{
	srand(0);

	if( doLighting )
		glEnable( GL_LIGHTING );
	else
		glDisable( GL_LIGHTING );

	switch( renderMode )
	{
		case RENDER_MODE_SELECTION:
		{
			// We want back-facing polygons to be selectable.
			glEnable( GL_DEPTH_TEST );
			glDisable( GL_CULL_FACE );
			break;
		}
		case RENDER_MODE_NO_ALPHA_SORTING:
		{
			glEnable( GL_DEPTH_TEST );
			glEnable( GL_CULL_FACE );
			glCullFace( GL_BACK );
			glFrontFace( GL_CCW );
			break;
		}
		case RENDER_MODE_ALPHA_SORTING:
		{
			glDisable( GL_CULL_FACE );

			// If everything drawn was done using the BSP tree, then
			// we could actually turn this off.
			glEnable( GL_DEPTH_TEST );
			break;
		}
	}

	// Is the cache valid?
	if( !activePrimitiveCache->IsValid() )
	{
		// No, wipe and repopulate the cache.
		activePrimitiveCache->Wipe();
		drawer.Draw( *this );

		// If we're doing alpha sorting, then do it!
		if( renderMode == RENDER_MODE_ALPHA_SORTING )
			activePrimitiveCache->OptimizeForAlphaSorting( bspTreeCreationMethod );

		// The cache is now valid.
		activePrimitiveCache->Validate();
	}

	// Go draw what's in the cache.
	activePrimitiveCache->Draw( *this );
}

//=============================================================================
void GAVisToolRender::InvalidatePrimitiveCache( void )
{
	activePrimitiveCache->Invalidate();
}

//=============================================================================
GAVisToolRender::PrimitiveCache::PrimitiveCache(
				int triangleHeapSize,
				int lineHeapSize,
				int pointHeapSize,
				int bspNodeHeapSize ) :
				triangleHeap( triangleHeapSize ),
				lineHeap( lineHeapSize ),
				pointHeap( pointHeapSize )
{
	if( bspNodeHeapSize > 0 )
		bspTree = new BspTree( bspNodeHeapSize );
	else
		bspTree = 0;
	cacheValid = false;
	optimizedForAlphaSorting = false;
}

//=============================================================================
/*virtual*/ GAVisToolRender::PrimitiveCache::~PrimitiveCache( void )
{
	Wipe();
	if( bspTree )
		delete bspTree;
}

//=============================================================================
GAVisToolRender::Triangle* GAVisToolRender::PrimitiveCache::AllocateTriangle( void )
{
	Triangle* triangle = triangleHeap.AllocateFresh();
	if( triangle )
		triangleList.InsertRightOf( triangleList.RightMost(), triangle );
	return triangle;
}

//=============================================================================
GAVisToolRender::Line* GAVisToolRender::PrimitiveCache::AllocateLine( void )
{
	Line* line = lineHeap.Allocate();
	if( line )
		lineList.InsertRightOf( lineList.RightMost(), line );
	return line;
}

//=============================================================================
GAVisToolRender::Point* GAVisToolRender::PrimitiveCache::AllocatePoint( void )
{
	Point* point = pointHeap.Allocate();
	if( point )
		pointList.InsertRightOf( pointList.RightMost(), point );
	return point;
}

//=============================================================================
void GAVisToolRender::PrimitiveCache::Draw( GAVisToolRender& render )
{
	// Draw the points seperately from the lines and triangles.
	// I'm not sure yet if I want to put them into the BSP tree.
	// Probably not, because they really should always be drawn
	// as fully opaque dots, in which case, we just draw them before
	// everything else anyway.
	if( pointList.Count() > 0 )
	{
		VectorMath::CoordFrame cameraFrame;
		wxGetApp().canvasFrame->canvas->camera.CameraFrame( cameraFrame );
		for( Point* point = ( Point* )pointList.LeftMost(); point; point = ( Point* )point->Right() )
			point->Draw( render.GetDoLighting(), cameraFrame );
	}

	// Now go draw all the lines and triangles.
	if( optimizedForAlphaSorting )
	{
		const VectorMath::Vector& cameraEye = wxGetApp().canvasFrame->canvas->camera.Eye();
		bspTree->Draw( cameraEye, render );
	}
	else
	{
		for( Triangle* triangle = ( Triangle* )triangleList.LeftMost(); triangle; triangle = ( Triangle* )triangle->Right() )
			triangle->Draw( render.GetShading(), render.GetDoLighting(), false );
		for( Line* line = ( Line* )lineList.LeftMost(); line; line = ( Line* )line->Right() )
			line->Draw( render.GetDoLighting() );
	}
}

//=============================================================================
void GAVisToolRender::PrimitiveCache::Wipe( void )
{
	if( bspTree )
	{
		bspTree->Destroy();
		optimizedForAlphaSorting = false;
	}
	triangleList.RemoveAll( false );
	triangleHeap.FreeAll();
	lineList.RemoveAll( false );
	lineHeap.FreeAll();
	pointList.RemoveAll( false );
	pointHeap.FreeAll();
}

//=============================================================================
void GAVisToolRender::PrimitiveCache::Flush( GAVisToolRender& render )
{
	Draw( render );
	Wipe();
}

//=============================================================================
void GAVisToolRender::PrimitiveCache::Invalidate( void )
{
	cacheValid = false;
}

//=============================================================================
void GAVisToolRender::PrimitiveCache::Validate( void )
{
	cacheValid = true;
}

//=============================================================================
bool GAVisToolRender::PrimitiveCache::IsValid( void )
{
	return cacheValid;
}

//=============================================================================
void GAVisToolRender::PrimitiveCache::OptimizeForAlphaSorting( BspTreeCreationMethod bspTreeCreationMethod )
{
	if( bspTree )
	{
		bspTree->Create( triangleList, triangleHeap, lineList, lineHeap, bspTreeCreationMethod );
		optimizedForAlphaSorting = true;

		float triangleGrowthFactor = 1.0;
		if( bspTree->preBspTriangleCount > 0 )
			triangleGrowthFactor = float( bspTree->postBspTriangleCount ) / float( bspTree->preBspTriangleCount );
		float lineGrowthFactor = 1.0;
		if( bspTree->preBspLineCount > 0 )
			lineGrowthFactor = float( bspTree->postBspLineCount ) / float( bspTree->preBspLineCount );

		wxString bspStats = wxString::Format(
						wxT( "triangles{ pre-bsp: %d, post-bsp: %d, cuts: %d, growth: %f }; "
								"lines{ pre-bsp: %d, post-bsp: %d, cuts %d, growth: %f }" ),
						bspTree->preBspTriangleCount,
						bspTree->postBspTriangleCount,
						bspTree->triangleSplitCount,
						triangleGrowthFactor,
						bspTree->preBspLineCount,
						bspTree->postBspLineCount,
						bspTree->lineSplitCount,
						lineGrowthFactor );

		wxGetApp().canvasFrame->statusBar->SetStatusText( bspStats );
	}
}

//=============================================================================
GAVisToolRender::Primitive::Primitive( void )
{
	doubleSided = false;
	VectorMath::RandomVector( bspDetailColor, 0.0, 1.0 );
}

//=============================================================================
/*virtual*/ GAVisToolRender::Primitive::~Primitive( void )
{
}

//=============================================================================
void GAVisToolRender::Primitive::CopyBaseData( const Primitive& primitive )
{
	VectorMath::Copy( color, primitive.color );
	VectorMath::Copy( normal, primitive.normal );
	alpha = primitive.alpha;
	highlightMethod = primitive.highlightMethod;
	doubleSided = primitive.doubleSided;
}

//=============================================================================
GAVisToolRender::Triangle::Triangle( void )
{
	bspAnalysisData = 0;
}

//=============================================================================
/*virtual*/ GAVisToolRender::Triangle::~Triangle( void )
{
}

//=============================================================================
void GAVisToolRender::Triangle::Reset( void )
{
	bspAnalysisData = 0;
}

//=============================================================================
/*virtual*/ void GAVisToolRender::Triangle::CalcCenter( VectorMath::Vector& center )
{
	VectorMath::CalcCenter( triangle, center );
}

//=============================================================================
// Per-triangle highlighting is done so that perhaps we can achieve
// some more interesting effects.
void GAVisToolRender::Primitive::Color( bool doLighting, bool showBspDetail )
{
	VectorMath::Vector finalColor;
	double finalAlpha;

	// Showing the BSP tree detail is a debugging thing.
	if( showBspDetail )
	{
		VectorMath::Copy( finalColor, bspDetailColor );
		finalAlpha = alpha;
	}
	else
	{
		// TODO: If something is highlighted, we could have the idle loop
		//       continually request a redraw so that we could animate some
		//       colors flashing here or something like that.
		switch( highlightMethod )
		{
			default:
			case NO_HIGHLIGHTING:
			{
				VectorMath::Copy( finalColor, color );
				finalAlpha = alpha;
				break;
			}
			case NORMAL_HIGHLIGHTING:
			{
				VectorMath::Set( finalColor, 1.0, 1.0, 1.0 );
				VectorMath::Sub( finalColor, finalColor, color );
				finalAlpha = alpha;
				break;
			}
			case SPACIAL_HIGHLIGHTING:
			{
				break;
			}
		}
	}

	GLfloat r, g, b, a;
	r = GLfloat( finalColor.x );
	g = GLfloat( finalColor.y );
	b = GLfloat( finalColor.z );
	a = GLfloat( finalAlpha );

	if( !doLighting )
		glColor4f( r, g, b, a );
	else
	{
		GLfloat diffuseColor[] = { r, g, b, a };
		GLfloat specularColor[] = { 1.f, 1.f, 1.f, a };
		GLfloat ambientColor[] = { r, g, b, a };
		GLfloat shininess[] = { 30.f };

		glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuseColor );
		glMaterialfv( GL_FRONT, GL_SPECULAR, specularColor );
		glMaterialfv( GL_FRONT, GL_AMBIENT, ambientColor );
		glMaterialfv( GL_FRONT, GL_SHININESS, shininess );

		glMaterialfv( GL_BACK, GL_DIFFUSE, diffuseColor );
		glMaterialfv( GL_BACK, GL_SPECULAR, specularColor );
		glMaterialfv( GL_BACK, GL_AMBIENT, ambientColor );
		glMaterialfv( GL_BACK, GL_SHININESS, shininess );
	}
}

//=============================================================================
void GAVisToolRender::Triangle::Draw( Shading shading, bool doLighting, bool showBspDetail )
{
	GLboolean cullingEnabled = glIsEnabled( GL_CULL_FACE );

	bool drawCCW = true;
	bool drawCW = false;

	if( cullingEnabled )
	{
		if( doubleSided )
			drawCW = true;
	}
	else
	{
		if( doubleSided )
		{
			VectorMath::Vector cameraLookVec;
			wxGetApp().canvasFrame->canvas->camera.CameraLookVec( cameraLookVec );
			double dot = VectorMath::Dot( cameraLookVec, normal );
			if( dot > 0.0 )
			{
				drawCCW = false;
				drawCW = true;
			}
		}
	}

	glBegin( GL_TRIANGLES );

	Color( doLighting, showBspDetail );

	if( drawCCW )
	{
		if( shading == SHADE_FLAT )
		{
			glNormal3f( normal.x, normal.y, normal.z );
			glVertex3f( triangle.vertex[0].x, triangle.vertex[0].y, triangle.vertex[0].z );
			glVertex3f( triangle.vertex[1].x, triangle.vertex[1].y, triangle.vertex[1].z );
			glVertex3f( triangle.vertex[2].x, triangle.vertex[2].y, triangle.vertex[2].z );
		}
		else if( shading == SHADE_SMOOTH )
		{
			glNormal3f( triangleNormals.normal[0].x, triangleNormals.normal[0].y, triangleNormals.normal[0].z );
			glVertex3f( triangle.vertex[0].x, triangle.vertex[0].y, triangle.vertex[0].z );

			glNormal3f( triangleNormals.normal[1].x, triangleNormals.normal[1].y, triangleNormals.normal[1].z );
			glVertex3f( triangle.vertex[1].x, triangle.vertex[1].y, triangle.vertex[1].z );

			glNormal3f( triangleNormals.normal[2].x, triangleNormals.normal[2].y, triangleNormals.normal[2].z );
			glVertex3f( triangle.vertex[2].x, triangle.vertex[2].y, triangle.vertex[2].z );
		}
	}

	if( drawCW )
	{
		if( shading == SHADE_FLAT )
		{
			glNormal3f( -normal.x, -normal.y, -normal.z );
			glVertex3f( triangle.vertex[2].x, triangle.vertex[2].y, triangle.vertex[2].z );
			glVertex3f( triangle.vertex[1].x, triangle.vertex[1].y, triangle.vertex[1].z );
			glVertex3f( triangle.vertex[0].x, triangle.vertex[0].y, triangle.vertex[0].z );
		}
		else if( shading == SHADE_SMOOTH )
		{
			glNormal3f( -triangleNormals.normal[2].x, -triangleNormals.normal[2].y, -triangleNormals.normal[2].z );
			glVertex3f( triangle.vertex[2].x, triangle.vertex[2].y, triangle.vertex[2].z );

			glNormal3f( -triangleNormals.normal[1].x, -triangleNormals.normal[1].y, -triangleNormals.normal[1].z );
			glVertex3f( triangle.vertex[1].x, triangle.vertex[1].y, triangle.vertex[1].z );

			glNormal3f( -triangleNormals.normal[0].x, -triangleNormals.normal[0].y, -triangleNormals.normal[0].z );
			glVertex3f( triangle.vertex[0].x, triangle.vertex[0].y, triangle.vertex[0].z );
		}
	}

	glEnd();
}

//=============================================================================
GAVisToolRender::Line::Line( void )
{
}

//=============================================================================
GAVisToolRender::Line::~Line( void )
{
}

//=============================================================================
/*virtual*/ void GAVisToolRender::Line::CalcCenter( VectorMath::Vector& center )
{
	VectorMath::Lerp( center, vertex[0], vertex[1], 0.5 );
}

//=============================================================================
void GAVisToolRender::Line::Reset( void )
{
}

//=============================================================================
void GAVisToolRender::Line::Draw( bool doLighting )
{
	glBegin( GL_LINES );
	glLineWidth( 2.f );

	Color( doLighting, false );

	VectorMath::Vector cameraLookVec;
	wxGetApp().canvasFrame->canvas->camera.CameraLookVec( cameraLookVec );
	double dot = VectorMath::Dot( cameraLookVec, normal );
	if( dot > 0.0 )
		glNormal3f( -normal.x, -normal.y, -normal.z );
	else
		glNormal3f( normal.x, normal.y, normal.z );

	glVertex3f( vertex[0].x, vertex[0].y, vertex[0].z );
	glVertex3f( vertex[1].x, vertex[1].y, vertex[1].z );

	glEnd();
}

//=============================================================================
GAVisToolRender::Point::Point( void )
{
}

//=============================================================================
/*virtual*/ GAVisToolRender::Point::~Point( void )
{
}

//=============================================================================
// We draw points as triangle fans, mainly because OpenGL selection
// does not work with GL_POINTS very well for some reason that I am
// too retarded to figure out.
void GAVisToolRender::Point::Draw( bool doLighting, VectorMath::CoordFrame& cameraFrame )
{
	Color( doLighting, false );

	glBegin( GL_TRIANGLE_FAN );

	glNormal3f( normal.x, normal.y, normal.z );
	glVertex3f( vertex.x, vertex.y, vertex.z );

	double dotRadius = 0.4;
	int segmentCount = 10;
	for( int segment = 0; segment <= segmentCount; segment++ )
	{
		double angle = 2.0 * PI * double( segment ) / double( segmentCount );
		VectorMath::Vector circleVertex;
		VectorMath::Set( circleVertex, dotRadius * cos( angle ), dotRadius * sin( angle ), 0.0 );
		VectorMath::Transform( circleVertex, cameraFrame, circleVertex );
		VectorMath::Add( circleVertex, circleVertex, vertex );
		glNormal3f( normal.x, normal.y, normal.z );
		glVertex3f( circleVertex.x, circleVertex.y, circleVertex.z );
	}

	glEnd();
}

//=============================================================================
/*virtual*/ void GAVisToolRender::Point::CalcCenter( VectorMath::Vector& center )
{
	VectorMath::Copy( center, vertex );
}

//=============================================================================
void GAVisToolRender::Highlight( HighlightMethod highlightMethod )
{
	currentHighlightMethod = highlightMethod;
}

//=============================================================================
void GAVisToolRender::Color( const VectorMath::Vector& color, double alpha )
{
	if( renderMode == RENDER_MODE_SELECTION )
		alpha = 1.0;
	VectorMath::Copy( currentColor, color );
	currentAlpha = alpha;
}

//=============================================================================
void GAVisToolRender::Color( double r, double g, double b, double a )
{
	if( renderMode == RENDER_MODE_SELECTION )
		a = 1.0;
	VectorMath::Set( currentColor, r, g, b );
	currentAlpha = a;
}

//=============================================================================
void GAVisToolRender::InstanceGeometry( const VectorMath::CoordFrame& coordFrame, const VectorMath::Vector& origin, const Geometry& geometry )
{
	for( int index = 0; index < geometry.triangleArraySize; index++ )
	{
		Triangle* triangle = activePrimitiveCache->AllocateTriangle();
		if( !triangle )
			break;

		VectorMath::Copy( triangle->color, currentColor );
		triangle->alpha = currentAlpha;
		triangle->highlightMethod = currentHighlightMethod;
		triangle->doubleSided = geometry.triangleArray[ index ].doubleSided;

		VectorMath::Transform( triangle->triangle.vertex[0], coordFrame, geometry.triangleArray[ index ].triangle.vertex[0] );
		VectorMath::Transform( triangle->triangle.vertex[1], coordFrame, geometry.triangleArray[ index ].triangle.vertex[1] );
		VectorMath::Transform( triangle->triangle.vertex[2], coordFrame, geometry.triangleArray[ index ].triangle.vertex[2] );

		VectorMath::Add( triangle->triangle.vertex[0], triangle->triangle.vertex[0], origin );
		VectorMath::Add( triangle->triangle.vertex[1], triangle->triangle.vertex[1], origin );
		VectorMath::Add( triangle->triangle.vertex[2], triangle->triangle.vertex[2], origin );

		VectorMath::TransformNormal( triangle->triangleNormals.normal[0], coordFrame, geometry.triangleArray[ index ].triangleNormals.normal[0] );
		VectorMath::TransformNormal( triangle->triangleNormals.normal[1], coordFrame, geometry.triangleArray[ index ].triangleNormals.normal[1] );
		VectorMath::TransformNormal( triangle->triangleNormals.normal[2], coordFrame, geometry.triangleArray[ index ].triangleNormals.normal[2] );

		VectorMath::CalcNormal( triangle->triangle, triangle->normal, true );
		VectorMath::TransformNormal( triangle->normal, coordFrame, triangle->normal );

		VectorMath::MakePlane( triangle->triangle, triangle->trianglePlane );
	}
}

//=============================================================================
GAVisToolRender::Resolution GAVisToolRender::DetermineResolution( Resolution overrideResolution, double sizeFactor )
{
	if( overrideResolution < NUM_RES_TYPES )
		return overrideResolution;

	// This is not a valid value for "userResolution", so just return the default here.
	if( userResolution == RES_USER )
		return RES_MEDIUM;

	return userResolution;
}

//=============================================================================
void GAVisToolRender::DrawPoint( const VectorMath::Vector& pos, const VectorMath::Vector& normal )
{
	Point* point = activePrimitiveCache->AllocatePoint();
	if( !point )
		return;

	VectorMath::Copy( point->color, currentColor );
	point->alpha = currentAlpha;
	point->highlightMethod = currentHighlightMethod;

	VectorMath::Copy( point->vertex, pos );
	VectorMath::Normalize( point->normal, normal );

	if( renderMode == RENDER_MODE_SELECTION )
		activePrimitiveCache->Flush( *this );
}

//=============================================================================
void GAVisToolRender::DrawTriangle( const VectorMath::Triangle& triangleGeometry )
{
	Triangle* triangle = activePrimitiveCache->AllocateTriangle();
	if( !triangle )
		return;

	VectorMath::Copy( triangle->color, currentColor );
	triangle->alpha = currentAlpha;
	triangle->highlightMethod = currentHighlightMethod;

	VectorMath::CopyTriangle( triangle->triangle, triangleGeometry );
	VectorMath::CalcNormal( triangleGeometry, triangle->normal, true );
	VectorMath::Copy( triangle->triangleNormals.normal[0], triangle->normal );
	VectorMath::Copy( triangle->triangleNormals.normal[1], triangle->normal );
	VectorMath::Copy( triangle->triangleNormals.normal[2], triangle->normal );
	VectorMath::MakePlane( triangle->triangle, triangle->trianglePlane );

	if( renderMode == RENDER_MODE_SELECTION )
		activePrimitiveCache->Flush( *this );
}

//=============================================================================
void GAVisToolRender::DrawLine( const VectorMath::Vector& pos0, const VectorMath::Vector& pos1 )
{
	Line* line = activePrimitiveCache->AllocateLine();
	if( line )
	{
		VectorMath::Copy( line->color, currentColor );
		line->alpha = currentAlpha;
		line->highlightMethod = currentHighlightMethod;

		VectorMath::Copy( line->vertex[0], pos0 );
		VectorMath::Copy( line->vertex[1], pos1 );

		VectorMath::Sub( line->normal, line->vertex[1], line->vertex[0] );
		VectorMath::Normalize( line->normal, line->normal );
	}

	if( renderMode == RENDER_MODE_SELECTION )
		activePrimitiveCache->Flush( *this );
}

//=============================================================================
void GAVisToolRender::DrawTube( const VectorMath::Vector& pos0, const VectorMath::Vector& pos1, double tubeRadius, Resolution resolution /*= RES_USER*/ )
{
	if( geometryMode == GEOMETRY_MODE_SKINNY && tubeRadius < 0.5 )
	{
		DrawLine( pos0, pos1 );
		return;
	}

	VectorMath::CoordFrame coordFrame;
	VectorMath::Sub( coordFrame.zAxis, pos1, pos0 );
	VectorMath::Orthogonal( coordFrame.yAxis, coordFrame.zAxis );
	VectorMath::Cross( coordFrame.xAxis, coordFrame.yAxis, coordFrame.zAxis );
	VectorMath::Normalize( coordFrame.xAxis, coordFrame.xAxis );
	VectorMath::Normalize( coordFrame.yAxis, coordFrame.yAxis );
	VectorMath::Scale( coordFrame.xAxis, coordFrame.xAxis, tubeRadius );
	VectorMath::Scale( coordFrame.yAxis, coordFrame.yAxis, tubeRadius );

	resolution = DetermineResolution( resolution, 0.0 );
	if( !tubeGeometry[ resolution ] )
		tubeGeometry[ resolution ] = new Geometry();
	tubeGeometry[ resolution ]->GenerateGeometry( Geometry::GEO_TUBE, resolution );

	InstanceGeometry( coordFrame, pos0, *tubeGeometry[ resolution ] );

	if( renderMode == RENDER_MODE_SELECTION )
		activePrimitiveCache->Flush( *this );
}

//=============================================================================
void GAVisToolRender::DrawSphere( const VectorMath::Vector& pos, double sphereRadius, Resolution resolution /*= RES_USER*/ )
{
	if( geometryMode == GEOMETRY_MODE_SKINNY && sphereRadius < 0.5 )
	{
		VectorMath::Vector normal;
		VectorMath::Set( normal, 0.0, 1.0, 0.0 );
		DrawPoint( pos, normal );
		return;
	}

	VectorMath::CoordFrame coordFrame;
	VectorMath::Set( coordFrame.xAxis, sphereRadius, 0.0, 0.0 );
	VectorMath::Set( coordFrame.yAxis, 0.0, sphereRadius, 0.0 );
	VectorMath::Set( coordFrame.zAxis, 0.0, 0.0, sphereRadius );

	resolution = DetermineResolution( resolution, 0.0 );
	if( !sphereGeometry[ resolution ] )
		sphereGeometry[ resolution ] = new Geometry();
	sphereGeometry[ resolution ]->GenerateGeometry( Geometry::GEO_SPHERE, resolution );

	InstanceGeometry( coordFrame, pos, *sphereGeometry[ resolution ] );

	if( renderMode == RENDER_MODE_SELECTION )
		activePrimitiveCache->Flush( *this );
}

//=============================================================================
void GAVisToolRender::DrawCircle( const VectorMath::Vector& pos, const VectorMath::Vector& norm, double circleRadius, Resolution resolution /*= RES_USER*/ )
{
	VectorMath::CoordFrame coordFrame;
	VectorMath::Normalize( coordFrame.zAxis, norm );
	VectorMath::Orthogonal( coordFrame.yAxis, coordFrame.zAxis );
	VectorMath::Cross( coordFrame.xAxis, coordFrame.yAxis, coordFrame.zAxis );
	VectorMath::Scale( coordFrame.xAxis, coordFrame.xAxis, circleRadius );
	VectorMath::Scale( coordFrame.yAxis, coordFrame.yAxis, circleRadius );

	resolution = DetermineResolution( resolution, 0.0 );

	int segmentCount = 3;
	if( resolution == RES_LOW )
		segmentCount = 10;
	else if( resolution == RES_MEDIUM )
		segmentCount = 20;
	else if( resolution == RES_HIGH )
		segmentCount = 30;

	for( int segment = 0; segment < segmentCount; segment++ )
	{
		double angle = 2.0 * PI * double( segment ) / double( segmentCount );
		double nextAngle = 2.0 * PI * double( ( segment + 1 ) % segmentCount ) / double( segmentCount );

		Line* line = activePrimitiveCache->AllocateLine();
		if( line )
		{
			VectorMath::Copy( line->color, currentColor );
			line->alpha = currentAlpha;
			line->highlightMethod = currentHighlightMethod;

			VectorMath::Set( line->vertex[0], cos( angle ), sin( angle ), 0.0 );
			VectorMath::Set( line->vertex[1], cos( nextAngle ), sin( nextAngle ), 0.0 );
			VectorMath::Transform( line->vertex, coordFrame, pos, line->vertex, 2 );

			VectorMath::Set( line->normal, 0.0, 0.0, 1.0 );
			VectorMath::TransformNormal( line->normal, coordFrame, line->normal );
		}
	}

	if( renderMode == RENDER_MODE_SELECTION )
		activePrimitiveCache->Flush( *this );
}

//=============================================================================
void GAVisToolRender::DrawTorus( const VectorMath::Vector& pos, const VectorMath::Vector& norm, double torusRadius, Resolution resolution /*= RES_USER*/ )
{
	if( geometryMode == GEOMETRY_MODE_SKINNY )
	{
		DrawCircle( pos, norm, torusRadius, resolution );
		return;
	}

	VectorMath::CoordFrame coordFrame;
	VectorMath::Normalize( coordFrame.zAxis, norm );
	VectorMath::Orthogonal( coordFrame.yAxis, coordFrame.zAxis );
	VectorMath::Cross( coordFrame.xAxis, coordFrame.yAxis, coordFrame.zAxis );
	VectorMath::Scale( coordFrame.xAxis, coordFrame.xAxis, torusRadius );
	VectorMath::Scale( coordFrame.yAxis, coordFrame.yAxis, torusRadius );
	VectorMath::Scale( coordFrame.zAxis, coordFrame.zAxis, torusRadius );

	resolution = DetermineResolution( resolution, 0.0 );
	if( !torusGeometry[ resolution ] )
		torusGeometry[ resolution ] = new Geometry();
	torusGeometry[ resolution ]->GenerateGeometry( Geometry::GEO_TORUS, resolution );
		
	InstanceGeometry( coordFrame, pos, *torusGeometry[ resolution ] );

	if( renderMode == RENDER_MODE_SELECTION )
		activePrimitiveCache->Flush( *this );
}

//=============================================================================
void GAVisToolRender::DrawDisk( const VectorMath::Vector& pos, const VectorMath::Vector& norm, double diskRadius, Resolution resolution /*= RES_USER*/ )
{
	VectorMath::CoordFrame coordFrame;
	VectorMath::Normalize( coordFrame.zAxis, norm );
	VectorMath::Orthogonal( coordFrame.yAxis, coordFrame.zAxis );
	VectorMath::Cross( coordFrame.xAxis, coordFrame.yAxis, coordFrame.zAxis );
	VectorMath::Scale( coordFrame.xAxis, coordFrame.xAxis, diskRadius );
	VectorMath::Scale( coordFrame.yAxis, coordFrame.yAxis, diskRadius );

	resolution = DetermineResolution( resolution, 0.0 );
	if( !diskGeometry[ resolution ] )
		diskGeometry[ resolution ] = new Geometry();
	diskGeometry[ resolution ]->GenerateGeometry( Geometry::GEO_DISK, resolution );

	InstanceGeometry( coordFrame, pos, *diskGeometry[ resolution ] );

	if( renderMode == RENDER_MODE_SELECTION )
		activePrimitiveCache->Flush( *this );
}

//=============================================================================
void GAVisToolRender::DrawVector( const VectorMath::Vector& pos, const VectorMath::Vector& vec, Resolution resolution /*= RES_USER*/, bool draw1DVector /*= false*/ )
{
	// Always draw the stem using a simple GL_LINE primtives.
	Line* line = activePrimitiveCache->AllocateLine();
	if( line )
	{
		VectorMath::Copy( line->color, currentColor );
		line->alpha = currentAlpha;
		line->highlightMethod = currentHighlightMethod;

		VectorMath::Copy( line->vertex[0], pos );
		VectorMath::Add( line->vertex[1], pos, vec );

		VectorMath::Normalize( line->normal, vec );
	}
	
	// Draw the arrow head as 3D if desired.
	if( draw3DVectors && !draw1DVector )
	{
		VectorMath::CoordFrame coordFrame;
		VectorMath::Copy( coordFrame.zAxis, vec );
		VectorMath::Orthogonal( coordFrame.yAxis, coordFrame.zAxis );
		VectorMath::Cross( coordFrame.xAxis, coordFrame.yAxis, coordFrame.zAxis );
		double vecLength = VectorMath::Length( vec );
		VectorMath::Scale( coordFrame.xAxis, coordFrame.xAxis, 1.0 / vecLength );

		resolution = DetermineResolution( resolution, 0.0 );
		if( !vectorGeometry[ resolution ] )
			vectorGeometry[ resolution ] = new Geometry();
		vectorGeometry[ resolution ]->GenerateGeometry( Geometry::GEO_VECTOR, resolution );

		InstanceGeometry( coordFrame, pos, *vectorGeometry[ resolution ] );

		if( renderMode == RENDER_MODE_SELECTION )
			activePrimitiveCache->Flush( *this );
	}
}

//=============================================================================
void GAVisToolRender::DrawArrow( const VectorMath::Vector& pos, const VectorMath::Vector& vec )
{
	// bill-board a line-arrow here...
}

//=============================================================================
GAVisToolRender::Geometry::Geometry( void )
{
	geoType = GEO_NONE;
	resolution = RES_MEDIUM;
	triangleArray = 0;
	triangleArraySize = 0;
}

//=============================================================================
/*virtual*/ GAVisToolRender::Geometry::~Geometry( void )
{
	WipeGeometry();
}

//=============================================================================
GAVisToolRender::Geometry::Triangle::Triangle( void )
{
	doubleSided = false;
}

//=============================================================================
void GAVisToolRender::Geometry::GenerateGeometry( GeoType geoType, Resolution resolution )
{
	if( geoType != this->geoType || resolution != this->resolution )
	{
		WipeGeometry();

		this->geoType = geoType;
		this->resolution = resolution;

		switch( geoType )
		{
			case GEO_TUBE:		GenerateCanonicalUnitTube();		break;
			case GEO_SPHERE:	GenerateCanonicalUnitSphere();		break;
			case GEO_TORUS:		GenerateCanonicalUnitTorus();		break;
			case GEO_DISK:		GenerateCanonicalUnitDisk();		break;
			case GEO_VECTOR:	GenerateCanonicalUnitVector();		break;
		}
	}
}

//=============================================================================
void GAVisToolRender::Geometry::WipeGeometry( void )
{
	if( triangleArray )
		delete[] triangleArray;
	triangleArray = 0;
	triangleArraySize = 0;
	geoType = GEO_NONE;
	resolution = RES_MEDIUM;
}

//=============================================================================
void GAVisToolRender::Geometry::GenerateCanonicalUnitTube( void )
{
	int segmentCount = 3;
	if( resolution == RES_LOW )
		segmentCount = 4;
	else if( resolution == RES_MEDIUM )
		segmentCount = 8;
	else if( resolution == RES_HIGH )
		segmentCount = 16;

	VectorMath::Vector* circle0 = new VectorMath::Vector[ segmentCount ];
	VectorMath::Vector* circle1 = new VectorMath::Vector[ segmentCount ];
	for( int segment = 0; segment < segmentCount; segment++ )
	{
		double angle = 2.0 * PI * double( segment ) / double( segmentCount );
		double cosAngle = cos( angle );
		double sinAngle = sin( angle );
		VectorMath::Set( circle0[ segment ], cosAngle, sinAngle, 0.0 );
		VectorMath::Set( circle1[ segment ], cosAngle, sinAngle, 1.0 );
	}

	triangleArraySize = segmentCount * 2;
	triangleArray = new Triangle[ triangleArraySize ];
	Triangle* triangle = triangleArray;
	for( int segment = 0; segment < segmentCount; segment++ )
	{
		int nextSegment = ( segment + 1 ) % segmentCount;

		VectorMath::Copy( triangle->triangle.vertex[0], circle0[ segment ] );
		VectorMath::Copy( triangle->triangle.vertex[1], circle0[ nextSegment ] );
		VectorMath::Copy( triangle->triangle.vertex[2], circle1[ nextSegment ] );
		VectorMath::Copy( triangle->triangleNormals.normal[0], triangle->triangle.vertex[0] );
		VectorMath::Copy( triangle->triangleNormals.normal[1], triangle->triangle.vertex[1] );
		VectorMath::Copy( triangle->triangleNormals.normal[2], triangle->triangle.vertex[2] );
		triangle++;

		VectorMath::Copy( triangle->triangle.vertex[0], circle0[ segment ] );
		VectorMath::Copy( triangle->triangle.vertex[1], circle1[ nextSegment ] );
		VectorMath::Copy( triangle->triangle.vertex[2], circle1[ segment ] );
		VectorMath::Copy( triangle->triangleNormals.normal[0], triangle->triangle.vertex[0] );
		VectorMath::Copy( triangle->triangleNormals.normal[1], triangle->triangle.vertex[1] );
		VectorMath::Copy( triangle->triangleNormals.normal[2], triangle->triangle.vertex[2] );
		triangle++;
	}

	delete[] circle0;
	delete[] circle1;
}

//=============================================================================
void GAVisToolRender::Geometry::GenerateCanonicalUnitSphere( void )
{
	int latitudeSegmentCount = 4;
	int longitudeSegmentCount = 4;
	if( resolution == RES_LOW )
	{
		latitudeSegmentCount = 6;
		longitudeSegmentCount = 6;
	}
	else if( resolution == RES_MEDIUM )
	{
		latitudeSegmentCount = 10;
		longitudeSegmentCount = 10;
	}
	else if( resolution == RES_HIGH )
	{
		latitudeSegmentCount = 16;
		longitudeSegmentCount = 16;
	}

	VectorMath::Vector** sphereVertices = new VectorMath::Vector*[ longitudeSegmentCount ];

	for( int longitudeSegment = 0; longitudeSegment < longitudeSegmentCount; longitudeSegment++ )
	{
		sphereVertices[ longitudeSegment ] = new VectorMath::Vector[ latitudeSegmentCount ];

		double longitudeAngle = 2.0 * PI * double( longitudeSegment ) / double( longitudeSegmentCount );

		for( int latitudeSegment = 0; latitudeSegment < latitudeSegmentCount; latitudeSegment++ )
		{
			double latitudeAngle = PI * double( latitudeSegment ) / double( latitudeSegmentCount - 1 );

			double cosLongitudeAngle = cos( longitudeAngle );
			double sinLongitudeAngle = sin( longitudeAngle );

			double cosLatitudeAngle = cos( latitudeAngle );
			double sinLatitudeAngle = sin( latitudeAngle );

			VectorMath::Vector* vertex = &sphereVertices[ longitudeSegment ][ latitudeSegment ];
			VectorMath::Set( *vertex, cosLongitudeAngle * sinLatitudeAngle, cosLatitudeAngle, -sinLongitudeAngle * sinLatitudeAngle );
		}
	}

	triangleArraySize = 2 * longitudeSegmentCount * ( latitudeSegmentCount - 2 );
	triangleArray = new Triangle[ triangleArraySize ];
	Triangle* triangle = triangleArray;
	for( int longitudeSegment = 0; longitudeSegment < longitudeSegmentCount; longitudeSegment++ )
	{
		int nextLongitudeSegment = ( longitudeSegment + 1 ) % longitudeSegmentCount;

		VectorMath::Copy( triangle->triangle.vertex[0], sphereVertices[ longitudeSegment ][0] );
		VectorMath::Copy( triangle->triangle.vertex[1], sphereVertices[ longitudeSegment ][1] );
		VectorMath::Copy( triangle->triangle.vertex[2], sphereVertices[ nextLongitudeSegment ][1] );
		VectorMath::Copy( triangle->triangleNormals.normal[0], triangle->triangle.vertex[0] );
		VectorMath::Copy( triangle->triangleNormals.normal[1], triangle->triangle.vertex[1] );
		VectorMath::Copy( triangle->triangleNormals.normal[2], triangle->triangle.vertex[2] );
		triangle++;

		VectorMath::Copy( triangle->triangle.vertex[0], sphereVertices[ longitudeSegment ][ latitudeSegmentCount - 2 ] );
		VectorMath::Copy( triangle->triangle.vertex[1], sphereVertices[ longitudeSegment ][ latitudeSegmentCount - 1 ] );
		VectorMath::Copy( triangle->triangle.vertex[2], sphereVertices[ nextLongitudeSegment ][ latitudeSegmentCount - 2 ] );
		VectorMath::Copy( triangle->triangleNormals.normal[0], triangle->triangle.vertex[0] );
		VectorMath::Copy( triangle->triangleNormals.normal[1], triangle->triangle.vertex[1] );
		VectorMath::Copy( triangle->triangleNormals.normal[2], triangle->triangle.vertex[2] );
		triangle++;

		for( int latitudeSegment = 1; latitudeSegment < latitudeSegmentCount - 2; latitudeSegment++ )
		{
			int nextLatitudeSegment = latitudeSegment + 1;

			VectorMath::Copy( triangle->triangle.vertex[0], sphereVertices[ longitudeSegment ][ latitudeSegment ] );
			VectorMath::Copy( triangle->triangle.vertex[1], sphereVertices[ longitudeSegment ][ nextLatitudeSegment ] );
			VectorMath::Copy( triangle->triangle.vertex[2], sphereVertices[ nextLongitudeSegment ][ nextLatitudeSegment ] );
			VectorMath::Copy( triangle->triangleNormals.normal[0], triangle->triangle.vertex[0] );
			VectorMath::Copy( triangle->triangleNormals.normal[1], triangle->triangle.vertex[1] );
			VectorMath::Copy( triangle->triangleNormals.normal[2], triangle->triangle.vertex[2] );
			triangle++;

			VectorMath::Copy( triangle->triangle.vertex[0], sphereVertices[ longitudeSegment ][ latitudeSegment ] );
			VectorMath::Copy( triangle->triangle.vertex[1], sphereVertices[ nextLongitudeSegment ][ nextLatitudeSegment ] );
			VectorMath::Copy( triangle->triangle.vertex[2], sphereVertices[ nextLongitudeSegment ][ latitudeSegment ] );
			VectorMath::Copy( triangle->triangleNormals.normal[0], triangle->triangle.vertex[0] );
			VectorMath::Copy( triangle->triangleNormals.normal[1], triangle->triangle.vertex[1] );
			VectorMath::Copy( triangle->triangleNormals.normal[2], triangle->triangle.vertex[2] );
			triangle++;
		}
	}

	for( int longitudeSegment = 0; longitudeSegment < longitudeSegmentCount; longitudeSegment++ )
		delete[] sphereVertices[ longitudeSegment ];
	delete[] sphereVertices;
}

//=============================================================================
void GAVisToolRender::Geometry::GenerateCanonicalUnitTorus( void )
{
	int torusSegmentCount = 4;
	int torusTubeSegmentCount = 3;
	if( resolution == RES_LOW )
	{
		torusSegmentCount = 6;
		torusTubeSegmentCount = 6;
	}
	else if( resolution == RES_MEDIUM )
	{
		torusSegmentCount = 14;
		torusTubeSegmentCount = 8;
	}
	else if( resolution == RES_HIGH )
	{
		torusSegmentCount = 18;
		torusTubeSegmentCount = 10;
	}

	double torusRadius = 1.0;
	double torusTubeRadius = 0.05;

	VectorMath::Vector** torusVertices = new VectorMath::Vector*[ torusSegmentCount ];
	VectorMath::Vector** torusNormals = new VectorMath::Vector*[ torusSegmentCount ];
	for( int torusSegment = 0; torusSegment < torusSegmentCount; torusSegment++ )
	{
		double torusSegmentAngle = 2.0 * PI * double( torusSegment ) / double( torusSegmentCount );
		double cosTorusSegmentAngle = cos( torusSegmentAngle );
		double sinTorusSegmentAngle = sin( torusSegmentAngle );

		torusVertices[ torusSegment ] = new VectorMath::Vector[ torusTubeSegmentCount ];
		torusNormals[ torusSegment ] = new VectorMath::Vector[ torusTubeSegmentCount ];
		for( int torusTubeSegment = 0; torusTubeSegment < torusTubeSegmentCount; torusTubeSegment++ )
		{
			double torusTubeSegmentAngle = 2.0 * PI * double( torusTubeSegment ) / double( torusTubeSegmentCount );
			double cosTorusTubeSegmentAngle = cos( torusTubeSegmentAngle );
			double sinTorusTubeSegmentAngle = sin( torusTubeSegmentAngle );

			VectorMath::Set( torusVertices[ torusSegment ][ torusTubeSegment ],
								cosTorusSegmentAngle * ( torusRadius + torusTubeRadius * cosTorusTubeSegmentAngle ),
								sinTorusSegmentAngle * ( torusRadius + torusTubeRadius * cosTorusTubeSegmentAngle ),
								torusTubeRadius * sinTorusTubeSegmentAngle );
			VectorMath::Set( torusNormals[ torusSegment ][ torusTubeSegment ],
								cosTorusSegmentAngle * cosTorusTubeSegmentAngle,
								sinTorusSegmentAngle * cosTorusTubeSegmentAngle,
								sinTorusTubeSegmentAngle );
		}
	}

	triangleArraySize = 2 * torusSegmentCount * torusTubeSegmentCount;
	triangleArray = new Triangle[ triangleArraySize ];
	Triangle* triangle = triangleArray;
	for( int torusSegment = 0; torusSegment < torusSegmentCount; torusSegment++ )
	{
		int nextTorusSegment = ( torusSegment + 1 ) % torusSegmentCount;

		for( int torusTubeSegment = 0; torusTubeSegment < torusTubeSegmentCount; torusTubeSegment++ )
		{
			int nextTorusTubeSegment = ( torusTubeSegment + 1 ) % torusTubeSegmentCount;

			VectorMath::Copy( triangle->triangle.vertex[0], torusVertices[ torusSegment ][ torusTubeSegment ] );
			VectorMath::Copy( triangle->triangle.vertex[1], torusVertices[ nextTorusSegment ][ torusTubeSegment ] );
			VectorMath::Copy( triangle->triangle.vertex[2], torusVertices[ nextTorusSegment ][ nextTorusTubeSegment ] );
			VectorMath::Copy( triangle->triangleNormals.normal[0], torusNormals[ torusSegment ][ torusTubeSegment ] );
			VectorMath::Copy( triangle->triangleNormals.normal[1], torusNormals[ nextTorusSegment ][ torusTubeSegment ] );
			VectorMath::Copy( triangle->triangleNormals.normal[2], torusNormals[ nextTorusSegment ][ nextTorusTubeSegment ] );
			triangle++;

			VectorMath::Copy( triangle->triangle.vertex[0], torusVertices[ torusSegment ][ torusTubeSegment ] );
			VectorMath::Copy( triangle->triangle.vertex[1], torusVertices[ nextTorusSegment ][ nextTorusTubeSegment ] );
			VectorMath::Copy( triangle->triangle.vertex[2], torusVertices[ torusSegment ][ nextTorusTubeSegment ] );
			VectorMath::Copy( triangle->triangleNormals.normal[0], torusNormals[ torusSegment ][ torusTubeSegment ] );
			VectorMath::Copy( triangle->triangleNormals.normal[1], torusNormals[ nextTorusSegment ][ nextTorusTubeSegment ] );
			VectorMath::Copy( triangle->triangleNormals.normal[2], torusNormals[ torusSegment ][ nextTorusTubeSegment ] );
			triangle++;
		}
	}

	for( int torusSegment = 0; torusSegment < torusSegmentCount; torusSegment++ )
	{
		delete[] torusVertices[ torusSegment ];
		delete[] torusNormals[ torusSegment ];
	}
	delete[] torusVertices;
	delete[] torusNormals;
}

//=============================================================================
// The layers here are provided instead of one big triangle fan, because
// I'm not doing any sort of fragment shader that would give us something
// better than vertex lighting.
void GAVisToolRender::Geometry::GenerateCanonicalUnitDisk( void )
{
	int segmentCount = 5;
	int layerCount = 5;
	if( resolution == RES_LOW )
	{
		segmentCount = 6;
		layerCount = 6;
	}
	else if( resolution == RES_MEDIUM )
	{
		segmentCount = 12;
		layerCount = 10;
	}
	else if( resolution == RES_HIGH )
	{
		segmentCount = 20;
		layerCount = 14;
	}
	
	VectorMath::Vector** disk = new VectorMath::Vector*[ layerCount ];
	for( int layer = 0; layer < layerCount; layer++ )
	{
		disk[ layer ] = new VectorMath::Vector[ segmentCount ];
		double radius = double( layer + 1 ) / double( layerCount );
		for( int segment = 0; segment < segmentCount; segment++ )
		{
			double angle = 2.0 * PI * double( segment ) / double( segmentCount );
			double x = radius * cos( angle );
			double y = radius * sin( angle );
			VectorMath::Set( disk[ layer ][ segment ], x, y, 0.0 );
		}
	}

	triangleArraySize = segmentCount * ( 2 * layerCount - 1 );
	triangleArray = new Triangle[ triangleArraySize ];
	Triangle* triangle = triangleArray;
	for( int layer = 0; layer < layerCount; layer++ )
	{
		int prevLayer = layer - 1;
		for( int segment = 0; segment < segmentCount; segment++ )
		{
			int nextSegment = ( segment + 1 ) % segmentCount;
			if( layer == 0 )
			{
				VectorMath::Zero( triangle->triangle.vertex[0] );
				VectorMath::Copy( triangle->triangle.vertex[1], disk[0][ segment ] );
				VectorMath::Copy( triangle->triangle.vertex[2], disk[0][ nextSegment ] );
				VectorMath::Set( triangle->triangleNormals.normal[0], 0.0, 0.0, 1.0 );
				VectorMath::Set( triangle->triangleNormals.normal[1], 0.0, 0.0, 1.0 );
				VectorMath::Set( triangle->triangleNormals.normal[2], 0.0, 0.0, 1.0 );
				triangle->doubleSided = true;
				triangle++;
			}
			else
			{
				VectorMath::Copy( triangle->triangle.vertex[0], disk[ layer ][ segment ] );
				VectorMath::Copy( triangle->triangle.vertex[1], disk[ layer ][ nextSegment ] );
				VectorMath::Copy( triangle->triangle.vertex[2], disk[ prevLayer ][ segment ] );
				VectorMath::Set( triangle->triangleNormals.normal[0], 0.0, 0.0, 1.0 );
				VectorMath::Set( triangle->triangleNormals.normal[1], 0.0, 0.0, 1.0 );
				VectorMath::Set( triangle->triangleNormals.normal[2], 0.0, 0.0, 1.0 );
				triangle->doubleSided = true;
				triangle++;

				VectorMath::Copy( triangle->triangle.vertex[0], disk[ layer ][ nextSegment ] );
				VectorMath::Copy( triangle->triangle.vertex[1], disk[ prevLayer ][ nextSegment ] );
				VectorMath::Copy( triangle->triangle.vertex[2], disk[ prevLayer ][ segment ] );
				VectorMath::Set( triangle->triangleNormals.normal[0], 0.0, 0.0, 1.0 );
				VectorMath::Set( triangle->triangleNormals.normal[1], 0.0, 0.0, 1.0 );
				VectorMath::Set( triangle->triangleNormals.normal[2], 0.0, 0.0, 1.0 );
				triangle->doubleSided = true;
				triangle++;
			}
		}
	}

	for( int layer = 0; layer < layerCount; layer++ )
		delete[] disk[ layer ];
	delete[] disk;
}

//=============================================================================
void GAVisToolRender::Geometry::GenerateCanonicalUnitVector( void )
{
	int segmentCount = 3;
	if( resolution == RES_LOW )
		segmentCount = 4;
	else if( resolution == RES_MEDIUM )
		segmentCount = 8;
	else if( resolution == RES_HIGH )
		segmentCount = 16;

	double arrowHeadRadius = 0.1;
	double arrowHeadBase = 0.8;
	VectorMath::Vector* circle = new VectorMath::Vector[ segmentCount ];
	for( int segment = 0; segment < segmentCount; segment++ )
	{
		double angle = 2.0 * PI * double( segment ) / double( segmentCount );
		double cosAngle = cos( angle );
		double sinAngle = sin( angle );
		VectorMath::Set( circle[ segment ], arrowHeadRadius * cosAngle, arrowHeadRadius * sinAngle, arrowHeadBase );
	}

	triangleArraySize = 2 * segmentCount;
	triangleArray = new Triangle[ triangleArraySize ];
	Triangle* triangle = triangleArray;
	for( int segment = 0; segment < segmentCount; segment++ )
	{
		int nextSegment = ( segment + 1 ) % segmentCount;

		VectorMath::Copy( triangle->triangle.vertex[0], circle[ segment ] );
		VectorMath::Copy( triangle->triangle.vertex[1], circle[ nextSegment ] );
		VectorMath::Set( triangle->triangle.vertex[2], 0.0, 0.0, 1.0 );
		VectorMath::Copy( triangle->triangleNormals.normal[0], triangle->triangle.vertex[0] );
		VectorMath::Copy( triangle->triangleNormals.normal[1], triangle->triangle.vertex[1] );
		VectorMath::Copy( triangle->triangleNormals.normal[2], triangle->triangle.vertex[2] );
		triangle++;

		VectorMath::Copy( triangle->triangle.vertex[0], circle[ nextSegment ] );
		VectorMath::Copy( triangle->triangle.vertex[1], circle[ segment ] );
		VectorMath::Set( triangle->triangle.vertex[2], 0.0, 0.0, arrowHeadBase );
		VectorMath::Set( triangle->triangleNormals.normal[0], 0.0, 0.0, -1.0 );
		VectorMath::Set( triangle->triangleNormals.normal[1], 0.0, 0.0, -1.0 );
		VectorMath::Set( triangle->triangleNormals.normal[2], 0.0, 0.0, -1.0 );
		triangle++;
	}

	delete[] circle;
}

//=============================================================================
GAVisToolRender::BspNode::BspNode( void )
{
	Reset();
}

//=============================================================================
GAVisToolRender::BspNode::~BspNode( void )
{
}

//=============================================================================
void GAVisToolRender::BspNode::Reset( void )
{
	frontNode = 0;
	backNode = 0;
	partitionPlaneCreated = false;
}

//=============================================================================
// Notice that like the triangle version of this routine, we don't leak memory
// when the line is split and we insert the parts of the line and forget about
// the original line that was broken up, because we're managing all line memory.
void GAVisToolRender::BspNode::DistributeLine(
									Line* line,
									Line*& backLine,
									Line*& frontLine,
									ObjectHeap< Line >& lineHeap,
									BspTree* bspTree )
{
	double halfPlaneThickness = BspTree::HALF_PLANE_THICKNESS;
	VectorMath::Plane::Side planeSide[2];
	planeSide[0] = VectorMath::PlaneSide( partitionPlane, line->vertex[0], halfPlaneThickness );
	planeSide[1] = VectorMath::PlaneSide( partitionPlane, line->vertex[1], halfPlaneThickness );

	backLine = 0;
	frontLine = 0;

	if( planeSide[0] == planeSide[1] )
	{
		switch( planeSide[0] )
		{
			case VectorMath::Plane::SIDE_BACK:
			{
				backLine = line;
				break;
			}
			case VectorMath::Plane::SIDE_FRONT:
			{
				frontLine = line;
				break;
			}
			case VectorMath::Plane::SIDE_NEITHER:	
			{
				lineList.InsertRightOf( lineList.RightMost(), line );
				break;
			}
		}
	}
	else if( planeSide[0] == VectorMath::Plane::SIDE_NEITHER )
	{
		if( planeSide[1] == VectorMath::Plane::SIDE_BACK )
			backLine = line;
		else
			frontLine = line;
	}
	else if( planeSide[1] == VectorMath::Plane::SIDE_NEITHER )
	{
		if( planeSide[0] == VectorMath::Plane::SIDE_BACK )
			backLine = line;
		else
			frontLine = line;
	}
	else
	{
		double lerp;
		VectorMath::PlaneLineIntersect( partitionPlane, line->vertex[0], line->vertex[1], lerp );
		VectorMath::Vector vertex;
		VectorMath::Lerp( vertex, line->vertex[0], line->vertex[1], lerp );

		// Keep track of some stats.
		bspTree->lineSplitCount++;

		backLine = lineHeap.AllocateFresh();
		backLine->CopyBaseData( *line );
		VectorMath::Copy( backLine->vertex[0], vertex );

		frontLine = lineHeap.AllocateFresh();
		frontLine->CopyBaseData( *line );
		VectorMath::Copy( frontLine->vertex[0], vertex );
		
		if( planeSide[0] == VectorMath::Plane::SIDE_BACK )
		{
			VectorMath::Copy( backLine->vertex[1], line->vertex[0] );
			VectorMath::Copy( frontLine->vertex[1], line->vertex[1] );
		}
		else
		{
			VectorMath::Copy( backLine->vertex[1], line->vertex[1] );
			VectorMath::Copy( frontLine->vertex[1], line->vertex[0] );
		}

		// This was an attempt to compensate for the crappy look
		// of a line that was split into a sequence of several line segments.
		/*
		double deltaLength = 0.02;
		VectorMath::Vector delta;

		VectorMath::Sub( delta, backLine->vertex[0], backLine->vertex[1] );
		VectorMath::Scale( delta, delta, deltaLength / VectorMath::Length( delta ) );
		VectorMath::Add( backLine->vertex[0], backLine->vertex[0], delta );

		VectorMath::Sub( delta, frontLine->vertex[0], frontLine->vertex[1] );
		VectorMath::Scale( delta, delta, deltaLength / VectorMath::Length( delta ) );
		VectorMath::Add( frontLine->vertex[0], frontLine->vertex[0], delta );
		*/
	}
}

//=============================================================================
// Append to the given lists parts of the given triangle if it straddles the plane,
// or append the given triangle to the correct list in all other cases.  We assume
// that the given triangle is not a member of any list.  Notice that we put the given
// triangle on this node's list if it is in the partition plane.
void GAVisToolRender::BspNode::DistributeTriangle(
									Triangle* triangle,
									Utilities::List& backTriangleList,
									Utilities::List& frontTriangleList,
									ObjectHeap< Triangle >& triangleHeap,
									BspTree* bspTree )
{
	double halfPlaneThickness = BspTree::HALF_PLANE_THICKNESS;
	VectorMath::SideCountData sideCountData;
	VectorMath::SideCount( triangle->triangle, partitionPlane, sideCountData, halfPlaneThickness );

	if( sideCountData.countOnPlane == 3 )
	{
		triangleList.InsertRightOf( triangleList.RightMost(), triangle );

		if( triangle->bspAnalysisData )
			BspTree::OldTriangleDies( triangle );
	}
	else if( sideCountData.countOnFront == 3 ||
		( sideCountData.countOnFront == 2 && sideCountData.countOnPlane == 1 ) ||
		( sideCountData.countOnFront == 1 && sideCountData.countOnPlane == 2 ) )
	{
		frontTriangleList.InsertRightOf( frontTriangleList.RightMost(), triangle );
	}
	else if( sideCountData.countOnBack == 3 ||
		( sideCountData.countOnBack == 2 && sideCountData.countOnPlane == 1 ) ||
		( sideCountData.countOnBack == 1 && sideCountData.countOnPlane == 2 ) )
	{
		backTriangleList.InsertRightOf( backTriangleList.RightMost(), triangle );
	}
	else
	{
		// In this case the triangle strattles the plane, and we need to cut it against the plane!
		VectorMath::Plane::Side sideArray[3];
		VectorMath::Triangle triangleArray[3];
		VectorMath::TriangleNormals triangleNormalsArray[3];
		int triangleArraySize = 0;

		// Keep track of some stats.
		bspTree->triangleSplitCount++;

		// This should always return true here.
		VectorMath::SplitTriangle( triangle->triangle, &triangle->triangleNormals, partitionPlane, sideCountData, triangleArray, triangleNormalsArray, sideArray, triangleArraySize );
		for( int index = 0; index < triangleArraySize; index++ )
		{
			Triangle* trianglePart = triangleHeap.AllocateFresh();
			if( !trianglePart )
				break;
		
			VectorMath::CopyTriangle( trianglePart->triangle, triangleArray[ index ] );
			VectorMath::CopyTriangleNormals( trianglePart->triangleNormals, triangleNormalsArray[ index ] );

			trianglePart->CopyBaseData( *triangle );

			VectorMath::MakePlane( trianglePart->triangle, trianglePart->trianglePlane );

			// Each triangle part should be on either the back or the front of the partitioning plane.
			switch( sideArray[ index ] )
			{
				case VectorMath::Plane::SIDE_FRONT:
				{
					frontTriangleList.InsertRightOf( frontTriangleList.RightMost(), trianglePart );
					break;
				}
				case VectorMath::Plane::SIDE_BACK:
				{
					backTriangleList.InsertRightOf( backTriangleList.RightMost(), trianglePart );
					break;
				}
			}

			if( triangle->bspAnalysisData )
				BspTree::NewTriangleBornFromOld( trianglePart, triangle, bspTree->bspAnalysisDataHeap );
		}

		if( triangle->bspAnalysisData )
			BspTree::OldTriangleDies( triangle );
	}
}

//=============================================================================
/*static*/ void GAVisToolRender::BspTree::OldTriangleDies( Triangle* oldTriangle )
{
	BspAnalysisData* bspAnalysisData = 0;
	
	// All the triangles that the old triangle split are no longer split by this triangle.
	while( oldTriangle->bspAnalysisData->trianglesSplitByThis.Count() > 0 )
	{
		bspAnalysisData = ( BspAnalysisData* )oldTriangle->bspAnalysisData->trianglesSplitByThis.LeftMost();
		oldTriangle->bspAnalysisData->trianglesSplitByThis.Remove( bspAnalysisData, false );

		bspAnalysisData->triangle->bspAnalysisData->trianglesSplittingThis.Remove( oldTriangle->bspAnalysisData, false );
	}

	// All the triangles that the old triangle was split by no longer split this triangle.
	while( oldTriangle->bspAnalysisData->trianglesSplittingThis.Count() > 0 )
	{
		bspAnalysisData = ( BspAnalysisData* )oldTriangle->bspAnalysisData->trianglesSplittingThis.LeftMost();
		oldTriangle->bspAnalysisData->trianglesSplittingThis.Remove( bspAnalysisData, false );

		bspAnalysisData->triangle->bspAnalysisData->trianglesSplitByThis.Remove( oldTriangle->bspAnalysisData, false );
	}
}

//=============================================================================
/*static*/ void GAVisToolRender::BspTree::NewTriangleBornFromOld( Triangle* newTriangle, Triangle* oldTriangle, ObjectHeap< BspAnalysisData >& bspAnalysisDataHeap )
{
	newTriangle->bspAnalysisData = bspAnalysisDataHeap.AllocateFresh();

	// A triangle split by the triangle to be tesselated will also be split by one of its sub-triangles.
	newTriangle->bspAnalysisData->trianglesSplitByThis.Assign( oldTriangle->bspAnalysisData->trianglesSplitByThis, false );

	// A triangle splitting the triangle to be tesselated, however, may not necessarily split one of its sub-triangles.
	BspAnalysisData* bspAnalysisData = ( BspAnalysisData* )oldTriangle->bspAnalysisData->trianglesSplittingThis.LeftMost();
	while( bspAnalysisData )
	{
		if( newTriangle->StraddlesPlane( bspAnalysisData->triangle->trianglePlane ) )
			newTriangle->bspAnalysisData->trianglesSplittingThis.InsertRightOf( newTriangle->bspAnalysisData->trianglesSplittingThis.RightMost(), bspAnalysisData );
		bspAnalysisData = ( BspAnalysisData* )bspAnalysisData->Right( &oldTriangle->bspAnalysisData->trianglesSplittingThis );
	}
}

//=============================================================================
// We have all the information here that I think we could ever need in figuring
// out which triangle or set of triangles represent the best choice or choices.
// It's now just a matter of knowing how to make the decision.  Clearly any triangle
// that cuts no other triangle is a good choice.  If there are no such triangles,
// then the decision becomes a little harder to make.  Certainly we want to minimize
// the number of cuts made by the chosen triangle.
GAVisToolRender::Triangle* GAVisToolRender::BspNode::ChooseBestRootTriangle( Utilities::List& subTreeTriangleList )
{
	Triangle* bestTriangle = 0;
	int minimalCutCount = -1;
	int maximalCutByCount = -1;		// I'm not sure that this matters at all.  Why again did I think it mattered?
	for( Triangle* triangle = ( Triangle* )subTreeTriangleList.LeftMost(); triangle; triangle = ( Triangle* )triangle->Right() )
	{
		Triangle* currentTriangle = bestTriangle;
		if( !bestTriangle )
			bestTriangle = triangle;
		else
		{
			if( triangle->bspAnalysisData->trianglesSplitByThis.Count() < minimalCutCount )
				bestTriangle = triangle;
			else if( triangle->bspAnalysisData->trianglesSplitByThis.Count() == minimalCutCount )
			{
				// Again, I'm not sure that this matters at all.
				if( triangle->bspAnalysisData->trianglesSplittingThis.Count() > maximalCutByCount )
					bestTriangle = triangle;
			}
		}

		if( currentTriangle != bestTriangle )
		{
			minimalCutCount = triangle->bspAnalysisData->trianglesSplitByThis.Count();
			maximalCutByCount = triangle->bspAnalysisData->trianglesSplittingThis.Count();
		}
	}

	return bestTriangle;
}

//=============================================================================
// Build a sub-tree at this node with the given list of triangles.
// We assume that the triangle list of this node is empty to begin with.
void GAVisToolRender::BspNode::Insert( Utilities::List& subTreeTriangleList, ObjectHeap< Triangle >& triangleHeap, BspTree* bspTree )
{
	// Our first task is to choose the best root node triangle.
	Triangle* rootTriangle = ChooseBestRootTriangle( subTreeTriangleList );
	subTreeTriangleList.Remove( rootTriangle, false );
	triangleList.InsertLeftOf( 0, rootTriangle );
	VectorMath::CopyPlane( partitionPlane, rootTriangle->trianglePlane );
	partitionPlaneCreated = true;		// This isn't used in this algorithm, but set it none-the-less.

	// Remove this triangle from consideration within the entirety of our analysis data.
	BspTree::OldTriangleDies( rootTriangle );

	// Having chosen a partition plane, go divide everything up into the seperate half-spaces.
	Utilities::List backTriangleList, frontTriangleList;
	while( subTreeTriangleList.Count() > 0 )
	{
		Triangle* triangle = ( Triangle* )subTreeTriangleList.LeftMost();
		subTreeTriangleList.Remove( triangle, false );

		// Put the triangle in the correct list or split it across the front and back lists.
		DistributeTriangle( triangle, backTriangleList, frontTriangleList, triangleHeap, bspTree );
	}

	// Now go divide and conquer.
	if( backTriangleList.Count() > 0 )
	{
		if( !backNode )
			backNode = bspTree->bspNodeHeap.AllocateFresh();
		if( backNode )
			backNode->Insert( backTriangleList, triangleHeap, bspTree );
	}
	if( frontTriangleList.Count() > 0 )
	{
		if( !frontNode )
			frontNode = bspTree->bspNodeHeap.AllocateFresh();
		if( frontNode )
			frontNode->Insert( frontTriangleList, triangleHeap, bspTree );
	}

	// If this is a leaf node, then this should not happen!  A triangle all by itself
	// in its own half-space is not cutting any other triangle or being cut by any other
	// triangle, because there are no other triangles in that space.  This also shouldn't
	// happen for any internal triangles, because the analysis data should dwindle to
	// empty as triangles are removed from consideration and put into the tree.  In short,
	// this should never happen, but I'm putting this here just in case for now.
	if( rootTriangle->bspAnalysisData->trianglesSplitByThis.Count() > 0 )
		rootTriangle->bspAnalysisData->trianglesSplitByThis.RemoveAll( false );
	if( rootTriangle->bspAnalysisData->trianglesSplittingThis.Count() > 0 )
		rootTriangle->bspAnalysisData->trianglesSplittingThis.RemoveAll( false );

	// Keep track of some stats.
	bspTree->postBspTriangleCount += triangleList.Count();
}

//=============================================================================
void GAVisToolRender::BspNode::Insert( Line* line, ObjectHeap< Line >& lineHeap, BspTree* bspTree )
{
	if( !partitionPlaneCreated )
	{
		lineList.InsertLeftOf( 0, line );

		// Arbitrarily choose a plane that contains the line.
		VectorMath::Vector lineVec;
		VectorMath::Sub( lineVec, line->vertex[1], line->vertex[0] );
		VectorMath::Orthogonal( partitionPlane.normal, lineVec );
		VectorMath::Normalize( partitionPlane.normal, partitionPlane.normal );
		VectorMath::SetPlanePos( partitionPlane, line->vertex[0] );
		partitionPlaneCreated = true;

		// Keep track of some stats.
		bspTree->postBspLineCount++;
	}
	else
	{
		int numLinesBefore = lineList.Count();
		Line* backLine, *frontLine;
		DistributeLine( line, backLine, frontLine, lineHeap, bspTree );
		int numLinesAfter = lineList.Count();

		bspTree->postBspLineCount += numLinesAfter - numLinesBefore;

		if( backLine )
		{
			if( !backNode )
				backNode = bspTree->bspNodeHeap.AllocateFresh();
			if( backNode )
				backNode->Insert( backLine, lineHeap, bspTree );
		}

		if( frontLine )
		{
			if( !frontNode )
				frontNode = bspTree->bspNodeHeap.AllocateFresh();
			if( frontNode )
				frontNode->Insert( frontLine, lineHeap, bspTree );
		}
	}
}

//=============================================================================
void GAVisToolRender::BspNode::Insert( Triangle* triangle, ObjectHeap< Triangle >& triangleHeap, BspTree* bspTree )
{
	if( !partitionPlaneCreated )
	{
		triangleList.InsertLeftOf( 0, triangle );
		VectorMath::CopyPlane( partitionPlane, triangle->trianglePlane );
		partitionPlaneCreated = true;
		
		// Keep track of some stats.
		bspTree->postBspTriangleCount++;
	}
	else
	{
		int numTrianglesBefore = triangleList.Count();
		Utilities::List backTriangleList, frontTriangleList;
		DistributeTriangle( triangle, backTriangleList, frontTriangleList, triangleHeap, bspTree );
		int numTrianglesAfter = triangleList.Count();
		
		bspTree->postBspTriangleCount += numTrianglesAfter - numTrianglesBefore;

		if( backTriangleList.Count() > 0 )
		{
			if( !backNode )
				backNode = bspTree->bspNodeHeap.AllocateFresh();
			if( backNode )
			{
				do
				{
					Triangle* triangle = ( Triangle* )backTriangleList.LeftMost();
					backTriangleList.Remove( triangle, false );
					backNode->Insert( triangle, triangleHeap, bspTree );
				}
				while( backTriangleList.Count() > 0 );
			}
		}

		if( frontTriangleList.Count() > 0 )
		{
			if( !frontNode )
				frontNode = bspTree->bspNodeHeap.AllocateFresh();
			if( frontNode )
			{
				do
				{
					Triangle* triangle = ( Triangle* )frontTriangleList.LeftMost();
					frontTriangleList.Remove( triangle, false );
					frontNode->Insert( triangle, triangleHeap, bspTree );
				}
				while( frontTriangleList.Count() > 0 );
			}
		}
	}
}

//=============================================================================
void GAVisToolRender::BspTree::Draw( const VectorMath::Vector& cameraEye, GAVisToolRender& render )
{
	if( rootNode )
		rootNode->Draw( cameraEye, render );
}

//=============================================================================
void GAVisToolRender::BspNode::Draw( const VectorMath::Vector& cameraEye, GAVisToolRender& render )
{
	BspNode* firstNode = 0;
	BspNode* lastNode = 0;

	switch( VectorMath::PlaneSide( partitionPlane, cameraEye ) )
	{
		case VectorMath::Plane::SIDE_NEITHER:
		case VectorMath::Plane::SIDE_BACK:
		{
			firstNode = frontNode;
			lastNode = backNode;
			break;
		}
		case VectorMath::Plane::SIDE_FRONT:
		{
			firstNode = backNode;
			lastNode = frontNode;
			break;
		}
	}

	if( firstNode )
		firstNode->Draw( cameraEye, render );

	for( Triangle* triangle = ( Triangle* )triangleList.LeftMost(); triangle; triangle = ( Triangle* )triangle->Right() )
		triangle->Draw( render.GetShading(), render.GetDoLighting(), render.ShowBspDetail() );

	for( Line* line = ( Line* )lineList.LeftMost(); line; line = ( Line* )line->Right() )
		line->Draw( render.GetDoLighting() );

	if( lastNode )
		lastNode->Draw( cameraEye, render );
}

//=============================================================================
const double GAVisToolRender::BspTree::HALF_PLANE_THICKNESS = 0.002;

//=============================================================================
GAVisToolRender::BspTree::BspTree( int bspNodeHeapSize ) :
				bspNodeHeap( bspNodeHeapSize ),
				bspAnalysisDataHeap( bspNodeHeapSize )
{
}

//=============================================================================
GAVisToolRender::BspTree::~BspTree( void )
{
}

//=============================================================================
void GAVisToolRender::BspTree::Destroy( void )
{
	for( int index = 0; index < bspNodeHeap.AllocationCount(); index++ )
	{
		BspNode* node = bspNodeHeap.Access( index );
		node->triangleList.RemoveAll( false );
		node->lineList.RemoveAll( false );
	}

	bspNodeHeap.FreeAll();
	rootNode = 0;
}

// Minimal Cut BSP Tree
// ====================
//
// We could throw all the triangles into the BSP tree in any order
// and get a BSP tree that would do proper alpha blending.  However,
// this tree may produce way more triangles than we really need.  To
// solve this problem, we want to find a BSP tree that can contain
// all of our triangles with the fewest number of triangle cuts.  An
// idea for accomplishing this is to perform a recursive algorithm where
// at each recursive iteration we choose from the set of best choices
// for the root node at a sub-tree that is to contain all triangles that
// will live in the half-space of the root node.  In consideration of
// which triangle should be the root, we need not compare it with any
// triangles in the original set except for those that will live with
// it in the same half-space.  There seems to me to be two criteria
// for choosing a sub-tree root node.  The first is: how much damage
// does a given triangle do in terms of triangle cuts to its fellow
// triangles; and secondly, how much damage is there done to a given
// triangle by its fellow members of the half-space?  After selecting
// all triangles that damage the least number of other half-space
// members, we may choose among these which is damaged the most
// by its fellow members.  All of this seems like it would help to
// minimize the number of triangle cuts in the finally constructed
// tree, however, it would be interesting and nice if we could prove
// that this or some other method really did give us one of the possible
// minimially cut BSP trees.  The proof would lie in knowing that the
// choice we made for each sub-tree root was really the best choice,
// or one of the best choices, for the entire sub-tree.

//=============================================================================
void GAVisToolRender::BspTree::Create(
							Utilities::List& triangleList,
							ObjectHeap< Triangle >& triangleHeap,
							Utilities::List& lineList,
							ObjectHeap< Line >& lineHeap,
							BspTreeCreationMethod creationMethod )
{
	preBspTriangleCount = triangleList.Count();
	postBspTriangleCount = 0;
	preBspLineCount = lineList.Count();
	postBspLineCount = 0;
	triangleSplitCount = 0;
	lineSplitCount = 0;

	if( creationMethod == RANDOM_TRIANGLE_INSERTION )
	{
		while( triangleList.Count() > 0 )
		{
			int randomIndex = rand() % triangleList.Count();
			Triangle* triangle = ( Triangle* )triangleList[ randomIndex ];
			triangleList.Remove( triangle, false );

			if( !rootNode )
				rootNode = bspNodeHeap.AllocateFresh();
			if( rootNode )
				rootNode->Insert( triangle, triangleHeap, this );
		}
	}
	else if( creationMethod == ANALYZED_TRIANGLE_INSERTION )
	{
		if( triangleList.Count() > 0 )
		{
			AnalyzeTriangleList( triangleList );

			if( !rootNode )
				rootNode = bspNodeHeap.AllocateFresh();
			if( rootNode )
				rootNode->Insert( triangleList, triangleHeap, this );

			bspAnalysisDataHeap.FreeAll();
		}
	}

	// Once all triangles are in the BSP tree, we throw in the lines.
	// I'm just going to randomly throw these in and forgo any analysis.
	// We want lines in here too, because we want to do propery anti-
	// aliasing, which require proper alpha blending.
	while( lineList.Count() > 0 )
	{
		int randomIndex = rand() % lineList.Count();
		Line* line = ( Line* )lineList[ randomIndex ];
		lineList.Remove( line, false );

		if( !rootNode )
			rootNode = bspNodeHeap.AllocateFresh();
		if( rootNode )
			rootNode->Insert( line, lineHeap, this );
	}
}

//=============================================================================
void GAVisToolRender::BspTree::AnalyzeTriangleList( Utilities::List& triangleList )
{
	for( Triangle* triangle = ( Triangle* )triangleList.LeftMost(); triangle; triangle = ( Triangle* )triangle->Right() )
	{
		triangle->bspAnalysisData = bspAnalysisDataHeap.AllocateFresh();
		triangle->bspAnalysisData->triangle = triangle;
	}

	// This is expensive!
	for( Triangle* triangle0 = ( Triangle* )triangleList.LeftMost(); triangle0; triangle0 = ( Triangle* )triangle0->Right() )
		for( Triangle* triangle1 = ( Triangle* )triangle0->Right(); triangle1; triangle1 = ( Triangle* )triangle1->Right() )
			AnalyzeTrianglePair( triangle0, triangle1 );
}

//=============================================================================
/*static*/ void GAVisToolRender::BspTree::AnalyzeTrianglePair( Triangle* triangle0, Triangle* triangle1 )
{
	if( triangle0->StraddlesPlane( triangle1->trianglePlane ) )
	{
		triangle1->bspAnalysisData->trianglesSplitByThis.InsertRightOf( triangle1->bspAnalysisData->trianglesSplitByThis.RightMost(), triangle0->bspAnalysisData );
		triangle0->bspAnalysisData->trianglesSplittingThis.InsertRightOf( triangle0->bspAnalysisData->trianglesSplittingThis.RightMost(), triangle1->bspAnalysisData );
	}

	if( triangle1->StraddlesPlane( triangle0->trianglePlane ) )
	{
		triangle0->bspAnalysisData->trianglesSplitByThis.InsertRightOf( triangle0->bspAnalysisData->trianglesSplitByThis.RightMost(), triangle1->bspAnalysisData );
		triangle1->bspAnalysisData->trianglesSplittingThis.InsertRightOf( triangle1->bspAnalysisData->trianglesSplittingThis.RightMost(), triangle0->bspAnalysisData );
	}
}

//=============================================================================
bool GAVisToolRender::Triangle::StraddlesPlane( const VectorMath::Plane& plane ) const
{
	VectorMath::SideCountData sideCountData;
	VectorMath::SideCount( triangle, plane, sideCountData, BspTree::HALF_PLANE_THICKNESS );
	if( sideCountData.countOnFront > 0 && sideCountData.countOnBack > 0 )
		return true;
	return false;
}

//=============================================================================
GAVisToolRender::BspAnalysisData::BspAnalysisData( void )
{
	triangle = 0;
}

//=============================================================================
/*virtual*/ GAVisToolRender::BspAnalysisData::~BspAnalysisData( void )
{
}

//=============================================================================
void GAVisToolRender::BspAnalysisData::Reset( void )
{
	triangle = 0;
}

// Render.cpp