// ProgressInterface.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

namespace VectorMath
{
	//=============================================================================
	// Some algorithms in this vector math library take
	// a bit of time to complete.  In those cases, this
	// interface can be used to provide feedback on how
	// far along the algorithm is and when it is expected
	// to finally complete.
	class ProgressInterface
	{
	public:

		ProgressInterface( void );
		virtual ~ProgressInterface( void );

		virtual void Begin( const char* message ) = 0;
		virtual void Update( double percentage ) = 0;
		virtual void Pulse( void ) = 0;
		virtual void Finished( void ) = 0;
	};
}

// ProgressInterface.h