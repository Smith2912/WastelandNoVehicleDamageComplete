modded class DrowningMdfr
{
    // Override the OnTick method to prevent health/shock depletion
    // Override the OnTick method to prevent health/shock depletion
    override void OnTick(PlayerBase player, float deltaT)
    {    
        if (!WastelandSettings.Get().EnablePlayerDrowningProtection)
        {
            super.OnTick(player, deltaT);
            return;
        }

        // Still keep track of stamina and sound effects
        if (player.GetStaminaHandler().GetStamina() <= 0)
        {
            // Remove these lines that cause damage:
            // player.AddHealth("","",deltaT * -CfgGameplayHandler.GetHealthDepletionSpeed());
            // player.AddHealth("","Shock",deltaT * -CfgGameplayHandler.GetShockDepletionSpeed());
            
            // Still play the drowning sound for immersion
           // player.RequestSoundEventEx(EPlayerSoundEventID.DROWNING_PAIN, false, EPlayerSoundEventParam.HIGHEST_PRIORITY);
        }
    }
    
    // Keep all other vanilla functionality unchanged
};