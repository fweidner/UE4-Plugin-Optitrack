#include "Transformer.h" //needs to be first.

#include "Helper.h"

namespace Optitrack
{
	static FRotator ViewDirectionForward = FRotator(0.f, 0.f, 0.f);

	Transformer::Transformer()
	{
		ViewDirectionForward = FRotator(0.f, 0.f,0.f);
	}

	Transformer::~Transformer()
	{
	}


	FTransform Transformer::UpdateTransformForPawn(APawn* _Pawn, FTransform _tmpTransform)
	{
		_Pawn->SetActorLocation(_tmpTransform.GetLocation());

		FRotator tmpRotator = Optitrack::Helper::ConvertRotatorOfTransformFromLHStoRHS(_tmpTransform);
		tmpRotator -= GetViewDirectionForward();
		_tmpTransform.SetRotation(tmpRotator.Quaternion());
		_Pawn->GetController()->SetControlRotation(tmpRotator);

		_tmpTransform.SetScale3D(_Pawn->GetActorScale3D());

		return _tmpTransform;
	}

	FTransform Transformer::UpdateTransformForSceneComponent(USceneComponent* _tmpSceneComponent, FTransform tmpTransform)
	{
		_tmpSceneComponent->SetWorldLocation(tmpTransform.GetLocation());
		_tmpSceneComponent->SetWorldRotation(Helper::ConvertRotatorOfTransformFromLHStoRHS(tmpTransform));
		tmpTransform.SetScale3D(_tmpSceneComponent->GetComponentScale());

		return tmpTransform;
	}

	FTransform Transformer::UpdateTransformForActor(AActor* _tmpActor, FTransform tmpTransform)
	{
		_tmpActor->SetActorLocation(tmpTransform.GetLocation());

		_tmpActor->SetActorRotation(Helper::ConvertRotatorOfTransformFromLHStoRHS(tmpTransform));
		
		tmpTransform.SetScale3D(_tmpActor->GetActorScale3D());

		return tmpTransform;

	}

	FRotator Transformer::SetViewDirectionForward(FRotator _newVec)
	{
		ViewDirectionForward = _newVec;
		return ViewDirectionForward;
	}

	FRotator Transformer::GetViewDirectionForward()
	{
		return ViewDirectionForward;
	}

}
