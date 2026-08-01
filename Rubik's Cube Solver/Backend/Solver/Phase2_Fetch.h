#pragma once

#include<iostream>
#include<fstream>
#include<unordered_map>
#include"corners.h"
#include"edges.h"
#include"Cube_3.h"

using namespace std;

class Fetcher_2{
    /*
    The table-based solver passes cp and ep directly into Fetch(). The older
    matrix-oriented wording below is background for how these databases were
    originally designed; the current lookup code is the source of truth.

    Loads the Phase 2 databases and provides the heuristic for Phase 2 IDA*.

    Data_A : Loaded from Phase2_A.bin
             Key   = Hash_Permutation() — Lehmer code of corner positions
             Value = minimum moves to solve corners from this permutation state
             
    Data_B : Loaded from Phase2_B.bin
             Key   = Hash_Phase2A() * 24 + Hash_Phase2B()
                     Combines non-slice edge permutation and slice edge permutation
             Value = minimum moves to solve edges from this permutation state

    O, E   : Used internally to compute hash keys from a given cube state.

    Functions:
    Fetcher_2()  : Constructor. Loads Phase2_A.bin and Phase2_B.bin into Data_A and Data_B.
    Fetch(C)     : Returns max(Data_A[keyA], Data_B[keyB]) — the Phase 2 heuristic.
    isSolved(C)  : Returns true if Fetch(C) == 0, meaning cube is fully solved.
    */
    unordered_map<int, int> Data_A, Data_B;
    Corners O;
    Edges E;
    public:
        Fetcher_2();
        int Fetch(int cp, int ep);
};

Fetcher_2::Fetcher_2(){
    
    int key,moves_req;
    ifstream fileA("../Databases/Phase2_A.bin", ios::binary);
    while(fileA.read((char*)&key, sizeof(int))){
        fileA.read((char*)&moves_req, sizeof(int));
        Data_A[key]=moves_req;
    }
    fileA.close();
    ifstream fileB("../Databases/Phase2_B.bin", ios::binary);
    while(fileB.read((char*)&key, sizeof(int))){
        fileB.read((char*)&moves_req, sizeof(int));
        Data_B[key]=moves_req;
    }
    fileB.close();
}

int Fetcher_2::Fetch(int cp, int ep){
    return max(Data_A[cp],Data_B[ep]);
}
