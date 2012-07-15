// Canvas.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Canvas.h"
#include "Application.h"
#include <GL/glu.h>

//================================================================================================
BEGIN_EVENT_TABLE( GAVisToolCanvas, wxGLCanvas )
	
	EVT_PAINT( GAVisToolCanvas::OnPaint )
	EVT_SIZE( GAVisToolCanvas::OnResize )
	EVT_MOTION( GAVisToolCanvas::OnMouseMove )
	
	EVT_LEFT_DOWN( GAVisToolCanvas::OnMouseLeftDown )
	EVT_LEFT_UP( GAVisToolCanvas::OnMouseLeftUp )
	
	EVT_RIGHT_DOWN( GAVisToolCanvas::OnMouseRightDown )
	EVT_RIGHT_UP( GAVisToolCanvas::OnMouseRightUp )
	
	EVT_MOUSEWHEEL( GAVisToolCanvas::OnMouseWheelMove )
	
	EVT_KEY_UP( GAVisToolCanvas::OnKeyUp )
	EVT_KEY_UP( GAVisToolCanvas::OnKeyDown )

END_EVENT_TABLE()

//=========================================================================================
GAVisToolCanvas::GAVisToolCanvas( wxWindow* parent ) : wxGLCanvas( parent, wxID_ANY, 0 )
{
	context = 0;
	selectedGeometry[0] = '\0';

	drawGeometryNames = false;
	drawCoordinateAxes = true;

	SetupOpenGL();
}

//=========================================================================================
/*virtual*/ GAVisToolCanvas::~GAVisToolCanvas( void )
{
	if( context )
	{
		delete context;
		context = 0;
	}
}

//=========================================================================================
void GAVisToolCanvas::RedrawNeeded( bool invalidatePrimitiveCache )
{
	if( invalidatePrimitiveCache )
		render.InvalidatePrimitiveCache();

	Refresh();
}

//=========================================================================================
void GAVisToolCanvas::OnPaint( wxPaintEvent& event )
{
	// Do this just before rendering.  It's not enough to
	// do it in the idle loop, because idle processing doesn't
	// get to happen as much as re-rendering does during a scrub.
	wxGetApp().environment->SatisfyConstraints();

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	camera.Orient();

	// In this mode we don't really need to invalidate the cache every frame,
	// but only when the geometry list sorts differently.  Do it every frame
	// anyway, because in any case, we should have a mode where we know that
	// we're never forgetting to invalidate the cache when needed.  In modes
	// where its best to invalidate the cache only when needed, we'll have to find
	// and plug all the places where we may be forgetting to invalidate the cache.
	if( render.GetRenderMode() == GAVisToolRender::RENDER_MODE_NO_ALPHA_SORTING )
		render.InvalidatePrimitiveCache();

	render.Draw( *this );

	if( drawGeometryNames )
		wxGetApp().environment->DrawNames();

	glFlush();
	SwapBuffers();

	// Tell windows that it no longer needs to throw WM_PAINT messages our way.
	HWND hWnd = ( HWND )GetHWND();
	::ValidateRect( hWnd, NULL );
}

//=========================================================================================
/*virtual*/ void GAVisToolCanvas::Draw( GAVisToolRender& render )
{
	if( render.GetRenderMode() == GAVisToolRender::RENDER_MODE_SELECTION )
		wxGetApp().environment->Draw( render, 0 );
	else
	{
		wxGetApp().environment->Draw( render, selectedGeometry );
		
		if( drawCoordinateAxes )
			DrawCoordinateAxes();
	}
}

//=========================================================================================
// This solves the problem the renderer has with instanced geometry.
// Instancing prevents us from having to calculate geometry all the time,
// but because we don't do that, we can't dynamically adjust such things
// as the ratio of a torus' inner to outer radius, for example.
void GAVisToolCanvas::DrawVector( const VectorMath::Vector& pos, const VectorMath::Vector& vec, double ratio )
{
	VectorMath::Vector midVec;
	VectorMath::Scale( midVec, vec, ratio );
	render.DrawVector( pos, midVec, GAVisToolRender::RES_USER, true );

	VectorMath::Vector midPos, remainingVec;
	VectorMath::Add( midPos, pos, midVec );
	VectorMath::Scale( remainingVec, vec, 1.0 - ratio );
	render.DrawVector( midPos, remainingVec );
}

