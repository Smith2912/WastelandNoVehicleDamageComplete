# Wasteland Mod Post-Release Checklist

## 1) Deployment sanity
- [ ] All pre-release tests in Tests/README.md passed against the exact candidate; diagnostic compile alone is insufficient
- [ ] Steam Workshop item is live with correct version/changelog
- [ ] Server updated to latest mod build
- [ ] Server fully restarted (not just mission restart)
- [ ] Clients validated/updated mod files

## 2) Config sanity
- [ ] `EnableNoVehicleDamage` expected value
- [ ] `EnablePlayerCollisionProtection` expected value
- [ ] `EnableInfiniteFuel` expected value
- [ ] `EnableInfiniteBattery` expected value
- [ ] `EnableIndestructibleTires` expected value
- [ ] `EnableVehicleWaterDamageProtection` expected value
- [ ] `EnableFlipVehicle` expected value
- [ ] Disabled flip setting reaches new/reconnecting clients and removes the prompt
- [ ] Invalid JSON produces a load failure and disabled features, not a false success message
- [ ] Debug flags OFF for production:
  - [ ] `DebugRepairLogs = 0`
  - [ ] `DebugCollisionLogs = 0`

## 3) Functional smoke tests (5–10 min)
- [ ] Crash into tree/wall at speed → vehicle survives as intended
- [ ] Occupant does not take unintended collision shock/health damage
- [ ] Vehicle attachments protected as intended
- [ ] Detached parts regain normal damage; pre-existing third-party protection is preserved
- [ ] Cargo/trunk items do **not** auto-repair
- [ ] Flip action appears only when vehicle is tipped
- [ ] Flip action works from the vehicle surface at normal reach; it rejects an upright vehicle and an out-of-reach request
- [ ] Battery stays charged when enabled
- [ ] Displayed battery quantity matches real charge; alternator charging works when infinite battery is off
- [ ] Vehicle-protected/player-unprotected crash combination damages occupants normally
- [ ] Fuel/fluids refill when enabled
- [ ] With water protection OFF, a submerged running engine can take normal drowning damage and stop
- [ ] With water protection ON, a submerged engine does not accumulate drowning damage
- [ ] With player drowning protection ON, an on-foot swimmer still takes normal drowning damage while a vehicle occupant does not
- [ ] Death/respawn does not leave stuck vehicle HUD

## 4) Performance check
- [ ] No unusual script FPS drop during vehicle-heavy gameplay
- [ ] Log growth rate is normal (no spam storms)
- [ ] No repeating warning/error loops in script log

## 5) Log audit (after ~15 min live)
- [ ] Scan latest script log for:
  - [ ] `Error`
  - [ ] `Undefined`
  - [ ] `Can't find variable`
  - [ ] `Broken expression`
- [ ] Confirm no unexpected spam from Wasteland debug tags
- [ ] Save path/date of validated log file

## 6) Compatibility quick-check
- [ ] Test with core server mod stack enabled
- [ ] Verify no conflicts with vehicle/framework mods
- [ ] Verify no issues with custom vehicle classes (if present)

## 7) Rollback readiness
- [ ] Keep previous known-good PBO/build archived
- [ ] Keep previous config snapshot archived
- [ ] One-line rollback note prepared for admins/community

## 8) Release comms
- [ ] Post "known fixed issues" summary
- [ ] Ask server owners for repro steps + logs if edge cases appear
- [ ] Add hotfix ETA guidance (if needed)
