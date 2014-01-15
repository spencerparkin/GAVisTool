// Camera.h

/*
 * Copyright (C) 2013-2014 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#pragma once

#include "VectorMath/Vector.h"
#include "VectorMath/CoordFrame.h"

//=============================================================================
class GAVisToolCamera
{
public:

	GAVisToolCamera( void );
	virtual ~GAVisToolCamera( void );

	void Orient( void );

	void MoveEye( const VectorMath::Vector& delta, bool maintainFocalDistance = true );
	void MoveEyeAndFocus( const VectorMath::Vector& delta );
	void SetFocus( const VectorMath::Vector& focus );

	void CameraFrame( VectorMath::CoordFrame& cameraFrame );
	void CameraLookVec( VectorMath::Vector& cameraLookVec );

	const VectorMath::Vector& Eye( void ) const { return eye; }
	const VectorMath::Vector& Focus( void ) const { return focus; }
	const VectorMath::Vector& Up( void ) const { return up; }

private:

	VectorMath::Vector eye;
	VectorMath::Vector focus;
	VectorMath::Vector up;
};

// Camera.h