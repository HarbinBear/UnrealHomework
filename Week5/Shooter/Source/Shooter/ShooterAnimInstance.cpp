// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"

#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


UShooterAnimInstance::UShooterAnimInstance():
	RootYawOffset(0.f)
{

}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if(ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter, APawn>(TryGetPawnOwner());
	}
	if(ShooterCharacter)
	{
		// speed
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// Is In Air
		bIsInAir = ShooterCharacter->GetMovementComponent()->IsFalling();

		// Is Moving
		if(ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f)
		{
			bIsAccelarating = true;
		}
		else
		{
			bIsAccelarating = false;
		}

		if(ShooterCharacter->GetVelocity().Size() > 0.f )
		{
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
		
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, MovementRotation).Yaw;



		bIsAimming = ShooterCharacter->GetIsAimming();
			
		BaseYawOffset = ShooterCharacter->GetBaseAimRotation().Yaw;
		BasePitchOffset = ShooterCharacter->GetBaseAimRotation().Pitch;

		bIsAlting = ShooterCharacter->GetIsAlting();



		//if(Speed > 0 )
		//{
		//	// do not turn in place while moving .
		//}
		//else
		//{
		//	CharacterYawLastFrame = CharacterYaw;
		//	CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		//	const float DeltaYaw{ CharacterYaw - CharacterYawLastFrame };
		//	RootYawOffset -= DeltaYaw;

		//}
		CharacterYawLastFrame = CharacterYaw;
		CharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float DeltaYaw{ CharacterYaw - CharacterYawLastFrame };

		if( bIsAlting )
		{

			RootYawOffset -= DeltaYaw;
		}
		else
		{
			RootYawOffset = 0;
		}






		//FString RotationMsg = FString::Printf(TEXT("Aim Rotation: %f"), AimRotation.Yaw);
		//FString MovementRotationMsg = FString::Printf(TEXT("Movement Rotation: %f"), MovementRotation.Yaw);
		//FString MovementOffsetYawMsg = FString::Printf(TEXT("Movement Offset Yaw: %f"), MovementOffsetYaw);
		//FString AimMsg = FString::Printf(TEXT("isAimming? %d"), bIsAimming);
		//FString RYOMsg = FString::Printf(TEXT("RootYawOffset: %f"), RootYawOffset);
		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(1, 0, FColor::Orange, RYOMsg);
		//}

	}

}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	ShooterCharacter = Cast<AShooterCharacter, APawn>(TryGetPawnOwner());

}
