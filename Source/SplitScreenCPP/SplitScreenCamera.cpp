// Fill out your copyright notice in the Description page of Project Settings.


#include "SplitScreenCamera.h"

#include "SplitScreenCPPCharacter.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/GameViewportClient.h"
#include "Kismet/KismetRenderingLibrary.h"

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
		UpdateResolution();
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
	TArray<FVector> PlayerLocations;
	TArray<FVector> PlayerLocationProjected;
	for (auto Player : Players)
	{
		PlayerLocations.Add(Player->GetActorLocation());
	}
	FVector PlayerAverage = UKismetMathLibrary::GetVectorArrayAverage(PlayerLocations);
	for(auto PlayerLocation : PlayerLocations)
	{
		FVector Point = UKismetMathLibrary::ProjectPointOnToPlane(PlayerLocation, PlayerAverage, GetCameraComponent()->GetForwardVector());
		PlayerLocationProjected.Add(Point);
	}
	//Angle splitting
	FVector FirstPlayerProjectedLocation = PlayerLocationProjected[0];
	FVector Middle = UKismetMathLibrary::Subtract_VectorVector(FirstPlayerProjectedLocation, PlayerAverage);
	FVector NormalizedVector = UKismetMathLibrary::Normal(Middle);
	FVector CameraRightVector = GetCameraComponent()->GetRightVector();
	float DotProduct = UKismetMathLibrary::Dot_VectorVector(NormalizedVector, CameraRightVector);
	auto CrossProduct = UKismetMathLibrary::Cross_VectorVector(NormalizedVector, CameraRightVector);
	float ACOSd = UKismetMathLibrary::DegAcos(DotProduct);
	float cross = UKismetMathLibrary::SignOfFloat(CrossProduct.Z);
	float Angle = ACOSd * cross;
	
	UpdateSplitAngle(Angle, DeltaTime);

	PP_Material_Instance->SetScalarParameterValue("Angle", Angle + 90);
	
	FVector2D Viewport;
	GEngine->GameViewport->GetViewportSize(Viewport);
	float ViewportHV = Viewport.Y / Viewport.X;
	float CameraFOVDivied = UKismetMathLibrary::Divide_DoubleDouble(GetCameraComponent()->FieldOfView, 2.0f);
	float CameraFOVTAN = UKismetMathLibrary::Tan(CameraFOVDivied);
	float TargetArmLength = Players[0]->CameraBoom->TargetArmLength;
	float ViewportHorizontal = CameraFOVTAN * TargetArmLength;
	float ViewportVertical = ViewportHV * ViewportHorizontal;

	float splitSIN = UKismetMathLibrary::DegSin(SplitAngle);
	float splitSINABS = UKismetMathLibrary::Abs(splitSIN);
	
	float splitCOS = UKismetMathLibrary::DegCos(SplitAngle);
	float splitCOSABS = UKismetMathLibrary::Abs(splitCOS);

	float splitHorizontal = ViewportHorizontal * splitCOSABS;
	float splitVertical = ViewportVertical * splitSINABS;

	float hypotenuse = UKismetMathLibrary::Hypotenuse(splitHorizontal, splitVertical);

	float splitStart = hypotenuse * MaxDistanceBetweenPlayers * 0.5f;

	
	FVector PlayerOneLocation = PlayerLocationProjected[0];
	FVector PlayerTwoLocation = PlayerLocationProjected[1];

	float DistancePlayerOne = UKismetMathLibrary::Vector_Distance(PlayerOneLocation, PlayerAverage);
	float DistancePlayerTwo = UKismetMathLibrary::Vector_Distance(PlayerTwoLocation, PlayerAverage);

	float MaxDistance = UKismetMathLibrary::Max(DistancePlayerOne, DistancePlayerTwo);
	float SplitRatio = UKismetMathLibrary::Divide_DoubleDouble(MaxDistance, splitStart);
	
	float SplitRatioMinus = UKismetMathLibrary::FClamp(SplitRatio - 1.0f, 0.0f, 1.0f);
	PP_Material_Instance->SetScalarParameterValue("Line Thickness", SplitRatioMinus);

	float ArmLengthAverage = UKismetMathLibrary::Subtract_DoubleDouble(ArmLengthMax, ArmLengthMin);
	float ClampedSplitRatio = UKismetMathLibrary::FClamp(SplitRatio, 0.0f, 1.0f);

	float SplitRatioArmLength = UKismetMathLibrary::Multiply_DoubleDouble(ArmLengthAverage, ClampedSplitRatio);
	float ArmLengthInterp = UKismetMathLibrary::Add_DoubleDouble(ArmLengthMin, SplitRatioArmLength);
	ArmLength = UKismetMathLibrary::FInterpTo(ArmLength, ArmLengthInterp, DeltaTime, ZoomSpeed);

	UE_LOG(LogTemp, Warning, TEXT("Arm Length: %f"), ArmLength);

	

	FVector PlayerLocation = PlayerLocations[0];
	FVector ProjectedLocation = PlayerLocationProjected[0];
	FVector AverageLocationA = UKismetMathLibrary::Subtract_VectorVector(PlayerLocation, PlayerAverage);
	FVector AverageLocationB = UKismetMathLibrary::Subtract_VectorVector(ProjectedLocation, PlayerAverage);
	FVector CapturePointA = UKismetMathLibrary::Normal(AverageLocationA);
	FVector CapturePointB = UKismetMathLibrary::Normal(AverageLocationB);
	float Dot = UKismetMathLibrary::Dot_VectorVector(CapturePointA, CapturePointB);
	float SpreadCameraAngleCorrection = Dot;

	for (int i = 0; i < Players.Num(); i++)
	{
		FRotator CameraRotation = GetCameraComponent()->GetComponentRotation();
		FVector CurrentPlayerLoc = PlayerLocations[i];
		FVector CurrentPlayerAverage = UKismetMathLibrary::Subtract_VectorVector(PlayerAverage, CurrentPlayerLoc);
		FVector NormalizedCurrentPlayerAverage = UKismetMathLibrary::Normal(CurrentPlayerAverage);
		FVector Split = NormalizedCurrentPlayerAverage * (splitStart * SpreadCameraAngleCorrection);
		FVector PlayerLocationPlusSplit = UKismetMathLibrary::Add_VectorVector(CurrentPlayerLoc, Split);
		FVector PlayerCameraNewLocation = UKismetMathLibrary::SelectVector(PlayerAverage, PlayerLocationPlusSplit, UKismetMathLibrary::LessEqual_DoubleDouble(SplitRatio, 1.0f));

		float NewAngle = UKismetMathLibrary::SelectFloat(180.0f, 0.0f, UKismetMathLibrary::Conv_IntToBool(i));
		float FinalAngle = UKismetMathLibrary::Add_DoubleDouble(SplitAngle, NewAngle);

		UpdatePlayerCapture(i, ArmLength, CameraRotation, PlayerCameraNewLocation, FinalAngle);
	}
	
	
}

