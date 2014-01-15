// Application.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "wxAll.h"
#include "ConsoleFrame.h"
#include "CanvasFrame.h"
#include "Environment.h"
#include "Calculator/CalcLib.h"

//=========================================================================================
class GAVisToolApp : public wxApp
{
public:

	GAVisToolApp( void );
	virtual ~GAVisToolApp( void );

	bool OnInit( void );
	int OnExit( void );

	void ExitApp( void );

	bool ProcessConsoleInput( const wxString& consoleInput, wxString& consoleOutput, wxImage& consoleOutputImage );

	void BuildUserInterface( void );

	GAVisToolConsoleFrame* consoleFrame;
	GAVisToolCanvasFrame* canvasFrame;
	GAVisToolEnvironment* environment;
	CalcLib::Calculator* calculator;

	void SaveWindowLayout( wxFrame* frame, const wxString& frameName );
	void RestoreWindowLayout( wxFrame* frame, const wxString& frameName );

	bool GetGenerateLatexOutput( void );
	void SetGenerateLatexOutput( bool generateLatexOutput );

	wxString GetLatexCommandDir( void );
	void SetLatexCommandDir( const wxString& latexCommandDir );

	bool CreateLatexTexture( const wxString& latexCode, GLuint& texture );

private:

	bool GenerateLatex( const wxString& latexCode, wxImage& latexImage );
	wxString WrapLatexCode( const wxString& latexCode );
	bool ExecuteCommand( const wxString& command, bool waitForProcessCompletion = true );

	bool generateLatexOutput;
	wxString latexCommandDir;
	wxString executionDir;

	wxConfig* config;
};

//=========================================================================================
DECLARE_APP( GAVisToolApp )

// Application.h