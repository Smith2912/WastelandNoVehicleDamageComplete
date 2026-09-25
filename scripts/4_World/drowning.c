modded class DrowningMdfr
{
    override void OnTick(PlayerBase player, float deltaT)
    {    
        // This is a defensive guard for a modifier that was active when the
        // player entered a vehicle. Activation/deactivation use the same
        // predicate, so on-foot swimming always remains vanilla.
        if (WastelandSettings.Get().EnablePlayerDrowningProtection && player && player.GetCommand_Vehicle())
            return;

        super.OnTick(player, deltaT);
    }
};
