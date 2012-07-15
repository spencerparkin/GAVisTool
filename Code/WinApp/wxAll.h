// wxAll.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#pragma warning( disable : 4267 )		// Disable warning about conversions from 'size_t' to 'unsigned int', etc.

#include <wx/setup.h>
#include <wx/app.h>
#include <wx/icon.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/glcanvas.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/textdlg.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtextformatdlg.h>
#include <wx/file.h>
#include <wx/filedlg.h>
#include <wx/event.h>
#include <wx/stopwatch.h>
#include <wx/config.h>
#include <wx/dirdlg.h>
#include <wx/aboutdlg.h>
#include <wx/statusbr.h>
#include <wx/fontdlg.h>
#include <wx/colordlg.h>
#include <wx/numdlg.h>
#include <wx/aui/aui.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/treectrl.h>

// This is cheating.
extern "C" HINSTANCE wxGetInstance();

// wxAll.h