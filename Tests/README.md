# Validation procedure

These tests are development-only. Never include Tests, docs, README, .git, keys, logs, or previous builds in a PBO.

1. Run `Tests/Test-SourceContracts.ps1`: source guardrails only, not gameplay assertions.
2. Stage root config.cpp and scripts only, verify every source hash, run CfgConvert and DayZDiag against that exact stage. A new runtime edit invalidates earlier compile evidence.
3. Use a genuinely isolated retail server root with official base/DLC content, the pinned clean Chernarus mission data, fresh profile/storage and no unrelated addons. Overlay `Tests/Mission/init.c` only in that disposable mission. Run without the candidate first: require `[WLMNVDTest] CONTROL READY`.
4. Run the exact candidate as a normal `-mod` folder. Require its mounted PBO, expected define, Game/World/Mission initialization, no parser errors, every fixture created, no `[WLMNVDTest] FAIL`, and `[WLMNVDTest] DONE failures=0`. The finite harness checks real attachments, cargo descendants, battery energy/quantity, parent resolution, detachment and protection ownership. It changes only disposable test objects/settings.
5. Perform the real-client matrix below. The no-player harness cannot substitute for it.

## Client/server acceptance

- Flip: settle an empty vanilla car on each side and upside down. Aim at body, intact door, ruined door and wheel. Test both ends of a long vehicle, then stand beyond 2 m from its body. Nearby legitimate targets should work; distant, forged/mismatched parents, upright, occupied or moving vehicles must be rejected. Ambiguous/null target objects fail closed; do not claim unsupported custom proxies work until observed.
- Flip placement: low roofs, walls, bystanders, slopes, bridges and modded vehicle sizes. An obstructed destination must not move the car. Observe from a second client; reconnect and repeat with the feature disabled. Confirm no prompt before initial settings synchronization and no disabled prompt after synchronization.
- Crash matrix: all four combinations of vehicle protection and occupant protection. With vehicle protection on and occupant protection off, a serious collision must hurt the occupant but spare the car. With both off, native damage applies. Also test protected/unprotected pedestrians and infected.
- Parts: attach a damageable battery/door/wheel, wait for protection, then detach/drop/store it. Normal damage must resume. An item protected before attachment must retain that prior state. Cargo items and attachments on cargo items must never be repaired/protected. Check admin-protected cars with this mod disabled.
- Battery: partially discharged battery with infinite on must gain real energy and matching displayed charge. With infinite off and a running engine, charge must rise natively; engine off plus headlights must drain normally. Include damaged batteries and reconnect/restart.
- Engine: submerged running car with water protection on/off, each crossed with vehicle protection on/off. Protection off must allow native drowning damage and engine shutdown. Also test redline/low-coolant damage and vehicles already ruined before this mod starts. Do not infer direct native AddHealth semantics from SetAllowDamage alone.
- Drowning: on-foot swimming remains native; vehicle occupants are protected only when enabled. Check entering/exiting underwater and an already active drowning modifier.
- Settings: first run creates defaults; existing valid legacy files retain values. Malformed/unreadable JSON disables features, logs the failure, and is not overwritten. Correct the file and restart. Settings edits are not hot reloads.
- Compatibility/persistence: actual server mod stack, supported custom cars, two-client/JIP, server restart and pre-existing persisted vehicles. Damage-flag ownership is transient: an old build's already-stuck flag cannot safely be distinguished from intentional third-party invulnerability.

Do not describe a candidate as release-ready from source checks, signature presence, or DayZDiag alone. Sign and verify the final release only after separate authorization and acceptance of the exact artifact.
