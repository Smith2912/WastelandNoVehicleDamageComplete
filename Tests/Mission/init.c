// Disposable dedicated-server mission only. Excluded from the runtime PBO.
void main() {}

#ifdef WastelandNoVehicleDamageComplete
class WLM_NVD_ProbeAction : ActionFlipVehicle
{
    CarScript ResolveForTest(ActionTarget target) { return ResolveVehicle(target); }
}

class WLM_NVD_TestMission : MissionServer
{
    protected int m_Failures;
    protected CarScript m_Car;
    protected ItemBase m_Battery;
    protected ItemBase m_CargoBattery;
    protected float m_CargoHealth;

    void Check(bool passed, string name)
    {
        if (passed)
            Print("[WLMNVDTest] PASS " + name);
        else
        {
            m_Failures++;
            Print("[WLMNVDTest] FAIL " + name);
        }
    }

    override void OnMissionStart()
    {
        super.OnMissionStart();
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(RunChecks, 2000, false);
    }

    void RunChecks()
    {
        WastelandSettings settings = WastelandSettings.Get();
        settings.EnableNoVehicleDamage = true;
        settings.EnableInfiniteBattery = true;
        m_Car = CarScript.Cast(GetGame().CreateObjectEx("CivilianSedan", "7500 0 7500", ECE_PLACE_ON_SURFACE));
        if (!m_Car)
        {
            Check(false, "spawn vehicle");
            Finish();
            return;
        }
        m_Battery = ItemBase.Cast(m_Car.GetInventory().CreateAttachment("CarBattery"));
        ItemBase cargo = ItemBase.Cast(m_Car.GetInventory().CreateInInventory("UniversalLight"));
        if (cargo)
            m_CargoBattery = ItemBase.Cast(cargo.GetInventory().CreateAttachment("Battery9V"));
        Check(m_Battery != null && m_CargoBattery != null, "fixture attachments created");
        if (!m_Battery || !m_CargoBattery)
        {
            Finish();
            return;
        }
        m_CargoBattery.SetHealth(m_CargoBattery.GetMaxHealth() * 0.1);
        m_CargoHealth = m_CargoBattery.GetHealth();
        m_Car.Maintenance_RepairAttachments();
        Check(Math.AbsFloat(m_CargoBattery.GetHealth() - m_CargoHealth) < 0.001, "cargo descendant not repaired");
        Check(m_CargoBattery.GetAllowDamage(), "cargo descendant not protected");
        Check(!m_Battery.GetAllowDamage(), "direct battery protected");

        m_Battery.GetCompEM().AddEnergy(-m_Battery.GetCompEM().GetEnergyMax() * 0.8);
        m_Car.Maintenance_Battery();
        Check(Math.AbsFloat(m_Battery.GetCompEM().GetEnergy() - m_Battery.GetCompEM().GetEnergyMax()) < 0.001, "infinite battery energy filled");
        Check(m_Battery.GetQuantity() >= m_Battery.GetQuantityMax() * 0.99, "battery displayed quantity refreshed");

        WLM_NVD_ProbeAction action = new WLM_NVD_ProbeAction;
        ActionTarget valid = new ActionTarget(m_Battery, m_Car, -1, vector.Zero, 0);
        Check(action.ResolveForTest(valid) == m_Car, "real attachment resolves");
        ActionTarget forged = new ActionTarget(m_CargoBattery, m_Car, -1, vector.Zero, 0);
        Check(action.ResolveForTest(forged) == null, "forged parent rejected");
        m_Car.GetInventory().DropEntity(InventoryMode.SERVER, m_Car, m_Battery);
        Check(m_Battery.GetHierarchyParent() != m_Car, "battery detached");
        Check(m_Battery.GetAllowDamage(), "detached protection released");

        m_CargoBattery.SetAllowDamage(false);
        WLM_NVD_DamageGuard guard = new WLM_NVD_DamageGuard(m_CargoBattery);
        guard.Protect();
        guard.Release();
        Check(!m_CargoBattery.GetAllowDamage(), "pre-existing invulnerability preserved");
        m_CargoBattery.SetAllowDamage(true);
        settings.EnableNoVehicleDamage = false;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckDisabled, 1000, false);
    }

    void CheckDisabled()
    {
        Check(m_Car.GetAllowDamage(), "vehicle protection released when disabled");
        Finish();
    }

    void Finish()
    {
        Print("[WLMNVDTest] DONE failures=" + m_Failures);
    }
}

Mission CreateCustomMission(string path) { return new WLM_NVD_TestMission; }
#else
class WLM_NVD_ControlMission : MissionServer
{
    override void OnMissionStart()
    {
        super.OnMissionStart();
        Print("[WLMNVDTest] CONTROL READY");
    }
}
Mission CreateCustomMission(string path) { return new WLM_NVD_ControlMission; }
#endif
