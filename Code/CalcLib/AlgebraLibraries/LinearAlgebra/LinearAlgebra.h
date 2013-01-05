// LinearAlgebra.h

/*
 * Copyright (C) 2013 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include <stdio.h>
#include <string.h>

#include "../../Utilities/Utilities.h"
#include "../ScalarAlgebra/ScalarAlgebra.h"

//=========================================================================================
namespace LinearAlgebra
{
	typedef ScalarAlgebra::Scalar Scalar;

	class Matrix;
	class Element;
}

#include "Matrix.h"
#include "Element.h"

// LinearAlgebra.h