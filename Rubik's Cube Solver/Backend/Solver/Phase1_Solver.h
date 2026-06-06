#pragma once

#include<iostream>
#include<vector>
#include"Phase1_Fetch.h"

using namespace std;

class PathFinder1{
    /*
    Implements Phase 1 of Kociemba's Two-Phase Algorithm.
    Finds a sequence of moves that takes the cube from any scrambled state to G1.

    G1 is the state where:
    - All corners are correctly oriented (not twisted)
    - All edges are correctly oriented (not flipped)
    - All 4 middle layer edges are in the middle layer

    F       : Fetcher_1 object. Provides heuristic and isG1() check.
    seq     : Stores the solution sequence as it is built up during search.
    moves[] : All 18 possible moves.
    inverse[]: Inverse of each move at the same index. Used to undo moves during backtracking.

    Functions:
    Path1()        : Recursive IDA* search. Applies moves, checks G1, prunes via heuristic.
    Redundant()    : Returns true if current move is on the same face as the previous move.
                     Prevents wasteful sequences like R then R'.
    Path_Finder_1(): Outer IDA* loop. Increases max_depth until G1 is found. Returns seq.
    */
    Fetcher_1 F;
    vector<string> seq;
    string moves[18] = { "R", "R'", "R2", "L", "L'", "L2", "U", "U'", "U2", "D", "D'", "D2", "F", "F'", "F2", "B", "B'", "B2"};
    string inverse[18] = { "R'", "R", "R2", "L'", "L", "L2", "U'", "U", "U2", "D'", "D", "D2", "F'", "F", "F2", "B'", "B", "B2"};
    bool Path1(Cube3 &C, int depth, int max_depth, string pre_move);
    bool Redundant(string move, string pre_move);
    public:
        vector<string> Path_Finder_1(Cube3 &C);
};

bool PathFinder1::Path1(Cube3 &C, int depth, int max_depth, string pre_move){
    if(F.isG1(C))
        return true;
    int f = F.Fetch(C);
    if(depth + f > max_depth)
        return false;
    for(int i = 0; i < 18;i++){
        if(Redundant(moves[i], pre_move))
            continue;
        C.move(moves[i]);
        if(Path1(C, depth + 1, max_depth, moves[i])){
            seq.insert(seq.begin(), moves[i]);
            return true;
        }
        C.move(inverse[i]);
    }
    return false;
}

bool PathFinder1::Redundant(string move, string pre_move){
    return move[0] == pre_move[0] ? true : false;
}

vector<string> PathFinder1::Path_Finder_1(Cube3 &C){
    int max_depth = F.Fetch(C);
    while(true){
        if(Path1(C,0,max_depth,"\0"))
            break;
        max_depth++;
    }
    return seq;
}