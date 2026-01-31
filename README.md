# Wasteland No Vehicle Damage Complete

## Overview
This mod provides a complete "God Mode" solution for vehicles in DayZ. It ensures vehicles are indestructible, easy to maintain, and easy to recover, making it perfect for "Wasteland" style servers or high-loot PVP servers where vehicles should be assets, not burdens.

## Features

### 🛡️ Complete Vehicle Protection
- **No Collision Damage**: Vehicles take no damage from crashing into walls, trees, or other cars.
- **Bullet & Explosion Immunity**: Vehicles are immune to gunfire, grenades, and melee attacks.
- **Indestructible Tires**: Tires will automatically repair themselves if damaged.
- **Protected Components**: Engine, radiator, and fuel tank cannot be ruined.

### ♾️ Infinite Maintenance
- **Infinite Fuel**: Fuel tank automatically refills.
- **Infinite Fluids**: Oil, Coolant, and Brake Fluid automatically keep themselves topped up.
- **Infinite Battery**: Battery never drains and stays at max charge.

### 🤸 Flip Vehicle Action
- **Easy Recovery**: Walk up to any flipped vehicle (upside down or on its side).
- **Context Action**: A "Flip Vehicle" action appears in the interaction menu.
- **Safe Flipping**: The car is gently uprighted and lifted slightly to prevent clipping.

### 🏊 Environmental Protection
- **Waterproof Engines**: Engines do not take damage when submerged.
- **No Drowning**: Players inside vehicles will not drown underwater (Configurable).
- **Collision Immunity**: Players and Zombies do not take damage/impact from vehicle collisions (Configurable).

## ⚙️ Server Configuration
The mod creates a JSON configuration file on the first server start:
`$profile:WLM_NoVehicleDamageComplete/settings.json`

You can toggle features on/off individually:

```json
{
    "EnableNoVehicleDamage": 1,        // Master toggle for vehicle god mode
    "EnableInfiniteFuel": 1,           // Toggle auto-refuel/fluids
    "EnableInfiniteBattery": 1,        // Toggle infinite battery
    "EnableIndestructibleTires": 1,    // Toggle tire auto-repair
    "EnableFlipVehicle": 1,            // Enable/Disable the Flip Action
    "EnablePlayerDrowningProtection": 1, // Prevent drowning (Configurable)
    "EnableVehicleWaterDamageProtection": 1, // Prevent engine water damage
    "EnablePlayerCollisionProtection": 1, // Prevent player physics damage from cars
    "EnableZombieCollisionProtection": 1  // Prevent zombie physics interaction with cars
}
```
*Changes require a server restart to take effect.*

## Compatibility
- DayZ Version: 1.15+ to Latest
- Works with most expanded interaction mods.
- Safe to add/remove mid-wipe (Vehicles will simply revert to taking damage).

## Credits
- **The Songsmith**: Core logic and development.
- **Inkihh**: Wheel smoke effects and particles.

## Support
For issues, questions, or feature requests, please open an issue on the GitHub repository.
You are free to use this mod on your server. Repacking is allowed with credit.