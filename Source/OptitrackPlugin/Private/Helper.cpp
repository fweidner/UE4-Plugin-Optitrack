#include "Helper.h"

namespace Optitrack
{
	Helper::Helper()
	{

	}

	Helper::~Helper()
	{

	}
	
	FRotator Helper::ConvertRotatorOfTransformFromLHStoRHS(FTransform _tmpTransform)
	{
		FQuat tmpQuat = _tmpTransform.GetRotation();
		_tmpTransform.SetRotation(FQuat(tmpQuat.Y, tmpQuat.X, -tmpQuat.Z, tmpQuat.W));
		return _tmpTransform.Rotator();
	}


}


