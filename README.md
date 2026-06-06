# Rubik's Cube Solver
A 3×3 Rubik's Cube solver written in C++, using Kociemba's Two-Phase Algorithm.

## Current Status
- Cube representation with all 18 moves: ✅ Done
- Corner pattern database generation: ✅ Done
- Phase 1 databases (corner orientation + edge orientation + UD slice): ✅ Done
- Phase 2 databases (corner permutation + edge permutation): ✅ Done
- Phase 1 IDA* solver (scrambled → G1): ✅ Done
- Phase 2 IDA* solver (G1 → solved): ✅ Done
- Full Kociemba Two-Phase solver: ✅ Done
- 3D visualizer (React + Three.js): 🔄 In progress

## How It Works
The cube is represented as a `6×9` integer matrix — one row per face,
nine cells per face. Each integer (0–5) maps to a colour:

| Integer | Colour |
|---------|--------|
| 0 | Yellow |
| 1 | White |
| 2 | Red |
| 3 | Orange |
| 4 | Green |
| 5 | Blue |

Kociemba's Two-Phase Algorithm is used to solve the cube:
- **Phase 1** finds a sequence of moves to reach G1 — a subgroup where all corners
  are untwisted, all edges are unflipped, and all middle layer edges are in the middle layer.
- **Phase 2** solves the cube from G1 using only U, D, R2, L2, F2, B2 moves.

Both phases use IDA* search with precomputed pattern databases as heuristics.

## Project Structure
| File | Description |
|------|-------------|
| `backend/solver/Cube_3.h` | Cube representation, all 18 moves, print and initialization |
| `backend/solver/Corners.h` | Corner orientation, permutation, and hash functions |
| `backend/solver/Edges.h` | Edge orientation, slice tracking, and hash functions |
| `backend/solver/Phase1_Fetch.h` | Loads Phase 1 databases, provides heuristic and isG1() |
| `backend/solver/Phase1_Solver.h` | Phase 1 IDA* — finds moves to reach G1 |
| `backend/solver/Phase2_Fetch.h` | Loads Phase 2 databases, provides heuristic and isSolved() |
| `backend/solver/Phase2_Solver.h` | Phase 2 IDA* — solves cube from G1 |
| `backend/solver/Cube_Solver.cpp` | Main program — ties both phases together |
| `backend/generators/Database_Gen.cpp` | Generates Corner_Full.bin (run once) |
| `backend/generators/Phase1_Gen.cpp` | Generates Phase1_A.bin and Phase1_B.bin (run once) |
| `backend/generators/Phase2_Gen.cpp` | Generates Phase2_A.bin and Phase2_B.bin (run once) |

## Databases
The solver requires 4 precomputed binary database files in the `databases/` folder.
Run the generator files once to create them, or download the zipped databases from the releases page.

| File | Description | Entries | Size |
|------|-------------|---------|------|
| `Phase1_A.bin` | Corner orientation + UD slice | ~3.2 million | ~25 MB |
| `Phase1_B.bin` | Edge orientation + UD slice | ~1 million | ~8 MB |
| `Phase2_A.bin` | Corner permutation | 40,320 | <1 MB |
| `Phase2_B.bin` | Edge permutation | ~967,680 | ~7 MB |

## How To Run
**Step 1 — Generate databases (first time only):**
```bash
g++ backend/generators/Phase1_Gen.cpp -o Phase1_Gen && ./Phase1_Gen
g++ backend/generators/Phase2_Gen.cpp -o Phase2_Gen && ./Phase2_Gen
```

**Step 2 — Run the solver:**
```bash
g++ backend/solver/Cube_Solver.cpp -o Cube_Solver && ./Cube_Solver
```

**Step 3 — Enter the 54 values of the cube when prompted.**

## Input Format
Enter 54 integers (0–5) representing the cube state, face by face in reading order:
Face order : Top(0) Bottom(1) Left(2) Right(3) Front(4) Back(5)
Sticker order per face:
0 1 2
3 4 5
6 7 8

## Planned
- Move table architecture for sub-second solving
- 3D visualizer (React + Three.js)
- Python bridge (Flask API)
- Arduino robot integration
