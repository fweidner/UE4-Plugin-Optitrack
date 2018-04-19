

#pragma once

#include "CoreMinimal.h"

/**
 *
 */


namespace Optitrack
{
	class Transformer
	{
	public:
		Transformer();
		~Transformer();

		static FTransform UpdateTransformForPawn(APawn* _Pawn, FTransform _tmpTransform);
		static FTransform UpdateTransformForSceneComponent(USceneComponent* _tmpSceneComponent, FTransform tmpTransform);

		static FTransform UpdateTransformForActor(AActor* _tmpActor, FTransform tmpTransform);

		static FRotator SetViewDirectionForward(FRotator _newVec);
		static FRotator GetViewDirectionForward();
	private:

	};
}