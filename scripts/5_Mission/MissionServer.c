modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        WastelandSettings.Get();
        if (WastelandSettings.WLM_LoadSucceeded)
            Print("[WastelandMod] Config loaded.");
        else
            Print("[WastelandMod] Config NOT loaded; features disabled. Check settings.json.");
    }
}
