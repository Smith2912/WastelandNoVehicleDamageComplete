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
    bool EnableAnimalCollisionProtection = true;
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
                // DayZ's JSON loader zeroes missing booleans. Migrate the old
                // file and keep protection enabled for this server session.
                string original;
                if (WLM_ReadText("$profile:WLM_NoVehicleDamageComplete/settings.json", original))
                {
                    if (original.IndexOf("\"EnableAnimalCollisionProtection\"") == -1)
                    {
                        WLM_MigrateAnimalSetting(original);
                        candidate.EnableAnimalCollisionProtection = true;
                    }
                }
                else
                {
                    Print("[WastelandMod] SETTINGS MIGRATION WARNING: could not inspect settings.json after load; animal protection uses the loaded value and the file was not updated.");
                }

                EnableNoVehicleDamage = candidate.EnableNoVehicleDamage;
                EnableInfiniteFuel = candidate.EnableInfiniteFuel;
                EnableInfiniteBattery = candidate.EnableInfiniteBattery;
                EnableIndestructibleTires = candidate.EnableIndestructibleTires;
                EnableFlipVehicle = candidate.EnableFlipVehicle;
                EnablePlayerDrowningProtection = candidate.EnablePlayerDrowningProtection;
                EnablePlayerCollisionProtection = candidate.EnablePlayerCollisionProtection;
                EnableZombieCollisionProtection = candidate.EnableZombieCollisionProtection;
                EnableAnimalCollisionProtection = candidate.EnableAnimalCollisionProtection;
                EnableVehicleWaterDamageProtection = candidate.EnableVehicleWaterDamageProtection;
                DebugRepairLogs = candidate.DebugRepairLogs;
                DebugCollisionLogs = candidate.DebugCollisionLogs;
                return true;
            }
        }
        else if (Save())
        {
            Print("[WastelandMod] SETTINGS CREATED: settings.json includes independent player, infected and animal collision protection.");
            return true;
        }

        // Fail closed without overwriting the invalid administrator file.
        EnableNoVehicleDamage = false;
        EnableInfiniteFuel = false;
        EnableInfiniteBattery = false;
        EnableIndestructibleTires = false;
        EnableFlipVehicle = false;
        EnablePlayerDrowningProtection = false;
        EnablePlayerCollisionProtection = false;
        EnableZombieCollisionProtection = false;
        EnableAnimalCollisionProtection = false;
        EnableVehicleWaterDamageProtection = false;
        Print("[WastelandMod] SETTINGS ERROR; features disabled. " + error);
        return false;
    }

    bool WLM_ReadText(string path, out string contents)
    {
        FileHandle handle = OpenFile(path, FileMode.READ);
        if (handle == 0)
            return false;

        int bytesRead = ReadFile(handle, contents, 100000000);
        CloseFile(handle);
        return bytesRead > 0;
    }

    bool WLM_WriteSettingsText(string contents)
    {
        FileHandle handle = OpenFile("$profile:WLM_NoVehicleDamageComplete/settings.json", FileMode.WRITE);
        if (handle == 0)
            return false;
        FPrint(handle, contents);
        CloseFile(handle);
        return true;
    }

    bool WLM_MigrateAnimalSetting(string original)
    {
        string backupPath = "$profile:WLM_NoVehicleDamageComplete/settings.before-animal-protection.json";
        if (FileExist(backupPath))
        {
            Print("[WastelandMod] SETTINGS MIGRATION WARNING: backup already exists; settings.json was not rewritten. Animal protection is enabled for this session. Add EnableAnimalCollisionProtection manually or review the backup: " + backupPath);
            return false;
        }

        int rootClose = original.LastIndexOf("}");
        int lastValue = rootClose - 1;
        while (lastValue >= 0)
        {
            string character = original.Substring(lastValue, 1);
            if (character != " " && character != "\t" && character != "\r" && character != "\n")
                break;
            lastValue--;
        }
        if (lastValue < 0)
        {
            Print("[WastelandMod] SETTINGS MIGRATION WARNING: could not locate the JSON object end; settings.json was not rewritten.");
            return false;
        }

        string separator = ",";
        if (original.Substring(lastValue, 1) == "{")
            separator = "";
        // Insert only the new key: reserializing would discard unknown owner entries.
        string updated = original.Substring(0, lastValue + 1) + separator + "\n  \"EnableAnimalCollisionProtection\": true" + original.Substring(lastValue + 1, original.Length() - lastValue - 1);

        WastelandSettings validation = new WastelandSettings;
        string error;
        if (!JsonFileLoader<WastelandSettings>.LoadData(updated, validation, error) || !validation.EnableAnimalCollisionProtection)
        {
            Print("[WastelandMod] SETTINGS MIGRATION WARNING: candidate JSON failed validation; settings.json was not rewritten. " + error);
            return false;
        }
        string backupContents;
        if (!CopyFile("$profile:WLM_NoVehicleDamageComplete/settings.json", backupPath) || !WLM_ReadText(backupPath, backupContents) || backupContents != original)
        {
            Print("[WastelandMod] SETTINGS MIGRATION WARNING: could not create and verify backup; settings.json was not rewritten. " + backupPath);
            return false;
        }

        string written;
        if (WLM_WriteSettingsText(updated) && WLM_ReadText("$profile:WLM_NoVehicleDamageComplete/settings.json", written) && written == updated)
        {
            Print("[WastelandMod] SETTINGS UPDATED: added EnableAnimalCollisionProtection=true; existing settings were preserved. Original backup: " + backupPath);
            return true;
        }

        string restored;
        bool restoreVerified = WLM_WriteSettingsText(original) && WLM_ReadText("$profile:WLM_NoVehicleDamageComplete/settings.json", restored) && restored == original;
        string restoreStatus = "failed";
        if (restoreVerified)
            restoreStatus = "verified";
        Print("[WastelandMod] SETTINGS MIGRATION ERROR: update verification failed; original backup: " + backupPath + "; restore=" + restoreStatus + ". Inspect settings.json before the next restart.");
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