//=========================================================================================
void GAVisToolCanvas::DrawCoordinateAxes( void )
{
	double axisLength = 10.0;
	double ratio = 0.6;

	VectorMath::Vector origin;
	VectorMath::Vector posXaxis, negXaxis;
	VectorMath::Vector posYaxis, negYaxis;
	VectorMath::Vector posZaxis, negZaxis;

	VectorMath::Set( origin, 0.f, 0.f, 0.f );
	
	VectorMath::Set( posXaxis, axisLength, 0.f, 0.f );
	VectorMath::Set( posYaxis, 0.f, axisLength, 0.f );
	VectorMath::Set( posZaxis, 0.f, 0.f, axisLength );

	VectorMath::Set( negXaxis, -axisLength, 0.f, 0.f );
	VectorMath::Set( negYaxis, 0.f, -axisLength, 0.f );
	VectorMath::Set( negZaxis, 0.f, 0.f, -axisLength );

	render.Highlight( GAVisToolRender::NO_HIGHLIGHTING );

	// Make these different colors so that we can differentiate
	// between the X, Y and Z axes.
	render.Color( 1.f, 0.f, 0.f, 1.f );
	DrawVector( origin, posXaxis, ratio );
	render.Color( 0.f, 1.f, 0.f, 1.f );
	DrawVector( origin, posYaxis, ratio );
	render.Color( 0.f, 0.f, 1.f, 1.f );
	DrawVector( origin, posZaxis, ratio );

	// Make these a different color so that we can
	// differentiate between the negative and positive axes.
	render.Color( 1.f, 1.f, 1.f, 1.f );
	DrawVector( origin, negXaxis, ratio );
	DrawVector( origin, negYaxis, ratio );
	DrawVector( origin, negZaxis, ratio );
}

//=========================================================================================
void GAVisToolCanvas::SetSelection( const char* selection )
{
	strcpy_s( selectedGeometry, sizeof( selectedGeometry ), selection );
}

//=========================================================================================
void GAVisToolCanvas::PerformSelection( void )
{
	selectedGeometry[0] = '\0';

	GLuint buffer[512];
	glSelectBuffer( sizeof( buffer ), buffer );

	glRenderMode( GL_SELECT );
	glInitNames();
	glPushName(-1);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	camera.Orient();

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	GLint viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );
	GLdouble x = mousePos.x;
	GLdouble y = GLdouble( viewport[3] ) - GLdouble( mousePos.y );
	GLdouble width = 1.0;
	GLdouble height = 1.0;
	gluPickMatrix( x, y, width, height, viewport );

	float aspectRatio = float( viewport[2] ) / float( viewport[3] );
	gluPerspective( 60.f, aspectRatio, 1.f, 1000.f );

	GAVisToolRender::RenderMode renderMode = render.GetRenderMode();
	render.SetRenderMode( GAVisToolRender::RENDER_MODE_SELECTION );
	render.InvalidatePrimitiveCache();
	render.Draw( *this );
	render.SetRenderMode( renderMode );

	int hitCount = glRenderMode( GL_RENDER );

	// Go process the hit records.  Find the closest hit.
	float closestZ = 9999.f;
	GLuint closestName = -1;
	GLuint* bufPtr = buffer;
	for( int hit = 0; hit < hitCount; hit++ )
	{
		GLuint nameCount = *bufPtr++;
		float zMin = float( *bufPtr++ ) / 0x7FFFFFFF;
		float zMax = float( *bufPtr++ ) / 0x7FFFFFFF;

		GLuint name = -1;
		for( GLuint nameIndex = 0; nameIndex < nameCount && name == -1; nameIndex++ )
			name = bufPtr[ nameIndex ];
		bufPtr += nameCount;

		// Keep track of which one is closest to the viewer.
		if( name != -1 && zMin < closestZ )
		{
			closestName = name;
			closestZ = zMin;
		}
	}

	// Did we find a closest hit?
	if( closestName != -1 )
	{
		GAVisToolGeometry* geometry = wxGetApp().environment->LookupGeometryByIndex( closestName );
		if( geometry )
			strcpy_s( selectedGeometry, sizeof( selectedGeometry ), geometry->GetName() );
	}

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
}

