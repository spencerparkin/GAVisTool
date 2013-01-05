// ProgressBar.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "wxAll.h"
#include "VectorMath/ProgressInterface.h"

//=========================================================================================
class GAVisToolProgressBar : public VectorMath::ProgressInterface
{
public:

	GAVisToolProgressBar( void );
	virtual ~GAVisToolProgressBar( void );

	virtual void Begin( const char* message ) override;
	virtual void Update( double percentage ) override;
	virtual void Pulse( void ) override;
	virtual void Finished( void ) override;

	wxProgressDialog* progressDialog;
};

// ProgressBar.h