modded class ZombieBase
{
    override protected void EOnContact(IEntity other, Contact extra)
    {
        if (GetGame().IsServer() && WastelandSettings.Get().EnableZombieCollisionProtection && Transport.Cast(other))
            return;
        super.EOnContact(other, extra);
    }

    // Block the stock TransportHit damage entry point even when another
    // vehicle script calls it without going through this EOnContact override.
    override void RegisterTransportHit(Transport transport)
    {
        if (GetGame().IsServer() && transport && WastelandSettings.Get().EnableZombieCollisionProtection)
            return;

        super.RegisterTransportHit(transport);
    }

    // Keep hit/death animation notifications and other mods' non-transport contacts.
}
