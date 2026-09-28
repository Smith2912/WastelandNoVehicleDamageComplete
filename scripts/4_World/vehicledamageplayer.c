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

	// Stock EOnContact reaches this method to apply TransportHit damage. Guard the
	// damage entry point too, including calls made by other vehicle scripts.
	override void RegisterTransportHit(Transport transport)
	{
		if (GetGame().IsServer() && transport && WastelandSettings.Get().EnablePlayerCollisionProtection)
		{
			WLM_LogPlayerCollision("RegisterTransportHit blocked transport=" + transport.GetType());
			return;
		}

		super.RegisterTransportHit(transport);
	}

    // Keep EEHitBy native: it is an after-damage notification, not a veto.
}
