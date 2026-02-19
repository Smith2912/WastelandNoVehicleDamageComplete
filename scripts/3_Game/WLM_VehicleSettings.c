class WastelandSettings
{
    static ref WastelandSettings m_Instance;

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
            m_Instance.Load();
        }
        return m_Instance;
    }

    void Load()
    {
        if (GetGame().IsServer())
        {
            if (FileExist("$profile:WLM_NoVehicleDamageComplete/settings.json"))
            {
                JsonFileLoader<WastelandSettings>.JsonLoadFile("$profile:WLM_NoVehicleDamageComplete/settings.json", this);
            }
            else
            {
                Save();
            }
        }
    }

    void Save()
    {
        if (GetGame().IsServer())
        {
            if (!FileExist("$profile:WLM_NoVehicleDamageComplete"))
            {
                MakeDirectory("$profile:WLM_NoVehicleDamageComplete");
            }
            JsonFileLoader<WastelandSettings>.JsonSaveFile("$profile:WLM_NoVehicleDamageComplete/settings.json", this);
        }
    }
}
