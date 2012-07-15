// Camera.cpp

/*
 * Copyright (C) 2012 Spencer T. Parkin
 *
 * This software has been released under the MIT License.
 * See the "License.txt" file in the project root directory
 * for more information about this license.
 *
 */

#include "Camera.h"
#include "wxAll.h"
#include <gl/GLU.h>

//=============================================================================
GAVisToolCamera::GAVisToolCamera( void )
{
	VectorMath::Set( eye, 20.f, 20.f, 20.f );
	VectorMath::Set( focus, 0.f, 0.f, 0.f );
	VectorMath::Set( up, 0.f, 1.f, 0.f );
}

//=============================================================================
GAVisToolCamera::~GAVisToolCamera( void )
{
}

//=============================================================================
void GAVisToolCamera::Orient( void )
{
	gluLookAt(
			eye.x, eye.y, eye.z,
			focus.x, focus.y, focus.z,
			up.x, up.y, up.z );
}

//=============================================================================
// The camera looks down the -Z axis.
void GAVisToolCamera::CameraFrame( VectorMath::CoordFrame& cameraFrame )
{
	VectorMath::Sub( cameraFrame.zAxis, eye, focus );
	VectorMath::Normalize( cameraFrame.zAxis, cameraFrame.zAxis );
	VectorMath::Cross( cameraFrame.xAxis, up, cameraFrame.zAxis );
	VectorMath::Normalize( cameraFrame.xAxis, cameraFrame.xAxis );
	VectorMath::Cross( cameraFrame.yAxis, cameraFrame.zAxis, cameraFrame.xAxis );
}

//=============================================================================
void GAVisToolCamera::CameraLookVec( VectorMath::Vector& cameraLookVec )
{
	VectorMath::Sub( cameraLookVec, focus, eye );
}

//=============================================================================
void GAVisToolCamera::MoveEye( const VectorMath::Vector& delta, bool maintainFocalDistance /*= true*/ )
{
	float oldFocalDist = VectorMath::Distance( eye, focus );

	VectorMath::Vector originalEye;
	VectorMath::Copy( originalEye, eye );

	VectorMath::Add( eye, eye, delta );

	float minFocalDist = 0.5f;
	float newFocalDist = VectorMath::Distance( eye, focus );
	if( newFocalDist < minFocalDist )
	{
		VectorMath::Copy( eye, originalEye );
		return;
	}

	if( maintainFocalDistance )
	{
		VectorMath::Vector lookVec;
		VectorMath::Sub( lookVec, eye, focus );
		VectorMath::Normalize( lookVec, lookVec );
		VectorMath::Scale( lookVec, lookVec, oldFocalDist );
		VectorMath::Add( eye, focus, lookVec );
	}
}

//=============================================================================
void GAVisToolCamera::MoveEyeAndFocus( const VectorMath::Vector& delta )
{
	VectorMath::Add( eye, eye, delta );
	VectorMath::Add( focus, focus, delta );
}

//=============================================================================
void GAVisToolCamera::SetFocus( const VectorMath::Vector& focus )
{
	VectorMath::Copy( this->focus, focus );
}

// Camera.cpp