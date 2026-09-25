// Own only flags changed by this mod; entity pointers are deliberately weak.
class WLM_NVD_DamageGuard
{
    EntityAI Target;
    protected bool m_Changed;
    void WLM_NVD_DamageGuard(EntityAI target) { Target = target; }

    void Protect()
    {
        if (Target && Target.GetAllowDamage())
        {
            m_Changed = true;
            Target.SetAllowDamage(false);
        }
    }

    void Release()
    {
        if (Target && m_Changed)
            Target.SetAllowDamage(true);
        m_Changed = false;
    }

    bool IsOwned() { return m_Changed; }
}

modded class CarScript
{
    protected float m_WLMMaintenanceTimer;
    protected float m_WLMRepairTimer;
    protected bool m_WLMFlipEnabled;
    protected int m_WLMLastRepairLogMs;
    protected int m_WLMLastCrewLogMs;
    protected ref WLM_NVD_DamageGuard m_WLMVehicleGuard;
    protected ref array<ref WLM_NVD_DamageGuard> m_WLMAttachmentGuards;

    void CarScript()
    {
        m_WLMAttachmentGuards = new array<ref WLM_NVD_DamageGuard>;
        // False until server state arrives, including for join-in-progress.
        RegisterNetSyncVariableBool("m_WLMFlipEnabled");
    }

    override void EEInit()
    {
        super.EEInit();
        if (GetGame().IsServer())
        {
            WLM_UpdateProtection();
            WLM_UpdateFlipSetting();
        }
    }

    bool WLM_IsFlipEnabled()
    {
        if (GetGame().IsServer())
            return WastelandSettings.Get().EnableFlipVehicle;
        return m_WLMFlipEnabled;
    }

    protected void WLM_UpdateFlipSetting()
    {
        bool enabled = WastelandSettings.Get().EnableFlipVehicle;
        if (m_WLMFlipEnabled != enabled)
        {
            m_WLMFlipEnabled = enabled;
            SetSynchDirty();
        }
    }

    protected void WLM_UpdateProtection()
    {
        if (!m_WLMVehicleGuard)
            m_WLMVehicleGuard = new WLM_NVD_DamageGuard(this);
        if (WastelandSettings.Get().EnableNoVehicleDamage)
            m_WLMVehicleGuard.Protect();
        else
        {
            m_WLMVehicleGuard.Release();
            WLM_ReleaseAttachments();
        }
    }

    protected void WLM_ProtectAttachment(EntityAI item)
    {
        if (!item || item.GetHierarchyParent() != this)
            return;
        foreach (WLM_NVD_DamageGuard existing : m_WLMAttachmentGuards)
        {
            if (existing.Target == item)
            {
                existing.Protect();
                return;
            }
        }
        WLM_NVD_DamageGuard guard = new WLM_NVD_DamageGuard(item);
        guard.Protect();
        m_WLMAttachmentGuards.Insert(guard);
    }

    protected void WLM_ReleaseAttachments(EntityAI detached = null)
    {
        for (int i = m_WLMAttachmentGuards.Count() - 1; i >= 0; i--)
        {
            WLM_NVD_DamageGuard guard = m_WLMAttachmentGuards[i];
            if (!detached || !guard.Target || guard.Target == detached)
            {
                guard.Release();
                m_WLMAttachmentGuards.Remove(i);
            }
        }
    }

    override void EEItemAttached(EntityAI item, string slot_name)
    {
        super.EEItemAttached(item, slot_name);
        if (GetGame().IsServer() && WastelandSettings.Get().EnableNoVehicleDamage)
            WLM_ProtectAttachment(item);
    }

    override void EEItemDetached(EntityAI item, string slot_name)
    {
        if (GetGame().IsServer())
            WLM_ReleaseAttachments(item);
        super.EEItemDetached(item, slot_name);
    }

    override void EEDelete(EntityAI parent)
    {
        if (GetGame() && GetGame().IsServer())
        {
            WLM_ReleaseAttachments();
            if (m_WLMVehicleGuard)
                m_WLMVehicleGuard.Release();
        }
        super.EEDelete(parent);
    }

    override void OnUpdate(float dt)
    {
        bool resetDrowning = false;
        bool resumeProtection = false;
        if (GetGame().IsServer())
        {
            WLM_UpdateProtection();
            WastelandSettings settings = WastelandSettings.Get();
            if (GetGame().GetWaterDepth(GetEnginePosWS()) > 0)
            {
                resetDrowning = settings.EnableVehicleWaterDamageProtection;
                if (resetDrowning)
                    m_DrownTime = 0;
                else if (settings.EnableNoVehicleDamage && m_WLMVehicleGuard.IsOwned())
                {
                    // Exempt stock drowning from our gate, not third-party protection.
                    m_WLMVehicleGuard.Release();
                    resumeProtection = true;
                }
            }
        }

        // Keep alternator, lights, braking and drowning on their native path.
        super.OnUpdate(dt);
        if (!GetGame().IsServer())
            return;
        if (resumeProtection)
            m_WLMVehicleGuard.Protect();
        if (resetDrowning)
            m_DrownTime = 0;

        m_WLMRepairTimer += dt;
        if (m_WLMRepairTimer >= 0.5)
        {
            m_WLMRepairTimer = 0;
            WLM_UpdateFlipSetting();
            if (WastelandSettings.Get().EnableNoVehicleDamage)
                Maintenance_RepairAttachments();
            if (WastelandSettings.Get().EnableNoVehicleDamage || WastelandSettings.Get().EnableIndestructibleTires)
                Maintenance_RepairTires();
        }

        m_WLMMaintenanceTimer += dt;
        if (m_WLMMaintenanceTimer >= 1.0)
        {
            m_WLMMaintenanceTimer = 0;
            if (WastelandSettings.Get().EnableInfiniteBattery)
                Maintenance_Battery();
            if (WastelandSettings.Get().EnableInfiniteFuel)
                Maintenance_RefillFluids();
        }
    }

    void Maintenance_Battery()
    {
        ItemBase battery = GetBattery();
        if (!battery || !battery.GetCompEM())
            return;
        ComponentEnergyManager energy = battery.GetCompEM();
        float missing = energy.GetEnergyMax() - energy.GetEnergy();
        if (missing > 0)
            energy.AddEnergy(missing);
        // Native charging/drain still owns m_BatteryTimer.
    }

    void Maintenance_RefillFluids()
    {
        if (GetFluidFraction(CarFluid.FUEL) < 0.98) Fill(CarFluid.FUEL, GetFluidCapacity(CarFluid.FUEL));
        if (GetFluidFraction(CarFluid.COOLANT) < 0.98) Fill(CarFluid.COOLANT, GetFluidCapacity(CarFluid.COOLANT));
        if (GetFluidFraction(CarFluid.OIL) < 0.98) Fill(CarFluid.OIL, GetFluidCapacity(CarFluid.OIL));
        if (GetFluidFraction(CarFluid.BRAKE) < 0.98) Fill(CarFluid.BRAKE, GetFluidCapacity(CarFluid.BRAKE));
    }

    void Maintenance_RepairTires()
    {
        for (int i = 0; i < WheelCount(); i++)
        {
            CarWheel wheel = CarWheel.Cast(WheelGetEntity(i));
            if (wheel && wheel.GetHealth() < wheel.GetMaxHealth())
                wheel.SetHealth(wheel.GetMaxHealth());
        }
    }

    void Maintenance_RepairAttachments()
    {
        // Also clean up inventory transitions made by other mods.
        for (int i = m_WLMAttachmentGuards.Count() - 1; i >= 0; i--)
        {
            WLM_NVD_DamageGuard guard = m_WLMAttachmentGuards[i];
            if (!guard.Target || guard.Target.GetHierarchyParent() != this)
            {
                guard.Release();
                m_WLMAttachmentGuards.Remove(i);
            }
        }
        for (int index = 0; index < GetInventory().AttachmentCount(); index++)
        {
            EntityAI item = GetInventory().GetAttachmentFromIndex(index);
            if (!item)
                continue;
            WLM_ProtectAttachment(item);
            if (item == GetBattery() && !WastelandSettings.Get().EnableInfiniteBattery)
                continue;
            if (item.GetHealth() < item.GetMaxHealth())
                item.SetHealth(item.GetMaxHealth());
        }
        int now = GetGame().GetTime();
        if (WastelandSettings.Get().DebugRepairLogs && now - m_WLMLastRepairLogMs >= 2000)
        {
            m_WLMLastRepairLogMs = now;
            Print("[WastelandMod][RepairDebug] " + GetType() + " directAttachments=" + GetInventory().AttachmentCount());
        }
    }

    // Inherit contact caching, post-simulation, hit and killed handling.
    // Stock computes crew damage before vehicle damage is gated.
    override void DamageCrew(float dmg)
    {
        if (!WastelandSettings.Get().EnablePlayerCollisionProtection)
            super.DamageCrew(dmg);
        else if (WastelandSettings.Get().DebugCollisionLogs && GetGame().GetTime() - m_WLMLastCrewLogMs >= 2000)
        {
            m_WLMLastCrewLogMs = GetGame().GetTime();
            Print("[WastelandMod][CollisionDebug] Crew damage blocked: " + dmg);
        }
    }

    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionFlipVehicle);
    }
}
