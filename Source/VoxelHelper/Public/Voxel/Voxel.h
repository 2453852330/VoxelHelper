// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastNoiseLite.h"
#include "VoxelTypes.h"

#include "GameFramework/Actor.h"
#include "Voxel.generated.h"

class ARender;

UCLASS()
class VOXELHELPER_API AVoxel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxel();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Voxel")
	FIntVector MapSize;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Voxel")
	float MapCheckValue = 0.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Voxel")
	bool bSmooth = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Voxel")
	float NoiseFrequency = 0.02f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Voxel")
	ENoiseType NoiseType = ENoiseType::NoiseType_Perlin;	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Voxel")
	ARender* Render = nullptr;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Voxel")
	ERenderType RenderType;

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Voxel")
	float TestRadius = 10.f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Voxel")
	bool bTestSphere = true;

	// --------------------------------------- 2D ------------------------------------------------
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Voxel")
	bool bUse2D = true;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Voxel")
	ERenderType2D RenderType2D = ERenderType2D::BOX;



	
	
	UFUNCTION(BlueprintCallable,CallInEditor,Category="Voxel")
	void BP_GeneratedWorld();

	//
	TArray<float> CF_GeneratedMap(int32 X,int32 Y,int32 Z);
	TArray<float> CF_GeneratedMap2D(int32 X,int32 Y);

	FastNoiseLite::NoiseType CF_GetNoise();

	/**
	* noise.get(1,1,1)=-0.058597                  
	* noise.get(1.5,1.5,1.5)=-0.089084                  
	* noise.get(1.55,1.55,1.55)=-0.092196 
	 */
	FastNoiseLite noise;

	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};

