modded class DayZPlayerImplement
{
    override protected void EOnContact(IEntity other, Contact extra)
    {
        if (GetGame().IsServer() && !WastelandSettings.Get().EnablePlayerCollisionProtection)
            super.EOnContact(other, extra);
    }

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if ( source && source.IsInherited(Transport) && WastelandSettings.Get().EnablePlayerCollisionProtection )
			return;

		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}
}