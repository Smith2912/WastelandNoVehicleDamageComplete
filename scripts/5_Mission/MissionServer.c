modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        WastelandSettings.Get().Load();
        Print("[WastelandMod] Config loaded.");
    }
}
