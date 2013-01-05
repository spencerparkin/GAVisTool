// Interface.h

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
#include "BindTarget.h"

//=========================================================================================
class GAVisToolInterface : public GAVisToolBindTarget
{
	DECLARE_CALCLIB_CLASS( GAVisToolInterface );

public:

	GAVisToolInterface( BindType bindType );
	virtual ~GAVisToolInterface( void );

protected:

	void InstallPanel( wxPanel* panel, const wxString& caption );
	void UninstallPanel( wxPanel* panel );
};

// Interface.h