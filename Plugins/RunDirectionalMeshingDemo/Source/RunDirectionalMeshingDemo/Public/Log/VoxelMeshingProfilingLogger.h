#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVoxelMeshingProfiling, Display, All);

class RUNDIRECTIONALMESHINGDEMO_API FVoxelMeshingProfilingLogger
{
public:
	static void LogAllocatedMemory(const FString& LevelName, SIZE_T AllocatedSize);
	static void LogGeneratedVertices(const FString& LevelName, SIZE_T VerticesCount);
	static void LogVoxelSparsity(const FString& LevelName, uint32 OpaqueVoxelCount, uint32 TransparentVoxelCount);
};
