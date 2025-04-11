// Fill out your copyright notice in the Description page of Project Settings.
#include "Algo/MarchingCubes.h"


FDrawElement MarchingCubes::GetDrawElement(const TArray<float>& Map,int32 X,int32 Y,int32 Z,float IsoValue,bool bSmooth)
{
	FDrawElement DrawElement;
	FVector MapSize = FVector(X,Y,Z);
	
	for (int32 i = 0; i < X - 1; ++i)
	{
		for (int32 j = 0; j < Y - 1; ++j)
		{
			for (int32 k = 0; k < Z - 1; ++k)
			{
				int32 edgeTableIndex = 0;
				// 计算周围8个点的值,获取对应边的索引
				for (int32 m = 0; m < 8; ++m)
				{
					if (Map[VoxelHelper::GetIndex(i + CUBE_TABLE_3D::Vertex[m].X,j + CUBE_TABLE_3D::Vertex[m].Y,k + CUBE_TABLE_3D::Vertex[m].Z,X,Y,Z)] > IsoValue)
					{
						edgeTableIndex += (1 << m );
					}
				}
				
				// 获取边的索引表
				CUBE_TABLE_3D::FEdgeTable EdgeTable = CUBE_TABLE_3D::EdgeTable[edgeTableIndex];
				// 遍历具体的边(一次遍历3个)
				for (int32 edgeIndex = 0 ; edgeIndex < EdgeTable.Edge.Num() ; edgeIndex+=3)
				{
					FVector origin = FVector(i,j,k);
					// 填充顶点的三角索引
					if (bSmooth)
					{
						FVector p1 = FillVertexAndIndexSmooth(EdgeTable,edgeIndex,origin,MapSize,Map,IsoValue,DrawElement);
						FVector p2 = FillVertexAndIndexSmooth(EdgeTable,edgeIndex + 1,origin,MapSize,Map,IsoValue,DrawElement);
						FVector p3 = FillVertexAndIndexSmooth(EdgeTable,edgeIndex + 2,origin,MapSize,Map,IsoValue,DrawElement);
						// 计算法线
						FVector normal = FVector::CrossProduct(FVector(p3-p1),FVector(p2-p1)).GetSafeNormal();
						DrawElement.Normals.Append({normal,normal,normal});
					}
					else
					{
						FVector p1 = FillVertexAndIndex(EdgeTable,edgeIndex,origin,DrawElement);
						FVector p2 = FillVertexAndIndex(EdgeTable,edgeIndex + 1,origin,DrawElement);
						FVector p3 = FillVertexAndIndex(EdgeTable,edgeIndex + 2,origin,DrawElement);
						// 计算法线
						FVector normal = FVector::CrossProduct(FVector(p3-p1),FVector(p2-p1)).GetSafeNormal();
						DrawElement.Normals.Append({normal,normal,normal});
					}
				}
			}
		}
	}

	return DrawElement;
}

FVector MarchingCubes::FillVertexAndIndex(const CUBE_TABLE_3D::FEdgeTable & EdgeTable,int32 edgeIndex,const FVector & Origin, OUT FDrawElement & DrawElement)
{
	// 拿到边的两个端点索引
	int32 currentEdgeIndex = EdgeTable.Edge[edgeIndex];
	FIntVector2 edgePoints =  CUBE_TABLE_3D::Edge[ currentEdgeIndex ];
	// 计算中点
	FVector p1 = CUBE_TABLE_3D::Vertex[edgePoints.X];
	FVector p2 = CUBE_TABLE_3D::Vertex[edgePoints.Y];
	FVector center = (p1 + p2) * 0.5f;
	FVector targetPos = center + Origin;
	// 填充顶点和索引
	int32 retIndex = DrawElement.Vertices.Add(targetPos * 100.f);
	DrawElement.Indices.Add(retIndex);

	return targetPos;
}

FVector MarchingCubes::FillVertexAndIndexSmooth(const CUBE_TABLE_3D::FEdgeTable& EdgeTable, int32 edgeIndex, const FVector& Origin,const FVector& MapSize ,const TArray<float>& Map, float IsoValue,
	FDrawElement& DrawElement)
{
	// 拿到边的两个端点索引
	int32 currentEdgeIndex = EdgeTable.Edge[edgeIndex];
	FIntVector2 edgePoints =  CUBE_TABLE_3D::Edge[ currentEdgeIndex ];
	// 计算插值点
	// 1.获取两个顶点坐标
	FVector p1 = CUBE_TABLE_3D::Vertex[edgePoints.X];
	FVector p2 = CUBE_TABLE_3D::Vertex[edgePoints.Y];
	// 2.获取两个顶点坐标的Map值
	FVector currentPos = p1 + Origin;
	float v1 = Map[currentPos.X * MapSize.Y * MapSize.Z + currentPos.Y * MapSize.Z + currentPos.Z];
	currentPos = p2 + Origin;
	float v2 = Map[currentPos.X * MapSize.Y * MapSize.Z + currentPos.Y * MapSize.Z + currentPos.Z];
	// 3.计算插值点
	float delta = IsoValue - v1;  // IsoValue 是中点,也就是等值面;这里计算的是 p1点到等值点的 值
	float weight = delta / (v2 - v1); // 这里计算 p1点到等值点的权重
	FVector lerpPos = p1 + (p2 - p1 ) * weight;
	// 
	FVector targetPos = lerpPos + Origin;
	// 填充顶点和索引
	int32 retIndex = DrawElement.Vertices.Add(targetPos * 100.f);
	DrawElement.Indices.Add(retIndex);

	return targetPos;
}

