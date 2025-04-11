#pragma once
#include "VoxelTypes.h"

class FastNoiseLite;

/**
*	3. Marching Tetrahedrons（行进四面体）
	📌 将每个立方体划分为 5~6 个四面体处理
	✅ 优点：解决 Marching Cubes 的歧义问题
	⚠️ 缺点：顶点多，面数多，效率低于 MC
 */
class MarchingTetrahedrons
{
public:
	static FDrawElement GetDrawElement(UWorld* World, const TArray<float>& Map,int32 X,int32 Y,int32 Z,float IsoValue,const FastNoiseLite & NoiseLite,bool bSmooth);

};
