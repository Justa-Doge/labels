# Labels

A standalone Geometry Dash Geode mod that puts a label editor in the in-level pause menu. Labels render on the gameplay HUD, support Eclipse-style RIFT expressions inside braces, save across launches, and can import Eclipse ECL files.

## Current editor

- Add, delete, rename, show, and hide labels.
- Edit label text with multiline expressions.
- Import ECL files from disk.
- Optionally import the two supplied presets: Percentage and Noclip. They are not added automatically.
- Preserves label font, scale, color, alignment, position, and event metadata when importing.

The Percentage preset uses progress, actual progress, best run, best record, FPS, attempts, time, clock, and platformer mode. The current HUD also provides practice/test/death/dual state, level name and timing, and player coordinates.

The Noclip preset refers to Eclipse-provided noclip, noclipDeaths, and noclipAccuracy values. This standalone mod currently supplies fallback values when Eclipse is absent, so the label can be imported and edited but does not independently detect noclip. Imported event metadata is retained; event effects are still on the roadmap.

## Build

Install the Geode SDK with the Geode CLI, set GEODE_SDK to the SDK folder, then run geode build.

The build links RIFT from the pinned Eclipse Menu repository revision in CMakeLists.txt.
