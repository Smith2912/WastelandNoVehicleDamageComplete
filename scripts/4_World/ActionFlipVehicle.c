class ActionFlipVehicle : ActionInteractBase
{
    void ActionFlipVehicle()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
    }

    override string GetText() { return "Flip Vehicle"; }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINone;
        // Check the resolved car below, not the ruin state of a door/wheel.
        m_ConditionTarget = new CCTNone;
    }

    protected CarScript ResolveVehicle(ActionTarget target)
    {
        if (!target || !target.GetObject())
            return null;
        Object object = target.GetObject();
        CarScript car = CarScript.Cast(object);
        if (car)
        {
            if (target.GetParent() && target.GetParent() != car)
                return null;
            return car;
        }
        car = CarScript.Cast(target.GetParent());
        if (!car)
            return null;
        EntityAI attachment = EntityAI.Cast(object);
        if (attachment && attachment.GetHierarchyParent() == car)
            return car;
        // Some native model proxies use scene rather than inventory parenting.
        if (object.GetParent() == car)
            return car;
        return null;
    }

    protected float DistanceToVehicleSq(CarScript car, vector worldPosition, vector minimum, vector maximum)
    {
        vector localPosition = car.WorldToModel(worldPosition);
        vector closest;
        closest[0] = Math.Clamp(localPosition[0], minimum[0], maximum[0]);
        closest[1] = Math.Clamp(localPosition[1], minimum[1], maximum[1]);
        closest[2] = Math.Clamp(localPosition[2], minimum[2], maximum[2]);
        return vector.DistanceSq(worldPosition, car.ModelToWorld(closest));
    }

    protected bool IsWithinVehicleReach(PlayerBase player, CarScript car)
    {
        vector bounds[2];
        if (!player || !car || !car.GetCollisionBox(bounds))
            return false;
        float reachSq = UAMaxDistances.DEFAULT * UAMaxDistances.DEFAULT;
        if (DistanceToVehicleSq(car, player.GetPosition(), bounds[0], bounds[1]) <= reachSq)
            return true;
        vector head;
        MiscGameplayFunctions.GetHeadBonePos(player, head);
        return DistanceToVehicleSq(car, head, bounds[0], bounds[1]) <= reachSq;
    }

    protected bool CanFlip(PlayerBase player, CarScript car)
    {
        if (!car || car.IsDamageDestroyed() || !car.WLM_IsFlipEnabled())
            return false;
        if (!IsWithinVehicleReach(player, car) || car.IsAnyCrewPresent())
            return false;
        if (GetVelocity(car).LengthSq() > 0.25 || dBodyGetAngularVelocity(car).LengthSq() > 0.25)
            return false;
        return car.IsActionFlipped(player);
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        return CanFlip(player, ResolveVehicle(target));
    }

    override void OnExecuteServer(ActionData action_data)
    {
        if (!GetGame().IsServer() || !action_data)
            return;
        CarScript car = ResolveVehicle(action_data.m_Target);
        if (!CanFlip(action_data.m_Player, car))
            return;

        vector bounds[2];
        if (!car.GetCollisionBox(bounds))
            return;
        vector orientation = car.GetOrientation();
        orientation[1] = 0;
        orientation[2] = 0;
        vector matrix[4];
        Math3D.YawPitchRollMatrix(orientation, matrix);
        matrix[3] = car.GetPosition() + "0 0.8 0";

        vector centerLocal = (bounds[0] + bounds[1]) * 0.5;
        vector center = centerLocal.Multiply4(matrix);
        vector size = bounds[1] - bounds[0];
        array<Object> excluded = new array<Object>;
        excluded.Insert(car);
        for (int i = 0; i < car.GetInventory().AttachmentCount(); i++)
            excluded.Insert(car.GetInventory().GetAttachmentFromIndex(i));
        // Do not teleport into a player, wall or low roof; requester is not excluded.
        if (GetGame().IsBoxCollidingGeometry(center, orientation, size, ObjIntersectGeom, ObjIntersectGeom, excluded))
        {
            action_data.m_Player.MessageStatus("Cannot flip: clear the space around and above the vehicle.");
            return;
        }

        dBodySetAngularVelocity(car, vector.Zero);
        SetVelocity(car, vector.Zero);
        car.SetTransform(matrix);
        dBodyActive(car, ActiveState.ACTIVE);
        car.SetSynchDirty();
    }
}
