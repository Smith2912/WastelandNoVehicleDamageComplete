modded class CarScript
{
    override void OnContact( string zoneName, vector localPos, IEntity other, Contact data )
    {
        if (GetGame().IsServer() && !WastelandSettings.Get().EnableNoVehicleDamage)
            super.OnContact(zoneName, localPos, other, data);
    }
    float m_MaintenanceTimer;

    override void OnUpdate( float dt )
    {
		if ( GetGame().IsServer() )
		{
            m_MaintenanceTimer += dt;
            if (m_MaintenanceTimer >= 1.0)
            {
                if (WastelandSettings.Get().EnableNoVehicleDamage)
                {
                    SetAllowDamage(false);
                    
                    // Heal Global
                    if (GetHealthLevel() != GameConstants.STATE_PRISTINE)
                        SetHealth(GetMaxHealth());
                    
                    Maintenance_RepairAttachments();
                }
                else
                {
                     SetAllowDamage(true);
                }

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
        if ( battery )
        {
            float maxEnergy = battery.GetCompEM().GetEnergyMax();
            battery.GetCompEM().SetEnergy(maxEnergy);
            m_BatteryTimer = 0;
        }
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
        
        foreach(EntityAI att : attachments)
        {
            if (att && att != this)
            {
                 // Force repair visual state (fixes textures/models)
                 att.SetHealth(att.GetMaxHealth());
                 
                 // Prevent damage
                 if (WastelandSettings.Get().EnableNoVehicleDamage)
                    att.SetAllowDamage(false);
                 else
                    att.SetAllowDamage(true);
            }
        }
    }

	// Prevents damage from bullets, explosions, melee, etc.
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		if (GetGame().IsServer() && WastelandSettings.Get().EnableNoVehicleDamage)
		{
            // Force reset global health if hit
            SetHealth(GetMaxHealth());
            
            // Explicitly fix common zones that might not reset with global health (like windows)
            SetHealth("WindowFront", "Health", GetMaxHealth("WindowFront", "Health"));
            SetHealth("WindowBack", "Health", GetMaxHealth("WindowBack", "Health"));
            SetHealth("WindowLeft", "Health", GetMaxHealth("WindowLeft", "Health"));
            SetHealth("WindowRight", "Health", GetMaxHealth("WindowRight", "Health"));
            SetHealth("Window", "Health", GetMaxHealth("Window", "Health"));
		}
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