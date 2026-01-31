class ActionFlipVehicle: ActionInteractBase
{
	void ActionFlipVehicle()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_HUDCursorIcon = CursorIcons.LootCorpse;
	}

	override string GetText()
	{
		return "Flip Vehicle";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTObject(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		CarScript car = CarScript.Cast(target.GetObject());
		if ( !car )
			return false;

		// Check if car is NOT upright
		vector ang = car.GetOrientation();
		if ( Math.AbsFloat(ang[1]) > 5 || Math.AbsFloat(ang[2]) > 5 )
			return true;

		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
        // Check settings on server only
        if (GetGame().IsServer() && !WastelandSettings.Get().EnableFlipVehicle)
            return;

		Object target = action_data.m_Target.GetObject();
		CarScript car = CarScript.Cast(target);

		if ( car )
		{
            Print("[WLM] ActionFlipVehicle: Flipping car " + car);
            
			// 1. Reset Velocities first
			dBodySetAngularVelocity(car, "0 0 0");
			SetVelocity(car, "0 0 0");
			
			// 2. Prepare new Transform
			vector pos = car.GetPosition();
			vector ori = car.GetOrientation();
			
			// Lift up safely to avoid ground collision (Increased to 0.8m for better clearance)
			pos[1] = pos[1] + 0.8;
			
			// Reset Pitch/Roll, keep Yaw
			ori[1] = 0; 
			ori[2] = 0;
			
			// Convert to Matrix for dBodySetTargetMatrix
			vector mat[4];
			Math3D.YawPitchRollMatrix(ori, mat);
			mat[3] = pos;
			
			// 3. Force Physics Transform
			// timeslice=dt means "next step". We use 0.8s for a firm but controlled reset.
			dBodySetTargetMatrix(car, mat, 0.8); 
			
			// Also set standard Sync helpers just in case
			car.SetPosition(pos);
			car.SetOrientation(ori);
            
			car.SetSynchDirty();
            
            Print("[WLM] ActionFlipVehicle: Flip Complete");
		}
	}
};
