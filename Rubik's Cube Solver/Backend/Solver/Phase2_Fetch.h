#pragma once

#include<iostream>
#include<fstream>
#include<unordered_map>
#include"Corners.h"
#include"Edges.h"
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
    #ifdef __EMSCRIPTEN__
    const string DB = "/Databases/";
    #else
    const string DB = "../Databases/";
    #endif
    int key,moves_req;
    ifstream fileA(DB + "Phase2_A.bin", ios::binary);
    if(!fileA.is_open()) cerr << "Failed to open " << DB + "Phase2_A.bin" << "\n";
    while(fileA.read((char*)&key, sizeof(int))){
        fileA.read((char*)&moves_req, sizeof(int));
        Data_A[key]=moves_req;
    }
    fileA.close();
    ifstream fileB(DB + "Phase2_B.bin", ios::binary);
    if(!fileB.is_open()) cerr << "Failed to open " << DB + "Phase2_B.bin" << "\n";
    while(fileB.read((char*)&key, sizeof(int))){
        fileB.read((char*)&moves_req, sizeof(int));
        Data_B[key]=moves_req;
    }
    fileB.close();
}

int Fetcher_2::Fetch(int cp, int ep){
    int keyA = cp;
    int keyB = ep;
    auto itA = Data_A.find(keyA);
    auto itB = Data_B.find(keyB);
    if (itA == Data_A.end() || itB == Data_B.end())
        return -1;
    return max(itA->second, itB->second);
}
