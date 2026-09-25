modded class DrowningMdfr
{
	override bool ActivateCondition(PlayerBase player)
	{
        // Preserve normal swimming damage. Only occupants of a vehicle are
        // protected from the drowning modifier.
        if (WastelandSettings.Get().EnablePlayerDrowningProtection && player && player.GetCommand_Vehicle())
            return false;

		return super.ActivateCondition(player);
	}
    
    // Safety check just in case it was already active
	override bool DeactivateCondition(PlayerBase player)
	{
        if (WastelandSettings.Get().EnablePlayerDrowningProtection && player && player.GetCommand_Vehicle())
            return true;
            
		return super.DeactivateCondition(player);
	}
};
