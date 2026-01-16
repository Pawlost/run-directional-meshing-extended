#pragma once
#include "MeshingEvent.h"
#include "VirtualMeshEventPlannerBase.h"

struct FBorderVirtualMeshEventPlanner : FVirtualMeshEventPlannerBase
{
	FMeshingEvent BorderMeshingEvent;
	
	FBorderVirtualMeshEventPlanner() : FVirtualMeshEventPlannerBase(0, 0, 0)
	{
	}
};
