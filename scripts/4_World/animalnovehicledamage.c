modded class AnimalBase
{
    override protected void EOnContact(IEntity other, Contact extra)
    {
        if (GetGame().IsServer() && WastelandSettings.Get().EnableAnimalCollisionProtection && Transport.Cast(other))
            return;

        super.EOnContact(other, extra);
    }

    // DayZAnimal.EOnContact calls this inherited method to apply TransportHit.
    // Also cover calls that reach the damage entry point through another script.
    override void RegisterTransportHit(Transport transport)
    {
        if (GetGame().IsServer() && transport && WastelandSettings.Get().EnableAnimalCollisionProtection)
            return;

        super.RegisterTransportHit(transport);
    }
}
