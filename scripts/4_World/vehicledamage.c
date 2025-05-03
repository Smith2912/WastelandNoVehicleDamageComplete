modded class CarScript extends Car
{
    override void OnContact( string zoneName, vector localPos, IEntity other, Contact data )
    {
    }
    override void OnUpdate( float dt )
    {
		if ( GetGame().IsServer() )
		{
			ItemBase battery = GetBattery();
			if ( battery )
			{
				// Simply keep battery at max charge always
				float maxEnergy = battery.GetCompEM().GetEnergyMax();
				battery.GetCompEM().SetEnergy(maxEnergy);
				
				// Rest of battery code can be simplified since we always keep it at max
				m_BatteryTimer = 0;
			}
			
			if ( GetGame().GetWaterDepth( GetEnginePosWS() ) > 0 )
			{
				m_DrownTime += dt;
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
    
    override void EEKilled(Object killer)
	{
	}
    override void EOnPostSimulate(IEntity other, float timeSlice)
    {  
    }
}