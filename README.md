# Rubik's Cube Solver

A 3×3 Rubik's Cube solver written in C++, using Kociemba's Two-Phase Algorithm.

## Current Status
- Cube representation with all 18 moves: ✅ Done
- Phase 1 & 2 pattern database generation: 🔄 In progress

## Planned
- Full solver (Phase 1 + Phase 2 search)
- Solution output as move sequence
- 3D visualizer (React + Three.js)

- ## How It Works

The cube is represented as a `6×9` integer matrix — one row per face,
nine cells per face. Each integer (0–5) maps to a colour.

Kociemba's Two-Phase Algorithm is used to solve the cube:
- **Phase 1** reduces the cube to a subgroup where only U, D, and double moves are needed
- **Phase 2** solves it from there to the solved state

All 18 standard moves (R, R', R2, L, L', L2 ...) are implemented.

## Project Structure

| File | Description |
|------|-------------|
| `Cube_3.h` | Cube representation, all 18 moves, print and initialization functions |
