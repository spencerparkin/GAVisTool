// BinaryFunctionSearch.h

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

namespace VectorMath
{
	class BinaryFunctionSearch
	{
	public:

		// We assume that this function is continuous on any
		// compact interval so that we can apply the intermediate
		// value theorem.
		class Function
		{
		public:
			virtual double Evaluate( double x ) = 0;
		};

		BinaryFunctionSearch( Function* function );
		virtual ~BinaryFunctionSearch( void );

		// Return the first found value x, if any, in the given range such that f(x)=y.
		bool FindValue( double& x, double y, double x0, double x1 );

		Function* function;
	};
}

// BinaryFunctionSearch.h