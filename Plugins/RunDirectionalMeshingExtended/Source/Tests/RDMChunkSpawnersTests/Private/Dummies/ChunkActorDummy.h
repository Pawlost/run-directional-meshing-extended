#pragma once
#include "Chunk/ChunkActor.h"

class AChunkActorDummy : public AChunkActor
{
public:
	using AChunkActor::AddMeshToActor; // workaround to access protected method as public
	using AChunkActor::EndPlay; // workaround to access protected method as public
};
