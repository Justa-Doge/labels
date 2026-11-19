# Roadmap

Goal: rebuild Eclipse's customizable label feature as a standalone Geode mod, then extend it with useful label features.

## Reference behavior to restore

Based on [Eclipse's label-system documentation](https://github.com/Srivanthv/eclipsemenu/blob/main/LABELS.md):

- User-created on-screen labels made from fixed text and dynamic expressions.
- RIFT-style expressions inside `{ ... }`, including variables, arithmetic, comparisons, ternary conditionals, and string/number conversion.
- Built-in helpers for strings, math, rounding, precision, and padding.
- Values from general game state, player stats, current level, gameplay, clicks, and optional Globed integration.
- Support for arrays/dictionaries and config lookups where available.

## Milestones

### 1. Project foundation
- [ ] Set up the Geode mod project and supported game-version metadata.
- [ ] Add a minimal in-game settings page and a working example label.
- [ ] Confirm label rendering and settings persistence on the current GD/Geode version.

### 2. Restore the label maker
- [ ] Create, edit, duplicate, reorder, enable/disable, and delete labels.
- [ ] Provide a live preview and useful error messages for invalid expressions.
- [ ] Support text, expression fields, visibility, screen position, alignment, and scale.
- [ ] Add starter presets for common labels (progress, attempts, time, FPS, CPS).

### 3. Expression engine and data
- [ ] Implement a documented expression grammar compatible with the useful RIFT syntax.
- [ ] Add safe evaluation limits and clear handling for unavailable values.
- [ ] Add providers for player, level, gameplay, clicks, and optional mod integrations.
- [ ] Keep the variable and function reference in sync with the implementation.

### 4. New features to consider
- [ ] Import/export label packs and share presets.
- [ ] Per-mode and per-scene label profiles.
- [ ] Conditional visibility and color/style changes based on expressions.
- [ ] Label groups, quick toggles, and a drag-to-position editor.
- [ ] Additional community-requested data providers.

### 5. Polish and release
- [ ] Test edge cases, performance, and behavior across supported GD versions.
- [ ] Add user documentation, examples, screenshots, and release notes.
- [ ] Publish a first stable release.

## Design notes

- Keep the common path simple: create a label, choose values, preview it, and place it.
- Make optional integrations degrade gracefully when their mods are absent.
- Treat the feature list under “New features to consider” as a proposal until prioritized.

## Upstream reference

- [Eclipse Menu label-system documentation](https://github.com/Srivanthv/eclipsemenu/blob/main/LABELS.md)
- [Eclipse Menu source repository](https://git.eclipse.menu/EclipseMenu/EclipseMenu)
