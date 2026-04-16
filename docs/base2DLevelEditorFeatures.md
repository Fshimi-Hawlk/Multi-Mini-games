# Base Features for 2D Level Editor (Exact Original List)

### 1. Core Editor Mode & UI
- Toggle between **Play mode** and **Editor mode** (e.g. press F1 or a dedicated key).
- In editor mode the game is paused (no player physics, no gravity, no movement).
- Simple overlay UI (ImGui or your own immediate-mode UI):
  - Top toolbar with mode selector, save/load buttons, undo/redo.
  - Left sidebar: Terrain Type palette (grid or list showing all `TerrainType_Et` with color preview and name).
  - Right sidebar: Properties panel for the currently selected terrain piece.
  - Bottom status bar showing mouse world position, current zoom, number of pieces.

### 2. Camera Controls (essential for large lobby)
- Mouse wheel zoom (centered on cursor).
- Middle mouse button (or WASD / arrow keys) to pan the view.
- Reset camera to default view (centered on spawn at 0,0).

### 3. Terrain Placement & Manipulation
- **Click to place**: Select a terrain type from the palette → left-click in world places a new `LobbyTerrain_St` at mouse position (default size e.g. 200×30).
- **Drag to place / resize**: Hold left mouse to drag out a rectangle and create/resize a platform.
- **Select**: Left-click on an existing piece to select it (highlight with outline).
- **Multi-select**: Shift + drag rectangle or Ctrl + click.
- **Move selected**: Drag selected piece(s) with left mouse.
- **Resize selected**: Drag corner or edge handles (or numeric fields in properties).
- **Delete**: Press Delete or right-click → Delete.
- **Duplicate**: Ctrl + D or right-click → Duplicate.
- **Copy / Paste**: Ctrl+C / Ctrl+V (with offset to avoid overlap).

### 4. Property Editing (per piece)
When one or more pieces are selected, show:
- Terrain Type (dropdown with all `_Et` values).
- Position X/Y (editable floats).
- Width / Height (editable floats).
- Roundness (slider 0.0–1.0).
- Color (color picker or RGBA fields).
- For moving platforms: velocity X/Y and moveDistance.
- For portals: target position X/Y and “Is Two-Way” checkbox.
- Snap to grid toggle (with configurable grid size, e.g. 10, 25, 50 units).

### 5. Grid & Snapping
- Toggleable grid (visual lines).
- Snap-to-grid while placing/moving/resizing (with optional hold-Shift to disable temporarily).
- Configurable grid step size.

### 6. Special Handling for Your Types
- Water: when placing, automatically create two stacked pieces (normal floor + water on top) or a single water piece with visual hint.
- Decorative: smaller default size, different highlight color.
- Portals: visual portal frame, ability to set target by clicking another location or another portal while editing the property.
- Visual feedback: different outline colors per type (e.g. blue for moving, purple for portal, red tint for water).

### 7. File I/O
- Save current level to a file (e.g. `lobbyLevel.dat` or JSON/text format).
- Load level from file (replace current `lobbyTerrains[]` array at runtime).
- Auto-backup on save (timestamped copies).
- Export as C array literal (so you can copy-paste the finished `lobbyTerrains[]` directly into your globals file – very useful for your workflow).

### 8. Undo / Redo Stack
- At least 20–50 steps of undo/redo for placement, move, resize, delete, property change.

### 9. Visual Aids & Debug
- Show player spawn point as a persistent marker (green circle at 0,0).
- Show all interaction zones (`gameInteractionZones`) as semi-transparent rectangles with text (toggleable).
- Collision preview: optional outline of solid vs non-solid pieces.
- Coordinate axis (small X/Y lines at origin).
- FPS and draw call counter in editor mode.

### 10. Interaction Zone Editing (bonus but recommended)
- Ability to move and resize the `gameInteractionZones` directly in the editor.
- Edit the `.name` string for each zone.
- Visual link from zone to its terrain platform underneath.

### 11. Quality of Life Features
- Search/filter in the terrain palette.
- “Eyedropper” tool: click an existing piece to select its type and copy properties.
- Hold Alt to temporarily disable collision preview while dragging.
- Keyboard shortcuts for everything (place specific type with number keys 1–9, etc.).
- Warning when placing overlapping solid platforms.
- “Test Play” button: quickly switch to play mode with current editor state without saving.

