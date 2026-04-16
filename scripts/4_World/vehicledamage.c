modded class CarScript
{
    float m_MaintenanceTimer;
    float m_NoDamageRefreshTimer;
    bool m_ContactRefreshPending;
    int m_LastRepairDebugLogMs;
    static const float WLM_CONTACT_MIN_IMPULSE = 25.0;

    void WLM_LogCollision(string msg)
    {
        if (WastelandSettings.Get().DebugCollisionLogs)
        {
            Print("[WastelandMod][CollisionDebug][Car:" + GetType() + "] " + msg);
        }
    }

    bool WLM_IsSignificantContact(Contact data)
    {
        float impulseLen = Math.AbsFloat(data.Impulse);
        return (impulseLen >= WLM_CONTACT_MIN_IMPULSE);
    }

    override void OnContact( string zoneName, vector localPos, IEntity other, Contact data )
    {
        if (!GetGame().IsServer())
            return;

        WastelandSettings settings = WastelandSettings.Get();
        if (!settings || !settings.EnableNoVehicleDamage)
        {
            super.OnContact(zoneName, localPos, other, data);
            return;
        }

        // 1.29 hardening: ignore tiny contact jitter to reduce callback spam side-effects.
        if (!WLM_IsSignificantContact(data))
        {
            super.OnContact(zoneName, localPos, other, data);
            return;
        }

        WLM_LogCollision("OnContact intercepted zone=" + zoneName + " impulse=" + data.Impulse.ToString());
        SetAllowDamage(false);
        if (GetHealthLevel() != GameConstants.STATE_PRISTINE)
            SetHealth(GetMaxHealth());

        // Defer heavy inventory/attachment pass to OnUpdate to avoid repeated work in burst contacts.
        m_ContactRefreshPending = true;
    }

    override void OnUpdate( float dt )
    {
		if ( GetGame().IsServer() )
		{
            bool noVehicleDamage = WastelandSettings.Get().EnableNoVehicleDamage;
            if (noVehicleDamage)
            {
                // Keep continuously enforced.
                SetAllowDamage(false);

                m_NoDamageRefreshTimer += dt;
                if (m_ContactRefreshPending || m_NoDamageRefreshTimer >= 0.50)
                {
                    if (GetHealthLevel() != GameConstants.STATE_PRISTINE)
                        SetHealth(GetMaxHealth());

                    Maintenance_RepairAttachments();
                    Maintenance_RepairTires();
                    m_NoDamageRefreshTimer = 0;
                    m_ContactRefreshPending = false;
                }
            }
            else
            {
                SetAllowDamage(true);
                m_NoDamageRefreshTimer = 0;
                m_ContactRefreshPending = false;
            }

            // Lower-frequency general maintenance.
            m_MaintenanceTimer += dt;
            if (m_MaintenanceTimer >= 1.0)
            {
                if (WastelandSettings.Get().EnableInfiniteBattery)
                    Maintenance_Battery();
                
                if (WastelandSettings.Get().EnableInfiniteFuel)
                    Maintenance_RefillFluids();
                
                if (WastelandSettings.Get().EnableIndestructibleTires)
                    Maintenance_RepairTires();
                
                m_MaintenanceTimer = 0;
            }

			// --- Drowning Logic ---
			if ( GetGame().GetWaterDepth( GetEnginePosWS() ) > 0 )
			{
                // Only accumulate drown time if protection is DISABLED
                if ( !WastelandSettings.Get().EnableVehicleWaterDamageProtection )
                {
				    m_DrownTime += dt;
                    if ( m_DrownTime > DROWN_ENGINE_THRESHOLD )
                    {
                        // *dt to get damage per second
                        AddHealth( "Engine", "Health", -DROWN_ENGINE_DAMAGE * dt);
                        SetEngineZoneReceivedHit(true);
                    }
                }
                else
                {
                    m_DrownTime = 0;
                }
			}
			else
			{
				m_DrownTime = 0;
			}
		}

		// For visualisation of brake lights for all players
		float brake_coef = GetBrake();
		if ( brake_coef > 0 )
		{
			if ( !m_BrakesArePressed )
			{
				m_BrakesArePressed = true;
				SetSynchDirty();
				OnBrakesPressed();
			}
		}
		else
		{
			if ( m_BrakesArePressed )
			{
				m_BrakesArePressed = false;
				SetSynchDirty();
				OnBrakesReleased();
			}
		}
		
		if ( (!GetGame().IsDedicatedServer()) && m_ForceUpdateLights )
		{
			UpdateLights();
			m_ForceUpdateLights = false;
		}
    }

    void Maintenance_Battery()
    {
        ItemBase battery = GetBattery();
        if (!battery)
            return;

        ComponentEnergyManager compEM = battery.GetCompEM();
        if (!compEM)
            return;

        float maxEnergy = compEM.GetEnergyMax();
        compEM.SetEnergy(maxEnergy);
        m_BatteryTimer = 0;
    }

    void Maintenance_RefillFluids()
    {
        if ( GetFluidFraction( CarFluid.FUEL ) < 0.98 ) Fill( CarFluid.FUEL, GetFluidCapacity( CarFluid.FUEL ) );
        if ( GetFluidFraction( CarFluid.COOLANT ) < 0.98 ) Fill( CarFluid.COOLANT, GetFluidCapacity( CarFluid.COOLANT ) );
        if ( GetFluidFraction( CarFluid.OIL ) < 0.98 ) Fill( CarFluid.OIL, GetFluidCapacity( CarFluid.OIL ) );
        if ( GetFluidFraction( CarFluid.BRAKE ) < 0.98 ) Fill( CarFluid.BRAKE, GetFluidCapacity( CarFluid.BRAKE ) );
    }

    void Maintenance_RepairTires()
    {
        for (int i = 0; i < WheelCount(); i++)
        {
            CarWheel wheel = CarWheel.Cast(WheelGetEntity(i));
            if (wheel)
            {
                // Force repair visual state (inflates tire)
                wheel.SetHealth(wheel.GetMaxHealth());
                // Prevent damage
                if (WastelandSettings.Get().EnableNoVehicleDamage)
                    wheel.SetAllowDamage(false);
                else
                    wheel.SetAllowDamage(true);
            }
        }
    }

    void Maintenance_RepairAttachments()
    {
        array<EntityAI> attachments = new array<EntityAI>;
        GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, attachments);
        EntityAI vehicleBattery = GetBattery();
        bool preserveBatteryCharging = (vehicleBattery != null) && !WastelandSettings.Get().EnableInfiniteBattery;

        int skippedCargoCount = 0;
        int skippedBatteryCount = 0;
        int maintainedCount = 0;

        foreach (EntityAI att : attachments)
        {
            if (!att || att == this)
                continue;

            if (preserveBatteryCharging && att == vehicleBattery)
            {
                // Let vanilla alternator logic manage charge when the infinite battery feature is off.
                if (WastelandSettings.Get().EnableNoVehicleDamage)
                    att.SetAllowDamage(false);
                else
                    att.SetAllowDamage(true);

                skippedBatteryCount++;
                continue;
            }

            // Skip cargo content (e.g. trunk items) so only true attachments are maintained.
            InventoryLocation invLoc = new InventoryLocation;
            if (att.GetInventory() && att.GetInventory().GetCurrentInventoryLocation(invLoc))
            {
                if (invLoc.GetType() == InventoryLocationType.CARGO)
                {
                    skippedCargoCount++;
                    continue;
                }
            }

            // Force repair visual state (fixes textures/models)
            att.SetHealth(att.GetMaxHealth());

            // Prevent damage for maintained attachment entities only.
            if (WastelandSettings.Get().EnableNoVehicleDamage)
                att.SetAllowDamage(false);
            else
                att.SetAllowDamage(true);

            maintainedCount++;
        }

        if (WastelandSettings.Get().DebugRepairLogs)
        {
            int nowMs = GetGame().GetTime();
            if (nowMs - m_LastRepairDebugLogMs >= 2000)
            {
                m_LastRepairDebugLogMs = nowMs;
                Print("[WastelandMod][RepairDebug] Vehicle=" + GetType() + " maintained=" + maintainedCount.ToString() + " skippedCargo=" + skippedCargoCount.ToString() + " skippedBattery=" + skippedBatteryCount.ToString());
            }
        }
    }

    override void CheckContactCache()
    {
        if (WastelandSettings.Get().EnableNoVehicleDamage)
        {
            int cached = m_ContactCache.Count();
            if (cached > 0)
            {
                WLM_LogCollision("CheckContactCache blocked cachedContacts=" + cached.ToString());
            }
            m_ContactCache.Clear();
            return;
        }

        super.CheckContactCache();
    }

    override void DamageCrew(float dmg)
    {
        if (WastelandSettings.Get().EnablePlayerCollisionProtection)
        {
            WLM_LogCollision("DamageCrew blocked dmg=" + dmg.ToString());
            return;
        }

        super.DamageCrew(dmg);
    }

	// Prevents damage from vehicle-impact style events while preserving other vanilla damage flow.
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		if (!GetGame().IsServer() || !WastelandSettings.Get().EnableNoVehicleDamage)
			return;

        bool isVehicleRelated = false;
        if (!source)
        {
            // Contact events can report null source depending on physics/event ordering.
            isVehicleRelated = true;
        }
        else
        {
            if (source.IsInherited(Transport) || source.IsInherited(CarScript) || source == this)
                isVehicleRelated = true;
        }

        if (!isVehicleRelated)
            return;

        string srcType = "null";
        if (source)
            srcType = source.GetType();
        WLM_LogCollision("EEHitBy intercepted type=" + damageType.ToString() + " zone=" + dmgZone + " source=" + srcType);

        // Force reset global health if hit
        SetHealth(GetMaxHealth());

        // Explicitly fix common zones that might not reset with global health (like windows)
        SetHealth("WindowFront", "Health", GetMaxHealth("WindowFront", "Health"));
        SetHealth("WindowBack", "Health", GetMaxHealth("WindowBack", "Health"));
        SetHealth("WindowLeft", "Health", GetMaxHealth("WindowLeft", "Health"));
        SetHealth("WindowRight", "Health", GetMaxHealth("WindowRight", "Health"));
        SetHealth("Window", "Health", GetMaxHealth("Window", "Health"));

        // Re-apply attachment and tire protection immediately on hit.
        Maintenance_RepairTires();
        Maintenance_RepairAttachments();
	}
    
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionFlipVehicle);
	}

    override void EEKilled(Object killer)
	{
	}
	///Credits to Inkihh for the smoke effect
    override void EOnPostSimulate(IEntity other, float timeSlice)
    {
        m_Time += timeSlice;
        
        if (!GetGame().IsDedicatedServer())
        {
            float carSpeed = GetVelocity(this).Length();
            for (int i = 0; i < WheelCount(); i++)
            {
                EffWheelSmoke eff = m_WheelSmokeFx.Get(i);
                int ptrEff = m_WheelSmokePtcFx.Get(i);
                bool haveParticle = false;

                CarWheel wheel = CarWheel.Cast(WheelGetEntity(i));
                if (wheel && WheelHasContact(i))
                {
                    float wheelSpeed = WheelGetAngularVelocity(i) * wheel.GetRadius();

                    vector wheelPos = WheelGetContactPosition(i);
                    vector wheelVel = dBodyGetVelocityAt(this, wheelPos);

                    vector transform[3];
                    transform[2] = WheelGetDirection(i);
                    transform[1] = vector.Up;
                    transform[0] = transform[2] * transform[1];

                    wheelVel = wheelVel.InvMultiply3(transform);

                    float bodySpeed = wheelVel[2];

                    bool applyEffect = false;
                    if ((wheelSpeed > 0 && bodySpeed > 0) || (wheelSpeed < 0 && bodySpeed < 0))
                    {
                        applyEffect = Math.AbsFloat(wheelSpeed) > Math.AbsFloat(bodySpeed) + EffWheelSmoke.WHEEL_SMOKE_THRESHOLD;
                    }
                    else
                    {
                        applyEffect = Math.AbsFloat(wheelSpeed) > EffWheelSmoke.WHEEL_SMOKE_THRESHOLD;
                    }

                    if (applyEffect)
                    {
                        haveParticle = true;

                        string surface;
                        GetGame().SurfaceGetType(wheelPos[0], wheelPos[2], surface);
                        wheelPos = WorldToModel(wheelPos);

                        if (!SEffectManager.IsEffectExist(ptrEff))
                        {
                            eff = new EffWheelSmoke();
                            eff.SetSurface(surface);
                            ptrEff = SEffectManager.PlayOnObject(eff, this, wheelPos, "0 1 -1");
                            eff.SetCurrentLocalPosition(wheelPos);
                            m_WheelSmokeFx.Set(i, eff);
                            m_WheelSmokePtcFx.Set(i, ptrEff);
                        }
                        else
                        {
                            if (!eff.IsPlaying() && Surface.GetWheelParticleID(surface) != 0)
                                eff.Start();
                            eff.SetSurface(surface);
                            eff.SetCurrentLocalPosition(wheelPos);
                        }
                    }
                }

                if (!haveParticle)
                {
                    if (eff && eff.IsPlaying())
                        eff.Stop();
                }
            }
        }
    }
}
