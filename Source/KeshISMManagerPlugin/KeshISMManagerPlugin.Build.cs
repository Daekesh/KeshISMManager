using UnrealBuildTool;

public class KeshISMManagerPlugin : ModuleRules
{
    public KeshISMManagerPlugin(TargetInfo Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "RenderCore" });
    }
}
