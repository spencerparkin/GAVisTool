// Canvas.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "Camera.h"
#include "wxAll.h"
#include "Render.h"

//=========================================================================================
class GAVisToolCanvas : public wxGLCanvas, public GAVisToolRender::Drawer
{
public:

	GAVisToolCanvas( wxWindow* parent );
	virtual ~GAVisToolCanvas( void );

	void OnPaint( wxPaintEvent& event );
	void OnResize( wxSizeEvent& event );
	void OnMouseMove( wxMouseEvent& event );
	void OnMouseLeftDown( wxMouseEvent& event );
	void OnMouseLeftUp( wxMouseEvent& event );
	void OnMouseRightDown( wxMouseEvent& event );
	void OnMouseRightUp( wxMouseEvent& event );
	void OnMouseWheelMove( wxMouseEvent& event );
	void OnKeyUp( wxKeyEvent& event );
	void OnKeyDown( wxKeyEvent& event );

	void SetupOpenGL( void );
	void DrawCoordinateAxes( void );
	void PerformSelection( void );

	void SaveRenderSettings( wxConfig* config );
	void RestoreRenderSettings( wxConfig* config );

	virtual void Draw( GAVisToolRender& render ) override;

	void RedrawNeeded( bool invalidatePrimitiveCache );

	void SetSelection( const char* selection );

	void DrawVector( const VectorMath::Vector& pos, const VectorMath::Vector& vec, double ratio );

	wxGLContext* context;
	wxPoint mousePos;

	char selectedGeometry[64];
	GAVisToolCamera camera;
	GAVisToolRender render;
	bool drawGeometryNames;
	bool drawCoordinateAxes;

	DECLARE_EVENT_TABLE()
};

// Canvas.h