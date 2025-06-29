
#include "VoxelGeneratorWrapper.h"
#include "FastNoise/VoxelFastNoise.inl"
#include "VoxelMaterialBuilder.h"
#include "VoxelWorld.h"
#include "Spawner/Area/PreloadedVoxelCenterAreaChunkSpawner.h"

TVoxelSharedRef<FVoxelGeneratorInstance> UVoxelGeneratorWrapper::GetInstance()
{
	return MakeVoxelShared<FVoxelGeneratorExampleInstance>(*this);
}

///////////////////////////////////////////////////////////////////////////////

FVoxelGeneratorExampleInstance::FVoxelGeneratorExampleInstance(UVoxelGeneratorWrapper& MyGenerator)
	: Super(&MyGenerator)
{
}

void FVoxelGeneratorExampleInstance::Init(const FVoxelGeneratorInit& InitStruct)
{
	if (ChunkSpawnerPtr == nullptr){
		TArray<AActor*> Children;
		InitStruct.World->GetAttachedActors(Children);
		for (const auto Child : Children)
		{
			auto CastResult = Cast<AChunkSpawnerBase>(Child);

			if (CastResult != nullptr)
			{
				ChunkSpawnerPtr = CastResult;
			}
		}
	}
	
	if (ChunkSpawnerPtr != nullptr && ChunkSpawnerPtr->IsInitialized())
	{
		ChunkSpawnerPtr->SpawnChunksAsync().Wait();
	}
}

v_flt FVoxelGeneratorExampleInstance::GetValueImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	v_flt Value = 1;
 
	if (ChunkSpawnerPtr != nullptr && ChunkSpawnerPtr->IsInitialized())
	{
		// Positive value -> empty voxel
		// Negative value -> full voxel
		// Value positive when Z > Height, and negative Z < Height
		
		// The voxel value is clamped between -1 and 1. That can result in a bad gradient/normal. To solve that we divide it
 		
		const auto VoxelName = ChunkSpawnerPtr->GetVoxelNameAtHit(FVector(X, Y, Z), FVector(0,0,0));
		if (!VoxelName.IsNone())
		{
			Value = -1;
		}
	}
 	
	return Value; 
}

FVoxelMaterial FVoxelGeneratorExampleInstance::GetMaterialImpl(v_flt X, v_flt Y, v_flt Z, int32 LOD, const FVoxelItemStack& Items) const
{
	FVoxelMaterialBuilder Builder;

	// RGB
	Builder.SetMaterialConfig(EVoxelMaterialConfig::RGB);
	Builder.SetColor(FColor::Red);

	// Single index
	//Builder.SetMaterialConfig(EVoxelMaterialConfig::SingleIndex);
	//Builder.SetSingleIndex(0); 

	// Multi index
	//Builder.SetMaterialConfig(EVoxelMaterialConfig::MultiIndex);
	//Builder.AddMultiIndex(0, 0.5f);
	//Builder.AddMultiIndex(1, 0.5f);
	
	return Builder.Build();
}

TVoxelRange<v_flt> FVoxelGeneratorExampleInstance::GetValueRangeImpl(const FVoxelIntBox& Bounds, int32 LOD, const FVoxelItemStack& Items) const
{
	// Return the values that GetValueImpl can return in Bounds
	// Used to skip chunks where the value does not change
	// Be careful, if wrong your world will have holes!
	// By default return infinite range to be safe
	return TVoxelRange<v_flt>::Infinite();
}

FVector FVoxelGeneratorExampleInstance::GetUpVector(v_flt X, v_flt Y, v_flt Z) const
{
	// Used by spawners
	return FVector::UpVector;
}