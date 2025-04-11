#pragma once
#include "VoxelTypes.h"

/*
* 2. Dual Marching Cubes（双重行进立方体）
	📌 改进版：顶点生成在多面体中心而不是边上
	✅ 优点：生成的网格更平滑、更简洁（少面）
	⚠️ 缺点：实现复杂，需要构建八叉树结构
	🔗 用于：OpenVDB、Sparse Voxel 场景、流体重建
	与传统 MC 不同，它强调 面向多边形质量（尤其是拓扑正确性与顶点位置准确性） 的优化，特别适合于高质量网格生成，如医学图像建模、3D 打印预处理等。
 */
class UWorld;

class FastNoiseLite;

class DualMarchingCubes
{
public:
	static FDrawElement GetDrawElement(UWorld* World, const TArray<float>& Map,int32 X,int32 Y,int32 Z,float IsoValue,const FastNoiseLite & NoiseLite,bool bSmooth);

	static FVector GetGradient(FVector Pos,const FastNoiseLite & noise);
	static FVector GetCenterNormal(const TArray<FHermite> & HermiteList);
	static FVector GetDualPoint(const TArray<FHermite> & HermiteList);
};
