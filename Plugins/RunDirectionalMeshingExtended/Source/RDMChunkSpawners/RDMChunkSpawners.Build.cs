using UnrealBuildTool;

public class RDMChunkSpawners : ModuleRules
{
    public RDMChunkSpawners(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "RDMMeshers", "RDMVoxelGeneration", "ProceduralMeshComponent"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}