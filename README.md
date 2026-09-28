# Wasteland No Vehicle Damage Complete

Server-configurable vehicle protection, maintenance and recovery for DayZ. The addon must be installed on both server and clients: it adds an action and a synchronized vehicle setting.

## Behavior

- Vehicle protection gates damage and maintains direct vehicle attachments. Native collision processing, battery updates, simulation, lights and death handling remain intact.
- Collision protection is independent of vehicle protection. The player setting covers occupants and pedestrians; infected and animals each have their own setting. These guards suppress stock transport-hit damage, not unrelated hit/death notifications.
- Infinite battery uses the native energy-addition path, including displayed quantity updates. Turning it off retains vanilla alternator charging and headlight drain.
- Infinite fuel also replenishes coolant, oil and brake fluid.
- The master vehicle protection setting includes tire maintenance. The separate tire option also works when master protection is off.
- Cargo and attachments on cargo items are excluded. Protection owned by this mod is released when a part detaches; pre-existing invulnerability is preserved.
- Water protection controls the stock engine drowning timer. When disabled, this mod temporarily lifts only its own damage gate around the stock drowning update.
- Player drowning protection applies only to vehicle occupants. On-foot swimming remains vanilla.
- Flip recovery uses native tipped-vehicle detection and actual vehicle bounds, not client-submitted cursor coordinates. It rejects ruined cars, invalid target relationships, occupied/moving vehicles and obstructed destinations. An intact car may be targeted through a ruined attached part. Flip availability is synchronized to clients.

## Settings

The server creates `$profile:WLM_NoVehicleDamageComplete/settings.json` on first initialization. Existing field names are retained. Restart after editing.

```json
{
  "EnableNoVehicleDamage": true,
  "EnableInfiniteFuel": true,
  "EnableInfiniteBattery": true,
  "EnableIndestructibleTires": true,
  "EnableFlipVehicle": true,
  "EnablePlayerDrowningProtection": true,
  "EnablePlayerCollisionProtection": true,
  "EnableZombieCollisionProtection": true,
  "EnableAnimalCollisionProtection": true,
  "EnableVehicleWaterDamageProtection": true,
  "DebugRepairLogs": false,
  "DebugCollisionLogs": false
}
```
The three collision settings work independently of `EnableNoVehicleDamage`. On first load of an older valid settings file, the server adds `"EnableAnimalCollisionProtection": true` while preserving its existing entries and writes the untouched original to `$profile:WLM_NoVehicleDamageComplete/settings.before-animal-protection.json`. The script log prints a success or warning message. Set the new field to `false` to opt out. New settings files include it by default.

## Credits and support

The Songsmith — core logic and development. Inkihh — original wheel-smoke contributions (current effects use the stock lifecycle).

Report issues at the GitHub repository. Repacking is allowed with credit. No source edit constitutes deployment or proof of live gameplay behavior.
