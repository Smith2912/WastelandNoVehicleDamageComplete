modded class ZombieBase
{
    override protected void EOnContact(IEntity other, Contact extra)
    {
        if (GetGame().IsServer() && WastelandSettings.Get().EnableZombieCollisionProtection && Transport.Cast(other))
            return;
        super.EOnContact(other, extra);
    }

    // Keep hit/death animation notifications and other mods' non-transport contacts.
}
