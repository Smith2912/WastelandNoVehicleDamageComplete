modded class DayZPlayerImplement
{
	void WLM_LogPlayerCollision(string msg)
	{
		if (WastelandSettings.Get().DebugCollisionLogs)
		{
			string playerName = "unknown";
			PlayerIdentity id = GetIdentity();
			if (id)
				playerName = id.GetName();
			Print("[WastelandMod][CollisionDebug][Player:" + playerName + "] " + msg);
		}
	}

	override protected void EOnContact(IEntity other, Contact extra)
	{
		if (GetGame().IsServer() && WastelandSettings.Get().EnablePlayerCollisionProtection)
		{
			Transport transportOther = Transport.Cast(other);
			if (transportOther)
			{
				WLM_LogPlayerCollision("EOnContact blocked transport other=" + transportOther.GetType());
				return;
			}
		}

		super.EOnContact(other, extra);
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		if (WastelandSettings.Get().EnablePlayerCollisionProtection)
		{
			if (source && source.IsInherited(Transport))
			{
				WLM_LogPlayerCollision("EEHitBy blocked transport source=" + source.GetType() + " zone=" + dmgZone);
				return;
			}
		}

		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}
}
