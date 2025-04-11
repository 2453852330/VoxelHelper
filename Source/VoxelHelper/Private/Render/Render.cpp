// Fill out your copyright notice in the Description page of Project Settings.


#include "Render/Render.h"
#include "ProceduralMeshComponent.h"


// Sets default values
ARender::ARender()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RenderComponent"));
	RootComponent = ProceduralMeshComponent;
}

void ARender::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARender::CF_Render(const FDrawElement& DrawElement)
{
	ProceduralMeshComponent->CreateMeshSection_LinearColor(0,DrawElement.Vertices,DrawElement.Indices,DrawElement.Normals,DrawElement.UV0,DrawElement.VertexColors,{},false);
}

void ARender::CF_Clear()
{
	ProceduralMeshComponent->ClearMeshSection(0);
}

void ARender::CF_FlushMaterial()
{
	if (ShowMaterial)
	{
		ProceduralMeshComponent->SetMaterial(0,ShowMaterial);
	}
}

