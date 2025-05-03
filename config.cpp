class CfgPatches
{
	class WastelandNoVehicleDamageComplete
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data","DZ_Scripts"
		};
	};
};
class CfgMods
{
    class WastelandNoVehicleDamageComplete
    {
        type="mod";
        author="The Songsmith";
        dir="WastelandNoVehicleDamageComplete";
        name="Wasteland No Vehicle Damage Complete";
        dependencies[]={"World"};
        class defs
        {
            class worldScriptModule
            {
                files[]={"WastelandNoVehicleDamageComplete/scripts/4_World"};
            };
        };
    };
};