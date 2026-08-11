# Rubik's Cube Solver

**🌐 Live Demo:** [https://rubik-s-cubesolver.vercel.app](https://rubik-s-cubesolver.vercel.app)

A 3x3 Rubik's Cube solver written in C++, built around Kociemba's Two-Phase Algorithm, featuring a fully interactive 3D WebAssembly frontend!

*To use the solver directly, just visit the website above. To build or run the project locally, see the instructions below.*

The project started with a sticker-matrix solver and pattern databases. It now also has a table-based search path: the cube is converted into five integer coordinates, and every search move is a lookup instead of a full matrix rotation. That change is what made the solver feel instant on the scrambles I have tested.

## Current status

- 6x9 sticker-matrix cube with all 18 face moves: done
- Corner and edge coordinates, permutation ranking, and UD-slice ranking: done
- Move tables for CO, EO, UDS, CP, and EP: done
- Phase 1 and Phase 2 pattern databases: done
- Table-based IDA* Kociemba solver: done
- WebAssembly (Emscripten) integration: done
- React / Three.js visualizer & interactive UI: done
- Mobile responsiveness & robust validity checks: done
- Robot integration: planned

## Cube representation

`Cube3` stores one row for each face and nine entries for each face:

| Face index | Face | Colour code |
|---:|---|---|
| 0 | Up | Yellow (`0`) |
| 1 | Down | White (`1`) |
| 2 | Left | Red (`2`) |
| 3 | Right | Orange (`3`) |
| 4 | Front | Green (`4`) |
| 5 | Back | Blue (`5`) |

Each face uses normal reading order:

```text
0 1 2
3 4 5
6 7 8
```

## Two-phase solver

Phase 1 takes a scrambled cube to G1. In this project, G1 means:

- every corner's Yellow/White sticker is on an Up/Down face;
- every edge has orientation `0`;
- the four middle-slice edges are in the middle slice.

The compact representation of that condition is:

```cpp
co == 0 && eo == 0 && uds == 425
```

Phase 2 solves the remaining permutations with only:

```text
U, U', U2, D, D', D2, R2, L2, F2, B2
```

## Table-based coordinates

The solver searches with this compact state instead of copying a sticker matrix at each IDA* node:

```cpp
struct cube {
    int co;   // corner orientation
    int eo;   // edge orientation
    int uds;  // position of the four middle-slice edges
    int cp;   // corner permutation
    int ep;   // edge permutation
};
```

The important idea is simple:

```cpp
next_state = move_table[current_state][move];
```

CO, EO, and UDS have 18 move columns for Phase 1. CP and EP have ten columns for the legal Phase 2 moves.

Corner orientation is based on the location of a cubie's Yellow/White sticker, not its numerical colour rank:

| Value | Y/W sticker currently lies on |
|---:|---|
| 0 | Up or Down |
| 1 | Front or Back |
| 2 | Left or Right |

This keeps the coordinate independent of corner permutation, which is necessary for a reliable move table.

## Project structure

| Path | Purpose |
|---|---|
| `Backend/Solver/Cube_3.h` | Sticker-matrix cube and all 18 moves |
| `Backend/Solver/Corners.h` | Corner identification, orientation, and permutation hashing |
| `Backend/Solver/Edges.h` | Edge identification, orientation, slice, and permutation hashing |
| `Backend/Solver/Tables_Fetch.h` | Loads move tables and converts `Cube3` into compact coordinates |
| `Backend/Solver/Phase1_Fetch.h` | Loads the Phase 1 heuristic databases |
| `Backend/Solver/Phase2_Fetch.h` | Loads the Phase 2 heuristic databases |
| `Backend/Solver/Solver.h` | Active table-based two-phase IDA* solver |
| `Backend/Solver/Cube_Solver.cpp` | Console entry point, input, output, and timing |
| `Backend/Generators/Tables_Gen.cpp` | Generates the five move tables |
| `Backend/Generators/Phase1_Gen.cpp` | Generates `Phase1_A.bin` and `Phase1_B.bin` |
| `Backend/Generators/Phase2_Gen.cpp` | Generates `Phase2_A.bin` and `Phase2_B.bin` |
| `Backend/Databases/` | Generated move tables and pattern databases |
| `Frontend/rubiks-cube/` | React and Three.js frontend application |

## Generated files

The solver expects these binary files in `Backend/Databases/`:

| File | Contents |
|---|---|
| `CO.bin` | Corner-orientation move table |
| `EO.bin` | Edge-orientation move table |
| `UDS.bin` | UD-slice move table |
| `CP.bin` | Corner-permutation Phase 2 move table |
| `EP.bin` | Edge-permutation Phase 2 move table |
| `Phase1_A.bin` | Corner-orientation + UD-slice heuristic (`co * 495 + uds`) |
| `Phase1_B.bin` | Edge-orientation + UD-slice heuristic (`eo * 495 + uds`) |
| `Phase2_A.bin` | Corner-permutation heuristic |
| `Phase2_B.bin` | Edge-permutation heuristic |

If the corner-orientation definition changes, rebuild `CO.bin` and `Phase1_A.bin` before solving again.

## Build and run

Run each program from its own folder because the database paths are relative to that working directory.

### First time only: generate move tables

```powershell
cd Backend\Generators
g++ -std=c++17 -O2 Tables_Gen.cpp -o Tables_Gen.exe
.\Tables_Gen.exe
```

### First time only: generate pattern databases

```powershell
g++ -std=c++17 -O2 Phase1_Gen.cpp -o Phase1_Gen.exe
.\Phase1_Gen.exe

g++ -std=c++17 -O2 Phase2_Gen.cpp -o Phase2_Gen.exe
.\Phase2_Gen.exe
```

### Build and run the solver

```powershell
cd ..\Solver
g++ -std=c++17 -O2 Cube_Solver.cpp -o Cube_Solver.exe
.\Cube_Solver.exe
```

Enter 54 values in this face order:

```text
Up, Down, Left, Right, Front, Back
```

The displayed time begins after the 54 values have been read, so it measures cube initialization and solving rather than typing time.

## Next directions

- Add a clean API layer for the robot controller.
- Improve search pruning and collect timing statistics across deeper scrambles.
- Turn a validated move sequence into reliable robot actions.
