#pragma once

#include "VoxelTypes.generated.h"

struct FDrawElement
{
	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<int32> Indices;
	TArray<FVector2D> UV0;
	TArray<FLinearColor> VertexColors;
	TArray<FVector> Tangents;

};

UENUM(BlueprintType)
enum class ENoiseType : uint8
{
	NoiseType_OpenSimplex2,
	NoiseType_OpenSimplex2S,
	NoiseType_Cellular,
	NoiseType_Perlin,
	NoiseType_ValueCubic,
	NoiseType_Value
};

UENUM(BlueprintType)
enum class ERenderType : uint8
{
	MarchingCubes,
	DualMarchingCubes,
	MarchingTetrahedrons
};


UENUM(BlueprintType)
enum class ERenderType2D : uint8
{
	BOX,
	SmoothBox
};

namespace VoxelHelper
{
	static int32 GetIndex(int32 i,int32 j,int32 k,int32 X,int32 Y,int32 Z);
	static int32 GetIndex(FVector Pos,int32 X,int32 Y,int32 Z);

	inline int32 GetIndex(int32 i, int32 j, int32 k, int32 X, int32 Y, int32 Z)
	{
		return i * Y * Z + j * Z + k;		
	}

	inline int32 GetIndex(FVector Pos, int32 X, int32 Y, int32 Z)
	{
		return Pos.X * Y * Z + Pos.Y * Z + Pos.Z;	
	}
	
	inline int32 GetIndex(FVector2D Pos, int32 X, int32 Y)
	{
		return Pos.X * Y + Pos.Y;
	}
	inline int32 GetIndex(int32 i, int32 j, int32 X, int32 Y)
	{
		return i * Y + j;
	}
}


struct FHermite
{
	FVector Position;
	FVector Normal;
};

struct FDualPoint
{
	FVector Position;
	bool IsValid = false;
	FVector Gradient; // 梯度,表示体素面的朝向,用来判断三角面的索引
	
	// int32 Index = -1 ; // 存储顶点索引,用于连接三角面
};


struct FEdgeKey
{
	FEdgeKey(const FIntVector & Start,const FIntVector &  End):start(Start),end(End){}
	FIntVector start;  // 起始标志
	FIntVector end;	// 起始标志
	FVector position; // 缓存的数据

	bool operator==(const FEdgeKey & other) const;
};

// 检测是否是同一条边
inline bool FEdgeKey::operator==(const FEdgeKey& other) const
{
	return ( start == other.start && end == other.end ) || ( start == other.end && end == other.start );
}
