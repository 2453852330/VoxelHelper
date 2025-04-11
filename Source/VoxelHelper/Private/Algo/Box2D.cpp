#include "Algo/Box2D.h"

#include "Algo/MarchingCubeTables.h"

FDrawElement Box2D::GetDrawElement(const TArray<float>& Map, const FIntVector& MapSize,float IsoValue, bool bSmooth)
{
	FDrawElement DrawElement;
	// get map size
	int32 X = MapSize.X;
	int32 Y = MapSize.Y;
	// 顶点重用
	TMap<FVector,int32> VertexBuffer;
	// 遍历体素
	for (int32 i = 0; i < X - 1; ++i)
	{
		for (int32 j = 0; j < Y - 1; ++j)
		{
			FVector2D currentVoxelPoint = FVector2D(i,j);
			FVector realDrawPoints[4];
			// 遍历顶点
			for (int32 k = 0; k < 4; ++k)
			{
				FVector2D targetPos = currentVoxelPoint + CUBE_TABLE_2D::Vertex[k];
				realDrawPoints[k] = FVector(targetPos.X,targetPos.Y ,Map[VoxelHelper::GetIndex(targetPos,X,Y)] * MapSize.Z);
			}
			// 绘制四边形
			// vertex
			int32 A = InsertPointAndGetIndex(VertexBuffer,DrawElement, realDrawPoints[0]);
			int32 B = InsertPointAndGetIndex(VertexBuffer,DrawElement, realDrawPoints[1]);
			int32 C = InsertPointAndGetIndex(VertexBuffer,DrawElement, realDrawPoints[2]);
			int32 D = InsertPointAndGetIndex(VertexBuffer,DrawElement, realDrawPoints[3]);
			// normal
			FVector n1 = realDrawPoints[2]-realDrawPoints[0];
			FVector n2 = realDrawPoints[1]-realDrawPoints[0];
			FVector normal = FVector::CrossProduct(n2,n1).GetSafeNormal();
			DrawElement.Normals.Append({normal,normal,normal,normal});
			// tri index
			DrawElement.Indices.Append({A,C,B,A,D,C});
		}
	}

	return DrawElement;
}

int32 Box2D::InsertPointAndGetIndex(TMap<FVector, int32>& VertexBuffer, FDrawElement& DrawElement, const FVector& point)
{
	int32* findIndex = VertexBuffer.Find(point);
	if (!findIndex)
	{
		int32 tmpA = DrawElement.Vertices.Add(point * 100);
		VertexBuffer.Emplace(point,tmpA);
		return tmpA;
	}
	return *findIndex;
}
