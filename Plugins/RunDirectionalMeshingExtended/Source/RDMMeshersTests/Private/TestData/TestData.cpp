#pragma once
#include "TestData.h"

#include "Voxel/RLEVoxel.h"

TArray<FRLEVoxel> TestData::NoiseRLEVoxelModel = {
		{254, FVoxel{1}}, {2, FVoxel{0}}, {27, FVoxel{1}}, {5, FVoxel{0}}, {25, FVoxel{1}}, {7, FVoxel{0}},
		{24, FVoxel{1}}, {8, FVoxel{0}}, {22, FVoxel{1}}, {10, FVoxel{0}}, {21, FVoxel{1}}, {11, FVoxel{0}},
		{19, FVoxel{1}}, {13, FVoxel{0}}, {18, FVoxel{1}}, {14, FVoxel{0}}, {17, FVoxel{1}}, {15, FVoxel{0}},
		{15, FVoxel{1}}, {17, FVoxel{0}}, {14, FVoxel{1}}, {18, FVoxel{0}}, {12, FVoxel{1}}, {20, FVoxel{0}},
		{11, FVoxel{1}}, {21, FVoxel{0}}, {8, FVoxel{1}}, {24, FVoxel{0}}, {3, FVoxel{1}}, {349, FVoxel{0}},
		{254, FVoxel{1}}, {2, FVoxel{0}}, {27, FVoxel{1}}, {5, FVoxel{0}}, {25, FVoxel{1}}, {7, FVoxel{0}},
		{24, FVoxel{1}}, {8, FVoxel{0}}, {22, FVoxel{1}}, {10, FVoxel{0}}, {21, FVoxel{1}}, {11, FVoxel{0}},
		{19, FVoxel{1}}, {13, FVoxel{0}}, {18, FVoxel{1}}, {14, FVoxel{0}}, {17, FVoxel{1}}, {15, FVoxel{0}},
		{15, FVoxel{1}}, {17, FVoxel{0}}, {14, FVoxel{1}}, {18, FVoxel{0}}, {12, FVoxel{1}}, {20, FVoxel{0}},
		{11, FVoxel{1}}, {21, FVoxel{0}}, {8, FVoxel{1}}, {24, FVoxel{0}}, {3, FVoxel{1}}, {349, FVoxel{0}}, {
			254, FVoxel
			{1}
		},
		{2, FVoxel{0}}, {27, FVoxel{1}}, {5, FVoxel{0}}, {25, FVoxel{1}}, {7, FVoxel{0}}, {24, FVoxel{1}},
		{8, FVoxel{0}}, {22, FVoxel{1}}, {10, FVoxel{0}}, {21, FVoxel{1}}, {11, FVoxel{0}}, {19, FVoxel{1}},
		{13, FVoxel{0}}, {18, FVoxel{1}}, {14, FVoxel{0}}, {17, FVoxel{1}}, {15, FVoxel{0}}, {15, FVoxel{1}},
		{17, FVoxel{0}}, {14, FVoxel{1}}, {18, FVoxel{0}}, {12, FVoxel{1}}, {20, FVoxel{0}}, {11, FVoxel{1}},
		{21, FVoxel{0}}, {8, FVoxel{1}}, {24, FVoxel{0}}, {3, FVoxel{1}}, {349, FVoxel{0}}, {254, FVoxel{1}},
		{2, FVoxel{0}}, {27, FVoxel{1}}, {5, FVoxel{0}}, {25, FVoxel{1}}, {7, FVoxel{0}}, {24, FVoxel{1}},
		{8, FVoxel{0}}, {22, FVoxel{1}}, {10, FVoxel{0}}, {21, FVoxel{1}}, {11, FVoxel{0}}, {19, FVoxel{1}},
		{13, FVoxel{0}}, {18, FVoxel{1}}, {14, FVoxel{0}}, {17, FVoxel{1}}, {15, FVoxel{0}}, {15, FVoxel{1}},
		{17, FVoxel{0}}, {14, FVoxel{1}}, {18, FVoxel{0}}, {12, FVoxel{1}}, {20, FVoxel{0}}, {11, FVoxel{1}},
		{21, FVoxel{0}}, {8, FVoxel{1}}, {24, FVoxel{0}}, {3, FVoxel{1}}, {349, FVoxel{0}}, {255, FVoxel{1}}, {
			1, FVoxel{0}
		},
		{28, FVoxel{1}}, {4, FVoxel{0}}, {26, FVoxel{1}}, {6, FVoxel{0}}, {24, FVoxel{1}}, {8, FVoxel{0}},
		{22, FVoxel{1}}, {10, FVoxel{0}}, {21, FVoxel{1}}, {11, FVoxel{0}}, {20, FVoxel{1}}, {12, FVoxel{0}},
		{18, FVoxel{1}}, {14, FVoxel{0}}, {17, FVoxel{1}}, {15, FVoxel{0}}, {16, FVoxel{1}}, {16, FVoxel{0}},
		{14, FVoxel{1}}, {18, FVoxel{0}}, {13, FVoxel{1}}, {19, FVoxel{0}}, {11, FVoxel{1}}, {21, FVoxel{0}},
		{8, FVoxel{1}}, {24, FVoxel{0}}, {3, FVoxel{1}}, {349, FVoxel{0}}, {284, FVoxel{1}}, {4, FVoxel{0}},
		{26, FVoxel{1}}, {6, FVoxel{0}}, {24, FVoxel{1}}, {8, FVoxel{0}}, {23, FVoxel{1}}, {9, FVoxel{0}},
		{21, FVoxel{1}}, {11, FVoxel{0}}, {20, FVoxel{1}}, {12, FVoxel{0}}, {18, FVoxel{1}}, {14, FVoxel{0}},
		{17, FVoxel{1}}, {15, FVoxel{0}}, {16, FVoxel{1}}, {16, FVoxel{0}}, {14, FVoxel{1}}, {18, FVoxel{0}},
		{13, FVoxel{1}}, {19, FVoxel{0}}, {11, FVoxel{1}}, {21, FVoxel{0}}, {8, FVoxel{1}}, {24, FVoxel{0}},
		{3, FVoxel{1}}, {349, FVoxel{0}}, {286, FVoxel{1}}, {2, FVoxel{0}}, {27, FVoxel{1}}, {5, FVoxel{0}},
		{25, FVoxel{1}}, {7, FVoxel{0}}, {23, FVoxel{1}}, {9, FVoxel{0}}, {22, FVoxel{1}}, {10, FVoxel{0}},
		{20, FVoxel{1}}, {12, FVoxel{0}}, {19, FVoxel{1}}, {13, FVoxel{0}}, {17, FVoxel{1}}, {15, FVoxel{0}},
		{16, FVoxel{1}}, {16, FVoxel{0}}, {14, FVoxel{1}}, {18, FVoxel{0}}, {13, FVoxel{1}}, {19, FVoxel{0}},
		{11, FVoxel{1}}, {21, FVoxel{0}}, {8, FVoxel{1}}, {24, FVoxel{0}}, {2, FVoxel{1}}, {350, FVoxel{0}},
		{316, FVoxel{1}}, {4, FVoxel{0}}, {26, FVoxel{1}}, {6, FVoxel{0}}, {24, FVoxel{1}}, {8, FVoxel{0}},
		{22, FVoxel{1}}, {10, FVoxel{0}}, {20, FVoxel{1}}, {12, FVoxel{0}}, {19, FVoxel{1}}, {13, FVoxel{0}},
		{18, FVoxel{1}}, {14, FVoxel{0}}, {16, FVoxel{1}}, {16, FVoxel{0}}, {15, FVoxel{1}}, {17, FVoxel{0}},
		{13, FVoxel{1}}, {19, FVoxel{0}}, {11, FVoxel{1}}, {21, FVoxel{0}}, {8, FVoxel{1}}, {376, FVoxel{0}},
		{318, FVoxel{1}}, {2, FVoxel{0}}, {27, FVoxel{1}}, {5, FVoxel{0}}, {25, FVoxel{1}}, {7, FVoxel{0}},
		{23, FVoxel{1}}, {9, FVoxel{0}}, {21, FVoxel{1}}, {11, FVoxel{0}}, {19, FVoxel{1}}, {13, FVoxel{0}},
		{18, FVoxel{1}}, {14, FVoxel{0}}, {16, FVoxel{1}}, {16, FVoxel{0}}, {15, FVoxel{1}}, {17, FVoxel{0}},
		{13, FVoxel{1}}, {19, FVoxel{0}}, {11, FVoxel{1}}, {21, FVoxel{0}}, {8, FVoxel{1}}, {376, FVoxel{0}},
		{349, FVoxel{1}}, {3, FVoxel{0}}, {26, FVoxel{1}}, {6, FVoxel{0}}, {24, FVoxel{1}}, {8, FVoxel{0}},
		{22, FVoxel{1}}, {10, FVoxel{0}}, {20, FVoxel{1}}, {12, FVoxel{0}}, {18, FVoxel{1}}, {14, FVoxel{0}},
		{17, FVoxel{1}}, {15, FVoxel{0}}, {15, FVoxel{1}}, {17, FVoxel{0}}, {13, FVoxel{1}}, {19, FVoxel{0}},
		{11, FVoxel{1}}, {21, FVoxel{0}}, {9, FVoxel{1}}, {375, FVoxel{0}}, {380, FVoxel{1}}, {4, FVoxel{0}},
		{25, FVoxel{1}}, {7, FVoxel{0}}, {23, FVoxel{1}}, {9, FVoxel{0}}, {21, FVoxel{1}}, {11, FVoxel{0}},
		{19, FVoxel{1}}, {13, FVoxel{0}}, {17, FVoxel{1}}, {15, FVoxel{0}}, {16, FVoxel{1}}, {16, FVoxel{0}},
		{14, FVoxel{1}}, {18, FVoxel{0}}, {12, FVoxel{1}}, {20, FVoxel{0}}, {9, FVoxel{1}}, {375, FVoxel{0}},
		{411, FVoxel{1}}, {5, FVoxel{0}}, {25, FVoxel{1}}, {7, FVoxel{0}}, {22, FVoxel{1}}, {10, FVoxel{0}},
		{20, FVoxel{1}}, {
			12, FVoxel{0}
		},
		{18, FVoxel{1}}, {14, FVoxel{0}}, {16, FVoxel{1}}, {16, FVoxel{0}}, {14, FVoxel{1}}, {18, FVoxel{0}},
		{12, FVoxel{1}}, {20, FVoxel{0}}, {9, FVoxel{1}}, {375, FVoxel{0}}, {443, FVoxel{1}}, {5, FVoxel{0}},
		{24, FVoxel{1}}, {8, FVoxel{0}}, {22, FVoxel{1}}, {10, FVoxel{0}}, {19, FVoxel{1}}, {13, FVoxel{0}},
		{17, FVoxel{1}}, {15, FVoxel{0}}, {15, FVoxel{1}}, {17, FVoxel{0}}, {12, FVoxel{1}}, {20, FVoxel{0}},
		{9, FVoxel{1}}, {375, FVoxel{0}}, {475, FVoxel{1}}, {5, FVoxel{0}}, {24, FVoxel{1}}, {8, FVoxel{0}},
		{21, FVoxel{1}}, {11, FVoxel{0}}, {18, FVoxel{1}}, {14, FVoxel{0}}, {16, FVoxel{1}}, {16, FVoxel{0}},
		{13, FVoxel{1}}, {19, FVoxel{0}}, {9, FVoxel{1}}, {375, FVoxel{0}}, {508, FVoxel{1}}, {4, FVoxel{0}},
		{24, FVoxel{1}}, {8, FVoxel{0}}, {20, FVoxel{1}}, {12, FVoxel{0}}, {17, FVoxel{1}}, {15, FVoxel{0}},
		{14, FVoxel{1}}, {18, FVoxel{0}}, {10, FVoxel{1}}, {374, FVoxel{0}}, {542, FVoxel{1}}, {2, FVoxel{0}},
		{24, FVoxel{1}}, {8, FVoxel{0}}, {20, FVoxel{1}}, {12, FVoxel{0}}, {16, FVoxel{1}}, {
			16, FVoxel{0}
		},
		{11, FVoxel{1}}, {373, FVoxel{0}}, {600, FVoxel{1}}, {8, FVoxel{0}}, {18, FVoxel{1}}, {14, FVoxel{0}},
		{13, FVoxel{1}}, {371, FVoxel{0}}, {634, FVoxel{1}}, {6, FVoxel{0}}, {16, FVoxel{1}}, {368, FVoxel{0}},
		{672, FVoxel{1}}, {352, FVoxel{0}}, {672, FVoxel{1}}, {12, FVoxel{0}}, {20, FVoxel{1}}, {320, FVoxel{0}},
		{672, FVoxel{1}}, {7, FVoxel{0}}, {25, FVoxel{1}}, {18, FVoxel{0}}, {14, FVoxel{1}}, {28, FVoxel{0}},
		{4, FVoxel{1}}, {256, FVoxel{0}}, {672, FVoxel{1}}, {3, FVoxel{0}}, {29, FVoxel{1}}, {14, FVoxel{0}},
		{18, FVoxel{1}}, {19, FVoxel{0}}, {13, FVoxel{1}}, {25, FVoxel{0}}, {7, FVoxel{1}}, {224, FVoxel{0}},
		{704, FVoxel{1}}, {12, FVoxel{0}}, {20, FVoxel{1}}, {16, FVoxel{0}}, {16, FVoxel{1}}, {20, FVoxel{0}},
		{12, FVoxel{1}}, {24, FVoxel{0}}, {8, FVoxel{1}}, {30, FVoxel{0}}, {2, FVoxel{1}}, {160, FVoxel{0}},
		{704, FVoxel{1}}, {10, FVoxel{0}}, {22, FVoxel{1}}, {14, FVoxel{0}}, {18, FVoxel{1}}, {17, FVoxel{0}},
		{15, FVoxel{1}}, {20, FVoxel{0}}, {12, FVoxel{1}}, {24, FVoxel{0}}, {8, FVoxel{1}}, {
			28, FVoxel
			{0}
		},
		{4, FVoxel{1}}, {128, FVoxel{0}}, {704, FVoxel{1}}, {9, FVoxel{0}}, {23, FVoxel{1}}, {13, FVoxel{0}},
		{19, FVoxel{1}}, {15, FVoxel{0}}, {17, FVoxel{1}}, {18, FVoxel{0}}, {14, FVoxel{1}}, {21, FVoxel{0}},
		{11, FVoxel{1}}, {23, FVoxel{0}}, {9, FVoxel{1}}, {27, FVoxel{0}}, {5, FVoxel{1}}, {96, FVoxel{0}},
		{704, FVoxel{1}}, {7, FVoxel{0}}, {25, FVoxel{1}}, {11, FVoxel{0}}, {21, FVoxel{1}}, {14, FVoxel{0}},
		{18, FVoxel{1}}, {16, FVoxel{0}}, {16, FVoxel{1}}, {19, FVoxel{0}}, {13, FVoxel{1}}, {21, FVoxel{0}},
		{11, FVoxel{1}}, {23, FVoxel{0}}, {9, FVoxel{1}}, {26, FVoxel{0}}, {6, FVoxel{1}}, {30, FVoxel{0}},
		{2, FVoxel{1}}, {32, FVoxel{0}}, {704, FVoxel{1}}, {5, FVoxel{0}}, {27, FVoxel{1}}, {10, FVoxel{0}},
		{22, FVoxel{1}}, {13, FVoxel{0}}, {19, FVoxel{1}}, {15, FVoxel{0}}, {17, FVoxel{1}}, {17, FVoxel{0}},
		{15, FVoxel{1}}, {19, FVoxel{0}}, {13, FVoxel{1}}, {21, FVoxel{0}}, {11, FVoxel{1}}, {23, FVoxel{0}},
		{9, FVoxel{1}}, {26, FVoxel{0}}, {6, FVoxel{1}}, {29, FVoxel{0}}, {739, FVoxel{1}}, {9, FVoxel{0}}, {
			23,
			FVoxel{1}
		},
		{12, FVoxel{0}}, {20, FVoxel{1}}, {14, FVoxel{0}}, {18, FVoxel{1}}, {16, FVoxel{0}}, {16, FVoxel{1}},
		{18, FVoxel{0}}, {14, FVoxel{1}}, {20, FVoxel{0}}, {12, FVoxel{1}}, {22, FVoxel{0}}, {10, FVoxel{1}},
		{24, FVoxel{0}}, {8, FVoxel{1}}, {26, FVoxel{0}}, {742, FVoxel{1}}, {8, FVoxel{0}}, {24, FVoxel{1}},
		{11, FVoxel{0}}, {21, FVoxel{1}}, {13, FVoxel{0}}, {19, FVoxel{1}}, {15, FVoxel{0}}, {17, FVoxel{1}},
		{17, FVoxel{0}}, {15, FVoxel{1}}, {19, FVoxel{0}}, {13, FVoxel{1}}, {20, FVoxel{0}}, {12, FVoxel{1}},
		{22, FVoxel{0}}, {10, FVoxel{1}}, {24, FVoxel{0}}, {744, FVoxel{1}}, {6, FVoxel{0}}, {26, FVoxel{1}},
		{10, FVoxel{0}}, {22, FVoxel{1}}, {12, FVoxel{0}}, {20, FVoxel{1}}, {14, FVoxel{0}}, {18, FVoxel{1}},
		{16, FVoxel{0}}, {16, FVoxel{1}}, {18, FVoxel{0}}, {14, FVoxel{1}}, {19, FVoxel{0}}, {13, FVoxel{1}},
		{21, FVoxel{0}}, {11, FVoxel{1}}, {22, FVoxel{0}}, {746, FVoxel{1}}, {4, FVoxel{0}}, {28, FVoxel{1}},
		{9, FVoxel{0}}, {23, FVoxel{1}}, {11, FVoxel{0}}, {21, FVoxel{1}}, {13, FVoxel{0}}, {
			19, FVoxel
			{1}
		},
		{15, FVoxel{0}}, {17, FVoxel{1}}, {17, FVoxel{0}}, {15, FVoxel{1}}, {18, FVoxel{0}}, {14, FVoxel{1}},
		{20, FVoxel{0}}, {12, FVoxel{1}}, {21, FVoxel{0}}, {779, FVoxel{1}}, {8, FVoxel{0}}, {24, FVoxel{1}},
		{11, FVoxel{0}}, {21, FVoxel{1}}, {13, FVoxel{0}}, {19, FVoxel{1}}, {14, FVoxel{0}}, {18, FVoxel{1}},
		{16, FVoxel{0}}, {16, FVoxel{1}}, {17, FVoxel{0}}, {15, FVoxel{1}}, {19, FVoxel{0}}, {13, FVoxel{1}},
		{20, FVoxel{0}}, {12, FVoxel{1}}
	};