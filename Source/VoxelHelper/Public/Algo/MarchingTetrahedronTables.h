#pragma once


// -------------------- MarchingTetrahedrons ---------------------------


//   4------5
//  /|     /|
// 7------6 |
// | |    | |
// | 0----|-1
// |/     |/
// 3------2

namespace Tetrahedrons
{
	// 所有四面体都使用点 0 和 6 作为连接桥，避免拓扑不一致。
	constexpr static int tetrahedrons[6][4] = {
		{0, 5, 1, 6},
		{0, 1, 2, 6},
		{0, 2, 3, 6},
		{0, 3, 7, 6},
		{0, 7, 4, 6},
		{0, 4, 5, 6}
	};
	// 13 57
	// 尝试其他四面体顺序
	constexpr static int tetrahedrons_new[6][4] = {
		{0,5,1,6},
		{0,5,6,4},
		{0,2,6,1},
		{0,6,2,3},
		{0,7,6,4},
		{0,3,6,7}
	};

	constexpr static int tetrahedrons_new1[6][4] = {
		{0, 7, 3, 2}, // Tetrahedron 0
		{0, 7, 2, 6}, // Tetrahedron 1
		{0, 4, 6, 7}, // Tetrahedron 2
		{0, 6, 1, 2}, // Tetrahedron 3
		{0, 6, 1, 4}, // Tetrahedron 4
		{5, 6, 1, 4}  // Tetrahedron 5
	};
	constexpr static int tetrahedrons_new2[6][4] = {
		{0, 5, 1, 6},
		{0, 1, 2, 6},
		{0, 2, 3, 6},
		{0, 3, 7, 6},
		{0, 7, 4, 6},
		{0, 4, 5, 6}
	};
	
	// 每个四面体有 4 个顶点，组合出 2⁴ = 16 种可能的“内外”配置。
	// 每种构型可能有 0~2 个三角形，每个三角形由 3 条边组成。
	struct FTetraTriangle
	{
		int32 Edge[3];

		FTetraTriangle(int32 E0, int32 E1, int32 E2)
		{
			Edge[0] = E0;
			Edge[1] = E1;
			Edge[2] = E2;
		}
	};

	/**
	 * 四面体边索引，共 6 条边，
	 * 边的顺序不要紧,但是必须和下面的索引规则匹配
	 */
	static constexpr int32 TetrahedronEdges[6][2] = {
		{0, 1}, // Edge 0	
		{0, 2}, // Edge 1
		{0, 3}, // Edge 2	
		{1, 2}, // Edge 3	
		{1, 3}, // Edge 4	
		{2, 3}  // Edge 5	
	};

	/**
	 * 16种构型下的三角形生成规则（最多2个三角形）
	 * mask = (v0_in << 0) | (v1_in << 1) | (v2_in << 2) | (v3_in << 3)
	 */

	/*

	 Tetrahedron layout:

		   0
		   *
		  /|\
		 / | \
	  3 *-----* 1
		 \ | /
		  \|/
		   *
		   2
	*/
	
	static const TArray<FTetraTriangle> TetrahedronLookupTable[16] = {
		{}, // 0000

	{ FTetraTriangle(0, 2, 1) },                   // 0001

	{ FTetraTriangle(0, 3, 4) },                   // 0010

	{ FTetraTriangle(2, 1, 4), FTetraTriangle(1, 3, 4) },  // 0011

	{ FTetraTriangle(1, 5, 3) },                   // 0100

	{ FTetraTriangle(2, 3, 0), FTetraTriangle(3, 2, 5) },  // 0101

	{ FTetraTriangle(0, 1, 4), FTetraTriangle(4, 1, 5) },  // 0110

	{ FTetraTriangle(2,5,4) },                   // 0111

	{ FTetraTriangle(4, 5, 2) },                   // 1000

	{ FTetraTriangle(0, 4, 1), FTetraTriangle(4, 5, 1) },  // 1001

	{ FTetraTriangle(2,0,3), FTetraTriangle(3,5,2) },  // 1010

	{ FTetraTriangle(3,5,1) },                   // 1011

	{ FTetraTriangle(4, 1, 2), FTetraTriangle(1, 4, 3) },  // 1100

	{ FTetraTriangle(0,4,3) },                   // 1101

	{ FTetraTriangle(0,1,2) },                   // 1110

	{} // 1111
	};


	static constexpr int32 CacheTetrahedronEdges[28][2] = {
		{0, 1},  // 边 0: 0 - 1
		{0, 2},  // 边 1: 0 - 2
		{0, 3},  // 边 2: 0 - 3
		{1, 2},  // 边 3: 1 - 2
		{1, 3},  // 边 4: 1 - 3
		{2, 3},  // 边 5: 2 - 3
		{4, 5},  // 边 6: 4 - 5
		{4, 6},  // 边 7: 4 - 6
		{4, 7},  // 边 8: 4 - 7
		{5, 6},  // 边 9: 5 - 6
		{5, 7},  // 边 10: 5 - 7
		{6, 7},  // 边 11: 6 - 7
		{0, 4},  // 边 12: 0 - 4
		{1, 5},  // 边 13: 1 - 5
		{2, 6},  // 边 14: 2 - 6
		{3, 7},  // 边 15: 3 - 7
		{0, 5},  // 边 16: 0 - 5
		{1, 6},  // 边 17: 1 - 6
		{2, 7},  // 边 18: 2 - 7
		{0, 6},  // 边 19: 0 - 6
		{1, 7},  // 边 20: 1 - 7
		{0, 7},  // 边 21: 0 - 7
		{3, 4},  // 边 22: 3 - 4
		{3, 5},  // 边 23: 3 - 5
		{3, 6},  // 边 24: 3 - 6
		{3, 7},  // 边 25: 3 - 7
		{4, 7},  // 边 26: 4 - 7
		{5, 6}   // 边 27: 5 - 6
	};
}

