// ProgressBar.cpp

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "ProgressBar.h"

//=========================================================================================
GAVisToolProgressBar::GAVisToolProgressBar( void )
{
	progressDialog = 0;
}

//=========================================================================================
/*virtual*/ GAVisToolProgressBar::~GAVisToolProgressBar( void )
{
	if( progressDialog )
		delete progressDialog;
}

//=========================================================================================
/*virtual*/ void GAVisToolProgressBar::Begin( const char* message )
{
	if( !progressDialog )
		progressDialog = new wxProgressDialog( wxString( "Working..." ), wxString( message ), 100, 0, wxPD_APP_MODAL );
}

//=========================================================================================
/*virtual*/ void GAVisToolProgressBar::Update( double percentage )
{
	if( progressDialog )
		progressDialog->Update( int( percentage ) );
}

//=========================================================================================
/*virtual*/ void GAVisToolProgressBar::Pulse( void )
{
	if( progressDialog )
		progressDialog->Pulse();
}

//=========================================================================================
/*virtual*/ void GAVisToolProgressBar::Finished( void )
{
	if( progressDialog )
	{
		delete progressDialog;
		progressDialog = 0;
	}
}

// ProgressBar.cpp