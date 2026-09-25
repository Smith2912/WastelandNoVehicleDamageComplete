[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
$modRoot = Split-Path -Parent $PSScriptRoot
$car = Get-Content -LiteralPath (Join-Path $modRoot 'scripts\4_World\vehicledamage.c') -Raw
$flip = Get-Content -LiteralPath (Join-Path $modRoot 'scripts\4_World\ActionFlipVehicle.c') -Raw
$settings = Get-Content -LiteralPath (Join-Path $modRoot 'scripts\3_Game\WLM_VehicleSettings.c') -Raw
$mission = Get-Content -LiteralPath (Join-Path $modRoot 'scripts\5_Mission\MissionServer.c') -Raw
$checks = [ordered]@{
    'Native contact and simulation paths remain inherited' = $car -notmatch 'override\s+void\s+(OnContact|CheckContactCache|EOnPostSimulate|EEKilled)\s*\('
    'Native battery update remains called' = $car.Contains('super.OnUpdate(dt);') -and $car -notmatch 'm_BatteryTimer\s*='
    'Infinite battery uses the notification path' = $car.Contains('energy.AddEnergy(missing);') -and $car -notmatch '\.SetEnergy\('
    'Attachment ownership has detach and deletion release' = $car.Contains('override void EEItemDetached(') -and $car.Contains('override void EEDelete(') -and $car.Contains('if (Target && m_Changed)')
    'Repair traversal is direct-only' = $car.Contains('GetAttachmentFromIndex(index)') -and $car -notmatch 'EnumerateInventory|PREORDER'
    'Flip ignores submitted cursor coordinates and checks real ancestry/bounds' = $flip -notmatch 'GetCursorHitPos' -and $flip.Contains('GetCollisionBox(bounds)') -and $flip.Contains('attachment.GetHierarchyParent() == car')
    'Flip has one transform writer and occupied/moving/obstacle gates' = $flip.Contains('car.SetTransform(matrix);') -and $flip -notmatch 'dBodySetTargetMatrix|car.SetPosition|car.SetOrientation' -and $flip.Contains('car.IsAnyCrewPresent()') -and $flip.Contains('GetVelocity(car)') -and $flip.Contains('IsBoxCollidingGeometry')
    'Flip option uses native replication' = $car.Contains('RegisterNetSyncVariableBool("m_WLMFlipEnabled")') -and $car.Contains('SetSynchDirty();') -and $flip.Contains('car.WLM_IsFlipEnabled()')
    'Settings errors are not reported as successful loads' = $settings.Contains('JsonFileLoader<WastelandSettings>.LoadFile(') -and $settings.Contains('features disabled.') -and $mission.Contains('if (WastelandSettings.WLM_LoadSucceeded)')
}
$failed = @($checks.Keys | Where-Object { !$checks[$_] })
foreach ($name in $checks.Keys) {
    if ($checks[$name]) { Write-Output "PASS $name" } else { Write-Output "FAIL $name" }
}
Write-Output 'These are source regression guardrails, not Enforce compilation or gameplay tests.'
if ($failed.Count) { throw "$($failed.Count) source contracts failed" }