void ASplitScreenCamera::UpdateResolution()
{
	FVector2D Viewport;
	GEngine->GameViewport->GetViewportSize(Viewport);
	if(Viewport != ViewportSize)
	{
		ViewportSize = Viewport;
		FLinearColor LinearColor(ViewportSize.X, ViewportSize.Y, 0, 0);
		PP_Material_Instance->SetVectorParameterValue("Resolution", LinearColor);
		for (auto TextureTarget : TextureTargets)
		{
			UKismetRenderingLibrary::ResizeRenderTarget2D(TextureTarget, ViewportSize.X, ViewportSize.Y);
		}
	}
}

void ASplitScreenCamera::UpdatePlayerCapture(int32 PlayerID, float _ArmLength, FRotator NewRotation, FVector NewLocation,
	float Angle)
{
	ASplitScreenCPPCharacter* Player = Players[PlayerID];
	Player->GetCameraBoom()->TargetArmLength = _ArmLength;
	Player->GetCameraBoom()->SetWorldRotation(NewRotation);
	Player->GetCameraBoom()->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::None);
	
	FVector Rotation = UKismetMathLibrary::RotateAngleAxis(Player->FollowCamera->GetRightVector(), Angle, Player->FollowCamera->GetForwardVector());
	FVector RotationMultiply = UKismetMathLibrary::Multiply_VectorFloat(Rotation, 15);
	Player->FollowCamera->ClipPlaneBase = UKismetMathLibrary::Subtract_VectorVector(Player->FollowCamera->GetComponentLocation(), RotationMultiply);
	Player->FollowCamera->ClipPlaneNormal = Rotation;
	
}

float ASplitScreenCamera::UpdateSplitAngle(float Angle, float DeltaTime)
{
	SplitAngle = Angle;
	return SplitAngle;
}

