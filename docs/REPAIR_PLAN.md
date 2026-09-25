# Local repair contract — 2026-09-05

Owner: lead agent; editable source remains P:\WastelandNoVehicleDamageComplete at the user's direction. Preserve the existing uncommitted repairs. Do not replace P:\@NoVehicleDamageComplete, publish, sign, or modify GitHub.

Route: script-bearing modded-class vehicle/action/settings repair. References: DayZ architecture, patterns, multiplayer-persistence, enforce-compatibility, layout-toolchain, validation; read-only vanilla scripts/data stamps 124708, installed retail server 1.29.0.163709.

Contract:
- Server independently resolves the flip target and measures reach against the actual vehicle bounds; ruined attachments must not veto an intact car. Reject ambiguous/forged parents, occupied/moving cars, upright cars, and obstructed destinations. Use one transform writer.
- Replicate flip availability on the car using a registered native net-sync boolean; no client-owned feature switch or custom RPC. New clients fail closed until synchronized.
- Preserve stock contact caching, crew calculation, post-simulation, battery update, and killed handling. Block crew damage only when its own setting is enabled.
- Track damage permissions actually changed by this mod. Release them when parts detach, protection turns off, or the owner is deleted. Preserve pre-existing invulnerability; do not continuously enable damage when this mod is off.
- Repair only direct attachments. Add infinite energy through the native notification path so quantity/display updates; infinite-off uses native alternator behavior.
- Check settings load/save results. Invalid input must not silently turn on all protections or report a successful load. No new persistent entity schema.
- Protect only vehicle occupants from drowning; retain ordinary swimming and non-transport hit behavior.

Proof: static regression checks, runtime-only stage/hash parity, approved disposable DayZDiag compile, then approved isolated stock/candidate retail tests. Real-client flip/UI, two-client/JIP, restart and full mod-stack acceptance remain separate gates. Test artifacts are not release packages.
