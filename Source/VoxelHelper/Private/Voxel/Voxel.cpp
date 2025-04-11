// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel/Voxel.h"

#include "FastNoiseLite.h"
#include "LogHelper.h"
#include "ProceduralMeshComponent.h"

#include "Algo/Box2D.h"
#include "Algo/DualMarchingCubes.h"
#include "Algo/MarchingCubes.h"
#include "Algo/MarchingTetrahedrons.h"

#include "Render/Render.h"

// Sets default values
AVoxel::AVoxel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MapSize = FIntVector(10);
	RenderType = ERenderType::MarchingCubes;
}

PRAGMA_DISABLE_OPTIMIZATION

void AVoxel::BP_GeneratedWorld()
{
	if (!Render)
	{
		UE_LOG(LogTemp,Error,TEXT("AVoxel::BP_GeneratedWorld -> Render is nullptr"));
		return;		
	}
	FlushPersistentDebugLines(GetWorld());
	Render->CF_Clear();
	Render->CF_FlushMaterial();
	
	if (bUse2D)
	{
		TArray<float> Map = CF_GeneratedMap2D(MapSize.X,MapSize.Y);
		switch (RenderType2D)
		{
		case ERenderType2D::BOX:
			{
				FDrawElement DrawElement = Box2D::GetDrawElement(Map,MapSize,MapCheckValue,false);
				Render->CF_Render(DrawElement);
				break;
			}
		case ERenderType2D::SmoothBox:
			{
				FDrawElement DrawElement = Box2D::GetDrawElement(Map,MapSize,MapCheckValue,true);
				Render->CF_Render(DrawElement);
				break;
			}
		}
		return;
	}

	
	// ------------------------- 3d-----------------------------------
	TArray<float> Map = CF_GeneratedMap(MapSize.X,MapSize.Y,MapSize.Z);
	switch (RenderType)
	{
	case ERenderType::MarchingCubes:
		{
			FDrawElement DrawElement = MarchingCubes::GetDrawElement(Map,MapSize.X,MapSize.Y,MapSize.Z,MapCheckValue,bSmooth);
			Render->CF_Render(DrawElement);
			break;
		}
	case ERenderType::DualMarchingCubes:
		{
			FDrawElement DrawElement = DualMarchingCubes::GetDrawElement(GetWorld(),Map,MapSize.X,MapSize.Y,MapSize.Z,MapCheckValue,noise,bSmooth);
			Render->CF_Render(DrawElement);
			break;
		}
	case ERenderType::MarchingTetrahedrons:
		{
			FDrawElement DrawElement = MarchingTetrahedrons::GetDrawElement(GetWorld(),Map,MapSize.X,MapSize.Y,MapSize.Z,MapCheckValue,noise,bSmooth);
			Render->CF_Render(DrawElement);
			break;
		}
	}
}


PRAGMA_ENABLE_OPTIMIZATION


TArray<float> AVoxel::CF_GeneratedMap(int32 X, int32 Y, int32 Z)
{
	
	TArray<float> Map;
	Map.AddZeroed(X * Y * Z);
	
	noise.SetNoiseType(CF_GetNoise());
	noise.SetSeed(245385);
	noise.SetFrequency(NoiseFrequency);
	for (int32 i = 0; i < X ; i++)
	{
		for (int32 j = 0; j < Y; ++j)
		{
			for (int32 k = 0; k < Z; ++k)
			{
				if (bTestSphere)
				{
					FVector center = FVector(X,Y,Z) * 0.5;
					Map[ i * Y * Z +  j * Z + k] = (FVector::Distance(center,FVector(i,j,k)) - TestRadius) / TestRadius;
				}
				else
				{
					Map[ i * Y * Z +  j * Z + k] = noise.GetNoise((float)i ,(float)j,(float)k);
				}
				
			}
		}
	}
	
	return Map;
}

TArray<float> AVoxel::CF_GeneratedMap2D(int32 X, int32 Y)
{
	TArray<float> Map;
	Map.AddZeroed(X * Y );
	
	noise.SetNoiseType(CF_GetNoise());
	noise.SetSeed(245385);
	noise.SetFrequency(NoiseFrequency);
	for (int32 i = 0; i < X ; i++)
	{
		for (int32 j = 0; j < Y; ++j)
		{
			Map[ i * Y + j] = noise.GetNoise((float)i ,(float)j);
		}
	}
	
	return Map;
}

FastNoiseLite::NoiseType AVoxel::CF_GetNoise()
{
	switch (NoiseType)
	{
	case ENoiseType::NoiseType_OpenSimplex2:
		return FastNoiseLite::NoiseType_OpenSimplex2;
	case ENoiseType::NoiseType_OpenSimplex2S:
		return FastNoiseLite::NoiseType_OpenSimplex2S;
	case ENoiseType::NoiseType_Cellular:
		return FastNoiseLite::NoiseType_Cellular;
	case ENoiseType::NoiseType_Perlin:
		return FastNoiseLite::NoiseType_Perlin;
	case ENoiseType::NoiseType_ValueCubic:
		return FastNoiseLite::NoiseType_ValueCubic;
	case ENoiseType::NoiseType_Value:
		return FastNoiseLite::NoiseType_Value;
	default:
		return FastNoiseLite::NoiseType_Perlin;
	}
}

#if WITH_EDITOR
void AVoxel::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	BP_GeneratedWorld();
}
#endif
