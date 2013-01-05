// Assert.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

namespace VectorMath
{
	class Assert
	{
	public:
		Assert( void );
		virtual ~Assert( void );

		void Condition( bool condition, const char* formatString, ... );

		virtual void AssertionFailed( const char* messageString );
	};

	extern Assert* assert;
}

// Assert.h