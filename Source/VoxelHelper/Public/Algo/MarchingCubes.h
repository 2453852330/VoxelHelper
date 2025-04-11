// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MarchingCubeTables.h"
#include "VoxelTypes.h"

/**
	* 1. Marching Cubes（行进立方体）
	📌 用途：从体素密度场中提取等值面
	✅ 优点：三角网格拓扑好、速度快、实现容易
	⚠️ 缺点：边界锯齿感明显、有歧义问题（需要 Dual 版本改进）

	原理: 按体素遍历8个顶点,检测每个顶点的值是否等于等值面;根据这8个结果,构建一个8位的索引 00001111,去查找和等值面相交的 Edge;
	有了 Edge 之后,就可以获取到构建三角面的坐标和索引,再进行建模即可;
 */
class VOXELHELPER_API MarchingCubes
{
public:
	static FDrawElement GetDrawElement(const TArray<float>& Map,int32 X,int32 Y,int32 Z,float IsoValue,bool bSmooth = false);
	static FVector FillVertexAndIndex(const CUBE_TABLE_3D::FEdgeTable & EdgeTable,int32 edgeIndex,const FVector & Origin, OUT FDrawElement & DrawElement);
	static FVector FillVertexAndIndexSmooth(const CUBE_TABLE_3D::FEdgeTable & EdgeTable,int32 edgeIndex,const FVector & Origin,const FVector& MapSize,const TArray<float>& Map,float IsoValue , OUT FDrawElement & DrawElement);
};

