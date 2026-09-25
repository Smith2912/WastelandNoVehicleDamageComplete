class WastelandSettings
{
    static ref WastelandSettings m_Instance;
    static bool WLM_LoadSucceeded;

    bool EnableNoVehicleDamage = true;
    bool EnableInfiniteFuel = true;
    bool EnableInfiniteBattery = true;
    bool EnableIndestructibleTires = true;
    bool EnableFlipVehicle = true;
    bool EnablePlayerDrowningProtection = true;
    bool EnablePlayerCollisionProtection = true;
    bool EnableZombieCollisionProtection = true;
    bool EnableVehicleWaterDamageProtection = true;
    bool DebugRepairLogs = false;
    bool DebugCollisionLogs = false;

    static WastelandSettings Get()
    {
        if (!m_Instance)
        {
            m_Instance = new WastelandSettings();
            if (GetGame().IsServer())
                WLM_LoadSucceeded = m_Instance.Load();
        }
        return m_Instance;
    }

    bool Load()
    {
        if (!GetGame().IsServer())
            return false;
        string error;
        if (FileExist("$profile:WLM_NoVehicleDamageComplete/settings.json"))
        {
            WastelandSettings candidate = new WastelandSettings;
            if (JsonFileLoader<WastelandSettings>.LoadFile("$profile:WLM_NoVehicleDamageComplete/settings.json", candidate, error))
            {
                EnableNoVehicleDamage = candidate.EnableNoVehicleDamage;
                EnableInfiniteFuel = candidate.EnableInfiniteFuel;
                EnableInfiniteBattery = candidate.EnableInfiniteBattery;
                EnableIndestructibleTires = candidate.EnableIndestructibleTires;
                EnableFlipVehicle = candidate.EnableFlipVehicle;
                EnablePlayerDrowningProtection = candidate.EnablePlayerDrowningProtection;
                EnablePlayerCollisionProtection = candidate.EnablePlayerCollisionProtection;
                EnableZombieCollisionProtection = candidate.EnableZombieCollisionProtection;
                EnableVehicleWaterDamageProtection = candidate.EnableVehicleWaterDamageProtection;
                DebugRepairLogs = candidate.DebugRepairLogs;
                DebugCollisionLogs = candidate.DebugCollisionLogs;
                return true;
            }
        }
        else if (Save())
            return true;

        // Fail closed without overwriting the invalid administrator file.
        EnableNoVehicleDamage = false;
        EnableInfiniteFuel = false;
        EnableInfiniteBattery = false;
        EnableIndestructibleTires = false;
        EnableFlipVehicle = false;
        EnablePlayerDrowningProtection = false;
        EnablePlayerCollisionProtection = false;
        EnableZombieCollisionProtection = false;
        EnableVehicleWaterDamageProtection = false;
        Print("[WastelandMod] SETTINGS ERROR; features disabled. " + error);
        return false;
    }

    bool Save()
    {
        if (GetGame().IsServer())
        {
            if (!FileExist("$profile:WLM_NoVehicleDamageComplete"))
            {
                MakeDirectory("$profile:WLM_NoVehicleDamageComplete");
            }
            string error;
            if (JsonFileLoader<WastelandSettings>.SaveFile("$profile:WLM_NoVehicleDamageComplete/settings.json", this, error))
                return true;
            Print("[WastelandMod] SETTINGS SAVE ERROR: " + error);
        }
        return false;
    }
}