### 12. Data Structure & Runtime Integration
- Editor works directly on a dynamic array of `LobbyTerrain_St` (not the static one).
- On exit from editor mode or on “Apply”, copy the dynamic array back into your global `lobbyTerrains[]` (or keep everything dynamic if you prefer).
- All terrain types and portal fields are fully supported.

# Unit-Testable Features & XP Reasonableness (Exact Original List)

**Unit-testable features from the list (in exact order presented)**

Only features (or clear sub-parts of them) that consist of **pure logic** — i.e. deterministic functions with clear inputs/outputs, no raylib drawing, no mouse/keyboard input, no global state, no timing — are realistically unit-testable in C (using a simple test harness like Unity or even plain assert-based tests).

Here is the breakdown for each item:

1. **Core Editor Mode & UI**  
   Not unit-testable. Involves mode toggling, ImGui/draw calls, and global editor state.

2. **Camera Controls**  
   Not unit-testable (mouse/keyboard input and rendering). The underlying math helpers (world-to-screen, zoom clamping) could be unit-tested, but the feature as a whole cannot.

3. **Terrain Placement & Manipulation**  
   Partially unit-testable. The pure-logic helpers (rectangle overlap detection, selection logic, move/resize calculations, overlap-warning check) can be unit-tested. The actual click/drag input cannot.

4. **Property Editing (per piece)**  
   Yes, fully unit-testable. Applying changes, clamping values, type validation, dropdown-to-enum conversion, and color/RGBA handling are pure functions.

5. **Grid & Snapping**  
   Yes, fully unit-testable. The snap-to-grid functions, grid-step calculations, and “hold Shift to disable” logic are pure math.

6. **Special Handling for Your Types**  
   Yes, fully unit-testable. Rules for auto-creating water floor + surface, default sizes for decorative/portals, visual hint logic, etc., are pure data transformations.

7. **File I/O**  
   Yes (with care). Serialization/deserialization to C-array literal, JSON, or binary can be unit-tested by writing to a memory buffer or temporary file and comparing results. Full disk I/O is integration-tested, but the conversion logic itself is unit-testable.

8. **Undo / Redo Stack**  
   Yes, fully unit-testable. Command-pattern or memento-based history stack is classic unit-test material (push action → undo → redo → verify exact state).

9. **Visual Aids & Debug**  
   Not unit-testable (rendering and debug overlays).

10. **Interaction Zone Editing**  
    Partially unit-testable (same as #3: move/resize/selection logic can be tested; input and drawing cannot).

11. **Quality of Life Features**  
    Mostly not unit-testable (shortcuts, search, eyedropper, etc., are UI/input). Only the pure helper functions inside them (e.g. “copy properties” logic) would be.

12. **Data Structure & Runtime Integration**  
    Yes, fully unit-testable. Dynamic array insert/remove/copy, validation, conversion from editor dynamic array to the global static `lobbyTerrains[]`, etc., are pure data operations.

**Summary of unit-testable features**  
The strongest candidates (fully or almost fully unit-testable) are:  
5, 6, 7, 8, 12, and substantial parts of 3, 4, and 10.

**Are they reasonable to implement via the XP method (TDD)?**  
Yes — and in fact they are **excellent** candidates for Extreme Programming / Test-Driven Development in your C codebase.

- XP (and TDD) shines exactly on these kinds of isolated, pure-logic pieces: you write the test first, then the minimal code that makes it pass, then refactor.  
- Grid & Snapping (5), Undo/Redo (8), and Data Structure helpers (12) are particularly well-suited: they have clear success criteria, no external dependencies, and prevent the most annoying bugs later.  
- File I/O serialization (7) and Special Type Handling (6) also benefit hugely from TDD because small mistakes (wrong float precision, missing portal fields, wrong water stacking order) become immediately visible in the tests.  
- For an intermediate C programmer these tests are easy to write and maintain (simple assert macros, no mocking needed for most of them).

The remaining features (UI, camera, rendering) would use manual testing or simple integration tests instead — which is the normal XP split between unit tests (automated) and acceptance/manual testing.

If you want to start applying XP right now, the highest-ROI places to begin with TDD are:
- Grid & Snapping (5)
- Undo/Redo stack (8)
- Serialization (7)