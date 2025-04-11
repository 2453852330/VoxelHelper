#include "Algo/MarchingTetrahedrons.h"

#include "LogHelper.h"

#include "Algo/MarchingCubeTables.h"
#include "Algo/MarchingTetrahedronTables.h"

FDrawElement MarchingTetrahedrons::GetDrawElement(UWorld* World, const TArray<float>& Map, int32 X, int32 Y, int32 Z, float IsoValue, const FastNoiseLite& NoiseLite, bool bSmooth)
{
	FDrawElement DrawElement;
	// 缓存所有共享边,解决共享边插值计算交点时,由于计算误差导致的破面(插值模式,单纯的取中点没有问题)
	TArray<FEdgeKey> CacheEdgeList;
	CacheEdgeList.Reserve(X*Y*Z*24);
	// 预先保存一个体素的所有边,也就是28条
	// 此方法计算速度超级慢;
#if 0
	for (int32 i = 0; i < X - 1; ++i)
	{
		for (int32 j = 0; j < Y - 1; ++j)
		{
			for (int32 k = 0; k < Z - 1; ++k)
			{
				// 计算所有边,建立所有边的缓存表
				for (int32 m = 0; m < 28; ++m)
				{
					int32 vertexIndex_A = Tetrahedrons::CacheTetrahedronEdges[m][0];
					int32 vertexIndex_B = Tetrahedrons::CacheTetrahedronEdges[m][1];
					// get pos
					FVector pos_A = CUBE_TABLE_3D::Vertex[vertexIndex_A] + FVector(i,j,k);
					FVector pos_B = CUBE_TABLE_3D::Vertex[vertexIndex_B] + FVector(i,j,k);
					//
					FEdgeKey edgeKey = FEdgeKey(FIntVector(pos_A),FIntVector(pos_B));
					// 返回Edge中点
					if (!bSmooth)
					{
						edgeKey.position =  (pos_A + pos_B) * 0.5;
						CacheEdgeList.Add(edgeKey);
						continue;
					}
							
					// 获取map值
					float sv = Map[VoxelHelper::GetIndex(pos_A,X,Y,Z)];
					float ev = Map[VoxelHelper::GetIndex(pos_B,X,Y,Z)];
					
					// 插值
					double t = (IsoValue - sv) / (ev - sv + 1e-6);
					FVector lerpPos = pos_A + (pos_B - pos_A) * t;
					// 保存缓存点
					edgeKey.position = lerpPos;
					CacheEdgeList.Add(edgeKey);
				}
			}
		}
	}
#endif
	
	
	
	// 1. 遍历体素
	for (int32 i = 0; i < X - 1; ++i)
	{
		for (int32 j = 0; j < Y - 1; ++j)
		{
			for (int32 k = 0; k < Z - 1; ++k)
			{
				// 8 个顶点分为 6 个四面体
				FVector currentVoxelPos = FVector(i,j,k);
				// 2. 遍历每个四面体
				for (int32 m = 0; m < 6; ++m)
				{
					// 获取每个四面体的4个顶点索引
					int32 trianglesIndex = 0;
					for (int32 n = 0; n < 4; ++n)
					{
						// 获取每个顶点的索引
						int32 vertexIndex = Tetrahedrons::tetrahedrons_new2[m][n];
						// 获取顶点 offset ,也就是坐标
						FVector vertexOffset = CUBE_TABLE_3D::Vertex[vertexIndex];
						// 获取当前点坐标
						FVector currentPos = currentVoxelPos + vertexOffset;
						// 获取 map 值
						int32 mapIndex = VoxelHelper::GetIndex(currentPos,X,Y,Z);
						float mapValue = Map[mapIndex];
						// 计算匹配的表格索引 0-16 : 0000 四位
						if (mapValue > IsoValue)
						{
							trianglesIndex |= (1 << n);
						}
					}
					// 获取到三角面索引
					TArray<Tetrahedrons::FTetraTriangle> triangles = Tetrahedrons::TetrahedronLookupTable[trianglesIndex];
					// 
					for (const Tetrahedrons::FTetraTriangle & tri : triangles)
					{
						// edge A
						int32 A1 = Tetrahedrons::TetrahedronEdges[ tri.Edge[0] ] [0];
						int32 A2 = Tetrahedrons::TetrahedronEdges[ tri.Edge[0] ] [1];
						// edge B
						int32 B1 = Tetrahedrons::TetrahedronEdges[ tri.Edge[1] ] [0];
						int32 B2 = Tetrahedrons::TetrahedronEdges[ tri.Edge[1] ] [1];
						// edge C
						int32 C1 = Tetrahedrons::TetrahedronEdges[ tri.Edge[2] ] [0];
						int32 C2 = Tetrahedrons::TetrahedronEdges[ tri.Edge[2] ] [1];
						
						// 获取坐标->取出 Map 值 -> 求出等值面交点 -> 连接成三角形
						auto CalcLerpPoint = [&CacheEdgeList,&Map,X,Y,Z,IsoValue,currentVoxelPos,bSmooth,m](int32 Start,int32 End)->FVector
						{
							// 获取坐标
							FVector startPos = CUBE_TABLE_3D::Vertex[ Tetrahedrons::tetrahedrons_new2[m][ Start] ] + currentVoxelPos;
							FVector endPos = CUBE_TABLE_3D::Vertex[ Tetrahedrons::tetrahedrons_new2[m][ End] ] + currentVoxelPos;

							
							// 存在共享边缓存点,直接返回
							FEdgeKey edgeKey = FEdgeKey(FIntVector(startPos),FIntVector(endPos));
							// int32 cacheEdgeIndex = CacheEdgeList.Find(edgeKey);
							// if (cacheEdgeIndex != INDEX_NONE)
							// {
							// 	return CacheEdgeList[cacheEdgeIndex].position;
							// }
							// LogError("cant find cache edge");
							// 返回Edge中点
							if (!bSmooth)
							{
								edgeKey.position =  (startPos + endPos) * 0.5;
								CacheEdgeList.Add(edgeKey);
								return edgeKey.position;
							}
							
							// 获取map值
							float sv = Map[VoxelHelper::GetIndex(startPos,X,Y,Z)];
							float ev = Map[VoxelHelper::GetIndex(endPos,X,Y,Z)];
							
							double delta = IsoValue - sv ;
							FVector result = FVector::ZeroVector;
							
							if (FMath::Abs(delta) < 1e-6)
							{
								result = (startPos + endPos) * 0.5;
							}
							else
							{
								// 插值
								double t = delta / (ev - sv);

								if (t < 1e-6)
								{
									result = startPos;
								}
								else if ( (1 - t) < 1e-6)
								{
									result = endPos;
								}
								else
								{
									result = startPos + (endPos - startPos) * t;
								}
							}
							// 检测是否存在且插值位置不一样
							int32 findIndex = CacheEdgeList.Find(edgeKey);
							if (findIndex == INDEX_NONE)
							{
								// 保存缓存点
								edgeKey.position = result;
								CacheEdgeList.Add(edgeKey);
								// 
							}
							else
							{
								if (CacheEdgeList[findIndex].position != result)
								{
									// LogWarning("find same edge , but has diff lerp position ; edge -> start:%s | end:%s | result:%s",
									// 	*startPos.ToString(),*endPos.ToString(),*result.ToString())
									// 使用缓存表中的数据
									result = CacheEdgeList[findIndex].position;
								}
							}
							
							return result;

						};
						
						
						// 获取三个坐标
						FVector s1 = CalcLerpPoint(A1,A2);
						FVector s2 = CalcLerpPoint(B1,B2);
						FVector s3 = CalcLerpPoint(C1,C2);
						// 绘制
						DrawDebugPoint(World,s1 * 100,5,FColor::Red,true);
						DrawDebugPoint(World,s2 * 100,5,FColor::Green,true);
						DrawDebugPoint(World,s3 * 100,5,FColor::Blue,true);
						// 连接
						int32 i1 = DrawElement.Vertices.Add(s1 * 100.f);
						int32 i2 = DrawElement.Vertices.Add(s2 * 100.f);
						int32 i3 = DrawElement.Vertices.Add(s3 * 100.f);
						DrawElement.Indices.Append({ i1,i3,i2});
						// 计算法线
						FVector normal = FVector::CrossProduct(s2 - s1,s3 - s1).GetSafeNormal();
						DrawElement.Normals.Append({normal,normal,normal});
					}
					
				}
				
			}
		}
	}
	
	return DrawElement;
}
