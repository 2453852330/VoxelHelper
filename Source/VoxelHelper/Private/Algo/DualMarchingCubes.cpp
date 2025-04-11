#include "Algo/DualMarchingCubes.h"

#include "FastNoiseLite.h"

#include "Algo/MarchingCubeTables.h"

#include "Eigen/Dense"


FDrawElement DualMarchingCubes::GetDrawElement(UWorld* World, const TArray<float>& Map, int32 X, int32 Y, int32 Z, float IsoValue, const FastNoiseLite& NoiseLite, bool bSmooth)
{
	FDrawElement DrawElement;
	// 1. 构建标量场,也就是这里的Map

	// 构建一个 dual 体素点数组
	TArray<FDualPoint> dualPoints;
	int32 dx = X - 1;
	int32 dy = Y - 1;
	int32 dz = Z - 1;
	dualPoints.AddDefaulted(dx * dy * dz);

	// 偏移:防止体素端点正好位于等值面的情况
	// const float epsilon = 1e-6f;
	
	// 遍历体素
	for (int32 i = 0; i < X - 1; ++i)
	{
		for (int32 j = 0; j < Y - 1; ++j)
		{
			for (int32 k = 0; k < Z - 1; ++k)
			{
				// 2. 检测等值面穿越(判断voxel是否活跃)
				// 遍历整个voxel网格,对每个voxel,检查它的8个顶点是否跨越等值面
				bool bHasGreaterThan = false;	// 是否存在大于等值面的点
				bool bHasLessThan = false;		// 是否存在小于等值面的点
				int32 VertexSignal[8] = {0};	// 存储每个顶点是否大于等值面的信号
				for (int32 m = 0; m < 8; ++m)
				{
					FVector offset = CUBE_TABLE_3D::Vertex[m];
					float V = Map[VoxelHelper::GetIndex(i+offset.X, j+offset.Y ,k+offset.Z ,X,Y,Z)];
					
					if (  V > IsoValue)
					{
						VertexSignal[m] = 1;
						bHasGreaterThan = true;
					}
					else
					{
						VertexSignal[m] = -1;
						bHasLessThan = true;
					}
				}
				// 检测是否存在大于等值面的点和小于等值面的点(判断这个体素是否被等值面切割)
				bool bIsValidVoxel = bHasGreaterThan && bHasLessThan;
				if (!bIsValidVoxel)
				{
					dualPoints[VoxelHelper::GetIndex(i,j,k,dx,dy,dz)].IsValid = false;
					continue;
				}
				
				// 3. 在有效体素内,计算交点和法线(Hermite)数据
				// 对于每条穿越等值面的边,插值求出交点位置,并估算法线
				FVector currentVoxelPos = FVector(i,j,k);
				TArray<FHermite> HermiteList;
				// 计算朝向用于三角面索引判断
				FVector Gradient = FVector::ZeroVector;

				for (int32 e = 0; e < 12; ++e)
				{
					FIntVector2 edge = CUBE_TABLE_3D::Edge[e];
					// 两个顶点的值不相等,说明为穿越等值面的边
					if (VertexSignal[edge.X] != VertexSignal[edge.Y])
					{
						FVector p1 = CUBE_TABLE_3D::Vertex[edge.X] + currentVoxelPos;
						FVector p2 = CUBE_TABLE_3D::Vertex[edge.Y] + currentVoxelPos;
						// 获取Map值
						float v1 = Map[VoxelHelper::GetIndex(p1,X,Y,Z)];
						float v2 = Map[VoxelHelper::GetIndex(p2,X,Y,Z)];
						// 插值计算交点位置
						float q = (IsoValue - v1); // 如果值特别小,直接用p1点代替
						FHermite Hermite;
						if (q < 1e-6)
						{
							Hermite.Position = p1;
						}
						else
						{
							float t =  q / (v2 - v1);
							Hermite.Position = p1 + t * (p2 - p1);
						}
						
						// 计算法线 -> 采用中央差分法
						float nx = NoiseLite.GetNoise(Hermite.Position.X + 1,Hermite.Position.Y,Hermite.Position.Z) - NoiseLite.GetNoise(Hermite.Position.X - 1,Hermite.Position.Y,Hermite.Position.Z);
						float ny = NoiseLite.GetNoise(Hermite.Position.X,Hermite.Position.Y + 1,Hermite.Position.Z) - NoiseLite.GetNoise(Hermite.Position.X,Hermite.Position.Y - 1,Hermite.Position.Z);
						float nz = NoiseLite.GetNoise(Hermite.Position.X,Hermite.Position.Y,Hermite.Position.Z + 1) - NoiseLite.GetNoise(Hermite.Position.X,Hermite.Position.Y,Hermite.Position.Z - 1);
						Hermite.Normal = FVector(nx * 0.5,ny * 0.5,nz * 0.5).GetSafeNormal();
						// 保存
						HermiteList.Add(Hermite);
					}


					// 通过12条边来计算平均梯度值
					FVector p1 = CUBE_TABLE_3D::Vertex[edge.X] + currentVoxelPos;
					FVector p2 = CUBE_TABLE_3D::Vertex[edge.Y] + currentVoxelPos;
					// 获取Map值
					float v1 = Map[VoxelHelper::GetIndex(p1,X,Y,Z)];
					float v2 = Map[VoxelHelper::GetIndex(p2,X,Y,Z)];
					// 
					float delta = v2 - v1;
					FVector dir = (p2 - p1).GetSafeNormal();
					Gradient += dir * delta;
				}

				// 4. 通过 QEF 计算Voxel 的 Dual Vertex
				// 收集所有的交点信息,进行 QEF 最小化拟合
				// 通过 最小二乘法 解决 Quadratic Error Function (QEF) 问题，以最小化各个交点的误差并计算出一个最佳拟合点（即 dual vertex）

				
				// ---------------------------- Eigen ----------------------------------------------
				/** Eigen 的计算结果也不对
				Eigen::Matrix3f ATA = Eigen::Matrix3f::Zero();
				Eigen::Vector3f ATb = Eigen::Vector3f::Zero();
				for (const FHermite & p : HermiteList)
				{
					Eigen::Vector3f nor(p.Normal.X, p.Normal.Y, - p.Normal.Z);
					Eigen::Vector3f pos(p.Position.X, p.Position.Y, - p.Position.Z);

					nor.normalize();
					
					ATA += nor * nor.transpose();
					ATb += nor * nor.dot(pos);
				}
				Eigen::Vector3f result = ATA.ldlt().solve(ATb);
				int32 currentDualIndex = VoxelHelper::GetIndex(currentVoxelPos,dx,dy,dz);
				dualPoints[currentDualIndex].Position = FVector(result.x(),result.y(),-result.z());
				dualPoints[currentDualIndex].IsValid = 1;
				*/

				// ---------------------------- UNREAL ----------------------------------------------
				/** UE的计算结果不正确
				// FMatrix ATA = FMatrix(
				// 	FPlane(0,0,0,0),
				// 	FPlane(0,0,0,0),
				// 	FPlane(0,0,0,0),
				// 	FPlane(0,0,0,1));
				// FVector ATb = FVector::ZeroVector;
				//
				// for (const FHermite & p : HermiteList)
				// {
				// 	ATA += FMatrix(
				// 	FPlane(p.Normal.X * p.Normal.X,p.Normal.X * p.Normal.Y,p.Normal.X * p.Normal.Z,0),
				// 	FPlane(p.Normal.Y * p.Normal.X,p.Normal.Y * p.Normal.Y,p.Normal.Y * p.Normal.Z,0),
				// 	FPlane(p.Normal.Z * p.Normal.X,p.Normal.Z * p.Normal.Y,p.Normal.Z * p.Normal.Z,0),
				// 	FPlane(0,0,0,1));
				//
				// 	ATb += p.Normal * FVector::DotProduct(p.Normal,p.Position);					
				// }
				//
				//
				//
				// FMatrix ATAInv = ATA.Inverse();
				// int32 currentDualIndex = VoxelHelper::GetIndex(currentVoxelPos,dx,dy,dz);
				// // 这里的QEF采样算法,结果不正确,暂时直接使用中心点
				// dualPoints[currentDualIndex].Position = ATAInv.TransformPosition(ATb);
				// // dualPoints[currentDualIndex].Position = currentVoxelPos + FVector(0.5f);
				// dualPoints[currentDualIndex].IsValid = 1;
				*/

				// -----------------------------------------------------------------------------
				int32 currentDualIndex = VoxelHelper::GetIndex(currentVoxelPos,dx,dy,dz);
				
				if (bSmooth)
				{
					// 没有hermite点,直接使用中心点
					if (HermiteList.Num() == 0)
					{
						dualPoints[currentDualIndex].Position = currentVoxelPos + FVector(0.5f);
					}
					else // 使用 hermite 的平均点 , 简易 QEF ,没有使用normal
					{
						dualPoints[currentDualIndex].Position = GetDualPoint(HermiteList);
					}
				}
				else // 非平滑版本直接使用中心点
				{
					dualPoints[currentDualIndex].Position = currentVoxelPos + FVector(0.5f);
				}
				
				
				dualPoints[currentDualIndex].IsValid = true;
				// 这个法线结果会存在部分面反向
				// dualPoints[currentDualIndex].Gradient = GetGradient(currentVoxelPos,NoiseLite);
				
				// 使用体素的平均法线 - 效果更差
				// dualPoints[currentDualIndex].Gradient = GetCenterNormal(HermiteList).GetSafeNormal();

				// 使用上面保存的外部点-中心点直接计算朝向
				dualPoints[currentDualIndex].Gradient = Gradient.GetSafeNormal();

				// 绘制体素边界+中心点
				// DrawDebugBox(World,dualPoints[currentDualIndex].Position*100.f,FVector(50),FColor::Black,true);
				// DrawDebugPoint(World,dualPoints[currentDualIndex].Position*100.f,8.f,FColor::Black,true);
			}
		}
	}
	
	// 5.生成连接关系(dual网格构建)
	// 每个voxel提供一个顶点,然后和相邻voxel形成一个多边形
	for (int32 i = 0; i < dx - 1; ++i)
	{
		for (int32 j = 0; j < dy - 1; ++j)
		{
			for (int32 k = 0; k < dz - 1; ++k)
			{

				FDualPoint & currentVoxel = dualPoints[VoxelHelper::GetIndex(i,j,k,dx,dy,dz)];
				if (!currentVoxel.IsValid)
				{
					continue;
				}

				// 检测 +x +y +z 三个方向
				TArray<FVector> Neighbors = {
					FVector{1,0,0},
					FVector{0,1,0},
					FVector{0,0,1}};

				// 遍历邻接体素
				for (const FVector & offset : Neighbors)
				{
					// 获取邻接的体素点
					int32 nx = i + offset.X;
					int32 ny = j + offset.Y;
					int32 nz = k + offset.Z;
					// 检测范围有效性
					if (nx >= dx || ny >= dy || nz >= dz )
					{
						continue;
					}
					// 获取这个点
					FDualPoint & neighbor = dualPoints[VoxelHelper::GetIndex(nx,ny,nz,dx,dy,dz)];
					if (!neighbor.IsValid)
					{
						continue;
					}
					// ** 最关键的一部分,这里真的是精华,解决了当前三角面的朝向问题 ->正交偏移
					// 尝试从当前连接方向（offset）推导出一个正交方向的偏移量，用于确定“对角线”方向的体素位置，进而构建一个四边形面片的另外两个点
					// 当前偏移方向	正交方向1	正交方向2
					// +X (1,0,0)	+Y (0,1,0)	+Z (0,0,1)
					// +Y (0,1,0)	+X (1,0,0)	+Z (0,0,1)
					// +Z (0,0,1)	+X (1,0,0)	+Y (0,1,0)
					// -------------------------------------------------------------------
					// 清晰完整版
					// TArray<FIntVector> OrthogonalOffsets;
					// if (offset.dx == 1) // +X 方向
					// {
					// 	OrthogonalOffsets = {
					// 		FIntVector(0, 1, 0), // +Y
					// 		FIntVector(0, 0, 1)  // +Z
					// 	};
					// }
					// else if (offset.dy == 1) // +Y 方向
					// {
					// 	OrthogonalOffsets = {
					// 		FIntVector(1, 0, 0), // +X
					// 		FIntVector(0, 0, 1)  // +Z
					// 	};
					// }
					// else if (offset.dz == 1) // +Z 方向
					// {
					// 	OrthogonalOffsets = {
					// 		FIntVector(1, 0, 0), // +X
					// 		FIntVector(0, 1, 0)  // +Y
					// 	};
					// }
					// -----------------------------------------------------------------------
					int32 dx2 = offset.Y;
					int32 dy2 = offset.Z;
					int32 dz2 = offset.X;

					// 获取周围的两个体素点,用于构建4边面
					int32 cx = i + dx2;
					int32 cy = j + dy2;
					int32 cz = k + dz2;

					int32 ex = nx + dx2;
					int32 ey = ny + dy2;
					int32 ez = nz + dz2;

					FDualPoint & neighbor2 = dualPoints[VoxelHelper::GetIndex(cx,cy,cz,dx,dy,dz)];
					FDualPoint & neighbor3 = dualPoints[VoxelHelper::GetIndex(ex,ey,ez,dx,dy,dz)];
					// 检测两个点有效性
					if (neighbor2.IsValid && neighbor3.IsValid)
					{
						// 计算法线
						FVector normal = FVector::CrossProduct(neighbor3.Position - currentVoxel.Position,neighbor.Position - currentVoxel.Position).GetSafeNormal();

						// 绘制面朝向
						// FVector start = currentVoxel.Position * 100.f;
						// FVector end = start + normal * 100.f;
						// DrawDebugDirectionalArrow(World,start,end,100.f,FColor::Yellow,true);
						
						
						// 三角面索引反向时,需要对法线进行反转
						// 只使用第一个点的梯度值计算,会存在极端情况,这里计算四个点的平均梯度值
						FVector average = currentVoxel.Gradient + neighbor.Gradient + neighbor2.Gradient + neighbor3.Gradient ;
						average = average.GetSafeNormal();
						// 绘制梯度
						// FVector startA = currentVoxel.Position * 100.f;
						// FVector endA = startA + average * 100.f;
						// DrawDebugDirectionalArrow(World,startA,endA,100.f,FColor(offset.X * 255,offset.Y * 255,offset.Z * 255),true);
						
						bool bOK = FVector::DotProduct(normal,average) > 0 ;
						if (!bOK)
						{
							normal *= -1.f;
						}
						int32 a = DrawElement.Vertices.Add(currentVoxel.Position * 100.f);
						DrawElement.Normals.Add(normal);
						int32 b = DrawElement.Vertices.Add(neighbor.Position * 100.f);
						DrawElement.Normals.Add(normal);
						int32 c = DrawElement.Vertices.Add(neighbor2.Position * 100.f);
						DrawElement.Normals.Add(normal);
						int32 d = DrawElement.Vertices.Add(neighbor3.Position * 100.f);
						DrawElement.Normals.Add(normal);
						
						// 使用梯度和当前面法线,检测三角面索引顺序
						if (bOK)
						{
							DrawElement.Indices.Append({a,b,d,a,d,c});
						}
						else
						{
							DrawElement.Indices.Append({a,d,b,a,c,d});
						}
						
					}

				}
			}
		}
	}

	return DrawElement;
}


