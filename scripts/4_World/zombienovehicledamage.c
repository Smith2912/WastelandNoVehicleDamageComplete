modded class ZombieBase
{
    override protected void EOnContact(IEntity other, Contact extra)
    {
        if (GetGame().IsServer() && !WastelandSettings.Get().EnableZombieCollisionProtection)
            super.EOnContact(other, extra);
    }

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if ( source && source.IsInherited(Transport) && WastelandSettings.Get().EnableZombieCollisionProtection )
			return;

		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}
}