//=========================================================================================
void GAVisToolCanvas::OnResize( wxSizeEvent& event )
{
	wxSize size = event.GetSize();
	glViewport( 0, 0, size.x, size.y );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	float aspectRatio = float( size.x ) / float( size.y );
	gluPerspective( 60.f, aspectRatio, 1.f, 1000.f );

	RedrawNeeded( false );
}

//=========================================================================================
void GAVisToolCanvas::OnMouseMove( wxMouseEvent& event )
{
	if( event.LeftIsDown() )
	{
		wxPoint curMousePos = event.GetPosition();
		wxPoint mouseMoveDelta = curMousePos - this->mousePos;
		this->mousePos = curMousePos;

		VectorMath::Vector delta;
		VectorMath::Set( delta, mouseMoveDelta.x, -mouseMoveDelta.y, 0.f );
		float sensativityFactor = 0.1f;
		VectorMath::Scale( delta, delta, sensativityFactor );

		VectorMath::CoordFrame cameraFrame;
		camera.CameraFrame( cameraFrame );
		VectorMath::Transform( delta, cameraFrame, delta );

		if( event.ControlDown() )
		{
			GAVisToolGeometry* geometry = wxGetApp().environment->LookupGeometryByName( selectedGeometry );
			if( geometry )
			{
				geometry->Translate( delta );
				geometry->HasChanged( true );
			}
		}
		else
		{
			// Notice that with just camera movements, we do
			// not need to invalidate the primitive cache!
			VectorMath::Scale( delta, delta, -1.f );
			bool strafe = event.AltDown();
			if( strafe )
				camera.MoveEyeAndFocus( delta );
			else
				camera.MoveEye( delta, true );
			RedrawNeeded( false );
		}
	}
}

//=========================================================================================
void GAVisToolCanvas::OnMouseLeftDown( wxMouseEvent& event )
{
	mousePos = event.GetPosition();
}

//=========================================================================================
void GAVisToolCanvas::OnMouseLeftUp( wxMouseEvent& event )
{
}

//=========================================================================================
void GAVisToolCanvas::OnMouseRightDown( wxMouseEvent& event )
{
	mousePos = event.GetPosition();
	PerformSelection();
	RedrawNeeded( true );
}

//=========================================================================================
void GAVisToolCanvas::OnMouseRightUp( wxMouseEvent& event )
{
}

//=========================================================================================
void GAVisToolCanvas::OnMouseWheelMove( wxMouseEvent& event )
{
	float wheelDelta = ( float )event.GetWheelDelta();
	float wheelRotation = ( float )event.GetWheelRotation();
	float wheelIncrement = wheelRotation / wheelDelta;

	VectorMath::CoordFrame cameraFrame;
	camera.CameraFrame( cameraFrame );

	if( event.ControlDown() )
	{
		GAVisToolGeometry* geometry = wxGetApp().environment->LookupGeometryByName( selectedGeometry );
		if( geometry )
		{
			if( event.ShiftDown() )
			{
				double sensativityFactor = PI / 64.0;
				geometry->Rotate( cameraFrame.xAxis, sensativityFactor * wheelIncrement );
			}
			else
			{
				float scalar = 1.f;
				if( wheelIncrement > 0.f )
					scalar = 1.1f;
				else
					scalar = 0.9f;
				geometry->Scale( scalar );
			}
			geometry->HasChanged( true );
		}
	}
	else
	{
		// Notice that with just camera movements, we do
		// not need to invalidate the primitive cache!
		float sensativityFactor = -1.2f;
		VectorMath::Vector delta;
		VectorMath::Scale( delta, cameraFrame.zAxis, sensativityFactor * wheelIncrement );
		camera.MoveEye( delta, false );
		RedrawNeeded( false );
	}
}