FVector DualMarchingCubes::GetGradient(FVector Pos, const FastNoiseLite& noise)
{
	float dx = (noise.GetNoise(Pos.X+1, Pos.Y, Pos.Z) - noise.GetNoise(Pos.X-1, Pos.Y, Pos.Z)) * 0.5f;
	float dy = (noise.GetNoise(Pos.X, Pos.Y+1, Pos.Z) - noise.GetNoise(Pos.X, Pos.Y-1, Pos.Z)) * 0.5f;
	float dz = (noise.GetNoise(Pos.X, Pos.Y, Pos.Z+1) - noise.GetNoise(Pos.X, Pos.Y, Pos.Z-1)) * 0.5f;
	return FVector(dx, dy, dz).GetSafeNormal();
}


FVector DualMarchingCubes::GetCenterNormal(const TArray<FHermite>& HermiteList)
{
	int32 Count = HermiteList.Num();
	if (Count == 0)
	{
		return FVector::ZeroVector;
	}
	FVector SumNormal;
	for (int32 i = 0; i < Count; ++i)
	{
		SumNormal += HermiteList[i].Normal;
	}
	return SumNormal / (float)Count;
}
FVector DualMarchingCubes::GetDualPoint(const TArray<FHermite>& HermiteList)
{
	int32 Count = HermiteList.Num();
	FVector Sum = FVector::ZeroVector;
	for (int32 i = 0; i < Count; ++i)
	{
		Sum.X += HermiteList[i].Position.X;
		Sum.Y += HermiteList[i].Position.Y;
		Sum.Z += HermiteList[i].Position.Z;
	}
	return Sum / (float)Count;
}