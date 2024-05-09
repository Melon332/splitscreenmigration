// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraActor.h"
#include "SplitScreenCamera.generated.h"


class ASplitScreenCPPCharacter;
class UCameraComponent;
UCLASS()
class SPLITSCREENCPP_API ASplitScreenCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASplitScreenCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/** Registers players to the system and assigns the render targets for their scene captures */
	UFUNCTION(BlueprintCallable)
	void AddPlayer(ASplitScreenCPPCharacter* Player);

	/** Please add a function description */
	UFUNCTION(BlueprintCallable)
	void RemovePlayer(ASplitScreenCPPCharacter* Player);

	/** Updates the captures and the rendered view */
	UFUNCTION(BlueprintCallable, Category="Default")
	void UpdateCamera(float DeltaTime);

	/** Updates render capture resolution based on the viewport size */
	UFUNCTION(BlueprintCallable)
	void UpdateResolution();

	/** Updates the scene capture settings on the players */
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerCapture(int32 PlayerID, float _ArmLength, FRotator NewRotation, FVector NewLocation, float Angle);

	/** Applies interpolation and step rotation to the split angle */
	UFUNCTION(BlueprintCallable)
	float UpdateSplitAngle(float Angle, float DeltaTime);

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TArray<ASplitScreenCPPCharacter*> Players;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TArray<UTextureRenderTarget2D*> TextureTargets;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TObjectPtr<UMaterialInstanceDynamic> PP_Material_Instance;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	bool OnlineMode;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Zoom")
	double ArmLengthMin;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Zoom")
	double ArmLengthMax;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Zoom")
	double ZoomSpeed;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Zoom")
	double ArmLength;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Step Rotation")
	bool DoStepRotation;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Step Rotation")
	float StepRotationStartingAngle;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Step Rotation")
	int32 StepRotationSteps;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Step Rotation")
	float StepRotationSpeed;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Step Rotation")
	int32 TargetStep;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	float SplitAngle;

	/** Max distance between players (in screen %) before the split starts */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	double MaxDistanceBetweenPlayers;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	FVector2D ViewportSize;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* SplitScreenMaterial;
};
