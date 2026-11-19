# Roadmap

Goal: rebuild Eclipse Menu's customizable label feature as a standalone Geode mod, then add new capabilities.

## Implemented in the current working version

- Standalone gameplay HUD labels.
- Label list stored in Geode saved data.
- Pause-dialog button opens the label editor.
- Add, delete, rename, show/hide, and multiline text editing.
- RIFT expression compilation and evaluation inside braces.
- Eclipse ECL import from disk.
- Optional Percentage and Noclip presets copied from the user's supplied files. They are available in the editor and are not loaded automatically.
- Import preserves font, scale, color, alignment, offsets, and event metadata.

## In progress

### Restore the Eclipse label maker
- [ ] Full style editing: font, scale, color, opacity, position, alignment.
- [ ] Preview panel and clearer compile errors.
- [ ] Apply Eclipse label events (visibility, style, death/noclip, hold, and custom-condition events).
- [ ] Import/export packs and duplicate/reorder labels.

### Expand RIFT variables
- [x] Progress, actual progress, best run, best record, FPS, attempts, in-level time, clock, platformer mode.
- [x] Practice/test/death/dual state, level name/time/length, and player coordinates.
- [ ] Broader player stats, clicks/CPS, trigger values, icons, level metadata, and optional mod providers.
- [ ] Add a variable/function reference in the mod documentation.

### Polish and release
- [ ] Compile against the current Geode SDK and resolve remaining API issues.
- [ ] Test the pause-menu button, saved labels, ECL imports, and expression behavior in Geometry Dash.
- [ ] Add release notes and publish a stable package.

## Notes

- Eclipse-only variables such as noclip, noclipDeaths, and noclipAccuracy require a provider; this standalone build currently supplies safe fallback values.
- Imported event metadata is saved, but event behavior is not yet evaluated.
- The supplied presets are optional imports, never defaults.
