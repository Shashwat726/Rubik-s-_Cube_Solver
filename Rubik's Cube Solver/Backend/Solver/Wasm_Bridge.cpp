#include <emscripten/bind.h>
#include <string>
#include <vector>

#include "Cube_3.h"
#include "Solver.h"

// Input: 54 integers in U, D, L, R, F, B order.
std::vector<std::string> solveCube(const std::vector<int>& flat) {

    if (flat.size() != 54) return {"ERROR: expected 54 values"};

    int cube[6][9];
    
    for (int face = 0; face < 6; ++face)
        for (int sticker = 0; sticker < 9; ++sticker)
            cube[face][sticker] = flat[face * 9 + sticker];

    Cube3 C;
    C.init(cube);

    PathFinder P;
    return P.Path_Finder(C);
}

EMSCRIPTEN_BINDINGS(cube_solver_module) {
    emscripten::register_vector<int>("VectorInt");
    emscripten::register_vector<std::string>("VectorString");
    emscripten::function("solveCube", &solveCube);
}