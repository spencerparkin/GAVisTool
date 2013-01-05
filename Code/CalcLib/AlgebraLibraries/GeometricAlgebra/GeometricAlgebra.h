// GeometricAlgebra.h

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
#include "../LinearAlgebra/LinearAlgebra.h"

//=========================================================================================
namespace GeometricAlgebra
{
	typedef ScalarAlgebra::Scalar Scalar;

	class Vector;
	class Blade;
	class PseudoVersor;
	class SumOfBlades;
	class SumOfPseudoVersors;

	class Vector_e0;
	class Vector_e1;
	class Vector_e2;
	class Vector_e3;
	class Vector_no;
	class Vector_ni;
	class Vector_e0_bar;
	class Vector_e1_bar;
	class Vector_e2_bar;
	class Vector_e3_bar;
	class Vector_no_bar;
	class Vector_ni_bar;

	class CompositeVector;
}

//=========================================================================================
#include "Vector.h"
#include "Blade.h"
#include "PseudoVersor.h"
#include "SumOfBlades.h"
#include "SumOfPseudoVersors.h"
#include "CompositeVector.h"

// GeometricAlgebra.h