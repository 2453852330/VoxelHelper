#pragma once
#include "VoxelTypes.h"

class Box2D
{
public:
	static FDrawElement GetDrawElement(const TArray<float> & Map,const FIntVector & MapSize,float IsoValue, bool bSmooth);
	static int32 InsertPointAndGetIndex(TMap<FVector,int32> & VertexBuffer,FDrawElement & DrawElement,const FVector& point);
};
