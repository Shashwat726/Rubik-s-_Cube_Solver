#pragma once

#include<iostream>
#include<vector>
#include"Phase1_Fetch.h"
#include"Phase2_Fetch.h"
#include"Tables_Fetch.h"

using namespace std;

class PathFinder{
    /*
    This is the active Kociemba solver. It runs IDA* in five integer
    coordinates rather than copying Cube3 matrices at every search node.

    Phase 1 uses all 18 moves to reach G1. Its final sequence is applied once
    to the matrix cube so that Phase 2 can be converted into CP/EP coordinates.
    Phase 2 then uses its ten legal moves to solve the remaining permutations.
    */
    Fetcher F;
    Fetcher_1 F1;
    Fetcher_2 F2;
    vector<string> seq1,seq2;
    string moves[18]={"U2","D2","R2","L2","F2","B2","U","U'","D","D'","R","R'","L","L'","F","F'","B","B'"};
    // string inverse_s[18]={"U2","D2","R2","L2","F2","B2","U'","U","D'","D","R'","R","L'","L","F'","F","B'","B"};
    int inverse[18]={0,1,2,3,4,5,7,6,9,8,11,10,13,12,15,14,17,16};
    bool Path1(cube &c, int depth, int max_depth, string pre_move);
    bool Path2(cube &c, int depth, int max_depth, string pre_move);
    bool Redundant(string move, string pre_move);
    public:
    vector<string> Path_Finder(Cube3 &C);
};

bool PathFinder::Redundant(string move, string pre_move){
    if(pre_move == "\0" || pre_move.empty()) return false;
    return move[0]==pre_move[0];
}

bool PathFinder::Path1(cube &c, int depth, int max_depth, string pre_move){
    if(F.is_G1(c)){
        return true;
    }
    int f1 = F1.Fetch(c.co, c.eo, c.uds);
    if(depth + f1 > max_depth)
        return false;
    for(int i = 0; i<18; i++){
        if(Redundant(moves[i], pre_move))
            continue;
        F.apply_move_p1(c , i);
        if(Path1(c , depth + 1, max_depth, moves[i])){
            seq1.insert(seq1.begin(), moves[i]);
            return true;
        }
        F.apply_move_p1(c , inverse[i]);
    }
    return false;
}

bool PathFinder::Path2(cube &c, int depth, int max_depth, string pre_move){
    if(F.is_Solved(c))
        return true;
    int f2 = F2.Fetch(c.cp, c.ep);
    if(depth + f2 > max_depth)
        return false;
    for(int i=0;i<10;i++){
        if(Redundant(moves[i],pre_move))
            continue;
        F.apply_move_p2(c, i);
        if(Path2(c,depth+1, max_depth, moves[i])){
            seq2.insert(seq2.begin(), moves[i]);
            return true;
        }
        F.apply_move_p2(c,inverse[i]);
    }
    return false;
}

vector<string> PathFinder::Path_Finder(Cube3 &C){
    seq1.clear();
    seq2.clear();

    Corners O_check;
    Edges E_check;
    O_check.Preprocess(C);
    E_check.Orientation(C);
    if(!O_check.is_valid() || !E_check.is_valid() || (O_check.get_parity() != E_check.get_parity())){
        return {"This state of the Rubik's Cube cannot be solved with standard moves"};
    }

    int max_depth;
    cube c = F.to_cube(C);

    if(F.is_Solved(c))
        return seq1;
    max_depth = F1.Fetch(c.co, c.eo, c.uds);
    while(true){
        if(Path1(c, 0, max_depth, "\0"))
            break;
        max_depth++;
    }
    cout<<endl;
    for(const string &move : seq1){
        C.move(move);
    }
    cout<<endl;
    cube c2 = F.to_cube(C);
    max_depth = F2.Fetch(c2.cp, c2.ep);
    while(true){
        if(Path2(c2, 0, max_depth, "\0"))
            break;
        max_depth++;
    }

    for (const string &move : seq2)
    C.move(move);

    if(seq1.back()[0] == seq2.front()[0]){
        seq1.back()+="'";
        seq2.erase(seq2.begin());
    }
    seq1.insert(seq1.end(), seq2.begin(), seq2.end());
    return seq1;
}
