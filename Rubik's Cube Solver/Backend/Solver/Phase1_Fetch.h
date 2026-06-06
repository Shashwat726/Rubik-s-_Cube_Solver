#pragma once

#include<iostream>
#include<fstream>
#include<unordered_map>
#include"corners.h"
#include"edges.h"
#include"Cube_3.h"

using namespace std;

class Fetcher_1{
    /*
    Loads the Phase 1 databases and provides the heuristic for Phase 1 IDA*.
    
    Data_A : Loaded from Phase1_A.bin
             Key   = Hash_Orientation() * 4096 + Hash_Slice()
             Value = minimum moves to reach G1 from this corner orientation + slice state
             
    Data_B : Loaded from Phase1_B.bin
             Key   = Hash_Phase1B() = Hash_Orientation() * 4096 + Hash_Slice()
             Value = minimum moves to reach G1 from this edge orientation + slice state

    O, E   : Used internally to compute hash keys from a given cube state.

    Functions:
    Fetcher_1() : Constructor. Loads Phase1_A.bin and Phase1_B.bin into Data_A and Data_B.
    Fetch(C)    : Returns max(Data_A[keyA], Data_B[keyB]) — the Phase 1 heuristic.
                  Higher value = farther from G1.
    isG1(C)     : Returns true if Fetch(C) == 0, meaning cube is in G1 state.
    */
    unordered_map<int,int> Data_A, Data_B;
    Corners O;
    Edges E;
    public:
        Fetcher_1();
        int Fetch(Cube3 &C);
        bool isG1(Cube3 &C);
};

Fetcher_1::Fetcher_1(){
    
    int key,moves_req;
    ifstream fileA("databases/Phase1_A.bin", ios::binary);
    while(fileA.read((char*)&key, sizeof(int))){
        fileA.read((char*)&moves_req, sizeof(int));
        Data_A[key]=moves_req;
    }
    fileA.close();
    ifstream fileB("databases/Phase1_B.bin", ios::binary);
    while(fileB.read((char*)&key, sizeof(int))){
        fileB.read((char*)&moves_req, sizeof(int));
        Data_B[key]=moves_req;
    }
    fileB.close();
}

int Fetcher_1::Fetch(Cube3 &C){
    O.Preprocess(C);
    E.Orientation(C);
    int keyA = O.Hash_Orientation() * 4096 + E.Hash_Slice(C);
    int keyB = E.Hash_Phase1B(C);
    return max(Data_A[keyA], Data_B[keyB]);
}

bool Fetcher_1::isG1(Cube3 &C){
    return (Fetch(C) == 0) ? true : false;
}