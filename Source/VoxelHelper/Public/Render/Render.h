// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxelTypes.h"

#include "GameFramework/Actor.h"
#include "Render.generated.h"

UCLASS()
class VOXELHELPER_API ARender : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARender();
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Render")
	class UProceduralMeshComponent* ProceduralMeshComponent = nullptr;


	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Render")
	UMaterialInterface* ShowMaterial = nullptr;
	
	
	void CF_Render(const FDrawElement& DrawElement);
	void CF_Clear();
	void CF_FlushMaterial();
};

