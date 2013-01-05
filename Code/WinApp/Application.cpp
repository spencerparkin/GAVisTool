// Application.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Application.h"
#include <GL/glu.h>
#include "resource.h"

//=========================================================================================
static int AllocHook(
				int allocType, void* data, size_t size,
				int blockUse, long request, const unsigned char* szFileName, int nLine )
{
	switch( allocType )
	{
		case _HOOK_ALLOC:
		{
			break;
		}
		case _HOOK_REALLOC:
		{
			break;
		}
		case _HOOK_FREE:
		{
			break;
		}
	}

	return 1;
}

//=========================================================================================
IMPLEMENT_APP( GAVisToolApp )

//=========================================================================================
GAVisToolApp::GAVisToolApp( void )
{
#ifdef _DEBUG
	_CrtSetAllocHook( AllocHook );
#endif //_DEBUG

	consoleFrame = 0;
	canvasFrame = 0;
	environment = 0;
	calculator = 0;

	generateLatexOutput = true;
	latexCommandDir = "";

	executionDir = wxGetCwd();
}

//=========================================================================================
/*virtual*/ GAVisToolApp::~GAVisToolApp( void )
{
}

//=========================================================================================
bool GAVisToolApp::OnInit( void )
{
	config = new wxConfig( "GAVisToolApp" );
	
	config->Read( wxT( "generateLatexOutput" ), &generateLatexOutput, false );
	config->Read( wxT( "latexCommandDir" ), &latexCommandDir, wxT( "" ) );

	wxImage::AddHandler( new wxPNGHandler );
	
	BuildUserInterface();

	canvasFrame->canvas->RestoreRenderSettings( config );
	canvasFrame->UpdateUserInterface();
	canvasFrame->canvas->RedrawNeeded( true );

	HICON hIcon = ::LoadIcon( wxGetInstance(), MAKEINTRESOURCE( IDI_ICON ) );
	if( hIcon != NULL )
	{
		wxIcon icon;
		icon.SetHICON( hIcon );
		canvasFrame->SetIcon( icon );
		consoleFrame->SetIcon( icon );
	}

	environment = new GAVisToolEnvironment();
	calculator = new CalcLib::Calculator( environment );

	// Return true here to indicate our desire to keep processing the message loop.
	return true;
}

//=========================================================================================
void GAVisToolApp::BuildUserInterface( void )
{
	consoleFrame = new GAVisToolConsoleFrame( wxPoint( 0, 0 ), wxSize( 700, 900 ) );
	canvasFrame = new GAVisToolCanvasFrame( wxPoint( 0, 0 ), wxSize( 700, 900 ) );

	RestoreWindowLayout( consoleFrame, wxT( "consoleFrame" ) );
	RestoreWindowLayout( canvasFrame, wxT( "canvasFrame" ) );
	
	consoleFrame->Show( true );
	canvasFrame->Show( true );
	
	consoleFrame->SetFocus();
}

//=========================================================================================
int GAVisToolApp::OnExit( void )
{
	delete calculator;
	calculator = 0;

	delete config;
	config = 0;

	return wxApp::OnExit();
}

//=========================================================================================
void GAVisToolApp::ExitApp( void )
{
	config->Write( wxT( "generateLatexOutput" ), generateLatexOutput );
	config->Write( wxT( "latexCommandDir" ), latexCommandDir );

	SaveWindowLayout( consoleFrame, wxT( "consoleFrame" ) );
	SaveWindowLayout( canvasFrame, wxT( "canvasFrame" ) );
	
	canvasFrame->canvas->SaveRenderSettings( config );

	canvasFrame->Close( true );
	consoleFrame->Close( true );
}

//=========================================================================================
void GAVisToolApp::SaveWindowLayout( wxFrame* frame, const wxString& frameName )
{
	wxSize size = frame->GetSize();
	wxPoint pos = frame->GetPosition();

	config->Write( frameName + wxT( "_width" ), size.GetX() );
	config->Write( frameName + wxT( "_height" ), size.GetY() );
	config->Write( frameName + wxT( "_posX" ), pos.x );
	config->Write( frameName + wxT( "_posY" ), pos.y );
}