//=========================================================================================
void GAVisToolCanvas::OnKeyDown( wxKeyEvent& event )
{
}

//=========================================================================================
void GAVisToolCanvas::OnKeyUp( wxKeyEvent& event )
{
	switch( event.GetKeyCode() )
	{
		case WXK_SPACE:
		{
			GAVisToolGeometry* geometry = wxGetApp().environment->LookupGeometryByName( selectedGeometry );
			if( geometry )
			{
				VectorMath::Vector center;
				geometry->CalcCenter( center );
				camera.SetFocus( center );
				RedrawNeeded( false );
			}

			break;
		}
		case WXK_F1:
		{
			static int testGeoCount = 0;
			GAVisToolGeometry* geometry = new TestGeometry( testGeoCount++ );
			wxGetApp().environment->AddBindTarget( geometry );
			RedrawNeeded( true );
			break;
		}
	}
}

//=========================================================================================
void GAVisToolCanvas::SetupOpenGL( void )
{
	context = new wxGLContext( this );
	SetCurrent( *context );

	glClearColor( 1.f, 1.f, 1.f, 1.f );
	glLineWidth( 1.5f );
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE );
	glShadeModel( GL_SMOOTH );

	GLfloat lightColor[] = { 1.f, 1.f, 1.f, 1.f };
	GLfloat lightPos[] = { 1000.f, 1000.f, 500.f };
	GLfloat lightSpec[] = { 0.3f, 0.3f, 0.3f, 0.3f };
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lightColor );
	glLightfv( GL_LIGHT0, GL_POSITION, lightPos );
	glLightfv( GL_LIGHT0, GL_SPECULAR, lightSpec );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
}

//=========================================================================================
void GAVisToolCanvas::SaveRenderSettings( wxConfig* config )
{
	config->Write( wxT( "doLighting" ), render.GetDoLighting() );
	config->Write( wxT( "draw3DVectors" ), render.GetDraw3DVectors() );
	config->Write( wxT( "renderMode" ), ( int )render.GetRenderMode() );
	config->Write( wxT( "renderRes" ), ( int )render.GetResolution() );
	config->Write( wxT( "geometryMode" ), ( int )render.GetGeometryMode() );
	config->Write( wxT( "renderShading" ), ( int )render.GetShading() );
	config->Write( wxT( "drawGeometryNames" ), drawGeometryNames );
	config->Write( wxT( "drawCoordinateAxes" ), drawCoordinateAxes );
}

//=========================================================================================
void GAVisToolCanvas::RestoreRenderSettings( wxConfig* config )
{
	bool doLighting = true;
	config->Read( wxT( "doLighting" ), &doLighting, true );
	render.SetDoLighting( doLighting );

	bool draw3DVectors = false;
	config->Read( wxT( "draw3DVectors" ), &draw3DVectors, false );
	render.SetDraw3DVectors( draw3DVectors );

	int renderMode;
	config->Read( wxT( "renderMode" ), &renderMode, GAVisToolRender::RENDER_MODE_NO_ALPHA_SORTING );
	render.SetRenderMode( ( GAVisToolRender::RenderMode )renderMode );

	int resolution;
	config->Read( wxT( "renderRes" ), &resolution, GAVisToolRender::RES_MEDIUM );
	render.SetResolution( ( GAVisToolRender::Resolution )resolution );

	int geometryMode;
	config->Read( wxT( "geometryMode" ), &geometryMode, GAVisToolRender::GEOMETRY_MODE_SKINNY );
	render.SetGeometryMode( ( GAVisToolRender::GeometryMode )geometryMode );

	int shading;
	config->Read( wxT( "renderShading" ), &shading, GAVisToolRender::SHADE_SMOOTH );
	render.SetShading( ( GAVisToolRender::Shading )shading );

	config->Read( wxT( "drawGeometryNames" ), &drawGeometryNames, false );
	config->Read( wxT( "drawCoordinateAxes" ), &drawCoordinateAxes, true );
}

// Canvas.cpp