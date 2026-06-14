# Monster Adventure

Terminal-based C++ game for the first iteration of the assignment.

## Build on Ubuntu 22.04

```bash
cmake -S . -B build
cmake --build build
./build/monster_adventure
```

The project uses only the C++ standard library and CMake. No extra runtime packages are required.

## Gameplay

- Create a character with a name.
- A new character starts with two `Hest` monsters.
- Start an adventure and choose an enemy monster to fight.
- Choose one of your living monsters for the fight.
- Combat picks the first attacker randomly, then attacks alternate.
- A defeated enemy can be added to the character's monster list.
- A character can keep at most four monsters; if the list is full, the player may replace one.
- If all of the character's monsters are defeated, the game returns to the main menu.
