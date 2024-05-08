// Fill out your copyright notice in the Description page of Project Settings.


#include "SplitScreenCamera.h"

#include "SplitScreenCPPCharacter.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASplitScreenCamera::ASplitScreenCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASplitScreenCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASplitScreenCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(Players.Num() == 2)
	{
		UpdateCamera(DeltaTime);
	}
}

void ASplitScreenCamera::AddPlayer(ASplitScreenCPPCharacter* Player)
{
	int index = Players.AddUnique(Player);
	if(index != -1)
	{
		if(Player->FollowCamera != nullptr)
		{
			Player->FollowCamera->TextureTarget = TextureTargets[index];
			if(index == 0)
			{
				APlayerController* Controller = Cast<APlayerController>(Player->GetController());
				if(Controller)
				{
					Controller->SetViewTargetWithBlend(this);
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("NO STUFF!"));
		}
	}
}

void ASplitScreenCamera::RemovePlayer(ASplitScreenCPPCharacter* Player)
{
	Players.Remove(Player);
}

void ASplitScreenCamera::UpdateCamera(float DeltaTime)
{
	
}

void ASplitScreenCamera::UpdateResolution()
{
	
}

void ASplitScreenCamera::UpdatePlayerCapture(int32 PlayerID, float _ArmLength, FRotator NewRotation, FVector NewLocation,
	float Angle)
{
	ASplitScreenCPPCharacter* Player = Players[PlayerID];
	Player->GetCameraBoom()->TargetArmLength = ArmLength;
	Player->GetCameraBoom()->SetWorldRotation(NewRotation);
	Player->GetCameraBoom()->SetWorldLocation(NewLocation);

	FVector Rotation = Player->FollowCamera->GetRightVector().RotateAngleAxis(Angle, Player->FollowCamera->GetForwardVector());
	Player->FollowCamera->ClipPlaneBase = Player->FollowCamera->GetComponentLocation() - (Rotation * 15);
	Player->FollowCamera->ClipPlaneNormal = Rotation;
	
}

float ASplitScreenCamera::UpdateSplitAngle(float Angle, float DeltaTime, float& _SplitAngle)
{
	SplitAngle = Angle;
	return SplitAngle;
}

