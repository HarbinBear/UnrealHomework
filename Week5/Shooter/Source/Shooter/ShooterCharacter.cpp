// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values;
AShooterCharacter::AShooterCharacter():

	HipTurnRate(45.f),
	HipLookUpRate(45.f),
	AimTurnRate(15.f),
	AimLookUpRate(15.f),

	MouseAimTurnRate(2.f),
	MouseAimLookUpRate(2.f),
	MouseHipTurnRate(10.f),
	MouseHipLookUpRate(10.f),

	BaseTurnRate(45.0f),
	BaseLookUpRate(45.0f),

	bIsAimming(false),

	CameraDefaultFOV(0.f), // init in BeginPlay()
	CameraZoomedFOV(35.f),
	CameraCurrentFOV(0.f),
	CameraZoomInterpSpeed(20.f),

	MoveSpeedFactor(0.6f)


{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 180.f;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0, 540.f, 0);
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->JumpZVelocity = 600.f;



}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();


	if(FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}


}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraInterpZoom(DeltaTime);

	UpdateLookUpTurnRate();

}




// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::MouseTurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::MouseLookUpAtRate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this,&ACharacter::Jump );
	PlayerInputComponent->BindAction("Jump", IE_Released, this,&ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this,&AShooterCharacter::FireWeapon);

	PlayerInputComponent->BindAction("AimmingButton", IE_Pressed, this,&AShooterCharacter::AimmingButtonPressed);
	PlayerInputComponent->BindAction("AimmingButton", IE_Released, this,&AShooterCharacter::AimmingButtonReleased);

	PlayerInputComponent->BindAction("AltButton", IE_Pressed, this, &AShooterCharacter::AltButtonPressed);
	PlayerInputComponent->BindAction("AltButton", IE_Released, this, &AShooterCharacter::AltButtonReleased);
}

void AShooterCharacter::MoveForward(float Value)
{
	if( Controller != nullptr && Value != 0.0f )
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value * MoveSpeedFactor );

	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0,Rotation.Yaw,0 };
		const FVector Direction{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value * MoveSpeedFactor);

	}
}

/**
 * @brief Called via input to turn at a given rate.
 * @param Rate  This is a normalized rate, i.e. 1.0f means 100% of desired turning rate.
 */
void AShooterCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame = deg/frame
}


void AShooterCharacter::LookUpAtRate(float Rate)
{
	int sign = -1;
	AddControllerPitchInput(sign * Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	
}

void AShooterCharacter::MouseTurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseMouseTurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame = deg/frame
}

void AShooterCharacter::MouseLookUpAtRate(float Rate)
{

	AddControllerPitchInput(Rate * BaseMouseLookUpRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame = deg/frame

}

void AShooterCharacter::FireWeapon()
{
	if(FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	/* particle */


	/* Montage */
	UAnimInstance* AnimInstance =  GetMesh()->GetAnimInstance();
	if(AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}

}

void AShooterCharacter::AimmingButtonPressed()
{
	bIsAimming = true;

}

void AShooterCharacter::AimmingButtonReleased()
{
	bIsAimming = false;
}

void AShooterCharacter::AltButtonPressed()
{
	bIsAlting = true;
	
}

void AShooterCharacter::AltButtonReleased()
{
	bIsAlting = false;
}

void AShooterCharacter::CameraInterpZoom( float DeltaTime )
{
	float CameraTargetFOV;

	/* set current FOV */
	if (bIsAimming)
	{
		CameraTargetFOV = CameraZoomedFOV;
	}
	else
	{
		CameraTargetFOV = CameraDefaultFOV;
	}

	CameraCurrentFOV = FMath::FInterpTo(
		CameraCurrentFOV,
		CameraTargetFOV,
		DeltaTime,
		CameraZoomInterpSpeed);

	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

bool AShooterCharacter::GetIsAimming()
{
	return bIsAimming;
}

bool AShooterCharacter::GetIsAlting()
{
	return bIsAlting;
}

void AShooterCharacter::UpdateLookUpTurnRate()
{
	if(bIsAimming)
	{
		BaseLookUpRate = AimLookUpRate;
		BaseTurnRate = AimTurnRate;

		BaseMouseLookUpRate = MouseAimLookUpRate;
		BaseMouseTurnRate = MouseAimTurnRate;
	}
	else
	{
		BaseLookUpRate = HipLookUpRate;
		BaseTurnRate = HipTurnRate;

		BaseMouseLookUpRate = MouseHipLookUpRate;
		BaseMouseTurnRate = MouseHipTurnRate;
	}

}







