#pragma once

#include "CoreMinimal.h"

/**
 *
 */

namespace Optitrack
{
	class Helper
	{
	public:
		Helper();
		~Helper();

		static FRotator ConvertRotatorOfTransformFromLHStoRHS(FTransform _tmpTransform);
	};

}