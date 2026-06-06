#pragma once

#include<iostream>
#include<vector>
#include"Phase2_Fetch.h"

using namespace std;

class PathFinder2{
    /*
    Implements Phase 2 of Kociemba's Two-Phase Algorithm.
    Finds a sequence of moves that fully solves the cube from a G1 state.

    Phase 2 uses a restricted move set: U, D, R2, L2, F2, B2
    Quarter turns of R, L, F, B are excluded because they would break
    the G1 conditions (twist corners or flip edges).

    F       : Fetcher_2 object. Provides heuristic and isSolved() check.
    seq     : Stores the solution sequence as it is built up during search.
    moves[] : The 6 Phase 2 moves (no quarter turns of R, L, F, B).
    inverse[]: Inverse of each move. R2, L2, F2, B2 are their own inverses.

    Functions:
    Path2()        : Recursive IDA* search. Applies moves, checks solved, prunes via heuristic.
    Redundant()    : Returns true if current move is on the same face as previous move,
                     or if current move is the opposite face of previous move (commutative pairs).
    Path_Finder_2(): Outer IDA* loop. Increases max_depth until solved. Returns seq.
    */
    Fetcher_2 F;
    vector<string> seq;
    string moves[6] = {"U", "D", "R2", "L2", "F2", "B2"};
    string inverse[6] = {"U'", "D'", "R2", "L2", "F2", "B2"};
    bool Path2(Cube3 &C, int depth, int max_depth, string pre_move);
    bool Redundant(string move, string pre_move);
    public:
        vector<string> Path_Finder_2(Cube3 &C);
};

bool PathFinder2::Path2(Cube3 &C, int depth, int max_depth, string pre_move){
    if(F.isSolved(C))
        return true;
    int f =F.Fetch(C);
    if(depth + f >max_depth)
        return false;
    for(int i=0; i<6; i++){
        if(Redundant(moves[i], pre_move))
            continue;
        C.move(moves[i]);
        if(Path2(C, depth+1, max_depth, moves[i])){
            seq.insert(seq.begin(), moves[i]);
            return true;
        }
        C.move(inverse[i]);
    }
    return false;
}

bool PathFinder2::Redundant(string move, string pre_move){
    return move[0] == pre_move[0] ? true:false;
}

vector<string> PathFinder2::Path_Finder_2(Cube3 &C){
    int max_depth = F.Fetch(C);
    while(true){
        if(Path2(C,0,max_depth,"\0"))
            break;
        max_depth++;
    }
    return seq;
}