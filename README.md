# Wasteland No Vehicle Damage Complete

Server-configurable vehicle protection, maintenance and recovery for DayZ. The addon must be installed on both server and clients: it adds an action and a synchronized vehicle setting.

## Behavior

- Vehicle protection gates damage and maintains direct vehicle attachments. Native collision processing, battery updates, simulation, lights and death handling remain intact.
- Occupant collision protection is independent of vehicle protection. Pedestrian and infected collision protection suppresses native transport-hit registration, not unrelated hit/death notifications.
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
  "EnableVehicleWaterDamageProtection": true,
  "DebugRepairLogs": false,
  "DebugCollisionLogs": false
}
```

Malformed/unreadable settings disable the features and report a load failure instead of silently enabling default protections. The invalid file is not overwritten. Fix it and restart. Existing valid files with omitted fields retain the documented defaults.

## Source, packaging and status

The editable Git repository is separate from the pack root. Pack only `P:\WastelandNoVehicleDamageComplete`, which contains root `config.cpp` and `scripts` and uses virtual prefix `WastelandNoVehicleDamageComplete`. This README, Git metadata, tests and development notes are not PBO inputs.

The source targets the locally inspected DayZ 1.29 API. Other versions, custom vehicles, exact server mod stacks, multiplayer/JIP and restart behavior require their own acceptance evidence; this is not a blanket compatibility claim. Diagnostic compile PBOs are test artifacts, not signed releases. The existing installed `P:\@NoVehicleDamageComplete` package is not automatically replaced by source changes.

Prior native invulnerability set by another mod, or left by an older build, cannot safely be identified as this mod's property after the fact. Do not clear arbitrary protection flags to migrate old objects. Tests and validation records are kept separately from this public repository and the packed addon.

## Credits and support

The Songsmith — core logic and development. Inkihh — original wheel-smoke contributions (current effects use the stock lifecycle).

Report issues at the GitHub repository. Repacking is allowed with credit. No source edit constitutes deployment or proof of live gameplay behavior.
