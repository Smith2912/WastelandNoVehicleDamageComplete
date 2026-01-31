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
        credits="Inkihh";
        author="The Songsmith";
        dir="WastelandNoVehicleDamageComplete";
        name="Wasteland No Vehicle Damage Complete";
        dependencies[]={"Game", "World", "Mission"};
        class defs
        {
            class gameScriptModule
            {
                value="";
                files[]={"WastelandNoVehicleDamageComplete/scripts/3_Game"};
            };
            class worldScriptModule
            {
                files[]={"WastelandNoVehicleDamageComplete/scripts/4_World"};
            };
            class missionScriptModule
            {
                value="";
                files[]={"WastelandNoVehicleDamageComplete/scripts/5_Mission"};
            };
        };
    };
};