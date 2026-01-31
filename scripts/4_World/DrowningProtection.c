modded class DrowningMdfr
{
	override bool ActivateCondition(PlayerBase player)
	{
        // If Drowning Protection is ENABLED, we return false to prevent the modifier from activating
        if ( WastelandSettings.Get().EnablePlayerDrowningProtection )
            return false;

		return super.ActivateCondition(player);
	}
    
    // Safety check just in case it was already active
	override bool DeactivateCondition(PlayerBase player)
	{
        if ( WastelandSettings.Get().EnablePlayerDrowningProtection )
            return true;
            
		return super.DeactivateCondition(player);
	}
};