//=========================================================================================
void GAVisToolApp::RestoreWindowLayout( wxFrame* frame, const wxString& frameName )
{
	wxSize size;
	wxPoint pos;
	
	config->Read( frameName + wxT( "_width" ), &size.x, 700 );
	config->Read( frameName + wxT( "_height" ), &size.y, 900 );
	config->Read( frameName + wxT( "_posX" ), &pos.x, 0 );
	config->Read( frameName + wxT( "_posY" ), &pos.y, 0 );
	
	// How do I do this with wxWidgets?
	int screenWidth = ::GetSystemMetrics( SM_CXVIRTUALSCREEN );
	int screenHeight = ::GetSystemMetrics( SM_CYVIRTUALSCREEN );
	
	if( pos.x < 0 )
		pos.x = 0;
	if( size.x > screenWidth )
	{
		size.x = screenWidth;
		pos.x = 0;
	}
	else if( pos.x + size.x > screenWidth )
		pos.x = screenWidth - size.x;

	if( pos.y < 0 )
		pos.y = 0;
	if( size.y > screenHeight )
	{
		size.y = screenHeight;
		pos.y = 0;
	}
	else if( pos.y + size.y > screenHeight )
		pos.y = screenHeight - size.y;

	frame->SetSize( size );
	frame->SetPosition( pos );
}

//=========================================================================================
bool GAVisToolApp::GetGenerateLatexOutput( void )
{
	return generateLatexOutput;
}

//=========================================================================================
void GAVisToolApp::SetGenerateLatexOutput( bool generateLatexOutput )
{
	this->generateLatexOutput = generateLatexOutput;
}

//=========================================================================================
wxString GAVisToolApp::GetLatexCommandDir( void )
{
	return latexCommandDir;
}

//=========================================================================================
void GAVisToolApp::SetLatexCommandDir( const wxString& latexCommandDir )
{
	this->latexCommandDir = latexCommandDir;
}

//=========================================================================================
bool GAVisToolApp::ProcessConsoleInput( const wxString& consoleInput, wxString& consoleOutput, wxImage& consoleOutputImage )
{
	// We haven't generated any output yet.
	bool imageGenerated = false;
	consoleOutput = "";

	// Capture environment output.
	CalcLib::Environment* environment = calculator->GetEnvironment();
	char envPrintBuffer[ PRINT_BUFFER_SIZE_LARGE ];
	environment->PrintBuffer( envPrintBuffer, sizeof( envPrintBuffer ) );
	
	// Generate the math output as a function of the math input.
	const char* mathExpression = consoleInput;
	char resultBuffer[ PRINT_BUFFER_SIZE_LARGE ];
	bool success = calculator->Calculate( mathExpression, resultBuffer, sizeof( resultBuffer ), generateLatexOutput );

	// If something went wront, generate an error report.
	if( !success )
		calculator->PrintErrorReport();

	// Write the environment output to the console first.
	if( strlen( envPrintBuffer ) > 0 )
		consoleOutput += envPrintBuffer;

	// Now write out the result buffer to the console, but only if we succeeded.
	consoleOutput += consoleInput + " --> ";
	if( success )
	{
		if( generateLatexOutput )
		{
			imageGenerated = GenerateLatex( resultBuffer, consoleOutputImage );
			if( !imageGenerated )
				success = false;
		}
		else
			consoleOutput += resultBuffer;
	}
	
	// If we didn't succeed, then we don't know what to show.
	if( !success )
		consoleOutput += "?";

	// Tell the caller if the given image was populated.
	return imageGenerated;
}

//=========================================================================================
bool GAVisToolApp::GenerateLatex( const wxString& latexCode, wxImage& latexImage )
{
	bool success = false;
	
	do
	{
		// The latex code needs to be wrapped in something that can be processed as a file.
		wxString latexFileCode = WrapLatexCode( latexCode );

		// Make sure we have a place to put latex processing files.  Set to work in that place.
		wxString workingDir = wxGetCwd();
		wxString latexOutputDir = executionDir + "\\latexOutput";
		if( latexOutputDir.CmpNoCase( workingDir ) )
		{
			if( !wxDirExists( latexOutputDir ) && !wxMkdir( latexOutputDir ) )
				break;
			if( !wxSetWorkingDirectory( latexOutputDir ) )
				break;
			workingDir = wxGetCwd();
			if( 0 != latexOutputDir.CmpNoCase( workingDir ) )
				break;
		}

		// If we had previously produced an image, delete it now as testing
		// for its existence will be our way of testing success here.
		wxString latexImageFilePath = latexOutputDir + "\\latex1.png";
		if( wxFileExists( latexImageFilePath ) )
			wxRemoveFile( latexImageFilePath );

		// Make sure we know where the user's latex installation resides.
		if( !wxDirExists( latexCommandDir ) )
			break;

		// Write the given code into a latex file.
		wxString latexInputFilePath = latexOutputDir + "\\latex.tex";
		wxFile latexInputFile( latexInputFilePath, wxFile::write );
		if( !latexInputFile.Write( latexFileCode ) )
			break;
		latexInputFile.Close();

		// Formulate and execute a command we can issue to process the latex.
		wxString command = latexCommandDir + "\\latex " + latexInputFilePath;
		if( !ExecuteCommand( command, true ) )
			break;

		// Formulate and execute a command we can issue to create the latex image.
		wxString latexOutputFilePath = latexOutputDir + "\\latex.dvi";
		command = latexCommandDir + "\\dvipng " + latexOutputFilePath;
		if( !ExecuteCommand( command, true ) )
			break;

		// Did we successfully create the image?
		if( !wxFileExists( latexImageFilePath ) )
			break;

		// Try to load up the PNG file.
		if( !latexImage.LoadFile( latexImageFilePath ) )
			break;

		success = true;
	}
	while( false );

	return success;
}

