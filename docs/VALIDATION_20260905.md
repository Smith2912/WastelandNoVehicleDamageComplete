# Repair validation — 2026-09-05

Status: local source remediation and diagnostic compilation passed. Gameplay/release acceptance remains open; this is not a 10/10 certification.

## Source and scope

Editable source remains `P:\WastelandNoVehicleDamageComplete`, branch `codex/fix-open-issues`. Existing edits were preserved. No installed package replacement, signing, publishing, GitHub write or deployment was performed. The user authorized disposable packing, DayZDiag and isolated local server tests.

Repairs cover server-authoritative flip target ancestry/reach, native side/roof detection, ruined attachment targeting, stationary/empty/clear-placement gates, a single transform writer, replicated flip availability, attachment damage-flag ownership and release, independent crew collision protection, native battery notifications/charging, checked settings load/save, and preservation of native simulation/hit/death paths. Drowning remains occupant-only; attachment maintenance excludes cargo descendants. Debug repair/crew messages are throttled.

## Exact candidate

- Stage: `D:\CodexTemp\WLM_NVD_Repair_20260905\stage-r4\WastelandNoVehicleDamageComplete_scripts_pboroot`
- PBO: `D:\CodexTemp\WLM_NVD_Repair_20260905\compile-r4\packed\WastelandNoVehicleDamageComplete\addons\WastelandNoVehicleDamageComplete_scripts_pboroot.pbo`
- PBO SHA-256: `8178C897D1305A9983D8C24CB6A0DF5D1EE892011EEE9E305D597AA6D70E2667`
- Stage manifest SHA-256: `307c5a1529d5a103215d273b1ccca26f3eb1a3282fbc39f1031bd13c5f8b1c4b`
- Ten runtime files: root config plus nine scripts; every source/stage SHA-256 matched after the final runtime edit. Tests, docs and repository metadata are excluded.
- This unsigned diagnostic pack-only artifact is not a release package.

## Evidence

| Gate | Result and limits |
| --- | --- |
| Source regression guardrails | 9/9 passed via `Tests/Test-SourceContracts.ps1`. These inspect source contracts, not native execution. |
| Whitespace | `git diff --check` passed; Git reports LF/CRLF normalization warnings only. |
| Config/layout | Final r4 stage: CfgConvert passed, zero errors, zero warnings, no unresolved required addons. |
| DayZDiag compilation | Final r4: expected define registered, all nine candidate scripts loaded, GameLib/Game/World/Mission compiled (13/417/2130/210 files), no candidate parser error. |
| Diagnostic startup | Not clean runtime evidence: after compilation, the stock diagnostic plugin emits `PluginConfigDebugProfile is not Registred in PluginManager.c!` from PluginItemDiagnostic.OnInit. Compilation success must not be described as a successful gameplay session. |
| Stock retail control | Two fresh-profile runs failed to reach Mission or `[WLMNVDTest] CONTROL READY` within their 120/90-second windows. Removing redirected console pipes did not resolve the stall. |
| Exact candidate retail | Expected candidate define present; Game and World modules load (417/2130 files). No Mission or harness completion marker observed. This does not prove retail Mission compilation or gameplay. |
| In-engine harness | NOT PASSED: `Tests/Mission/init.c` has not executed its checks. Its assertions must not be counted as passing tests. |
| Client UI / multiplayer / JIP | NOT TESTED. There is no custom GUI; the action prompt, rejection feedback and battery quantity still require connected-client observation. |
| Persistence / compatibility / performance | NOT TESTED under a working retail session, restart, real mod stack or representative vehicle load. |
| Release signing / deployment | NOT PERFORMED; outside current authority. |

Diagnostic evidence: `D:\CodexTemp\WLM_NVD_Repair_20260905\compile-r4\target\script_2026-09-05_06-21-58.log` and adjacent RPT. Retail candidate evidence: `D:\CodexTemp\WLM_NVD_Repair_20260905\profiles-candidate-r4\script_2026-09-05_06-27-14.log` and `DayZServer_x64_2026-09-05_06-27-12.RPT`. Stock evidence is retained in `profiles-stock-r1` and `profiles-stock-native-console` under the same validation root.

The isolated retail root contains hash-checked official base and Sakhal PBOs/signatures, excludes five unsigned local addons, uses a clean pinned official Chernarus CE mission (commit `4696b4b0ed09e9ed8076c3f397649f71a871abf1`), loopback binding and fresh profile/storage. Both stock and candidate stop after World during native model initialization. The cause is not established and must not be attributed to, or treated as a pass for, the candidate.

## Remaining acceptance

Follow [the real-client matrix](../Tests/README.md). In particular, prove both side orientations and roof recovery; ruined-door/wheel targets; moving/occupied/obstructed rejection; independent crash toggles; detached parts becoming damageable; displayed battery charge and native charging with infinite off; engine drowning/redline/low-coolant behavior; settings errors; two-client/JIP and restart. Direct native AddHealth behavior is not proven by a SetAllowDamage source inspection. Pre-existing invulnerability left by an older build cannot safely be distinguished from another mod's intentional protection.

## Next gate

- Candidate/artifact: exact r4 PBO above; validation owner is the local tester/developer. Any runtime source change requires a new stage, hashes and compile.
- Scenario/action: establish a stock-only retail control reaching Mission first, then run the finite candidate harness and connected-client acceptance matrix. Resolve the stock startup failure before drawing gameplay conclusions.
- Pass evidence: fresh mounted-PBO/RPT/script evidence, `[WLMNVDTest] CONTROL READY`, then `[WLMNVDTest] DONE failures=0` with no harness failures/parser errors, followed by observed side/roof recovery, UI, multiplayer/JIP, restart and performance results for that exact candidate.

Learning review: the existing DayZ authority, lifecycle, native-tracing and separate-evidence-gate guidance covers these findings. Mod-specific contracts/tests were added locally; no shared skill or memory changes were made.