//=========================================================================================
wxString GAVisToolApp::WrapLatexCode( const wxString& latexCode )
{
	wxString preLatexCode =
		"\\documentclass[fleqn]{article}\n"
		"\n"
		"\\usepackage{amsmath}\n"
		"\\usepackage{amssymb}\n"
		"\\usepackage[margin=0.05in]{geometry}\n"
		"\n"
		"\\pagestyle{empty}\n"
		"\n"
		"\\begin{document}\n"
		"\n"
		"\\begin{flalign*}\n";

	wxString postLatexCode =
		"\\end{flalign*}\n"
		"\n"
		"\\end{document}\n";

	return preLatexCode + latexCode + postLatexCode;
}

//=========================================================================================
bool GAVisToolApp::CreateLatexTexture( const wxString& latexCode, GLuint& texture )
{
	wxImage latexImage;
	if( !GenerateLatex( latexCode, latexImage ) )
		return false;

	glGenTextures( 1, &texture );
	glBindTexture( GL_TEXTURE_2D, texture );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	unsigned char* imageData = latexImage.GetData();
	int imageWidth = latexImage.GetWidth();
	int imageHeight = latexImage.GetHeight();
	
	unsigned char* textureData = new unsigned char[ imageWidth * imageHeight * 4 ];
	for( int i = 0; i < imageWidth; i++ )
	{
		for( int j = 0; j < imageHeight; j++ )
		{
			unsigned char* texel = &textureData[ ( i + j*imageWidth )*4 ];
			unsigned char* pixel = &imageData[ ( i + ( imageHeight - 1 - j )*imageWidth )*3 ];

			double intensity = double( pixel[0]*pixel[0] + pixel[1]*pixel[1] + pixel[2]*pixel[2] ) / ( 255.0*255.0*3.0 );
			if( intensity < 0.0 )
				intensity = 0.0;
			if( intensity > 1.0 )
				intensity = 1.0;

			unsigned char alpha = 255 - unsigned char( intensity * 255.0 );

			texel[0] = pixel[0];
			texel[1] = pixel[1];
			texel[2] = pixel[2];
			texel[3] = alpha;
		}
	}
	
	if( 0 != gluBuild2DMipmaps( GL_TEXTURE_2D, 4, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, textureData ) )
		return false;
	
	delete[] textureData;

	return true;
}

//=========================================================================================
// Can we capture the output of the command?
bool GAVisToolApp::ExecuteCommand( const wxString& command, bool waitForProcessCompletion /*= true*/ )
{
	// Can't use wxExecute until I upgrade to a new version of WX.
#if 0
	
	long exitCode = wxExecute( command, wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE );
	if( exitCode != 0 )
		return false;
	return true;

#else

	STARTUPINFO startupInfo;
	memset( &startupInfo, 0, sizeof( STARTUPINFO ) );
	startupInfo.cb = sizeof ( startupInfo );

	PROCESS_INFORMATION processInformation;
	memset( &processInformation, 0, sizeof( PROCESS_INFORMATION ) );

	// Try to execute the command.
	LPWSTR lpszCommand = ( LPWSTR )( const char* )command;
	if( 0 == CreateProcess( NULL, lpszCommand, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInformation ) )
		return false;
	
	// Spin until the process finishes if desired.
	if( waitForProcessCompletion )
	{
		long timeOutMilliseconds = 10 * 1000;
		wxStopWatch stopWatch;
		DWORD dwExitCode = STILL_ACTIVE;
		bool keepWaiting = true;
		do
		{
			dwExitCode = STILL_ACTIVE;
			if( !GetExitCodeProcess( processInformation.hProcess, &dwExitCode ) || dwExitCode != STILL_ACTIVE )
				keepWaiting = false;
			if( stopWatch.Time() >= timeOutMilliseconds )
				keepWaiting = false;
			if( keepWaiting )
				Sleep( 500 );
		}
		while( keepWaiting );

		// Did the process exit with success?
		if( dwExitCode != 0 )
			return false;
		return true;
	}

	return false;

#endif
}

// Application.cpp